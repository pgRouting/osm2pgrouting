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


#ifndef SRC_OSM_ELEMENT_H_
#define SRC_OSM_ELEMENT_H_
#pragma once

#include <string>
#include <vector>
#include <map>
#include "./osm_tag.h"

namespace osm2pgr {


    /** @brief osm elements

      osm elements can be:

      @code
      <node
      <way
      <relation
      @endcode

      */

class Element {
 public:
     Element() = default;
     Element(const Element&) = default;
     /**
      *    Constructor
      *    @param atts attributes pointer returned by the XML parser
      */
     explicit Element(const char **atts);
     virtual ~Element() {};

     Tag add_tag(const Tag &);

     inline int64_t osm_id() const {return m_osm_id;}
     inline bool visible() const {return m_visible;}
     virtual void tag_config(const Tag &tag);
     inline Tag tag_config() const {return m_tag_config;}
     bool is_tag_configured() const;



     std::string attributes_str() const;
     std::string tags_str() const;

     virtual std::string get_geometry() const {return "";}
     bool has_attribute(const std::string&) const;
     std::string get_attribute(const std::string&) const;
     std::map<std::string, std::string>& attributes() {return m_attributes;}
     const std::map<std::string, std::string> attributes() const {
         return m_attributes;
     }


     bool has_tag(const std::string&) const;
     std::string get_tag(const std::string&) const;

     bool has_tags() const {return !m_tags.empty();}
     std::map<std::string, std::string>& tags() {return m_tags;}
     const std::map<std::string, std::string> tags() const {return m_tags;}

     std::vector<std::string> values(
             const std::vector<std::string> &columns,
             bool is_hstore) const;
     virtual std::string members_str() const {return std::string();};

 protected:
     // ! OSM ID of the element
     // or id of a configuraton
     int64_t m_osm_id;
     bool m_visible;
     Tag m_tag_config;


     std::map<std::string, std::string> m_tags;
     std::map<std::string, std::string> m_attributes;
};


}  // namespace osm2pgr
#endif  // SRC_OSM_ELEMENT_H_
