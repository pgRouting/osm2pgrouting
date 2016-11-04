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


#include "./Export2DB.h"

#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "./print_progress.h"
#include "boost/algorithm/string/replace.hpp"


#include "./prog_options.h"

namespace osm2pgr {

template <typename T>
static
std::string
TO_STR(const T &x) {
    return  boost::lexical_cast<std::string>(x);
}


Export2DB::Export2DB(const  po::variables_map &vm) :
    mycon(0),
    conninf("host=" + vm["host"].as<std::string>()
            + " user=" +  vm["username"].as<std::string>()
            + " dbname=" + vm["dbname"].as<std::string>()
            + " port=" + vm["port"].as<std::string>()),
    tables_schema(vm["schema"].as<std::string>()),
    tables_prefix(vm["prefix"].as<std::string>()),
    tables_suffix(vm["suffix"].as<std::string>()) {
        if (!vm["password"].as<std::string>().empty())
            this->conninf+=" password=" + vm["password"].as<std::string>();

        create_types = std::string(
                " type_id integer PRIMARY KEY,"
                " name text");

        create_way_tag = std::string(
                " class_id integer PRIMARY KEY,"
                " way_id bigint");

        create_nodes = std::string(
                " node_id bigserial PRIMARY KEY,"
                " osm_id bigint,"
                " lon decimal(11,8),"
                " lat decimal(11,8),"
                " numOfUse int");

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
                " maxspeed_forward integer,"
                    " maxspeed_backward integer,"
                    " osm_id bigint,"
                    " source_osm bigint,"
                    " target_osm bigint,"
                    " priority double precision DEFAULT 1");

        create_relations = std::string(
                "relation_id bigint PRIMARY KEY,"
                " type_id integer,"
                " class_id integer,"
                " name text");

        create_relations_ways = std::string(
                " relation_id bigint,"
                " way_id bigint,"
                " type character varying(200)");

        create_classes = std::string(
                " class_id integer PRIMARY KEY,"
                " type_id integer,"
                " name text,"
                " priority double precision,"
                " default_maxspeed integer");
    }  //  constructor

Export2DB::~Export2DB() {
    PQfinish(mycon);
}

int Export2DB::connect() {
    cout << conninf << endl;
    mycon = PQconnectdb(conninf.c_str());

    ConnStatusType type = PQstatus(mycon);
    if (type == CONNECTION_BAD) {
        cout << "connection failed: "<< PQerrorMessage(mycon) << endl;
        return 1;
    } else {
        cout << "connection success"<< endl;
        return 0;
    }
    /***
CONNECTION_STARTED: Waiting for connection to be made.
CONNECTION_MADE: Connection OK; waiting to send.
CONNECTION_AWAITING_RESPONSE: Waiting for a response from the postmaster.
CONNECTION_AUTH_OK: Received authentication; waiting for backend start-up.
CONNECTION_SETENV: Negotiating environment.
     ***/
}


/*!

  CREATE TEMP TABLE table (
  table_description
  );

*/
bool Export2DB::createTempTable(const std::string &table_description,
        const std::string &table) const {
    std::string sql =
        "CREATE TEMP TABLE " + table + "("    //  + schema + "." + prefix etc
        + table_description + ")";


    PGresult *result = PQexec(mycon, sql.c_str());
    bool created = (PQresultStatus(result) == PGRES_COMMAND_OK);

    PQclear(result);
    return created;
}

/*!

  CREATE TABLE table (
  table_description,
  constraint
  );

*/
bool Export2DB::has_postGIS() const {
    std::string sql = "SELECT * FROM pg_extension WHERE extname = 'postgis'";
    PGresult *result = PQexec(mycon, sql.c_str());
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        std::cout << PQresultErrorMessage(result) << "\n";
        throw;
    }
    return  static_cast<bool>(PQntuples(result));
}

bool Export2DB::createTable(const std::string &table_description,
        const std::string &table,
        const std::string &constraint) const {
    std::string sql =
        "CREATE TABLE " + table + "("
        + table_description + constraint + ");";

    PGresult *result = PQexec(mycon, sql.c_str());
    bool created = (PQresultStatus(result) == PGRES_COMMAND_OK);

    std::cout << (created? "Creating '" : " Exists: '") << table <<"': OK\n";

    PQclear(result);
    return created;
}


