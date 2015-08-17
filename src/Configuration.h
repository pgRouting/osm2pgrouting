/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt                                    *
 *   gentoo.murray@gmail.com                                               *
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
 
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Type.h"
#include "Class.h"

namespace osm
{

/**
 * @brief Template to delete the map
 * @details [long description]
 * 
 * @param c The map to be deleted
 * @tparam  Map  [description]
 */
template< class Map >
inline void ez_mapdelete( Map& c )
{
	typename Map::iterator it( c.begin() );
	typename Map::iterator last( c.end() );
	while( it!=last )
	{
		delete (*it++).second;
	}
}

/**
 * @brief Template to delete the vector
 * @details [long description]
 * 
 * @param c The vector to be deleted
 * @tparam  Map  [description]
 */
template< class Vector >
inline void ez_vectordelete( Vector& c )
{
	typename Vector::iterator it( c.begin() );
	typename Vector::iterator last( c.end() );
	while( it!=last )
	{
		delete (*it++);
	}
}
          

/**
A configuration document.
*/
class Configuration
{
public:
	//! Map, which saves the parsed types
	std::map<std::string, Type*> m_Types;
	//std::map<long long, Type*> m_Types;
public:

	//! Constructor
	Configuration();
	//! Destructor
	virtual ~Configuration();
	//! add node to the map
	void AddType( Type* t );
	Type* FindType( std::string typeName );
	Class* FindClass( std::string typeName, std::string className );
};


} // end namespace osm
#endif
