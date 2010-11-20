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
	result = PQexec(mycon, "CREATE TABLE ways (gid integer, class_id integer not null, length double precision, name char(200), x1 double precision, y1 double precision, x2 double precision,y2 double precision, reverse_cost double precision,rule text, to_cost double precision, osm_id integer); SELECT AddGeometryColumn('ways','the_geom',4326,'MULTILINESTRING',2);");
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

void Export2DB::exportNode(long long id, double lon, double lat, ushort numOfUse )
{
	char tmp_id[20];
	char tmp_lon[15];
	char tmp_lat[15];
	
	sprintf(tmp_id,"%lld",id);
	gcvt(lon,12,tmp_lon);
	gcvt(lat,12,tmp_lat);
	
	std::string query = "INSERT into nodes(id,lon,lat) values(";
				query+= tmp_id;
				query+=",";
				query+= tmp_lon;
				query+=",";
				query+= tmp_lat;
				query+=");";
	
	PGresult *result = PQexec(mycon, query.c_str());
}

void Export2DB::exportRelation(Relation* relation )
{
	
	std::vector<Tag*>::iterator it_tag( relation->m_Tags.begin() );
	std::vector<Tag*>::iterator last_tag( relation->m_Tags.end() );
	
	while( it_tag!=last_tag )
	{
		Tag* pTag = *it_tag++;
		
		std::string query = "INSERT into relations(relation_id, type_id, class_id";
		if(!relation->name.empty()) query+=", name";
    	query+=") values(";

		query+= boost::lexical_cast<std::string>(relation->id) + ",";
		query+= "(SELECT id FROM types WHERE name ='" + boost::lexical_cast<std::string>(pTag->key) + "'),";		
		query+= "(SELECT id FROM classes WHERE name ='" + boost::lexical_cast<std::string>(pTag->value) + "' and type_id = (select id from types where name='"+ boost::lexical_cast<std::string>(pTag->key) + "'))";		
		if(!relation->name.empty()) query+=",$$"+ relation->name +"$$";
    	query+=");";

		PGresult *result2 = PQexec(mycon, query.c_str());
		if (PQresultStatus(result2) != PGRES_COMMAND_OK)
        {
                std::cerr << "insert relations failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
	        std::cerr << "SQL:" << std::endl << query << std::endl;
                PQclear(result2);
        }
	}
	
	/*
	std::string query = "INSERT into relations(relation_id, type) values(";
				query+= boost::lexical_cast<std::string>(relation->id);
				query+=");";
				
	PGresult *result = PQexec(mycon, query.c_str());
		if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "insert into relations failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
	        std::cerr << "SQL:" << std::endl << query << std::endl;
                PQclear(result);
        }
	*/
	
	std::vector<long long>::iterator it_way( relation->m_WayRefs.begin() );
	std::vector<long long>::iterator last_way( relation->m_WayRefs.end() );
	// cout << "Number of tags: " << way->m_Tags.size() << endl;
	// std::cout << "First tag: " << way->m_Tags.front()->key << ":" << way->m_Tags.front()->value << std::endl;

	while( it_way!=last_way )
	{
		
		std::string query = "INSERT into relation_ways(relation_id, way_id) values";

		query+= "(" + boost::lexical_cast<std::string>(relation->id) + ",";		
		query+= boost::lexical_cast<std::string>(*it_way) + ")";
		// std::cout << query << std::endl;
		
		PGresult *result2 = PQexec(mycon, query.c_str());
		if (PQresultStatus(result2) != PGRES_COMMAND_OK)
        {
                std::cerr << "insert relation_ways failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
	        std::cerr << "SQL:" << std::endl << query << std::endl;
                PQclear(result2);
        }
        
        ++it_way;
        
	}
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



void Export2DB::exportWay(Way* way)
{

	std::vector<Tag*>::iterator it_tag( way->m_Tags.begin() );
	std::vector<Tag*>::iterator last_tag( way->m_Tags.end() );
	// cout << "Number of tags: " << way->m_Tags.size() << endl;
	// std::cout << "First tag: " << way->m_Tags.front()->key << ":" << way->m_Tags.front()->value << std::endl;

	while( it_tag!=last_tag )
	{
		Tag* pTag = *it_tag++;
		
		std::string query = "INSERT into way_tag(type_id, class_id, way_id) values(";

		query+= "(SELECT id FROM types WHERE name ='" + boost::lexical_cast<std::string>(pTag->key) + "'),";		
		query+= "(SELECT id FROM classes WHERE name ='" + boost::lexical_cast<std::string>(pTag->value) + "' and type_id = (select id from types where name='"+ boost::lexical_cast<std::string>(pTag->key) + "')),";		
		query+= boost::lexical_cast<std::string>(way->id) + ")";
	// std::cout << query << std::endl;
		
		PGresult *result = PQexec(mycon, query.c_str());
		if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create way_tag failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
	        std::cerr << "SQL:" << std::endl << query << std::endl;
                PQclear(result);
        }
	}
	

	// for( int i = 0; i < way->m_Tags.size(); i++ ) {
    //  cout << "About to insert tag: " << i << " is " << way->m_Tags[i] << endl;
	// }

	std::string query = "INSERT into ways(gid, class_id, length, x1, y1, x2, y2, osm_id, the_geom, reverse_cost";
	if(!way->name.empty())
		query+=", name";
	query+=") values(";
	
	query+=boost::lexical_cast<std::string>(way->id) + 
		", (SELECT id FROM classes WHERE name ='" + boost::lexical_cast<std::string>(way->clss) + "' and type_id = (select id from types where name='"+ boost::lexical_cast<std::string>(way->type) + "'))," 
		+ boost::lexical_cast<std::string>(way->length) + "," 
		 + boost::lexical_cast<std::string>(way->m_NodeRefs.front()->lon) + ","+ boost::lexical_cast<std::string>(way->m_NodeRefs.front()->lat) + ","
		 + boost::lexical_cast<std::string>(way->m_NodeRefs.back()->lon)  + ","+ boost::lexical_cast<std::string>(way->m_NodeRefs.back()->lat) + ","
		+ boost::lexical_cast<std::string>(way->osm_id) + ",";
		 
	query+="GeometryFromText('" + way->geom +"', 4326)";

	if(way->oneway)
	{
	    query+=", "+ boost::lexical_cast<std::string>(way->length*1000000);
	}
	else
	{
	    query+=", "+ boost::lexical_cast<std::string>(way->length);
	}	

	if(!way->name.empty())
		query+=",$$"+ way->name +"$$";
	query+=");";
		//std::cout << query <<std::endl;
	PGresult *result = PQexec(mycon, query.c_str());
	if (PQresultStatus(result) != PGRES_COMMAND_OK)
        {
                std::cerr << "create Ways failed: " 
			<< PQerrorMessage(mycon) 
			<< std::endl;
	        std::cerr << "SQL:" << std::endl << query << std::endl;
                PQclear(result);
        }
}

void Export2DB::exportType(Type* type)
{
	std::string query = "INSERT into types(id, name) values(";
	
	query+=boost::lexical_cast<std::string>(type->id) + ", '" + type->name +"');";
	PGresult *result = PQexec(mycon, query.c_str());
}

void Export2DB::exportClass(Type* type, Class* clss)
{
	std::string query = "INSERT into classes(id, type_id, name) values(";
	
	query+=boost::lexical_cast<std::string>(clss->id) + ", " + boost::lexical_cast<std::string>(type->id) + ", '" + clss->name +"');";
	PGresult *result = PQexec(mycon, query.c_str());
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
