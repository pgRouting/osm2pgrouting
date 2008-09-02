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
	std::cout << "Nodes table created" << std::endl;
	result = PQexec(mycon, "CREATE TABLE ways (gid integer, class_id integer, length double precision, name char(200), x1 double precision, y1 double precision, x2 double precision,y2 double precision, reverse_cost double precision,rule text, to_cost double precision, PRIMARY KEY(gid)); SELECT AddGeometryColumn('ways','the_geom',4326,'MULTILINESTRING',2);");
	std::cout << "Ways table created" << std::endl;
	result = PQexec(mycon, "CREATE TABLE types (id integer, name char(200));");
	std::cout << "Types table created" << std::endl;
	result = PQexec(mycon, "CREATE TABLE classes (id integer, type_id integer, name char(200), cost double precision);");
	std::cout << "Classes table created" << std::endl;	
}

void Export2DB::dropTables()
{
	PGresult *result = PQexec(mycon, "DROP TABLE ways; DROP TABLE nodes; DROP TABLE types; DROP TABLE classes;");
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

void Export2DB::exportWay(Way* way)
{
	std::string query = "INSERT into ways(gid, class_id, length, x1, y1, x2, y2, the_geom, reverse_cost";
	if(!way->name.empty())
		query+=", name";
	query+=") values(";
	
	query+=boost::lexical_cast<std::string>(way->id) + ", (SELECT id FROM classes WHERE name ='" + boost::lexical_cast<std::string>(way->clss) + "')," + boost::lexical_cast<std::string>(way->length) + "," 
		 + boost::lexical_cast<std::string>(way->m_NodeRefs.front()->lon) + ","+ boost::lexical_cast<std::string>(way->m_NodeRefs.front()->lat) + ","
		 + boost::lexical_cast<std::string>(way->m_NodeRefs.back()->lon)  + ","+ boost::lexical_cast<std::string>(way->m_NodeRefs.back()->lat) + ",";
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
	result = PQexec(mycon,"SELECT assign_vertex_id('ways', 0.00001, 'the_geom', 'gid');");
	
}
