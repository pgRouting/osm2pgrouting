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

#ifndef SRC_CONFIGURATION_H_
#define SRC_CONFIGURATION_H_

#include <string>
#include <map>
#include "Type.h"
#include "Class.h"

namespace osm2pgr {

#if 1
template< class Map >
void ez_mapdelete(Map &c) {
    auto it(c.begin());
    auto last(c.end());
    while (it != last) {
        delete (*it++).second;
    }
}
#endif

#if 1
template< class Vector >
void ez_vectordelete(Vector &c) {
    auto it(c.begin());
    auto last(c.end());
    while (it != last) {
        delete (*it++);
    }
}
#endif

/**
A configuration document.
*/
class Configuration {
 public:
     //! Constructor
     Configuration() = default;

     //! add node to the map
     void AddType(Type t);
     Type FindType(std::string typeName) const;
     Type& FindType(std::string typeName);
     Class FindClass(const std::string &typeName, const std::string &className) const;
     std::string priority_str(const std::string &typeName, const std::string &className) const;
     inline size_t count_classes(const std::string &class_name, const std::string &type_name) const {
         return m_Types.at(class_name).count_classes(type_name);
     }

 public:
     //! Map, which saves the parsed types
     std::map<std::string, Type> m_Types;
};


}  // end namespace osm2pgr
#endif  // SRC_CONFIGURATION_H_
