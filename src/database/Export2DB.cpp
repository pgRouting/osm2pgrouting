/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program IS free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License AS published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program IS distributed in the hope that it will be useful,       *
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
    m_vm(vm),
    conninf(connection),
    m_tables(vm)
{
}

Export2DB::~Export2DB() {
#if 0
    PQfinish(mycon);
#endif
}

int Export2DB::connect() {
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);
        cout << "connection success"<< endl;
        return 0;

    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        return 1;
    }
}


bool
Export2DB::has_extension(const std::string &name) const {
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);
        std::string sql = "SELECT * FROM pg_extension WHERE extname = '" + name + "'";
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
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);
        Xaction.exec("CREATE EXTENSION postgis");
        Xaction.exec("CREATE EXTENSION hstore");
        Xaction.commit();
        return true;
    } catch (const std::exception &e) {
        // cerr << e.what() << std::endl;
    }
    return false;
}
#endif




// /////////////////////


bool Export2DB::exists(const std::string &table) const {
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);

        Xaction.exec(std::string("SELECT '") + table + "'::regclass");
        std::cout << "TABLE: " << vertices().addSchema() << " already exists.\n";
        return true;
    } catch (const std::exception &e) {
        return false;
    }
}



void Export2DB::createTables() const {
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);

        if (!exists(vertices().addSchema())) {
            Xaction.exec(vertices().create());
            std::cout << "TABLE: " << vertices().addSchema() << " created ... OK.\n";
        }

        if (!exists(ways().addSchema())) {
            Xaction.exec(ways().create());
            std::cout << "TABLE: " << ways().addSchema() << " created ... OK.\n";
        }

        if (!exists(pois().addSchema())) {
            Xaction.exec(pois().create());
            std::cout << "TABLE: " << pois().addSchema() << " created ... OK.\n";
        }

        if (!exists(configuration().addSchema())) {
            Xaction.exec(configuration().create());
            std::cout << "TABLE: " << configuration().addSchema() << " created ... OK.\n";
        }


        Xaction.commit();
    } catch (const std::exception &e) {
        std::cerr <<  "\n" << e.what() << std::endl;
        std::cerr <<  "FATAL ERROR: could not routing tables" << std::endl;
        exit(1);
    }

    if (m_vm.count("addnodes")) {
        try {
            pqxx::connection db_conn(conninf);
            pqxx::work Xaction(db_conn);
            /*
             * optional tables
             */
            if (!exists(osm_nodes().addSchema())) {
                Xaction.exec(osm_nodes().create());
                std::cout << "TABLE: " << osm_nodes().addSchema() << " created ... OK.\n";
            }

            if (!exists(osm_ways().addSchema())) {
                Xaction.exec(osm_ways().create());
                std::cout << "TABLE: " << osm_ways().addSchema() << " created ... OK.\n";
            }

            if (!exists(osm_relations().addSchema())) {
                Xaction.exec(osm_relations().create());
                std::cout << "TABLE: " << osm_relations().addSchema() << " created ... OK.\n";
        }

        Xaction.commit();
        } catch (const std::exception &e) {
            std::cerr <<  "\n" << e.what() << std::endl;
            std::cerr <<  "WARNING: could not create osm-*  tables" << std::endl;
            std::cerr <<  "   Insertions on osm_* tables are going to be ignored" << std::endl;
        }
    }
}




void Export2DB::dropTables() const {
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);

        Xaction.exec(ways().drop());
        std::cout << "TABLE: " << ways().addSchema() << " dropped ... OK.\n";

        Xaction.exec(vertices().drop());
        std::cout << "TABLE: " << vertices().addSchema() << " dropped ... OK.\n";

        Xaction.exec(pois().drop());
        std::cout << "TABLE: " << pois().addSchema() << " dropped ... OK.\n";

        Xaction.exec(configuration().drop());
        std::cout << "TABLE: " << configuration().addSchema() << " dropped ... OK.\n";

        Xaction.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
        cerr << "ROLLBACK applied";
    }

    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);
        Xaction.exec(osm_nodes().drop());
        std::cout << "TABLE: " << osm_nodes().addSchema() << " dropped ... OK.\n";

        Xaction.exec(osm_ways().drop());
        std::cout << "TABLE: " << osm_ways().addSchema() << " dropped ... OK.\n";

        Xaction.exec(osm_relations().drop());
        std::cout << "TABLE: " << osm_relations().addSchema() << " dropped ... OK.\n";

        Xaction.commit();
    } catch (const std::exception &e) {
        cerr << e.what() << std::endl;
    }
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
    if (values.empty()) return;

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
        if (res) {};

        for (auto it = values.begin(); it != values.end(); ++it) {
            auto str = *it;

            ++count;

            PQputline(mycon, str.c_str());
        }

        PQputline(mycon, "\\.\n");

        if (PQendcopy(mycon) != 0) {
            Xaction.exec("DROP TABLE " + temp_table);
            PQfinish(mycon);
            Xaction.commit();

            if (values.size() < 2) {
                for (const auto &v : values) {
                std::cout << "\n*****ERROR HERE:\n" << v << "\n******";
                }
                return;
            }
            size_t inc = values.size() / 2;
            export_osm(std::vector<std::string>(values.begin(), values.begin() + inc), table);
            export_osm(std::vector<std::string>(values.begin() + inc , values.end()), table);
            return;
        };

        PQfinish(mycon);
        Xaction.exec(m_tables.post_process(table));
        Xaction.exec("DROP TABLE " + temp_table);
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
            "(SELECT source_osm AS osm_id, x1 AS lon, y1 AS lat FROM " + table + " where source IS NULL)"
            " union "
            "(SELECT target_osm AS osm_id, x2 AS lon, y2 AS lat FROM " + table + " where target IS NULL)"
            ") , "
            " data1 AS (SELECT osm_id, lon, lat FROM (SELECT DISTINCT * FROM osm_vertex) a "
            ") "
            " INSERT INTO " + vertices_tab + " (osm_id, lon, lat, the_geom) (SELECT data1.*, ST_SetSRID(ST_Point(lon, lat), 4326) FROM data1)");
    auto result = Xaction.exec(sql);

    std::cout << "\t Vertices inserted: " << result.affected_rows();
}





