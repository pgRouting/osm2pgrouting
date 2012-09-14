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

#define TO_STR(x)	boost::lexical_cast<std::string>(x)

using namespace std;

Export2DB::Export2DB(std::string host, std::string user, std::string dbname, std::string port, std::string passwd)
:mycon(0)
{
	
this->conninf="host="+host+" user="+user+" dbname="+ dbname +" port="+port;
if(!passwd.empty())
	this->conninf+=" password="+passwd;
	
}

Export2DB::~Export2DB()
{
	PQfinish(mycon);
}

int Export2DB::connect()
{
	cout << conninf<< endl;
	//mycon =PQconnectdb("user=postgres dbname=template1 hostaddr=127.0.0.1 port=5432");
	mycon =PQconnectdb(conninf.c_str());
	
	ConnStatusType type =PQstatus(mycon);
		if(type==CONNECTION_BAD)
		{
			cout << "connection failed"<< endl;
			return 1;
		}
		else
		{
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


void Export2DB::createTables()
{
	PGresult *result = PQexec(mycon, "CREATE TABLE nodes (ID integer PRIMARY KEY,  lon decimal(11,8), lat decimal(11,8), numOfUse smallint);");
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create Nodes failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
                PQclear(result);
        }

	std::cout << "Nodes table created" << std::endl;
	result = PQexec(mycon, "CREATE TABLE ways (gid integer, class_id integer not null, length double precision, name char(200), x1 double precision, y1 double precision, x2 double precision,y2 double precision, reverse_cost double precision,rule text, to_cost double precision, osm_id integer); SELECT AddGeometryColumn('ways','the_geom',4326,'LINESTRING',2);");
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
				 std::cerr << PQresultStatus(result);
                std::cerr << "create ways failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
                PQclear(result);
        } else {
		std::cout << "Ways table created" << std::endl;
	}
	result = PQexec(mycon, "CREATE TABLE types (id integer, name char(200));");
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create types failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
                PQclear(result);
        } else {
		std::cout << "Types table created" << std::endl;
	}
	
	result = PQexec(mycon, "CREATE TABLE way_tag (type_id integer, class_id integer, way_id integer);");
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create way_tag failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
                PQclear(result);
        } else {
		std::cout << "Way_tag table created" << std::endl;	
	}

	result = PQexec(mycon, "CREATE TABLE relations (relation_id integer, type_id integer, class_id integer, name character varying(200));");
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create relations failed: "
            << PQerrorMessage(mycon)
            << std::endl;
                PQclear(result);
        } else {
        	std::cout << "Relations table created" << std::endl;
    	}
    	
    result = PQexec(mycon, "CREATE TABLE relation_ways (relation_id integer, way_id integer, type character varying(200));");
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create relation_ways failed: "
            << PQerrorMessage(mycon)
            << std::endl;
                PQclear(result);
        } else {
        	std::cout << "Relation_ways table created" << std::endl;
    	}
	
	result = PQexec(mycon, "CREATE TABLE classes (id integer, type_id integer, name char(200), cost double precision);");
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create classes failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
                PQclear(result);
        } else {
		std::cout << "Classes table created" << std::endl;	
	}
}

void Export2DB::dropTables()
{
	PGresult *result = PQexec(mycon, "DROP TABLE ways; DROP TABLE nodes; DROP TABLE types; DROP TABLE classes; DROP TABLE way_tag; DROP TABLE relations; DROP TABLE relation_ways;");
}

