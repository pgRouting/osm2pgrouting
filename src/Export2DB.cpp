/***************************************************************************
 *   Copyright (C) 2025 by Vicky Vergara                                   *
 *   vicky_vergara@hotmail.com                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "stdafx.h"
#include "Export2DB.h"
#include "boost/algorithm/string/replace.hpp"
#include "prog_options.h"
#include <iostream>

#define TO_STR(x)    boost::lexical_cast<std::string>(x)


Export2DB::Export2DB(const  po::variables_map &vm)
:     mycon(0),
      conninf ( "host=" + vm["host"].as<std::string>()
            + " user=" +  vm["user"].as<std::string>()
            + " dbname=" + vm["dbname"].as<std::string>()
            + " port=" + vm["db_port"].as<std::string>() ),
    tables_prefix( vm["prefix"].as<std::string>() ),
    tables_suffix( vm["suffix"].as<std::string>() ) {

        if(!vm["passwd"].as<std::string>().empty())
            this->conninf+=" password=" + vm["passwd"].as<std::string>();

    create_types = std::string(
               " type_id integer PRIMARY KEY,"
               " name text"
    );

    create_way_tag = std::string(
           " class_id integer,"
           " way_id bigint"
    );

    create_nodes =std::string(
        " node_id bigserial PRIMARY KEY,"
        " osm_id bigint,"
        " lon decimal(11,8),"
        " lat decimal(11,8),"
        " numOfUse int"
    );

        create_vertices =std::string(
        " id bigserial PRIMARY KEY,"
        " osm_id bigint,"
        " cnt integer,"
        " chk integer,"
        " ein integer,"
        " eout integer,"
        " lon decimal(11,8),"
        " lat decimal(11,8),"
        " CONSTRAINT vertex_id UNIQUE(osm_id)"
    );
        create_ways =std::string(

            " gid bigserial,"
            " class_id integer not null,"
            " length double precision,"
            " name text,"
            " source bigint,"
            " target bigint,"
            " x1 double precision,"
            " y1 double precision,"
            " x2 double precision,"
            " y2 double precision,"
            " cost double precision,"
            " reverse_cost double precision,"
            " rule text,"
            " one_way int, "  // 0 unknown, 1 yes(normal direction), 2 (2 way), -1 reversed (1 way but geometry is reversed) 
            " maxspeed_forward integer,"
            " maxspeed_backward integer,"
            " osm_id bigint,"
            " source_osm bigint,"
            " target_osm bigint,"
            " priority double precision DEFAULT 1"
        );
        create_relations =std::string(
           "relation_id bigint,"
           " type_id integer,"
           " class_id integer,"
           " name text"
        );
        create_relations_ways =std::string(

           " relation_id bigint,"
           " way_id bigint,"
           " type character varying(200)"
       ); 
       create_classes = std::string(
           " class_id integer PRIMARY KEY,"
           " type_id integer,"
           " name text,"
           " priority double precision,"
           " default_maxspeed integer"
       ); 

}  // constructor

Export2DB::~Export2DB() {
    PQfinish(mycon);
}

int Export2DB::connect() {

    cout << conninf<< endl;
    mycon =PQconnectdb(conninf.c_str());

    ConnStatusType type =PQstatus(mycon);
        if(type==CONNECTION_BAD) {
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
bool Export2DB::createTempTable (const std::string &table_description,
                            const std::string &table) const {
    std::string sql = 
	"CREATE TEMP TABLE " + table + "("    // + schema + "." + prefix etc
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
bool Export2DB::createTable(const std::string &table_description,
                            const std::string &table,
                            const std::string &constraint) const {
    std::string sql = 
	"CREATE TABLE " + table + "("    // + schema + "." + prefix etc
        + table_description + ")";

    PGresult *result = PQexec(mycon, sql.c_str());
    bool created = (PQresultStatus(result) == PGRES_COMMAND_OK);

    std::cout << (created? "Creating '" : " Exists: '") << table <<"': OK\n" ;

    PQclear(result);
    return created;
}


void Export2DB::addGeometry(
             const std::string &table,
             const std::string &geometry_type) const {
           std::cout << "   Adding Geometry: ";
    std::string sql = 
               + " SELECT AddGeometryColumn('" 
         + table + "'," 
               + "'the_geom', 4326, '" + geometry_type + "',2 );";

        PGresult *result = PQexec(mycon, sql.c_str());
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
            std::cout << "   Something went wrong when adding the geomtery column in Table " << table << ".\n"
                << std::endl;
	    throw;
        } else {
            std::cout << "   OK" << std::endl;
        }
    PQclear(result);
}

void Export2DB::addTempGeometry(
             const std::string &table,
             const std::string &geometry_type) const {
    std::string sql = 
               + " SELECT AddGeometryColumn('" 
         + table + "'," 
               + "'the_geom', 4326, '" + geometry_type + "',2 );";

    PGresult *result = PQexec(mycon, sql.c_str());
    PQclear(result);
}


///////////////////////
void Export2DB::createTables() const{
    // the following are particular of the file tables
    if (createTable( create_vertices, full_table_name("ways") + "_vertices_pgr" ))
        addGeometry( full_table_name("ways") + "_vertices_pgr" , "POINT" );
    if (createTable( create_ways, full_table_name("ways") ))
        addGeometry( full_table_name("ways"), "LINESTRING" );
    createTable( create_relations_ways, full_table_name("relations_ways") );

    // the following are general tables
    if ( createTable( create_nodes,
                     "osm_nodes",
                     ", CONSTRAINT node_id UNIQUE(osm_id)") )
        addGeometry( "osm_nodes", "POINT" );
    createTable( create_relations, "osm_relations" );
    createTable( create_way_tag, "osm_way_tags" );
    createTable( create_types, "osm_way_types" );
    createTable( create_classes, "osm_way_classes" );
}



void Export2DB::dropTable(const std::string &table) const {
    std::string drop_tables( "DROP TABLE IF EXIST " +  table);
    PGresult *result = PQexec(mycon, drop_tables.c_str());
    PQclear(result);
}   



void Export2DB::dropTables() const {
    dropTable(full_table_name("ways"));
    dropTable(full_table_name("ways") + "_vertices_pgr");
    dropTable(full_table_name("relations_ways"));

    // we are not deleting general tables osm_
}

/*!

Inserts the data into a temporary table
Inserts the data to the final table only if
**osm_id**
doesnt exist already

*/
void Export2DB::exportNodes(const std::map<long long, Node*> &nodes) const {
    std::cout << "    Processing " <<  nodes.size() <<  " nodes"  << ": ";

    if (createTempTable( create_nodes, "__nodes_temp" ))
       addTempGeometry( "__nodes_temp", "POINT" );

    std::string nodes_columns(" osm_id, lon, lat, numofuse, the_geom " );   
    std::string copy_nodes( "COPY __nodes_temp (" + nodes_columns + ") FROM STDIN");
    PGresult* q_result = PQexec(mycon, copy_nodes.c_str());

    if ( PQresultStatus(q_result) == PGRES_COPY_IN) {
        for (const auto &n : nodes)
        {
            Node* node = n.second;
            std::string row_data = TO_STR(node->id);
            row_data += "\t";
            row_data += TO_STR(node->lon);
            row_data += "\t";
            row_data += TO_STR(node->lat);
            row_data += "\t";
            row_data += TO_STR(node->numsOfUse);
            row_data += "\t";
            row_data += "srid=4326; POINT(" + TO_STR(node->lon) + " " + TO_STR(node->lat) + ")";
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
        PQputline(mycon, "\\.\n");
        PQendcopy(mycon);
    }
    PQclear(q_result);

    std::string insert_into_nodes(
         " WITH data AS ( "
         " SELECT a.* "
         " FROM  __nodes_temp a LEFT JOIN osm_nodes b USING (osm_id) WHERE (b.osm_id IS NULL))"

         " INSERT INTO osm_nodes" 
          "( " + nodes_columns + " ) "
         " (SELECT " + nodes_columns + " FROM data); ");
    q_result = PQexec(mycon, insert_into_nodes.c_str());
    PQclear(q_result);
    std::cout << " Inserted: " << PQcmdTuples(q_result) << "\n";

    dropTable("__nodes_temp");
}

/*!

*/
void Export2DB::fill_vertices_table(const std::string &table, const std::string &node_table) const {
    std::cout << "Filling table '" << table << "_vertices_pgr': ";
    std::string sql(
        "WITH osm_vertex AS ( "
             "(select source_osm as osm_id, x1 as lon, y1 as lat FROM " + table + " where source is NULL)"
             " union "
             "(select target_osm as osm_id, x2 as lon, y2 as lat FROM " + table + " where target is NULL)"
          ") , "
	  " data1 AS (SELECT osm_id, lon, lat,  ST_SetSRID(ST_Point(lon, lat), 4326)  FROM (SELECT DISTINCT * from osm_vertex) a "
          ") "
          " INSERT INTO " + table + "_vertices_pgr (osm_id, lon, lat, the_geom) (SELECT * FROM data1)");

    PGresult* q_result = PQexec(mycon, sql.c_str());
    if (PQresultStatus(q_result) == PGRES_COMMAND_OK) 
         std::cout << " OK " << PQcmdStatus(q_result) << std::endl;
    else
         std::cout  << "   " << PQresultErrorMessage( q_result )  << std::endl;
    PQclear(q_result);
}





void Export2DB::fill_source_target(const std::string &table) const {
    std::cout << "Filling source and target columns of '" << table << "': ";
    std::string sql1(
        " UPDATE " + table + " AS w"
        " SET source = v.id "
        " FROM " + table + "_vertices_pgr AS v"
        " WHERE source is NULL and w.source_osm = v.osm_id;");
    std::string sql2(
        " UPDATE " + table + " AS w"
        " SET target = v.id "
        " FROM " + table + "_vertices_pgr AS v"
        " WHERE target is NULL and w.target_osm = v.osm_id;");

    PGresult* q_result = PQexec(mycon, sql1.c_str());

    std::cout << " Updated: " << PQcmdTuples(q_result) << " rows\n";

    PQclear(q_result);
}


void Export2DB::exportRelations(const std::vector<Relation*> &relations, Configuration *config) const
{
    std::cout << "    Processing " << relations.size() << " relations\n";
    createTempTable( create_relations, "__relations_temp" );
    
    std::string relations_columns("relation_id, type_id, class_id, name " );   
    std::string copy_relations( "COPY __relations_temp (" + relations_columns + ") FROM STDIN");

    
    PGresult* q_result = PQexec(mycon, copy_relations.c_str());
    for (const auto &relation : relations)
    {
std::cout << relation->m_Tags.size();
        for (const auto &tag : relation->m_Tags)
        {
            // std::pair<std::string, std::string> pair = *it_tag++;
            std::string row_data = TO_STR(relation->id);
            row_data += "\t";
            row_data += TO_STR(config->FindType(tag.first)->id);
            row_data += "\t";
            row_data += TO_STR(config->FindClass(tag.first, tag.second)->id);
            row_data += "\t";
            if(!relation->name.empty()) {
                std::string escaped_name = relation->name;
                boost::replace_all(escaped_name, "\t", "\\\t");
                row_data += escaped_name;
            }
            row_data += "\n";
std::cout << row_data << "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);
    std::string insert_into_relations(
         " WITH data AS ( "
         " SELECT a.* "
         " FROM  __relations_temp a LEFT JOIN osm_relations b USING (relation_id, type_id)"
         "     WHERE (b.relation_id IS NULL OR b.type_id IS NULL)"

         " INSERT INTO osm_relations" 
          "( " + relations_columns + " ) "
         " (SELECT " + relations_columns + " FROM data); ");
    q_result = PQexec(mycon, insert_into_relations.c_str());
    std::cout << " Inserted: " << PQcmdTuples(q_result) << "\n";
    PQclear(q_result);
    dropTable("__relations_temp");
}


//////////should break into 2 functions

void Export2DB::exportRelationsWays(const std::vector<Relation*> &relations, Configuration *config) const {
    std::cout << "    Processing way's relations: ";
    createTempTable( create_relations_ways, "__relations_ways_temp" );
    
    std::string relations_ways_columns(" relation_id, way_id " );   
    std::string copy_relations_ways( "COPY __relations_ways_temp (" + relations_ways_columns + ") FROM STDIN");
    PGresult* q_result = PQexec(mycon, copy_relations_ways.c_str());


    for (const auto &relation : relations) {
        for (const auto &way_id : relation->m_WayRefs) {
            std::string row_data = TO_STR(relation->id);
            row_data += "\t";
            row_data += TO_STR(way_id);
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_relations_ways(
         " WITH data AS ( "
         " SELECT a.* "
         " FROM  __relations_ways_temp a LEFT JOIN " + full_table_name("relations_ways") + " b USING (relation_id, way_id)"
         "     WHERE (b.relation_id IS NULL OR b.way_id IS NULL))"

         " INSERT INTO " + full_table_name("relations_ways") +
         " SELECT * FROM data; ");
    q_result = PQexec(mycon, insert_into_relations_ways.c_str());
    std::cout << " Inserted: " << PQcmdTuples(q_result) << "way's relations\n" ;
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

void Export2DB::exportTags(const std::vector<Way*> &ways, Configuration *config) const
{
    std::cout << "    Processing way's tags"  << ": ";

    createTempTable( create_way_tag, "__way_tag_temp"); 
    std::string copy_way_tag( "COPY  __way_tag_temp (class_id, way_id) FROM STDIN");
    PGresult* q_result = PQexec(mycon, copy_way_tag.c_str());
    
    for (const auto &way : ways) {
        for (const auto &tag : way->m_Tags) {
            std::string row_data = TO_STR(config->FindClass(tag.first, tag.second)->id);
            row_data += "\t";
            row_data += TO_STR(way->id);
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_tags(
         " WITH data AS ( "
         " SELECT a.class_id, a.way_id "
         " FROM  __way_tag_temp a LEFT JOIN osm_way_tags b USING ( class_id, way_id ) "
         "     WHERE ( b.class_id IS NULL OR b.way_id IS NULL))"

         " INSERT INTO osm_way_tags "
         " SELECT * FROM data; ");

    q_result = PQexec(mycon, insert_into_tags.c_str());
    std::cout << " Inserted " << PQcmdTuples(q_result) << " way's tags\n";
    PQclear(q_result);

    dropTable("__way_tag_temp");
}



void Export2DB::exportWays(const std::vector<Way*> &ways, Configuration *config) const {

    std::cout << "    Processing " <<  ways.size() <<  " ways"  << ": ";
    if (createTempTable( create_ways, "__ways_temp") )
        addTempGeometry( "__ways_temp", "LINESTRING" );

    std::string ways_columns(
                     " class_id, length,"
                     " x1, y1,"
                     " x2, y2,"
                     " osm_id, source_osm," " target_osm,"
                     " the_geom,"
                     " cost, reverse_cost,"
                     " one_way,"
                     " maxspeed_forward, maxspeed_backward,"
                     " priority,"
                     " name");
    std::string copy_ways( "COPY __ways_temp  ("
                    + ways_columns
                    + " ) FROM STDIN");
     PGresult* q_result = PQexec(mycon, copy_ways.c_str());

    for (const auto &way : ways)
    {
        std::string row_data = TO_STR(config->FindClass(way->type, way->clss)->id);
        row_data += "\t";
        row_data += TO_STR(way->length);
        row_data += "\t";
        row_data += TO_STR(way->m_NodeRefs.front()->lon);
        row_data += "\t";
        row_data += TO_STR(way->m_NodeRefs.front()->lat);
        row_data += "\t";
        row_data += TO_STR(way->m_NodeRefs.back()->lon);
        row_data += "\t";
        row_data += TO_STR(way->m_NodeRefs.back()->lat);
        row_data += "\t";
        row_data += TO_STR(way->osm_id);
        row_data += "\t";
        row_data += TO_STR(way->m_NodeRefs.front()->id);
        row_data += "\t";
        row_data += TO_STR(way->m_NodeRefs.back()->id);
        row_data += "\t";
        row_data += "srid=4326;" + way->geom;
        row_data += "\t";

        //cost based on oneway
        if(way->oneWayType==REVERSED)
            row_data += TO_STR(- way->length);
        else
            row_data += TO_STR(way->length);
        //reverse_cost
        row_data += "\t";
        if(way->oneWayType==YES)
                row_data += TO_STR(- way->length);
        else
            row_data += TO_STR(way->length);

        row_data += "\t";
        row_data += TO_STR(way->oneWayType);
        row_data += "\t";

        //maxspeed
        row_data += TO_STR(way->maxspeed_forward);
        row_data += "\t";
        row_data += TO_STR(way->maxspeed_backward);
        row_data += "\t";

        //priority
        row_data += TO_STR(config->FindClass(way->type,way->clss)->priority);
        row_data += "\t";

        //name
          if(!way->name.empty())
          {
            std::string escaped_name = way->name;
            boost::replace_all(escaped_name, "\\", "");
            boost::replace_all(escaped_name, "\t", "\\\t");
            boost::replace_all(escaped_name, "\n", "");
            boost::replace_all(escaped_name, "\r", "");
            row_data += escaped_name.substr(0,199);
          }
        row_data += "\n";

        PQputline(mycon, row_data.c_str());
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_ways(
         " WITH data AS ( "
         " SELECT a.* "
         " FROM  __ways_temp a LEFT JOIN " + full_table_name("ways") + " b USING (source_osm, target_osm, the_geom) "
         "     WHERE (b.source_osm IS NULL OR target_osm IS NULL OR the_geom IS NULL))"

         " INSERT INTO " + full_table_name("ways") +
          "( " + ways_columns + " ) "
         " (SELECT " + ways_columns + " FROM data); ");


    q_result = PQexec(mycon, insert_into_ways.c_str());
    std::cout << " Inserted " << PQcmdTuples(q_result) << " ways\n";

    PQclear(q_result);
    dropTable("__ways_temp");

}








void Export2DB::exportTypes(const std::map<std::string, Type*> &types)  const
{
    std::cout << "    Processing " << types.size() << " way types: ";

    createTempTable( create_types, "__way_types_temp"); 
    std::string copy_types( "COPY __way_types_temp ( type_id, name ) FROM STDIN");

    PGresult* q_result = PQexec(mycon, copy_types.c_str());

    for ( const auto &e : types)
    {
        Type* type = e.second;
        std::string row_data = TO_STR(type->id);
        row_data += "\t";
        row_data += type->name;
        row_data += "\n";
        PQputline(mycon, row_data.c_str());
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_types(
         " WITH data AS ( "
         " SELECT a.* "
         " FROM  __way_types_temp a LEFT JOIN osm_way_types b USING ( type_id ) "
         "     WHERE (b.type_id IS NULL))"

         " INSERT INTO osm_way_types ( type_id, name ) "
         " (SELECT *  FROM data); ");

    q_result = PQexec(mycon, insert_into_types.c_str());
    std::cout << " Inserted " << PQcmdTuples(q_result) << " way types\n";

    PQclear(q_result);
    dropTable("__way_types_temp");
}






void Export2DB::exportClasses(const std::map<std::string, Type*> &types)  const {
    std::cout << "    Processing way's classes: ";

    std::string copy_classes(
            "COPY __classes_temp"   
            "   (class_id, type_id, name, priority, default_maxspeed)"
            "   FROM STDIN");

    createTempTable( create_classes, "__classes_temp" );
    PGresult *q_result = PQexec(mycon, copy_classes.c_str());

    for ( const auto &t : types) {
        Type type(*t.second);

        for (const auto &c : type.m_Classes ) {
            Class clss(*c.second);
            std::string row_data = TO_STR(clss.id);
            row_data += "\t";
            row_data += TO_STR(type.id);
            row_data += "\t";
            row_data += clss.name;
            row_data += "\t";
            row_data += TO_STR(clss.priority);
            row_data += "\t";
            row_data += TO_STR(clss.default_maxspeed);
            row_data += "\n";
            PQputline(mycon, row_data.c_str());
        }
    }
    PQputline(mycon, "\\.\n");
    PQendcopy(mycon);
    PQclear(q_result);

    std::string insert_into_classes(
         " WITH data AS ( "
         " SELECT a.* "
         " FROM  __classes_temp a LEFT JOIN osm_way_classes b USING ( class_id ) "
         "     WHERE (b.class_id IS NULL))"

         " INSERT INTO osm_way_classes "
         " SELECT *  FROM data; ");

    q_result = PQexec(mycon, insert_into_classes.c_str());
    std::cout << " Inserted " << PQcmdTuples(q_result) << " way's classes\n";

    PQclear(q_result);
    dropTable("__classes_temp");
}

void Export2DB::createTopology() const
{
    fill_source_target( full_table_name( "ways" ) );
    fill_vertices_table( full_table_name( "ways" ), full_table_name( "nodes" ) );
    fill_source_target( full_table_name( "ways" ) );
}
