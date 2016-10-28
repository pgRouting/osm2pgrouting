/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License t &or more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#include "database/Export2DB.h"
#include "database/table_management.h"

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "utilities/print_progress.h"
#include "utilities/prog_options.h"
#include "utilities/utilities.h"

#include "boost/algorithm/string/replace.hpp"


namespace osm2pgr {

template <typename T>
static
std::string
TO_STR(const T &x) {
    return  boost::lexical_cast<std::string>(x);
}


Export2DB::Export2DB(const  po::variables_map &vm, const std::string &connection) :
    mycon(0),
    db_conn(connection),
    m_vm(vm),
    conninf(connection),
    tables_schema(vm["schema"].as<std::string>()),
    tables_prefix(vm["prefix"].as<std::string>()),
    tables_suffix(vm["suffix"].as<std::string>()),
    m_tables(vm) {

#if 0
        create_types = std::string(
                " type_id integer PRIMARY KEY,"
                " name text");

        create_way_tag = std::string(
                " class_id integer,"
                " way_id bigint");
#endif

        create_vertices = std::string(
                " id bigserial PRIMARY KEY,"
                " osm_id bigint,"
                " cnt integer,"
                " chk integer,"
                " ein integer,"
                " eout integer,"
                " lon decimal(11,8),"
                " lat decimal(11,8),"
                " CONSTRAINT vertex_id UNIQUE(osm_id)");

        create_ways = std::string(
                " gid bigserial PRIMARY KEY,"
                " class_id integer not null,"
                " length double precision,"
                " length_m double precision,"
                " name text,"
                " source bigint,"
                " target bigint,"
                " x1 double precision,"
                " y1 double precision,"
                " x2 double precision,"
                " y2 double precision,"
                " cost double precision,"
                " reverse_cost double precision,"
                " cost_s double precision, "
                " reverse_cost_s double precision,"
                " rule text,"
                " one_way int, "  //  0 unknown, 1 yes(normal direction), 2 (2 way),
                //  -1 reversed (1 way but geometry is reversed)
                //  3 - reversible (one way street but direction chnges on time)
                " maxspeed_forward double precision,"
                " maxspeed_backward double precision,"
                " osm_id bigint,"
                " source_osm bigint,"
                " target_osm bigint,"
                " priority double precision DEFAULT 1");

    }  //  constructor

Export2DB::~Export2DB() {
    PQfinish(mycon);
}

int Export2DB::connect() {
    cout << conninf << endl;
    mycon = PQconnectdb(conninf.c_str());

    auto type = PQstatus(mycon);
    if (type == CONNECTION_BAD) {
        cout << "connection failed: "<< PQerrorMessage(mycon) << endl;
        return 1;
    } else {
        cout << "connection success"<< endl;
        return 0;
    }
}


bool
Export2DB::has_hstore() const {
    try {
        pqxx::work Xaction(db_conn);
        std::string sql = "SELECT * FROM pg_extension WHERE extname = 'hstore'";
        auto result = Xaction.exec(sql);
        return result.size() == 1;

    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return false;
    }
}

bool
Export2DB::has_postGIS() const {
    try {
        pqxx::work Xaction(db_conn);
        std::string sql = "SELECT * FROM pg_extension WHERE extname = 'postgis'";
        auto result = Xaction.exec(sql);
        return result.size() == 1;

    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return false;
    }
}

#ifndef NDEBUG
bool
Export2DB::install_postGIS() const {
    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec("CREATE EXTENSION postgis");
        Xaction.exec("CREATE EXTENSION hstore");
        Xaction.commit();
        return true;
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
    return false;
}
#endif



bool Export2DB::createTable(
        const std::string &table_description,
        const std::string &table,
        const std::string &constraint) const {
    std::string sql =
        "CREATE TABLE " + table + " ("
        + table_description + constraint + ");";

    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec(sql);
        Xaction.commit();
        std::cout << "NOTICE: " << table << " created ... OK." << std::endl;
        return true;
    } catch (const std::exception &e) {
        std::cout << "NOTICE: " << table << " already exists." << std::endl;
    }
    return false;
}


