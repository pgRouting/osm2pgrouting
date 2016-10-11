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


     inline size_t has_class(const std::string &type_name, const std::string &class_name) const {
         return has_type(type_name)
             && m_Types.at(type_name).has_class(class_name);
     }

     double class_default_maxspeed(const std::string &type_name, const std::string &class_name) const {
         return m_Types.at(type_name).classes().at(class_name).default_maxspeed();
     }

     double class_priority(const std::string &type_name, const std::string &class_name) const {
         return m_Types.at(type_name).classes().at(class_name).priority();
     }

     const std::map<std::string, Type>& types() const {return m_Types;}


 private:
     inline bool has_type(const std::string &type_name) const {
         return m_Types.count(type_name) != 0;
     }

     //! Map, which saves the parsed types
     std::map<std::string, Type> m_Types;
};


}  // end namespace osm2pgr
#endif  // SRC_CONFIGURATION_H_
