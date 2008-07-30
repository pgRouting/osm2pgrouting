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
#include "OSMDocument.h"
#include "OSMDocumentParserCallback.h"
#include "Way.h"
#include "Node.h"
#include "Export2DB.h"

using namespace osm;
using namespace xml;
using namespace std;

void _error()
{
				cout << "following params are required: " << endl;
				cout << "-file <file>  -- name of your osm xml file" << endl;
				cout << "-dbname <dbname> -- name of your database" << endl;
				cout << "-user <user> -- name of the user, which have write access to the database" << endl;
				cout << "optional:" << endl;
				cout << "-host <host>  -- host of your postgresql database (default: 127.0.0.1)" << endl;
				cout << "-port <port> -- port of your database (default: 5432)" << endl;
				cout << "-passwd <passwd> --  password for database access" << endl;
					
}

int main(int argc, char* argv[])
{
	std::string file;
	std::string host="127.0.0.1";
	std::string user;
	std::string port="5432";
	std::string dbname;
	std::string passwd;
	if(argc >=7 && argc <=13)
	{
		int i=1;
		while( i<argc)
		{
			
			if(strcmp(argv[i],"-file")==0)
			{	
				i++;
				file = argv[i];
			}
				
			else if(strcmp(argv[i],"-host")==0)
			{
				i++;
				host = argv[i];
			}
			else if(strcmp(argv[i],"-dbname")==0)
			{
				i++;
				dbname = argv[i];
			}
			else if(strcmp(argv[i],"-user")==0)
			{
				i++;
				user = argv[i];
			}
			else if(strcmp(argv[i],"-port")==0)
			{
				i++;
				port = argv[i];
			}
			else if(strcmp(argv[i],"-passwd")==0)
			{
				i++;
				passwd = argv[i];
			}
			else
			{
				cout << "unknown paramer: " << argv[i] << endl;
				_error();
				return 1;
			}
			
			i++;
		}
		
	}
	else
	{
		_error();
		return 1;
	}
	
	if(file.empty() || dbname.empty() || user.empty())
	{
		_error();
		return 1;
	}
	
	Export2DB test(host, user, dbname, port, passwd);
	if(test.connect()==1)
		return 1;

	//std::string fileName = "../data/oldenburg.osm";
	OSMDocument document;
    OSMDocumentParserCallback callback( document );

	XMLParser parser;
	int ret = parser.Parse( callback, file.c_str() );
	if( ret!=0 ) return 1;

	document.SplitWays();
	//############# Export2DB
	{

		test.createTables();
		std::map<long long, Node*>& nodes= document.m_Nodes;
		std::map<long long, Node*>::iterator it(nodes.begin());
		std::map<long long, Node*>::iterator last(nodes.end());
		
		while(it!=last)
		{
			Node* node = (*it++).second;
			test.exportNode(node->id,node->lon, node->lat, node->numsOfUse);
		}
		
		std::vector<Way*>& ways= document.m_SplittedWays;
		std::vector<Way*>::iterator it_way( ways.begin() );
		std::vector<Way*>::iterator last_way( ways.end() );	
		while( it_way!=last_way )
		{
			Way* pWay = *it_way++;
			test.exportWay(pWay);
		}
		cout << "create topology" << endl;
		test.createTopology();
	}	
	
	//#############
	
	/*
	std::vector<Way*>& ways= document.m_Ways;
	std::vector<Way*>::iterator it( ways.begin() );
	std::vector<Way*>::iterator last( ways.end() );
	while( it!=last )
	{
		Way* pWay = *it;
		
		if( !pWay->name.empty() )
		{
			if( pWay->m_NodeRefs.empty() )
			{
				std::cout << pWay->name.c_str() << endl;
			}
			else
			{
				Node* n0 = pWay->m_NodeRefs.front();
				Node* n1 = pWay->m_NodeRefs.back();
				//if(n1->numsOfUse==1)
				//cout << "way-id: " << pWay->id << " name: " << pWay->name <<endl; 
				//std::cout << n0->lon << " "  << n0->lat << " " << n1->lon << " " << n1->lat << " " << pWay->name.c_str() << " highway: " << pWay->highway.c_str() << " Start numberOfUse: " << n0->numsOfUse << " End numberOfUse: " << n1->numsOfUse  << " ID: " << n1->id <<  endl;
			}
		}
		if( pWay->id == 20215432 ) // Pfaenderweg
		{
			cout << pWay->name << endl;
			int a=4;
		}
		++it;
	}
	*/
	
	cout << "#########################" << endl;
	
	cout << "size of streets: " << document.m_Ways.size() <<	endl;
	cout << "size of splitted ways : " << document.m_SplittedWays.size() <<	endl;

	cout << "finished" << endl;

	//string n;
	//getline( cin, n );
	return 0;
}