void Export2DB::addGeometry(
        const std::string &schema, const std::string &table,
        const std::string &geometry_type) const {
    std::cout << "   Adding Geometry: ";
    /** PostGIS requires the schema to be specified as separate arg if not default user's schema **/
    std::string sql =
        + " SELECT AddGeometryColumn(" + (schema == "" ? "" : "'" + schema + "' ,") + " '"
        + table + "',"
        + "'the_geom', 4326, '" + geometry_type + "',2);";


    PGresult *result = PQexec(mycon, sql.c_str());

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        std::cout << PQresultErrorMessage(result) << "<-------\n";
        throw std::string(PQresultErrorMessage(result));
    }
    PQclear(result);
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
    auto ways_name = full_table_name("ways");
    auto vertices_name = ways_name + "_vertices_pgr";

    //  the following are particular of the file tables
    if (createTable(create_vertices, addSchema(vertices_name))) {
        addGeometry(default_tables_schema(), vertices_name, "POINT");
        create_gindex(vertices_name, addSchema(vertices_name));
        create_idindex("osm_id", addSchema(vertices_name));
    }

    if (createTable(create_ways, addSchema(ways_name))) {
        addGeometry(default_tables_schema(), ways_name, "LINESTRING");
        create_gindex(ways_name, addSchema(ways_name));
        create_idindex("source_osm", addSchema(ways_name));
        create_idindex("target_osm", addSchema(ways_name));
        create_idindex("source", addSchema(ways_name));
        create_idindex("target", addSchema(ways_name));
    }

    createTable(create_relations_ways, addSchema(full_table_name("relations_ways")));

    //  the following are general tables
    if (createTable(create_nodes,
                addSchema("osm_nodes"),
                ", CONSTRAINT node_id UNIQUE(osm_id)"))
        addGeometry(default_tables_schema(), "osm_nodes", "POINT");
    createTable(create_relations,  addSchema("osm_relations"));
#if 0
    createTable(create_way_tag, addSchema("osm_way_tags"));
#endif
    createTable(create_types, addSchema("osm_way_types"));
    createTable(create_classes, addSchema("osm_way_classes"));
}



void Export2DB::dropTable(const std::string &table) const {
    std::string drop_tables("DROP TABLE IF EXISTS " +  table);
    PGresult *result = PQexec(mycon, drop_tables.c_str());
    PQclear(result);
}



void Export2DB::dropTables() const {
    auto ways_name = full_table_name("ways");
    auto vertices_name = ways_name + "_vertices_pgr";

    dropTable(addSchema(ways_name));
    dropTable(addSchema(vertices_name));

    dropTable(addSchema(full_table_name("relations_ways")));

    //  we are not deleting general tables osm_
}

/*!

  Inserts the data into a temporary table
  Inserts the data to the final table only if
 **osm_id**
 doesn't exist already

*/
void  Export2DB::prepareExportNodes(const std::string nodes_columns) const {
    if (createTempTable(create_nodes, "__nodes_temp"))
        addTempGeometry("__nodes_temp", "POINT");

    std::string copy_nodes("COPY __nodes_temp (" + nodes_columns + ") FROM STDIN");
    PGresult* q_result = PQexec(mycon, copy_nodes.c_str());
    PQclear(q_result);
}



void Export2DB::exportNodes(const std::map<int64_t, Node> &nodes) const {
    std::cout << "    Processing " <<  nodes.size() <<  " nodes"  << ":\n";
    std::string nodes_columns(" osm_id, lon, lat, numofuse, the_geom ");

    prepareExportNodes(nodes_columns);
    uint32_t chunck_size = 20000;


    int64_t count = 0;
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        auto n = *it;

        if ((count % (chunck_size / 100)) == 0) {
            print_progress(nodes.size(), count);
            std::cout << " Total Processed: " << count;
        }

        if ((++count % chunck_size) == 0) {
            PQputline(mycon, "\\.\n");
            PQendcopy(mycon);
            processSectionExportNodes(nodes_columns);
            prepareExportNodes(nodes_columns);
        }

        auto node = n.second;
        std::string row_data = TO_STR(node.osm_id());
        row_data += "\t";
        row_data += node.geom_str("\t");
        row_data += "\t";
        row_data += TO_STR(node.numsOfUse());
        row_data += "\t";
        row_data += node.point_geometry();
        row_data += "\n";
        PQputline(mycon, row_data.c_str());
    }

    print_progress(nodes.size(), count);
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    processSectionExportNodes(nodes_columns);
    std::cout << "\n";
}