void Export2DB::fill_source_target(
        const std::string &table,
        const std::string &vertices_tab,
        pqxx::work &Xaction) const {
    // std::cout << "    Filling 'source' column of '" << table << "':'" << vertices_tab << "'\n";
    std::string sql1(
            " UPDATE " + table + " AS w"
            " SET source = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.source IS NULL and w.source_osm = v.osm_id;");
    Xaction.exec(sql1);

    std::string sql2(
            " UPDATE " + table + " AS w"
            " SET target = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.target IS NULL and w.target_osm = v.osm_id;");
    Xaction.exec(sql2);

    std::string sql3(
            " UPDATE " + table +
            " SET  length_m = ST_length(geography(ST_Transform(the_geom, 4326))),"
            "      cost_s = CASE "
            "           WHEN one_way = -1 THEN -ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_forward::float * 5.0 / 18.0)"
            "           ELSE ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "             END, "
            "      reverse_cost_s = CASE "
            "           WHEN one_way = 1 THEN -ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "           ELSE ST_length(geography(ST_Transform(the_geom, 4326))) / (maxspeed_backward::float * 5.0 / 18.0)"
            "             END "
            " WHERE length_m IS NULL AND maxspeed_backward !=0 AND maxspeed_forward != 0;");
    Xaction.exec(sql3);
}




void Export2DB::exportWays(const Ways &ways, const Configuration &config) const {
    std::cout << "    Processing " <<  ways.size() <<  " ways"  << ":\n";

    Table table = this->ways();

    auto columns = table.columns();
    auto ways_columns = comma_separated(columns);

    size_t chunck_size = m_vm["chunk"].as<size_t>();

    auto create_sql = table.tmp_create();
    auto temp_table(table.temp_name());

    std::string copy_sql( "COPY " + temp_table + " (" + comma_separated(columns) + ") FROM STDIN");


    int64_t split_count = 0;
    int64_t count = 0;
    size_t start = 0;
    auto it = ways.begin();

    while (start < ways.size()) {
        auto limit = (start + chunck_size) < ways.size() ? start + chunck_size : ways.size();
        try {
            pqxx::connection db_con(conninf);
            pqxx::work Xaction(db_con);

            PGconn *mycon = PQconnectdb(conninf.c_str());
            PGresult *res = PQexec(mycon, create_sql.c_str());
            res = PQexec(mycon, copy_sql.c_str());
            if (res) {};


            for (auto i = start; i < limit; ++i) {
                auto way = *it;

                ++count;
                ++it;

                if (way.tag_config().key() == "" || way.tag_config().value() == "") continue;

                std::vector<std::string> common_values;
                common_values.push_back(TO_STR(config.tag_value(way.tag_config()).id()));
                common_values.push_back(TO_STR(way.osm_id()));
                common_values.push_back(way.maxspeed_forward_str() == "-1" ? TO_STR(config.maxspeed_forward(way.tag_config())) : way.maxspeed_forward_str()) ;
                common_values.push_back(way.maxspeed_backward_str() == "-1" ? TO_STR(config.maxspeed_backward(way.tag_config())) : way.maxspeed_backward_str()) ;
                common_values.push_back(way.oneWayType_str());
                common_values.push_back(way.oneWay());
                // common_values.push_back(way.has_attribute("oneway") ? way.get_attribute("oneway") : std::string(""));
                common_values.push_back(TO_STR(config.priority(way.tag_config())));

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
                    PQputline(mycon, tab_separated(values).c_str());
                }
            }

            PQputline(mycon, "\\.\n");
            PQendcopy(mycon);

            print_progress(ways.size(), count);
            process_section(ways_columns, Xaction);
            Xaction.exec("DROP TABLE " + temp_table);
            Xaction.commit();
        } catch (const std::exception &e) {
            std::cerr <<  "\n" << e.what() << std::endl;
            std::cerr << "While processing FROM " << start << "th \t to: " << limit << "th way\n";
            std::cerr << "count" << count << " While processing FROM " << start << "th \t to: " << limit << "th way\n";
        }

        start = limit;
    }
}