void Export2DB::exportNodes(std::map<long long, Node*>& nodes)
{	
	std::map<long long, Node*>::iterator it(nodes.begin());
	std::map<long long, Node*>::iterator last(nodes.end());
	PGresult* res = PQexec(mycon, "COPY nodes(id, lon, lat) from STDIN");
	PQclear(res);
	while(it!=last)
	{
		Node* node = (*it++).second;
		std::string row_data = TO_STR(node->id);
		row_data += "\t";
		row_data += TO_STR(node->lon);
		row_data += "\t";
		row_data += TO_STR(node->lat);
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
	PGresult* res = PQexec(mycon, "COPY relations(relation_id, type_id, class_id, name) from STDIN");
	PQclear(res);
	while( it_relation!=last_relation )
	{
		Relation* relation = *it_relation++;
		std::vector<Tag*>::iterator it_tag( relation->m_Tags.begin() );
		std::vector<Tag*>::iterator last_tag( relation->m_Tags.end() );
		while( it_tag!=last_tag )
		{
			Tag* tag = *it_tag++;
			std::string row_data = TO_STR(relation->id);
			row_data += "\t";
			row_data += TO_STR(config->FindType(tag->key)->id);
			row_data += "\t";
			row_data += TO_STR(config->FindClass(tag->key, tag->value)->id);
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
	res = PQexec(mycon, "COPY relation_ways(relation_id, way_id) from STDIN");
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
	PGresult* res = PQexec(mycon, "COPY way_tag(type_id, class_id, way_id) from STDIN");
	PQclear(res);
	while( it_way!=last_way )
	{
		Way* way = *it_way++;
		std::vector<Tag*>::iterator it_tag( way->m_Tags.begin() );
		std::vector<Tag*>::iterator last_tag( way->m_Tags.end() );
		while( it_tag!=last_tag )
		{
			Tag* tag = *it_tag++;
			std::string row_data = TO_STR(config->FindType(tag->key)->id);
			row_data += "\t";
			row_data += TO_STR(config->FindClass(tag->key, tag->value)->id);
			row_data += "\t";
			row_data += TO_STR(way->id);
			row_data += "\n";

			PQputline(mycon, row_data.c_str());
		}
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);
	
	it_way = ways.begin();
	res = PQexec(mycon, "COPY ways(gid, class_id, length, x1, y1, x2, y2, osm_id, the_geom, reverse_cost, name) from STDIN");
	while( it_way!=last_way )
	{
		Way* way = *it_way++;
		std::string row_data = TO_STR(way->id);
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
		if(way->oneway)
	    	row_data += TO_STR(way->length*1000000);
		else
			row_data += TO_STR(way->length);
		row_data += "\t";
	  	if(!way->name.empty())
	  	{
        std::string escaped_name = way->name;
        boost::replace_all(escaped_name, "\t", "\\\t");
        boost::replace_all(escaped_name, "\n", "");
        boost::replace_all(escaped_name, "\r", "");
        boost::replace_all(escaped_name, "\\", "");
        row_data += escaped_name.substr(0,199);
	  	}
	  	row_data += "\n";
		PQputline(mycon, row_data.c_str());
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);
}
void Export2DB::exportTypesWithClasses(std::map<std::string, Type*>& types)
{
	std::map<std::string, Type*>::iterator tIt(types.begin());
	std::map<std::string, Type*>::iterator tLast(types.end());
	PGresult* res = PQexec(mycon, "COPY types(id, name) from STDIN");
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
	res = PQexec(mycon, "COPY classes(id, type_id, name) from STDIN");
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
			row_data += "\n";
			PQputline(mycon, row_data.c_str());
		}
	}
	PQputline(mycon, "\\.\n");
	PQendcopy(mycon);
}

void Export2DB::createTopology()
{
	PGresult *result = PQexec(mycon,"ALTER TABLE ways ADD COLUMN source integer;");
	result = PQexec(mycon,"ALTER TABLE ways ADD COLUMN target integer;");
	result = PQexec(mycon,"CREATE INDEX source_idx ON ways(source);");
	result = PQexec(mycon,"CREATE INDEX target_idx ON ways(target);");
    result = PQexec(mycon,"CREATE INDEX geom_idx ON ways USING GIST(the_geom GIST_GEOMETRY_OPS);");
	result = PQexec(mycon,"CREATE UNIQUE INDEX ways_gid_idx ON ways(gid);");
	result = PQexec(mycon,"SELECT assign_vertex_id('ways', 0.00001, 'the_geom', 'gid');");
	
}
