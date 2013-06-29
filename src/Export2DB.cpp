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

Export2DB::Export2DB(std::string host, std::string user, std::string dbname, std::string port, std::string passwd, std::string tables_prefix)
:mycon(0)
{
    this->conninf="host="+host+" user="+user+" dbname="+ dbname +" port="+port;
    this->tables_prefix = tables_prefix;
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
    std::string create_nodes("CREATE TABLE " + tables_prefix + "nodes (id bigint PRIMARY KEY, lon decimal(11,8), lat decimal(11,8), numOfUse smallint);");
	PGresult *result = PQexec(mycon, create_nodes.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "create Nodes failed: "
        << PQerrorMessage(mycon)
        << std::endl;
        PQclear(result);
    } else {
        std::cout << "Nodes table created" << std::endl;
    }

	// gid cannot be "bigint" right now because pgRouting doesn't support "bigint"
    std::string create_ways("CREATE TABLE " + tables_prefix + "ways (gid integer, class_id integer not null, length double precision, name text, x1 double precision, y1 double precision, x2 double precision, y2 double precision, reverse_cost double precision, rule text, to_cost double precision, maxspeed_forward integer, maxspeed_backward integer, osm_id bigint, priority double precision DEFAULT 1);"
            + " SELECT AddGeometryColumn('" + tables_prefix + "ways','the_geom',4326,'LINESTRING',2);");
	result = PQexec(mycon, create_ways.c_str());
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

    std::string create_types("CREATE TABLE " + tables_prefix + "types (id integer PRIMARY KEY, name text);");
	result = PQexec(mycon, create_types.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "create types failed: "
        << PQerrorMessage(mycon)
        << std::endl;
        PQclear(result);
	} else {
        std::cout << "Types table created" << std::endl;
    }

    std::string create_way_tag("CREATE TABLE " + tables_prefix + "way_tag (type_id integer, class_id integer, way_id bigint);");
	result = PQexec(mycon, create_way_tag.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "create way_tag failed: "
        << PQerrorMessage(mycon)
        << std::endl;
        PQclear(result);
	} else {
        std::cout << "Way_tag table created" << std::endl;
    }

    std::string create_relations("CREATE TABLE " + tables_prefix + "relations (relation_id bigint, type_id integer, class_id integer, name text);");
	result = PQexec(mycon, create_relations.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "create relations failed: "
        << PQerrorMessage(mycon)
        << std::endl;
        PQclear(result);
    } else {
        std::cout << "Relations table created" << std::endl;
    }

    std::string create_relations_ways("CREATE TABLE " + tables_prefix + "relation_ways (relation_id bigint, way_id bigint, type character varying(200));");
    result = PQexec(mycon, create_relations_ways.c_str());
    if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "create relation_ways failed: "
        << PQerrorMessage(mycon)
        << std::endl;
        PQclear(result);
    } else {
        std::cout << "Relation_ways table created" << std::endl;
    }

    std::string create_classes("CREATE TABLE " + tables_prefix + "classes (id integer PRIMARY KEY, type_id integer, name text, cost double precision, priority double precision, default_maxspeed integer);");
	result = PQexec(mycon, create_classes.c_str());
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
    std::string drop_tables( "DROP TABLE " + tables_prefix + "ways;"
                            + " DROP TABLE " + tables_prefix + "nodes;"
                            + " DROP TABLE " + tables_prefix + "types;"
                            + " DROP TABLE " + tables_prefix + "classes;"
                            + " DROP TABLE " + tables_prefix + "way_tag;"
                            + " DROP TABLE " + tables_prefix + "relations;"
                            + " DROP TABLE " + tables_prefix + "relation_ways;");
	PGresult *result = PQexec(mycon, drop_tables.c_str());
}

