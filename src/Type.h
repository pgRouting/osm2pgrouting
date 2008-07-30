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
                  
#ifndef TYPE_H
#define TYPE_H

#include "Class.h"

namespace osm
{

class Type
{
public:
	//std::vector<Class*> m_ClassRefs;
	std::map<std::string, Class*> m_Classes;
	//std::map<long long, Class*> m_Classes;
	long long id;
	std::string name;
public:
	/** 
	 *	Constructor
	 *	@param name name of the type
	 */
	Type( long long id, std::string );
	//! Destructor
	~Type();
	/**
	 *	saves the classes of the type
	 *	@param pClass class
	 */
	void AddClass( Class* pClass );
};


} // end namespace osm
#endif