void Export2DB::process_section(const std::string &ways_columns, pqxx::work &Xaction) const {
    //  std::cout << "Creating indices in temporary table\n";
    auto temp_table(ways().temp_name());

    Xaction.exec("CREATE INDEX "+ temp_table + "_gdx ON "+ temp_table + " using gist(the_geom);");
    Xaction.exec("CREATE INDEX ON "+ temp_table + "  USING btree (source_osm)");
    Xaction.exec("CREATE INDEX ON "+ temp_table + "  USING btree (target_osm)");




    //  std::cout << "Deleting  duplicated ways FROM temporary table\n";
    std::string delete_from_temp(
            " DELETE FROM "+ temp_table + " a "
            "     USING " + ways().addSchema() + " b "
            "     WHERE a.the_geom ~= b.the_geom AND ST_OrderingEquals(a.the_geom, b.the_geom);");
    Xaction.exec(delete_from_temp);

    //  std::cout << "Updating to existing toplology the temporary table\n";
    fill_source_target(temp_table, vertices().addSchema(), Xaction);

    //  std::cout << "Inserting new vertices in the vertex table\n";
    fill_vertices_table(temp_table, vertices().addSchema(), Xaction);

    //  std::cout << "Updating to new toplology the temporary table\n";
    fill_source_target(temp_table, vertices().addSchema(), Xaction);


    //  std::cout << "Inserting new split ways to '" << addSchema(full_table_name("ways")) << "'\n";
    std::string insert_into_ways(
            " INSERT INTO " + ways().addSchema() +
            "(" + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s) "
            " (SELECT " + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s FROM " + temp_table + "); ");
    auto result = Xaction.exec(insert_into_ways);
    std::cout << "\tSplit ways inserted " << result.affected_rows() << "\n";
}





int64_t
Export2DB::get_val(const std::string sql) const {
#if 0
    std::cout << "\nExecuting: \n" << sql << "\n";
#endif
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);
        auto result = Xaction.exec(sql);
        Xaction.commit();
        if (result.size() == 0) return 0;
        return result[0][0].as<int64_t>();
    } catch (const std::exception &e) {
        std::cout << "\nWARNING: " << e.what() << std::endl;
        std::cout <<  sql << "\n";
    }
    return 0;
}

void
Export2DB::execute(const std::string sql) const {
#if 0
    std::cout << "\nExecuting: \n" << sql << "\n";
#endif
    try {
        pqxx::connection db_conn(conninf);
        pqxx::work Xaction(db_conn);
        Xaction.exec(sql);
        Xaction.commit();
    } catch (const std::exception &e) {
        std::cout << "\nWARNING: " << e.what() << std::endl;
        std::cout <<  sql << "\n";
    }
}



/*
 *
 *  Integrity of the OSM data IS not ensured so failings are ignored
 *
 *  Due to the fact that indexes slow down the process, no index IS created
 *
 *  After all the data IS inserted then its time to create indices & foreign keys
 *
 */
void Export2DB::createFKeys() const {
    /*
     * configuration:
     */
    execute(configuration().primary_key("id"));
    execute(configuration().unique("tag_id"));

    /*
     * vertices
     */
    execute(vertices().primary_key("id"));
    execute(vertices().unique("osm_id"));
    execute(vertices().gist_index());

    /*
     * Ways
     */
    execute(ways().primary_key("gid"));
    execute(ways().foreign_key("source", vertices(), "id"));
    execute(ways().foreign_key("target", vertices(), "id"));
    execute(ways().foreign_key("source_osm", vertices(), "osm_id"));
    execute(ways().foreign_key("target_osm", vertices(), "osm_id"));
    execute(ways().foreign_key("tag_id", configuration(), "tag_id"));
    execute(ways().gist_index());

    /*
     * ponitsOfInterest
     */
    execute(pois().primary_key("pid"));
    execute(pois().gist_index());
    execute(pois().unique("osm_id"));
}


