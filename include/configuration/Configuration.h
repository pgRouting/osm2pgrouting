/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License t &or more details.                        *
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
#include "./osm_tag.h"
#include "./Class.h"

namespace osm2pgr {

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
     Class FindClass(const Tag &tag) const;
     std::string priority_str(const Tag &tag) const;


     inline size_t has_class(const Tag &tag) const {
         return has_type(tag.key())
             && m_Types.at(tag.key()).has_class(tag.value());
     }

     double class_default_maxspeed(const Tag &tag) const {
         return m_Types.at(
                 tag.key()).classes().at(tag.value()).default_maxspeed();
     }

     double class_priority(const Tag &tag) const {
         return m_Types.at(tag.key()).classes().at(tag.value()).priority();
     }

     const std::map<std::string, Type>& types() const {return m_Types;}

     inline bool has_type(const std::string &type_name) const {
         return m_Types.count(type_name) != 0;
     }

 private:
     //! Map, which saves the parsed types
     std::map<std::string, Type> m_Types;
};


}  // end namespace osm2pgr
#endif  // SRC_CONFIGURATION_H_