void Export2DB::addGeometry(
        const std::string &schema, const std::string &table,
        const std::string &geometry_type) const {
    /** PostGIS requires the schema to be specified as separate arg if not default user's schema **/
    std::string sql =
        + " SELECT AddGeometryColumn(" + (schema == "" ? "" : "'" + schema + "' ,") + " '"
        + table + "',"
        + "'the_geom', 4326, '" + geometry_type + "',2);";

    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec(sql);
        std::cout << "    NOTICE: geometry " << addSchema(table) << ".the_geom created ... OK." << std::endl;
        Xaction.commit();
    } catch (const std::exception &e) {
        std::cout << "    NOTICE: geometry " << addSchema(table) << ".the_geom already exists." << std::endl;
    }
}

void Export2DB::addTempGeometry(
        const std::string &table,
        const std::string &geometry_type,
        pqxx::work &Xaction) const {
    std::string sql =
        " SELECT AddGeometryColumn('"
        + table + "',"
        + "'the_geom', 4326, '" + geometry_type + "',2);";

    Xaction.exec(sql);
}


void Export2DB::addTempGeometry(
        const std::string &table,
        const std::string &geometry_type) const {
    std::string sql =
        " SELECT AddGeometryColumn('"
        + table + "',"
        + "'the_geom', 4326, '" + geometry_type + "',2);";


    PGresult *result = PQexec(mycon, sql.c_str());
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        std::cout << PQresultErrorMessage(result);
        throw std::string(PQresultErrorMessage(result));
    }
    PQclear(result);
}


void Export2DB::create_gindex(const std::string &index, const std::string &table) const {
    std::string sql = (
            " CREATE INDEX "
            + index + "_gdx ON "
            + table + " using gist(the_geom);");
    PGresult *result = PQexec(mycon, sql.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cout << PQresultErrorMessage(result);
        throw std::string(PQresultErrorMessage(result));
    }
    PQclear(result);
}

void Export2DB::create_idindex(const std::string &colname, const std::string &table) const {
    std::string sql = (
            " CREATE INDEX "
            "     ON " + table +
            "     USING btree (" + colname + ");");
    PGresult *result = PQexec(mycon, sql.c_str());
    // TODO(who) check missing
    PQclear(result);
}

// /////////////////////
void Export2DB::createTables() const {
    //  the following are particular of the file tables
    if (createTable(create_vertices, addSchema(full_table_name("ways_vertices_pgr")) )) {
        addGeometry(default_tables_schema(), full_table_name("ways_vertices_pgr"), "POINT");
        create_gindex(full_table_name("ways_vertices_pgr"), addSchema(full_table_name("ways_vertices_pgr")));
        create_idindex("osm_id", addSchema(full_table_name("ways_vertices_pgr")));
    }

    if (createTable(create_ways, addSchema(full_table_name("ways")))) {
        addGeometry(default_tables_schema(), full_table_name("ways"), "LINESTRING");
        create_gindex(full_table_name("ways"), addSchema(full_table_name("ways")));
        create_idindex("source_osm", addSchema(full_table_name("ways")));
        create_idindex("target_osm", addSchema(full_table_name("ways")));
        create_idindex("source", addSchema(full_table_name("ways")));
        create_idindex("target", addSchema(full_table_name("ways")));
    }

    try {
        pqxx::work Xaction(db_conn);
        Xaction.exec(m_tables.osm_nodes.create());
        Xaction.exec(m_tables.osm_ways.create());
        Xaction.exec(m_tables.osm_relations.create());
        Xaction.exec(m_tables.configuration.create());
        Xaction.commit();
    } catch (const std::exception &e) {
        std::cerr <<  "\n" << e.what() << std::endl;
    }

}



void
Export2DB::dropTable(const std::string &table, pqxx::work &Xaction) const {
    std::string sql("DROP TABLE IF EXISTS " +  table + " CASCADE");
    Xaction.exec(sql);
}



void Export2DB::dropTables() const {
    try {
        pqxx::work Xaction(db_conn);
        dropTable(addSchema(full_table_name("ways")), Xaction);
        dropTable(addSchema(full_table_name("ways_vertices_pgr")), Xaction);
        Xaction.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }

    //  we are not deleting general tables osm_
}

void
Export2DB::export_configuration(const std::map<std::string, Tag_key>& items) const {

    auto osm_table = m_tables.get_table("configuration");

    std::vector<std::string> values;

    for (const auto &item : items) {
        auto row = item.second.values(osm_table.columns());
        values.insert(values.end(), row.begin(), row.end());
    }

    export_osm(values, osm_table);
}