void Export2DB::process_pois() const {
    if (!m_vm.count("addnodes")) return;

    std::cout << "\nAdding functions for processing Points of Interest ..." << endl;
    /* osm2pgr_pois_update_part_of_topology */
    execute(pois().sql(0));


    /* osm2pgr_pois_update_not_part_of_topology */
    execute(pois().sql(1));


    /* osm2pgr_pois_find_side */
    execute(pois().sql(2));


    /* osm2pgr_pois_new_geom */
    execute(pois().sql(3));


    /* osm2pgr_pois_update */
    execute(pois().sql(4));

    std::cout << "\nTo process pointsOfInterest table:\n"
#if 0 //TODO
        << m_schema << (m_schema == "" ? "" :  ".")
#endif
        << "osm2pgr_pois_update(radius default 200, within default 50)\n"
        "\n  - Using areas of (radius)mts on POIS"
        "\n  - Using edges that are at least (within) mts of each POI"
        "\nPOIS that do not have a closest edge is considered as too far\n";


    return;
#if 0
    std::string array;
    int64_t total = 0;
    auto limit = get_val(
            "SELECT count(*) FROM " + pois().addSchema()
            + "\n  WHERE vertex_id IS NULL AND edge_id IS NULL");


    std::cout << "\nFinding closest edge to " << limit << " Points Of Interest\n";
    for (int64_t i = 0; i < limit; ++i) {
        auto curr_tot = get_val(
                "SELECT osm2pgr_pois_update_not_part_of_topology(200, 50, ARRAY[" + array + "]::BIGINT[])");
        total += curr_tot;

        if (curr_tot == 0) {
            auto pid_outOfRange = get_val(" SELECT pid FROM " + pois().addSchema()
                    +"\n  WHERE vertex_id IS NULL AND edge_id IS NULL"
                    +"\n  AND pid not in (SELECT unnest(ARRAY[" + array +"]::BIGINT[]))"
                    +"\n  limit 1;");
            if (pid_outOfRange == 0) break;
            if (array.empty()) {
                array += boost::lexical_cast<std::string>(pid_outOfRange);
            } else {
                array += "," + boost::lexical_cast<std::string>(pid_outOfRange);
            }

            if (get_val(
                    +"SELECT count(*) FROM (SELECT * FROM " + pois().addSchema()
                    +"\n  WHERE vertex_id IS NULL AND edge_id IS NULL"
                    +"\n  AND pid not in (SELECT unnest(ARRAY[" + array +"]::BIGINT[]))) AS a"
                    ) == 0) break;
        }

        print_progress(limit, total);
    }

    if (!array.empty()) {
        std::cout << "\nNo edge found within distance (200 + 50)mts on pid(s): " << array << "\n";
    }

    execute("SELECT osm2pgr_pois_find_side()");
    execute("SELECT osm2pgr_pois_new_geom()");
    execute(
            "\n WITH "
            "\n base AS ("
            "\n     SELECT pid, w.id AS wid, w.the_geom AS wgeom, p.the_geom AS pgeom"
            "\n     FROM " + pois().addSchema() + " AS p JOIN " + ways().addSchema() + " AS w ON (edge_id = w.id)"
            + "\n     WHERE edge_id IS not NULL"
            + "\n ),"

            + "\n foo AS ("
            + "\n     SELECT wid, ST_dumppoints(wgeom) AS dp"
            + "\n     FROM base"
            + "\n ),"

            + "\n blade AS ("
            + "\n     SELECT wid, ST_collect((dp).geom) AS blade"
            + "\n     FROM foo"
            + "\n     GROUP BY wid"
            + "\n ),"

            + "\n split AS ("
            + "\n     SELECT base.*, (ST_Dump(ST_split(wgeom, blade))).geom AS line"
            + "\n     FROM blade JOIN base"
            + "\n     USING (wid)"
            + "\n ),"

            + "\n distance AS ("
            + "\n     SELECT split.*, ST_distance(line, pgeom) AS dist FROM split"
            + "\n ),"

            + "\n second AS ("
            + "\n     SELECT  pid, min(dist) FROM distance GROUP BY pid"
            + "\n ), "

            + "\n last AS ("
            + "\n     SELECT pid,"
            + "\n         (ST_y(ST_startpoint(line)) - ST_y(ST_endpoint(line))) * ST_x(pgeom)"
            + "\n      +  (ST_x(ST_endpoint(line)) - ST_x(ST_startpoint(line))) * ST_y(pgeom)"
            + "\n      +  (ST_x(ST_startpoint(line)) * ST_y(ST_endpoint(line)) - ST_x(ST_endpoint(line))"
            + "\n      * ST_y(ST_startpoint(line))) AS val"
            + "\n     FROM distance join second using (pid) where dist = min"
            + "\n )"

            + "\n UPDATE " + pois().addSchema() + " set side = case when val>0 then 'L' when val<0 then 'R' else 'B' end "
            + "\n FROM last "
            + "\n WHERE last.pid = " + pois().addSchema() + ".pid;"

            );
#endif
}

}  // namespace osm2pgr
