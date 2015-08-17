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
#include "Configuration.h"
#include "Type.h"
#include "Class.h"


namespace osm
{
    
    //! Constructor
	Configuration::Configuration()
	{
	}

	//! Destructor
	Configuration::~Configuration()
	{
		ez_mapdelete( m_Types );
	}

	//! Adds type t to the map
	void Configuration::AddType( Type* t )
	{
		m_Types[t->name] = t;
	}

	//! Finds the name in the map
	Type* Configuration::FindType( std::string name ) 
	{
		return m_Types[name];
	}

	//! Finds the name in the class
	Class* Configuration::FindClass( std::string typeName, std::string className ) 
	{
		return m_Types[typeName]->m_Classes[className];
	}

} // end namespace osm