void
Export2DB::export_osm(
        const std::vector<std::string> &values,
        const Table &table) const {

    auto columns = table.columns();
    std::string temp_table(table.temp_name());
    auto create_sql = table.tmp_create();
    std::string copy_sql( "COPY " + temp_table + " (" + comma_separated(columns) + ") FROM STDIN");
    
#if 0
    std::cout << "\n" << create_sql;
    std::cout << "\n" << copy_sql;

#endif

    size_t count = 0;
    try {


        pqxx::connection db_con(conninf);
        pqxx::work Xaction(db_con);
        PGconn *mycon = PQconnectdb(conninf.c_str());

        PGresult *res = PQexec(mycon, create_sql.c_str());
        res = PQexec(mycon, copy_sql.c_str());

        for (auto it = values.begin(); it != values.end(); ++it) {
            auto str = *it;

            ++count;

            PQputline(mycon, str.c_str());
        }

        PQputline(mycon, "\\.\n");
        PQendcopy(mycon);
        Xaction.exec(m_tables.post_process(table));
        Xaction.exec("DROP TABLE " + temp_table);
        PQfinish(mycon);
        Xaction.commit();

    } catch (const std::exception &e) {
        std::cerr <<  "\n" << e.what() << std::endl;
        std::cerr << "While exporting to " << table.addSchema() << " TODO insert one by one skip the guilty one\n";
    }
}




/*!

*/
void Export2DB::fill_vertices_table(
        const std::string &table,
        const std::string &vertices_tab,
        pqxx::work &Xaction) const {
    // std::cout << "Filling '" << vertices_tab << "' based on '" << table <<"'\n";
    std::string sql(
            "WITH osm_vertex AS ("
            "(select source_osm as osm_id, x1 as lon, y1 as lat FROM " + table + " where source is NULL)"
            " union "
            "(select target_osm as osm_id, x2 as lon, y2 as lat FROM " + table + " where target is NULL)"
            ") , "
            " data1 AS (SELECT osm_id, lon, lat FROM (SELECT DISTINCT * from osm_vertex) a "
            ") "
            " INSERT INTO " + vertices_tab + " (osm_id, lon, lat, the_geom) (SELECT data1.*, ST_SetSRID(ST_Point(lon, lat), 4326) FROM data1)");
    auto result = Xaction.exec(sql);

    std::cout << "\t Vertices inserted: " << result.affected_rows();
}





void Export2DB::fill_source_target(
        const std::string &table,
        const std::string &vertices_tab,
        pqxx::work &Xaction) const {
    // std::cout << "    Filling 'source' column of '" << table << "': ";
    std::string sql1(
            " UPDATE " + table + " AS w"
            " SET source = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.source is NULL and w.source_osm = v.osm_id;");
    Xaction.exec(sql1);

    std::string sql2(
            " UPDATE " + table + " AS w"
            " SET target = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.target is NULL and w.target_osm = v.osm_id;");
    Xaction.exec(sql2);

    std::string sql3(
            " UPDATE " + table +
            " SET  length_m = st_length(geography(ST_Transform(the_geom, 4326))),"
            "      cost_s = CASE "
            "           WHEN one_way = -1 THEN -st_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_forward::float * 5.0 / 18.0)"
            "           ELSE st_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "             END, "
            "      reverse_cost_s = CASE "
            "           WHEN one_way = 1 THEN -st_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "           ELSE st_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "             END "
            " WHERE length_m IS NULL;");
    Xaction.exec(sql3);
}




