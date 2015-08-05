/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt   								   *
 *   gentoo.murray@gmail.com   											   *
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

#define TO_STR(x)	boost::lexical_cast<std::string>(x)


Export2DB::Export2DB(const  po::variables_map &vm)
: 	mycon(0),
  	conninf ( "host=" + vm["host"].as<std::string>()
        	+ " user=" +  vm["user"].as<std::string>()
	        + " dbname=" + vm["dbname"].as<std::string>()
        	+ " port=" + vm["db_port"].as<std::string>() ),
	tables_prefix( vm["prefix"].as<std::string>() ),
	tables_suffix( vm["suffix"].as<std::string>() ) {

    	if(!vm["passwd"].as<std::string>().empty())
        	this->conninf+=" password=" + vm["passwd"].as<std::string>();

    	create_types = std::string(
           	"id integer PRIMARY KEY,"
           	" name text") ;

        create_way_tag = std::string(

           "type_id integer,"
           " class_id integer,"
           " way_id bigint"
	);
        create_nodes =std::string(

		"id bigserial PRIMARY KEY,"
		" osm_id bigint,"
           	" lon decimal(11,8),"
           	" lat decimal(11,8),"
           	" numOfUse int"
	);
        create_ways =std::string(

             	"gid bigserial,"
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
		" one_way int, "  // 0 unknown, 1 yes(normal direction), 2 (2 way), -1 reversible(1 way but geometry is reversed) 
           	" maxspeed_forward integer,"
           	" maxspeed_backward integer,"
           	" osm_id bigint,"
           	" priority double precision DEFAULT 1"
        );
        create_relations =std::string(

           "relation_id bigint,"
           " type_id integer,"
           " class_id integer,"
           " name text"
        );
        create_relations_ways =std::string(

           "relation_id bigint,"
           " way_id bigint,"
           " type character varying(200)"
       ); 
       create_classes =std::string(

           "id integer PRIMARY KEY,"
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
	//mycon =PQconnectdb("user=postgres dbname=template1 hostaddr=127.0.0.1 port=5432");
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

/*
 CREATE TABLE prefix_table_suffix (
    table_description
 );
*/
void Export2DB::createTable(const std::string &table_description, const std::string &table) const {
       	std::cout << "Creating table: " << table << "\n";
	std::string sql = 
		"CREATE TABLE " + full_table_name(table) + "("    // + schema + "." + prefix etc
		+ table_description + ");";
        PGresult *result = PQexec(mycon, sql.c_str());
        if (PQresultStatus(result) != PGRES_COMMAND_OK) {
        	std::cout << "   Table " << full_table_name(table) << " already exists... skipping."
		        << PQerrorMessage(mycon)
		        << std::endl;
	        PQclear(result);
        } else {
        	std::cout << "   Table " << full_table_name(table) << " created." << std::endl;
	        PQclear(result);
        }
}


void Export2DB::addGeometry(
			 const std::string &table,
			 const std::string &geometry_type) const {
       	std::cout << "Adding Geometry to table: " << table << "\n";
	std::string sql = 
           	+ " SELECT AddGeometryColumn('" 
	 	+ full_table_name(table) + "'," 
           	+ "'the_geom', 4326, '" + geometry_type + "',2 );";

        PGresult *result = PQexec(mycon, sql.c_str());
	if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        	std::cout << "   Something went wrong when adding the geomtery column in Table " << full_table_name(table) << ".\n"
		        << sql //  PQerrorMessage(mycon)
		        << std::endl;
	        PQclear(result);
        } else {
        	std::cout << "   the_geom column added in table " << table <<  std::endl;
	        PQclear(result);
        }
}


///////////////////////
void Export2DB::createTables() {

	createTable( create_classes, "clsses" );
	createTable( create_nodes, "nodes" );
        addGeometry( "nodes", "POINT" );
	createTable( create_ways, "ways" );
        addGeometry( "ways", "LINESTRING" );
	createTable( create_relations, "relations" );
	createTable( create_relations_ways, "relations_ways" );
	createTable( create_way_tag, "way_tag" );
	createTable( create_types, "way_types" );
}



void Export2DB::dropTables()
{
    std::string drop_tables( "DROP TABLE IF EXISTS " + tables_prefix + "ways;"
                            + " DROP TABLE IF EXISTS " + tables_prefix + "nodes;"
                            + " DROP TABLE IF EXISTS " + tables_prefix + "types;"
                            + " DROP TABLE IF EXISTS " + tables_prefix + "classes;"
                            + " DROP TABLE IF EXISTS " + tables_prefix + "way_tag;"
                            + " DROP TABLE IF EXISTS " + tables_prefix + "relations;"
                            + " DROP TABLE IF EXISTS " + tables_prefix + "relation_ways;");
	PGresult *result = PQexec(mycon, drop_tables.c_str());
	PQclear(result);
}

void Export2DB::exportNodes(std::map<long long, Node*>& nodes)
{
	std::map<long long, Node*>::iterator it(nodes.begin());
	std::map<long long, Node*>::iterator last(nodes.end());
    std::string copy_nodes( "COPY " + full_table_name( "nodes" ) + "(osm_id, lon, lat, numofuse, the_geom) FROM STDIN");
	//PGresult* res = PQexec(mycon, tables_prefix.c_str());
	PGresult* res = PQexec(mycon, copy_nodes.c_str());
	PQclear(res);
	while(it!=last)
	{
		Node* node = (*it++).second;
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

void Export2DB::exportRelations(std::vector<Relation*>& relations, Configuration* config)
{
	std::vector<Relation*>::iterator it_relation( relations.begin() );
	std::vector<Relation*>::iterator last_relation( relations.end() );
    std::string copy_relations( "COPY " + full_table_name("relations")+ "(relation_id, type_id, class_id, name) FROM STDIN");
	PGresult* res = PQexec(mycon, copy_relations.c_str());
	PQclear(res);
	while( it_relation!=last_relation )
	{
		Relation* relation = *it_relation++;
		std::map<std::string, std::string>::iterator it_tag( relation->m_Tags.begin() );
		std::map<std::string, std::string>::iterator last_tag( relation->m_Tags.end() );
		while( it_tag!=last_tag )
		{
			std::pair<std::string, std::string> pair = *it_tag++;
			std::string row_data = TO_STR(relation->id);
			row_data += "\t";
			row_data += TO_STR(config->FindType(pair.first)->id);
			row_data += "\t";
			row_data += TO_STR(config->FindClass(pair.first, pair.second)->id);
			row_data += "\t";
			if(!relation->name.empty())
  		{
  			std::string escaped_name = relation->name;
  			boost::replace_all(escaped_name, "\t", "\\\t");
				row_data += escaped_name;
			}
			row_data += "\n";
			PQputline(mycon, row_data.c_str());
		}
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);

	// Second round of iteration is needed to copy relation_ways
	it_relation = relations.begin();
    std::string copy_relation_ways( "COPY " + full_table_name("relation_ways") + "(relation_id, way_id) FROM STDIN");
	res = PQexec(mycon, copy_relation_ways.c_str());
	PQclear(res);
	while( it_relation!=last_relation )
	{
		Relation* relation = *it_relation++;
		std::vector<long long>::iterator it_way( relation->m_WayRefs.begin() );
		std::vector<long long>::iterator last_way( relation->m_WayRefs.end() );

		while( it_way!=last_way )
		{
			long long way_id = *it_way++;
			std::string row_data = TO_STR(relation->id);
			row_data += "\t";
			row_data += TO_STR(way_id);
			row_data += "\n";
			PQputline(mycon, row_data.c_str());
		}
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);
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
void Export2DB::exportWays(std::vector<Way*>& ways, Configuration* config)
{
	std::vector<Way*>::iterator it_way( ways.begin() );
	std::vector<Way*>::iterator last_way( ways.end() );
    std::string copy_way_tag( "COPY " + full_table_name( "way_tag" ) + "(type_id, class_id, way_id) FROM STDIN");
	PGresult* res = PQexec(mycon, copy_way_tag.c_str());
	PQclear(res);
	while( it_way!=last_way )
	{
		Way* way = *it_way++;
		std::map<std::string, std::string>::iterator it_tag( way->m_Tags.begin() );
		std::map<std::string, std::string>::iterator last_tag( way->m_Tags.end() );
		while( it_tag!=last_tag )
		{
			std::pair<std::string, std::string> pair = *it_tag++;
			std::string row_data = TO_STR(config->FindType(pair.first)->id);
			row_data += "\t";
			row_data += TO_STR(config->FindClass(pair.first, pair.second)->id);
			row_data += "\t";
			row_data += TO_STR(way->id);
			row_data += "\n";

			PQputline(mycon, row_data.c_str());
		}
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);

	it_way = ways.begin();
    std::string copy_ways( "COPY " + full_table_name( "ways" ) + "("
                    + "gid,"
                    + " class_id,"
                    + " length,"
                    + " x1,"
                    + " y1,"
                    + " x2,"
                    + " y2,"
                    + " osm_id,"
                    + " the_geom,"
                    + " cost,"
                    + " reverse_cost,"
                    + " one_way,"
                    + " maxspeed_forward,"
                    + " maxspeed_backward,"
                    + " priority,"
                    + " name) FROM STDIN");
	res = PQexec(mycon, copy_ways.c_str());
	while( it_way!=last_way )
	{
		Way* way = *it_way++;
		std::string row_data = TO_STR(way->id); //gid
		row_data += "\t";
		row_data += TO_STR(config->FindClass(way->type, way->clss)->id);
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
		//cout<<row_data<<endl;
		PQputline(mycon, row_data.c_str());
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);
}

void Export2DB::exportTypesWithClasses(std::map<std::string, Type*>& types)
{
	std::map<std::string, Type*>::iterator tIt(types.begin());
	std::map<std::string, Type*>::iterator tLast(types.end());
    std::string copy_types( "COPY " + full_table_name( "types" ) + "(id, name) FROM STDIN");
	PGresult* res = PQexec(mycon, copy_types.c_str());
	PQclear(res);
	while(tIt!=tLast)
	{
		Type* type = (*tIt++).second;
		std::string row_data = TO_STR(type->id);
		row_data += "\t";
		row_data += type->name;
		row_data += "\n";
		PQputline(mycon, row_data.c_str());
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);

	tIt = types.begin();
    std::string copy_classes( "COPY " + full_table_name( "classes" ) + "(id, type_id, name, priority, default_maxspeed) FROM STDIN");
	res = PQexec(mycon, copy_classes.c_str());
	PQclear(res);
	while(tIt!=tLast)
	{
		Type* type = (*tIt++).second;
		std::map<std::string, Class*>& classes= type->m_Classes;
		std::map<std::string, Class*>::iterator cIt(classes.begin());
		std::map<std::string, Class*>::iterator cLast(classes.end());

		while(cIt!=cLast)
		{
			Class* clss = (*cIt++).second;
			std::string row_data = TO_STR(clss->id);
			row_data += "\t";
			row_data += TO_STR(type->id);
			row_data += "\t";
			row_data += clss->name;
			row_data += "\t";
			row_data += TO_STR(clss->priority);
			row_data += "\t";
			row_data += TO_STR(clss->default_maxspeed);
			row_data += "\n";
			PQputline(mycon, row_data.c_str());
		}
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);
}

void Export2DB::createTopology()
{
    bool everything_fine = true;

	PGresult *result;
    std::string create_topology("SELECT pgr_createTopology('"+ full_table_name( "ways" ) + "', 0.00001, 'the_geom', 'gid');");
	result = PQexec(mycon, create_topology.c_str());
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
    	{
        	std::cerr << "Create Topology failed: " << PQerrorMessage(mycon) << std::endl;
        	PQclear(result);
        	everything_fine = false;
	}

    if (everything_fine) {
        std::cout << "Create Topology success" << std::endl;
    }
}