void Export2DB::exportNodes(std::map<long long, Node*>& nodes)
{
	std::map<long long, Node*>::iterator it(nodes.begin());
	std::map<long long, Node*>::iterator last(nodes.end());
    std::string copy_nodes( "COPY " + tables_prefix + "nodes(id, lon, lat, numofuse) FROM STDIN");
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
    std::string copy_relations( "COPY " + tables_prefix + "relations(relation_id, type_id, class_id, name) FROM STDIN");
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
    std::string copy_relation_ways( "COPY " + tables_prefix + "relation_ways(relation_id, way_id) FROM STDIN");
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
    std::string copy_way_tag( "COPY " + tables_prefix + "way_tag(type_id, class_id, way_id) FROM STDIN");
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
    std::string copy_ways( "COPY " + tables_prefix + "ways(gid, class_id, length, x1, y1, x2, y2, osm_id, the_geom, reverse_cost, maxspeed_forward, maxspeed_backward, priority, name) FROM STDIN");
	res = PQexec(mycon, copy_ways.c_str());
	while( it_way!=last_way )
	{
		Way* way = *it_way++;
		std::string row_data = TO_STR(way->id);
		row_data += "\t";
		row_data += TO_STR(config->FindClass(way->type, way->clss)->id);
		row_data += "\t";
		//row_data += TO_STR(way->length);
		//length based on oneway
		if(way->oneWayType==REVERSED)
			row_data += TO_STR(way->length*1000000);
		else
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

		//reverse_cost
		if(way->oneWayType==YES)
	    	row_data += TO_STR(way->length*1000000);
		else
			row_data += TO_STR(way->length);

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
    std::string copy_types( "COPY " + tables_prefix + "types(id, name) FROM STDIN");
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
    std::string copy_classes( "COPY " + tables_prefix + "classes(id, type_id, name, priority, default_maxspeed) FROM STDIN");
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

    std::string alter_ways_source( "ALTER TABLE " + tables_prefix + "ways ADD COLUMN source integer;");
	PGresult *result = PQexec(mycon, alter_ways_source.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "Alter table add source failed: " << PQerrorMessage(mycon) << std::endl;
        PQclear(result);
        everything_fine = false;
	}

    std::string alter_ways_target( "ALTER TABLE " + tables_prefix + "ways ADD COLUMN target integer;");
	result = PQexec(mycon, alter_ways_target.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "Alter table add target failed: " << PQerrorMessage(mycon) << std::endl;
        PQclear(result);
        everything_fine = false;
	}

    std::string source_idx("CREATE INDEX " + tables_prefix + "source_idx ON " + tables_prefix + "ways(source);");
	result = PQexec(mycon, source_idx.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "Create index add on source failed: " << PQerrorMessage(mycon) << std::endl;
        PQclear(result);
        everything_fine = false;
	}

    std::string target_idx("CREATE INDEX " + tables_prefix + "target_idx ON " + tables_prefix + "ways(target);");
	result = PQexec(mycon, target_idx.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "Create index add on target failed: " << PQerrorMessage(mycon) << std::endl;
        PQclear(result);
        everything_fine = false;
	}

    std::string geom_idx("CREATE INDEX " + tables_prefix + "geom_idx ON " + tables_prefix + "ways USING GIST (the_geom);");
    result = PQexec(mycon, geom_idx.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "Create geom index on ways failed: " << PQerrorMessage(mycon) << std::endl;
        PQclear(result);
        everything_fine = false;
	}

    std::string ways_gid_idx("CREATE UNIQUE INDEX "+ tables_prefix + "ways_gid_idx ON "+ tables_prefix + "ways(gid);");
	result = PQexec(mycon, ways_gid_idx.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
    {
        std::cerr << "Create unique index on ways failed: " << PQerrorMessage(mycon) << std::endl;
        PQclear(result);
        everything_fine = false;
	}

    std::string create_topology("SELECT pgr_createTopology('"+ tables_prefix + "ways', 0.00001, 'the_geom', 'gid');");
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
