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
#include <string>
#include "./stdafx.h"
#include "./Class.h"


namespace osm {
/**
 * @brief Constructor
 * @details Constructor of Class
 * 
 * @param long The ID of the class
 * @param name The name of the class
 * @param priority The priority of the class
 * @param default_maxspeed The default value of maxspeed
 */
Class::Class(
    long long id,
    std::string name,
    double priority,
    int default_maxspeed
  
:
  id(id),
  name(name),
  priority(priority),
  default_maxspeed(default_maxspeed) {
}

/**
 * @brief Destructor
 * @details Destructor for Class
 * @return [description]
 */
Class::~Class() {
}

}  // end namespace osm