void Export2DB::exportWays(const Ways &ways, const Configuration &config) const {
    std::cout << "    Processing " <<  ways.size() <<  " ways"  << ":\n";
    std::string ways_columns(
            " class_id, "
            " osm_id, "
            " maxspeed_forward, maxspeed_backward, "
            " one_way, "
            " priority, "

            " length,"
            " x1, y1,"
            " x2, y2,"
            " source_osm,"
            " target_osm,"
            " the_geom,"
            " cost, "
            " reverse_cost,"

            " name ");

    std::vector<std::string> columns;
    columns.push_back("class_id");
    columns.push_back("osm_id");
    columns.push_back("maxspeed_forward");
    columns.push_back("maxspeed_backward");
    columns.push_back("one_way");
    columns.push_back("priority");

    columns.push_back("length");
    columns.push_back("x1"); columns.push_back("y1");
    columns.push_back("x2"); columns.push_back("y2");
    columns.push_back("source_osm");
    columns.push_back("target_osm");
    columns.push_back("the_geom");
    columns.push_back("cost");
    columns.push_back("reverse_cost");
    columns.push_back("name");


    size_t chunck_size = 20000;


    int64_t split_count = 0;
    int64_t count = 0;
    size_t start = 0;
    auto it = ways.begin();
    while (start < ways.size()) {
        auto limit = (start + chunck_size) < ways.size() ? start + chunck_size : ways.size();
#if 0
        try {
#endif
            pqxx::work Xaction(db_conn);
            Xaction.exec("CREATE TABLE  __ways_temp ("
                    + create_ways + ")");

            Xaction.exec("SELECT AddGeometryColumn('__ways_temp', 'the_geom', 4326, 'LINESTRING', 2)");
#if 1
            pqxx::tablewriter tw(Xaction, "__ways_temp", columns.begin(), columns.end());
#endif
            for (auto i = start; i < limit; ++i) {
                auto way = *it;

                ++count;
                ++it;

                if (way.tag_config().key() == "" || way.tag_config().value() == "") continue;

                std::vector<std::string> common_values;
                common_values.push_back(TO_STR(config.FindTag_value(way.tag_config()).id()));
                common_values.push_back(TO_STR(way.osm_id()));
                common_values.push_back(way.maxspeed_forward_str());
                common_values.push_back(way.maxspeed_backward_str());
                common_values.push_back(way.oneWayType_str());
                common_values.push_back(config.priority_str(way.tag_config()) );

                auto splits = way.split_me();
                split_count +=  splits.size();
                for (size_t j = 0; j < splits.size(); ++j) {
                    auto length = way.length_str(splits[j]);

                    auto values = common_values;
                    values.push_back(length);
                    values.push_back(splits[j].front()->lon());
                    values.push_back(splits[j].front()->lat());
                    values.push_back(splits[j].back()->lon());
                    values.push_back(splits[j].back()->lat());
                    values.push_back(TO_STR(splits[j].front()->osm_id()));
                    values.push_back(TO_STR(splits[j].back()->osm_id()));
                    values.push_back(way.geometry_str(splits[j]));

                    // cost based on oneway
                    if (way.is_reversed())
                        values.push_back(std::string("-") + length);
                    else
                        values.push_back(length);

                    // reverse_cost
                    if (way.is_oneway())
                        values.push_back(std::string("-") + length);
                    else
                        values.push_back(length);

                    values.push_back(way.name());
                    tw.insert(values);
                }
            }

            print_progress(ways.size(), count);
            std::cout << " Total Processed: " << count;
            tw.complete();
            process_section(ways_columns, Xaction);
            Xaction.exec("DROP TABLE __ways_temp");
            Xaction.commit();
#if 0
        } catch (const std::exception &e) {
            std::cerr <<  "\n" << e.what() << std::endl;
            std::cerr << "While processing from " << start << "th \t to: " << limit << "th way\n";
            std::cerr << "count" << count << " While processing from " << start << "th \t to: " << limit << "th way\n";
        }
#endif

        start = limit;
    }
}



void Export2DB::process_section(const std::string &ways_columns, pqxx::work &Xaction) const {
    //  std::cout << "Creating indices in temporary table\n";
    Xaction.exec("CREATE INDEX __ways_temp_gdx ON __ways_temp using gist(the_geom);");
    Xaction.exec("CREATE INDEX ON __ways_temp  USING btree (source_osm)");
    Xaction.exec("CREATE INDEX ON __ways_temp  USING btree (target_osm)");



    //  std::cout << "Deleting  duplicated ways from temporary table\n";
    std::string delete_from_temp(
            " DELETE FROM __ways_temp a "
            "     USING " + addSchema(full_table_name("ways")) + " b "
            "     WHERE a.the_geom ~= b.the_geom AND ST_OrderingEquals(a.the_geom, b.the_geom);");
    Xaction.exec(delete_from_temp);

    //  std::cout << "Updating to existing toplology the temporary table\n";
    fill_source_target("__ways_temp" , addSchema(full_table_name("ways_vertices_pgr")), Xaction);

    //  std::cout << "Inserting new vertices in the vertex table\n";
    fill_vertices_table("__ways_temp" , addSchema(full_table_name("ways_vertices_pgr")), Xaction);

    //  std::cout << "Updating to new toplology the temporary table\n";
    fill_source_target("__ways_temp" , addSchema(full_table_name("ways_vertices_pgr")), Xaction);


    //  std::cout << "Inserting new split ways to '" << addSchema(full_table_name("ways")) << "'\n";
    std::string insert_into_ways(
            " INSERT INTO " + addSchema(full_table_name("ways")) +
            "(" + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s) "
            " (SELECT " + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s FROM __ways_temp); ");
    auto result = Xaction.exec(insert_into_ways);
    std::cout << "\tSplit ways inserted " << result.affected_rows() << "\n";
}