void  Export2DB::processSectionExportNodes(const std::string nodes_columns) const {
    std::string insert_into_nodes(
            " WITH data AS ("
            " SELECT a.* "
            " FROM  __nodes_temp a LEFT JOIN  " + addSchema("osm_nodes") + " b USING (osm_id) WHERE (b.osm_id IS NULL))"

            " INSERT INTO "  + addSchema("osm_nodes") +
            "(" + nodes_columns + ") "
            " (SELECT " + nodes_columns + " FROM data); ");
    PGresult* q_result = PQexec(mycon, insert_into_nodes.c_str());
    PQclear(q_result);
    dropTable("__nodes_temp");
}


/*!

*/
void Export2DB::fill_vertices_table(const std::string &table, const std::string &vertices_tab) const {
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

    PGresult* q_result = PQexec(mycon, sql.c_str());
    std::cout << "Vertices inserted " << PQcmdTuples(q_result);
    PQclear(q_result);
}





void Export2DB::fill_source_target(const std::string &table, const std::string &vertices_tab) const {
    // std::cout << "    Filling 'source' column of '" << table << "': ";
    std::string sql1(
            " UPDATE " + table + " AS w"
            " SET source = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.source is NULL and w.source_osm = v.osm_id;");
    PGresult* q_result = PQexec(mycon, sql1.c_str());
    //     std::cout << " Updated: " << PQcmdTuples(q_result) << " rows\n";
    PQclear(q_result);

    //     std::cout << "    Filling 'target' column of '" << table << "': ";
    std::string sql2(
            " UPDATE " + table + " AS w"
            " SET target = v.id "
            " FROM " + vertices_tab + " AS v"
            " WHERE w.target is NULL and w.target_osm = v.osm_id;");
    q_result = PQexec(mycon, sql2.c_str());
    //     std::cout << " Updated: " << PQcmdTuples(q_result) << " rows\n";
    PQclear(q_result);

    //     std::cout << "    Filling other columns of '" << table << "': ";
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
    q_result = PQexec(mycon, sql3.c_str());
    // std::cout << " Updated: " << PQcmdTuples(q_result) << " rows\n";
    PQclear(q_result);
}


void Export2DB::exportRelations(
        const std::vector<Relation> &relations,
        const Configuration &config) const {
    std::cout << "    Processing " << relations.size() << " relations:";
    createTempTable(create_relations, "__relations_temp");

    std::string relations_columns("relation_id, type_id, class_id, name ");
    std::string copy_relations("COPY __relations_temp (" + relations_columns + ") FROM STDIN");


    PGresult* q_result = PQexec(mycon, copy_relations.c_str());
    for (auto it = relations.begin(); it != relations.end(); ++it) {
        auto relation = *it;

        std::string row_data = TO_STR(relation.osm_id());
        row_data += "\t";
        row_data += TO_STR(config.FindType(relation.tag_config().key()).id());
        row_data += "\t";
        row_data += TO_STR(config.FindClass(relation.tag_config()).id());
        row_data += "\t";
        row_data = row_data + relation.tag_config().key() + "=" + relation.tag_config().value();
        row_data += "\n";
        PQputline(mycon, row_data.c_str());
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);
    std::string insert_into_relations(
            " WITH data AS ("
            " SELECT a.* "
            " FROM  __relations_temp a LEFT JOIN " +  addSchema("osm_relations") + " b USING (relation_id, type_id, class_id)"
            "     WHERE (b.relation_id IS NULL OR b.type_id IS NULL OR b.class_id IS NULL))"

            " INSERT INTO " + addSchema("osm_relations")  + " "
            "(" + relations_columns + ") "
            " (SELECT " + relations_columns + " FROM data); ");
    q_result = PQexec(mycon, insert_into_relations.c_str());
    if (PQresultStatus(q_result) != PGRES_COMMAND_OK) {
        std::cerr << PQresultStatus(q_result);
        std::cerr << "Inserting to osm_relations failed \n"
            << PQerrorMessage(mycon)
            << std::endl;
    } else {
        std::cout << "\tInserted: " << PQcmdTuples(q_result) << " in " << addSchema("osm_relations") << "\n";
    }

    PQclear(q_result);
    dropTable("__relations_temp");
}


// ////////should break into 2 functions

