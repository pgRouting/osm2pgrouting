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
#include "OSMDocumentParserCallback.h"
#include "OSMDocument.h"
#include "Way.h"
#include "Node.h"

// define here, which streetstype you want to parse
// for applying this filter, compile with "DISTRICT" as flag (g++ -DRESTRICT)
//#define _FILTER if(m_pActWay->highway == "motorway" || m_pActWay->highway == "primary" || m_pActWay->highway == "secondary")

namespace osm
{
	
	
/**
	Parser callback for OSMDocument files
*/
void OSMDocumentParserCallback::StartElement( const char *name, const char** atts )
{
	if( strcmp(name,"nd") == 0 )
	{
		if( m_pActWay && atts != NULL )
		{
			const char* name = *atts++;
			const char* value = *atts++;
			if( strcmp(name,"ref")==0 )
			{
				long long nodeRefId = atol( value );
                                m_pActWay->AddNodeRef( m_rDocument.FindNode( nodeRefId ) );
                                  Node * node = m_rDocument.FindNode( nodeRefId );
                                  if(node != 0 ){
                                    node->numsOfUse+=1;
                                  }else {
                                    std::cout << "Reference nd=" << nodeRefId << " has no corresponding Node Entry (Maybe Node entry after Reference?)" << std::endl;
                                  }
			}
		}
	}
	else if( strcmp(name,"node") == 0 )
	{
		if (atts != NULL)
		{
			long long id=-1;
			double lat=-1;
			double lon=-1;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "id" ) == 0 )
				{
					id = atol( value);
				}
				else if( strcmp( name, "lat" ) == 0 )
				{
					lat = atof( value );
				}
				else if( strcmp( name, "lon" ) == 0 )
				{
					lon = atof( value );
				}
			}
			if( id>0 ) m_rDocument.AddNode( new Node( id, lat, lon ) );
		}
	}
	else if( strcmp(name,"tag") == 0 )
	{
		// <tag k="name" v="Pfänderweg"/>
		if (atts != NULL)
		{
			std::string k;
			std::string v;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "k" ) == 0 )
				{
					k = value;
				}
				else if( strcmp( name, "v" ) == 0 )
				{
					v = value;
				}
			}
			if( !k.empty() )
			{
				if( m_pActWay && k.compare("name")==0 )
				{
					m_pActWay->name = v;
				}
				else if( m_pActWay && k.compare("oneway")==0 )
				{
					m_pActWay->oneway = true;					
					std::cout<<"Edge "<<m_pActWay->id<<" is oneway"<<std::endl;

				}				
				//else if( m_pActWay && k.compare("highway")==0 )
				else if( m_pActWay && m_rDocument.m_rConfig.m_Types.count(k) )
				{
					m_pActWay->type = k;
					m_pActWay->clss = v;
				}
			}
		}
	}
	else if( strcmp(name,"way") == 0 )
	{
		if (atts != NULL)
		{
			long long id=-1;
			bool visibility = false;
			const char** attribut = (const char**)atts;
			while( *attribut != NULL )
			{
				const char* name = *attribut++;
				const char* value = *attribut++;
				if( strcmp( name, "id" ) == 0 )
				{
					id = atol( value);
				}
				else if( strcmp( name, "visible" ) == 0 )
				{
					visibility = strcmp(value,"true")==0;
				}
			}
			if( id>0 )
			{
				m_pActWay = new Way( id, visibility );
				
			}
		}
	}
	else if( strcmp(name,"osm") == 0 )
	{
	}
}



void OSMDocumentParserCallback::EndElement( const char* name )
{
	if( strcmp(name,"way") == 0 )
	{
		//#ifdef RESTRICT
		//_FILTER
		
		if( m_rDocument.m_rConfig.m_Types.count(m_pActWay->type) && m_rDocument.m_rConfig.m_Types[m_pActWay->type]->m_Classes.count(m_pActWay->clss) )
		{
		//#endif
		std::cout<<"We need a way of type "<<m_pActWay->type<<" and class "<< m_pActWay->clss<<std::endl;
		
			m_rDocument.AddWay( m_pActWay );

		//#ifdef RESTRICT
		}
		else
		{
		std::cout<<"We DON'T need a way of type "<<m_pActWay->type<<" and class "<< m_pActWay->clss<<std::endl;
			delete m_pActWay;
		}
		//#endif
		
		m_pActWay = 0;
	}
}

}; // end namespace osm