void Export2DB::createFKeys() {
    // return; // TODO
    /*
       ALTER TABLE osm_way_classes
       ADD FOREIGN KEY (type_id) REFERENCES osm_way_types (type_id) ON UPDATE NO ACTION ON DELETE NO ACTION;
       */

    std::string fk_classes(
            "ALTER TABLE " + addSchema("config_classes")  + " ADD  FOREIGN KEY (type_id) REFERENCES " +  addSchema("osm_way_types")  + "(type_id)");
    PGresult *result = PQexec(mycon, fk_classes.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << PQresultStatus(result);
        std::cerr << "foreign keys for " + addSchema("config_classes")  + " failed:"
            << PQerrorMessage(mycon)
            << std::endl;
        PQclear(result);
    } else {
        std::cout << "Foreign keys for " + addSchema("config_classes")  + " table created" << std::endl;
    }

    std::string fk_way_tag(
            "ALTER TABLE " + addSchema("osm_way_tags")  + " ADD FOREIGN KEY (class_id) REFERENCES " + addSchema("config_classes") + "(class_id); ");
#if 0
    // DOES NOT WORK because osm_id is not unique
    "ALTER TABLE " + addSchema("osm_way_tags")  + " ADD FOREIGN KEY (way_id) REFERENCES " + addSchema(full_table_name("ways")) + "(osm_id); ");
#endif
    result = PQexec(mycon, fk_way_tag.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << PQresultStatus(result);
        std::cerr << "foreign keys for " + addSchema("osm_way_tags") + " failed: "
            << PQerrorMessage(mycon)
            << std::endl;
        PQclear(result);
    } else {
        std::cout << "Foreign keys for " + addSchema("osm_way_tags") + " table created" << std::endl;
    }

#if 0
    std::string fk_relations(
            "ALTER TABLE " + addSchema(full_table_name("relations_ways"))  + " ADD FOREIGN KEY (relation_id) REFERENCES " + addSchema("osm_relations") + "(osm_id); ");
    result = PQexec(mycon, fk_relations.c_str());
    // its not working as there are several ways with the same osm_id
    // the gid is not possible because that is "on the fly" sequential
    "ALTER TABLE " + addSchema(full_table_name("relations_ways"))  + " ADD FOREIGN KEY (way_id) REFERENCES " +  addSchema(full_table_name("ways")) + "(osm_id);");
#endif
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << PQresultStatus(result);
        std::cerr << "foreign keys for " + addSchema(full_table_name("relations_ways"))  + " failed: "
            << PQerrorMessage(mycon)
            << std::endl;
        PQclear(result);
    } else {
        std::cout << "Foreign keys for " + addSchema(full_table_name("relations_ways"))  + " table created" << std::endl;
    }

    std::string fk_ways(
            "ALTER TABLE " + addSchema(full_table_name("ways")) + " ADD FOREIGN KEY (class_id) REFERENCES " + addSchema("config_classes") + "(class_id);" +
            "ALTER TABLE " + addSchema(full_table_name("ways")) + " ADD FOREIGN KEY (source) REFERENCES " + addSchema(full_table_name("ways_vertices_pgr")) + "(id); " +
            "ALTER TABLE " + addSchema(full_table_name("ways")) + " ADD FOREIGN KEY (target) REFERENCES " + addSchema(full_table_name("ways_vertices_pgr")) + "(id);");
    result = PQexec(mycon, fk_ways.c_str());

    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << PQresultStatus(result);
        std::cerr << "foreign keys for ways failed: "
            << PQerrorMessage(mycon)
            << std::endl;
        PQclear(result);
    } else {
        std::cout << "Foreign keys for Ways table created" << std::endl;
    }
}

}  // namespace osm2pgr