void Export2DB::exportRelationsWays(const std::vector<Relation> &relations, const Configuration &config) const {
    std::cout << "    Processing way's relations: ";
    createTempTable(create_relations_ways, "__relations_ways_temp");

    std::string relations_ways_columns(" relation_id, way_id, type ");
    std::string copy_relations_ways("COPY __relations_ways_temp (" + relations_ways_columns + ") FROM STDIN");
    PGresult* q_result = PQexec(mycon, copy_relations_ways.c_str());


    for (auto it = relations.begin(); it != relations.end(); ++it) {
        auto relation = *it;
        for (auto it_ref = relation.way_refs().begin(); it_ref != relation.way_refs().end(); ++it_ref) {
            auto way_id = *it_ref;
            std::string row_data = TO_STR(relation.osm_id());
            row_data += "\t";
            row_data += TO_STR(way_id);
            row_data += "\t";
            row_data += TO_STR(config.FindType(relation.tag_config().key()).id());
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_relations_ways(
            " WITH data AS ("
            " SELECT a.* "
            " FROM  __relations_ways_temp a LEFT JOIN " + addSchema(full_table_name("relations_ways")) + " b USING (relation_id, way_id)"
            "     WHERE (b.relation_id IS NULL OR b.way_id IS NULL))"

            " INSERT INTO " + addSchema(full_table_name("relations_ways")) +
            " SELECT * FROM data; ");
    q_result = PQexec(mycon, insert_into_relations_ways.c_str());
    std::cout << "\t Inserted: " << PQcmdTuples(q_result) << " in " << addSchema(full_table_name("relations_ways")) << "\n";
    PQclear(q_result);
    dropTable("__relations_ways_temp");
}



/*
   <relation id="147411" version="5" timestamp="2010-01-22T17:02:14Z" uid="24299" user="james_hiebert" changeset="3684904">
   <member type="way" ref="25584788" role=""/>
   <member type="way" ref="35064036" role=""/>
   <member type="way" ref="35064035" role=""/>
   <member type="way" ref="35064037" role=""/>
   <member type="way" ref="35064038" role=""/>
   <member type="way" ref="35065746" role=""/>
   <member type="way" ref="48690404" role=""/>
   <member type="way" ref="24221632" role=""/>
   <tag k="name" v="Mt. Douglas Park Local Connector"/>
   <tag k="network" v="rcn"/>
   <tag k="route" v="bicycle"/>
   <tag k="type" v="route"/>
   </relation>
   */

void Export2DB::exportTags(const std::map<int64_t, Way> &ways, const Configuration &config) const {
    std::cout << "    Processing way's tags"  << ": ";

    createTempTable(create_way_tag, "__way_tag_temp");
    std::string copy_way_tag("COPY  __way_tag_temp (class_id, way_id) FROM STDIN");
    PGresult* q_result = PQexec(mycon, copy_way_tag.c_str());

    for (auto it = ways.begin(); it != ways.end(); ++it) {
        auto way = it->second;
        for (auto it_tag = way.tags().begin(); it_tag != way.tags().end(); ++it_tag) {
            auto tag = *it_tag;
            std::string row_data = TO_STR(config.FindClass(Tag(tag.first, tag.second)).id());
            row_data += "\t";
            row_data += TO_STR(way.osm_id());
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_tags(
            " WITH data AS ("
            " SELECT a.class_id, a.way_id "
            " FROM  __way_tag_temp a LEFT JOIN  " +  addSchema("osm_way_tags") + " b USING (class_id, way_id) "
            "     WHERE (b.class_id IS NULL OR b.way_id IS NULL))"

            " INSERT INTO " +  addSchema("osm_way_tags") +
            " SELECT * FROM data; ");

    q_result = PQexec(mycon, insert_into_tags.c_str());
    std::cout << "\t Inserted: " << PQcmdTuples(q_result) << " in " << addSchema("osm_way_tags") << "\n";
    PQclear(q_result);

    dropTable("__way_tag_temp");
}


void Export2DB::prepare_table(const std::string &ways_columns) const {
    if (createTempTable(create_ways, "__ways_temp")) {
        addTempGeometry("__ways_temp", "LINESTRING");
    } else {
        std::cerr << "could not createTempTable\n";
    }

    std::string copy_ways("COPY __ways_temp  ("
            + ways_columns
            + ") FROM STDIN");

    PGresult* q_result = PQexec(mycon, copy_ways.c_str());
    PQclear(q_result);
}

void Export2DB::exportWays(const std::map<int64_t, Way> &ways, const Configuration &config) const {
    std::cout << "    Processing " <<  ways.size() <<  " ways"  << ":\n";

    std::string separator("\t");
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

    prepare_table(ways_columns);

    uint32_t chunck_size = 20000;
    int64_t count = 0;
    int64_t split_count = 0;
    for (auto it = ways.begin(); it != ways.end(); ++it) {
        auto way = it->second;

        // std::cout << way << "\n";
        if ((count % (chunck_size / 100)) == 0) {
            print_progress(ways.size(), count);
        }
        ++count;
        if (way.tag_config().key() == "" || way.tag_config().value() == "") continue;

        if ((count % chunck_size) == 0) {
            PQputline(mycon, "\\.\n");
            PQendcopy(mycon);
            std::cout << "    Ways Processed: " << count << "\t";
            std::cout << "    Split Ways generated: " << split_count << "\t";
            process_section(ways_columns);
            split_count = 0;
            prepare_table(ways_columns);
        }

        // common information of the split ways
        auto way_data =
            TO_STR(config.FindClass(way.tag_config()).id())  + "\t"
            + TO_STR(way.osm_id()) + "\t"
            // maxspeed
            + way.maxspeed_forward_str() + "\t"
            + way.maxspeed_backward_str() + "\t"
            // one_way
            + way.oneWayType_str() + "\t"
            // priority
            + config.priority_str(way.tag_config()) + "\t";

        // name
        std::string name_data;
        if (!way.name().empty()) {
            std::string escaped_name = way.name();
            boost::replace_all(escaped_name, "\\", "");
            boost::replace_all(escaped_name, "\t", "\\\t");
            boost::replace_all(escaped_name, "\n", "");
            boost::replace_all(escaped_name, "\r", "");
            name_data = escaped_name.substr(0, 199);
        }

        auto splits = way.split_me();
        split_count +=  splits.size();

        for (size_t i = 0; i < splits.size(); ++i) {
            auto length = way.length_str(splits[i]);

            // length (degrees)
            auto split_data = length + "\t"
                // x1, y1
                + splits[i].front()->geom_str("\t") + "\t"
                // x2, y2
                + splits[i].back()->geom_str("\t") + "\t"
                // source_osm
                + TO_STR(splits[i].front()->osm_id()) + "\t"
                // target_osm
                + TO_STR(splits[i].back()->osm_id()) + "\t"
                // geometry
                + "srid=4326;" + way.geometry_str(splits[i]) + "\t";

            // cost based on oneway
            if (way.is_reversed())
                split_data += "-" + length;
            else
                split_data += length;
            split_data += "\t";

            // reverse_cost
            if (way.is_oneway())
                split_data += "-" + length;
            else
                split_data += length;
            split_data += "\t";

            split_data =  way_data + split_data + name_data + "\n";
            PQputline(mycon, split_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    std::cout << "    Ways Processed: " << count << "\t";
    std::cout << "    Split Ways generated: " << split_count << "\t";
    process_section(ways_columns);
}

void Export2DB::process_section(const std::string &ways_columns) const {
    auto ways_name = full_table_name("ways");
    auto vertices_name = ways_name + "_vertices_pgr";

    //  std::cout << "Creating indices in temporary table\n";
    create_gindex("__ways_temp", "__ways_temp");
    create_idindex("source_osm", "__ways_temp");
    create_idindex("target_osm", "__ways_temp");

    //  std::cout << "Deleting  duplicated ways from temporary table\n";
    std::string delete_from_temp(
            " DELETE FROM __ways_temp a "
            "     USING " + addSchema(ways_name) + " b "
            "     WHERE a.the_geom ~= b.the_geom AND ST_OrderingEquals(a.the_geom, b.the_geom);");

    PGresult* q_result = PQexec(mycon, delete_from_temp.c_str());
    //  std::cout << "     Deleted " << PQcmdTuples(q_result) << " duplicated ways from temporary table\n";
    PQclear(q_result);

    //  std::cout << "Updating to existing toplology the temporary table\n";
    fill_source_target("__ways_temp" , addSchema(vertices_name));

    //  std::cout << "Inserting new vertices in the vertex table\n";
    fill_vertices_table("__ways_temp" , addSchema(vertices_name));

    //  std::cout << "Updating to new toplology the temporary table\n";
    fill_source_target("__ways_temp" , addSchema(vertices_name));


    //  std::cout << "Inserting new split ways to '" << addSchema(full_table_name("ways")) << "'\n";
    std::string insert_into_ways(
            " INSERT INTO " + addSchema(ways_name) +
            "(" + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s) "
            " (SELECT " + ways_columns + ", source, target, length_m, cost_s, reverse_cost_s FROM __ways_temp); ");
    q_result = PQexec(mycon, insert_into_ways.c_str());
    std::cout << " Inserted " << PQcmdTuples(q_result) << " split ways\n";
    PQclear(q_result);
    dropTable("__ways_temp");
}







void Export2DB::exportTypes(const std::map<std::string, Type> &types)  const {
    std::cout << "    Processing " << types.size() << " way types: ";

    createTempTable(create_types, "__way_types_temp");
    std::string copy_types("COPY __way_types_temp (type_id, name) FROM STDIN");

    PGresult* q_result = PQexec(mycon, copy_types.c_str());

    for (auto it = types.begin(); it != types.end(); ++it) {
        auto e = *it;
        auto type = e.second;
        std::string row_data = TO_STR(type.id());
        row_data += "\t";
        row_data += type.name();
        row_data += "\n";
        PQputline(mycon, row_data.c_str());
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_types(
            " WITH data AS ("
            " SELECT a.* "
            " FROM  __way_types_temp a LEFT JOIN  " + addSchema("osm_way_types") + " b USING (type_id) "
            "     WHERE (b.type_id IS NULL))"

            " INSERT INTO "  + addSchema("osm_way_types") + " (type_id, name) "
            " (SELECT *  FROM data); ");

    q_result = PQexec(mycon, insert_into_types.c_str());
    std::cout << "\t Inserted: " << PQcmdTuples(q_result) << " in " << addSchema("osm_way_types") << "\n";

    PQclear(q_result);
    dropTable("__way_types_temp");
}






void Export2DB::exportClasses(const std::map<std::string, Type> &types)  const {
    std::cout << "    Processing way's classes: ";

    std::string copy_classes(
            "COPY __classes_temp"
            "   (class_id, type_id, name, priority, default_maxspeed)"
            "   FROM STDIN");

    createTempTable(create_classes, "__classes_temp");
    PGresult *q_result = PQexec(mycon, copy_classes.c_str());

    for (auto it = types.begin(); it != types.end(); ++it) {
        auto t = *it;
        auto type(t.second);

        for (auto it_c = type.classes().begin(); it_c != type.classes().end(); ++it_c) {
            auto c = *it_c;
            Class clss(c.second);
            std::string row_data = TO_STR(clss.id());
            row_data += "\t";
            row_data += TO_STR(type.id());
            row_data += "\t";
            row_data += clss.name();
            row_data += "\t";
            row_data += TO_STR(clss.priority());
            row_data += "\t";
            row_data += TO_STR(clss.default_maxspeed());
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_classes(
            " WITH data AS ("
            " SELECT a.* "
            " FROM  __classes_temp a LEFT JOIN " + addSchema("osm_way_classes") + " b USING (class_id) "
            "     WHERE (b.class_id IS NULL))"

            " INSERT INTO " + addSchema("osm_way_classes") +
            " SELECT *  FROM data; ");

    q_result = PQexec(mycon, insert_into_classes.c_str());
    std::cout << "\t Inserted: " << PQcmdTuples(q_result) << " in " << addSchema("osm_way_classes") << "\n";

    PQclear(q_result);
    dropTable("__classes_temp");
}


void Export2DB::createFKeys() {
    auto ways_name = full_table_name("ways");
    auto vertices_name = ways_name + "_vertices_pgr";


    std::string fk_classes(
            "ALTER TABLE " + addSchema("osm_way_classes")  + " ADD  FOREIGN KEY (type_id) REFERENCES " +  addSchema("osm_way_types")  + "(type_id)");
    PGresult *result = PQexec(mycon, fk_classes.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        std::cerr << PQresultStatus(result);
        std::cerr << "foreign keys for " + addSchema("osm_way_classes")  + " failed:"
            << PQerrorMessage(mycon)
            << std::endl;
        PQclear(result);
    } else {
        std::cout << "Foreign keys for " + addSchema("osm_way_classes")  + " table created" << std::endl;
    }


    std::string fk_relations(
            "ALTER TABLE " + addSchema(full_table_name("relations_ways"))  + " ADD FOREIGN KEY (relation_id) REFERENCES " + addSchema("osm_relations") + "(relation_id); ");
    result = PQexec(mycon, fk_relations.c_str());
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
            "ALTER TABLE " + addSchema(ways_name) + " ADD FOREIGN KEY (class_id) REFERENCES " + addSchema("osm_way_classes") + "(class_id);" +
            "ALTER TABLE " + addSchema(ways_name) + " ADD FOREIGN KEY (source) REFERENCES " + addSchema(vertices_name) + "(id); " +
            "ALTER TABLE " + addSchema(ways_name) + " ADD FOREIGN KEY (target) REFERENCES " + addSchema(vertices_name) + "(id);");
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
