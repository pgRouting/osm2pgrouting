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
#include "configuration/tag_key.h"
#include "configuration/tag_value.h"
#include "osm_elements/osm_tag.h"

namespace osm2pgr {

/** @brief A configuration document.

@code
<?xml version="1.0" encoding="UTF-8"?>
<configuration>
  <type name="highway" id="1">
      <class name="motorway" id="101" />
      <class name="motorway_link" id="102" />
   </type>  
</configuration>
@endcode

*/
class Configuration {
 public:
     Configuration() = default;

     //! @brief found a <type name="highway" id="1">
     void AddTag_key(Tag_key t);

     Tag_key FindTag_key(std::string typeName) const;
     Tag_key& FindTag_key(std::string typeName);

     Tag_value FindTag_value(const Tag &tag) const;
     std::string priority_str(const Tag &tag) const;


     
     inline size_t has_tag(const Tag &tag) const {
         return has_type(tag.key())
             && m_Tag_keys.at(tag.key()).has_class(tag.value());
     }

     double class_default_maxspeed(const Tag &tag) const {
         return m_Tag_keys.at(
                 tag.key()).classes().at(tag.value()).default_maxspeed();
     }

     double class_priority(const Tag &tag) const {
         return m_Tag_keys.at(tag.key()).classes().at(tag.value()).priority();
     }

     const std::map<std::string, Tag_key>& types() const {return m_Tag_keys;}

     inline bool has_type(const std::string &type_name) const {
         return m_Tag_keys.count(type_name) != 0;
     }

 private:
     //! Map, which saves the parsed types
     std::map<std::string, Tag_key> m_Tag_keys;
};


}  // end namespace osm2pgr
#endif  // SRC_CONFIGURATION_H_
