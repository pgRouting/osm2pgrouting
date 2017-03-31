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
#include <boost/lexical_cast.hpp>
#include "configuration/tag_key.h"
#include "configuration/tag_value.h"
#include "osm_elements/osm_tag.h"

namespace osm2pgr {
class Configuration {
 public:
     Configuration() = default;

     /** @brief add group of tag_key + all the tag_values
      *
      * @param[in] t_key Tag_key found in the configuration
      */
     void add_tag_key(const Tag_key &t_key);

     /** @brief retrieves the Tag_value (attrributes
      *
      * @param[in] tag Tag found in the configuration
      * @returns Tag_value
      */
     const Tag_value& tag_value(const Tag &tag) const;

     /* Is the (key, value) pair in the configuration?
      *
      *
      * @param[in] tag Tag (key, value) pair
      */
     bool has_tag(const Tag &tag) const;

     /** retrieves the maxspeed based on the tag
      * 
      * if the (key,value) has a value this is returned
      * else if the (key, *) has a value this is returned
      * else 50  is returned
      */

     double maxspeed(const Tag &tag) const;
     double maxspeed_forward(const Tag &tag) const;
     double maxspeed_backward(const Tag &tag) const;

     /** retrieves the priority based on the tag
      * 
      * if the (key,value) has a value this is returned
      * else if the (key, *) has a value this is returned
      * else 0  is returned
      */

     double priority(const Tag &tag) const;

     /*
      * data to be exported to configuration TABLE
      */
     const std::map<std::string, Tag_key>& types() const {return m_Tag_keys;}

 private:
     /** @brief is the tag key in the configuration file
      * 
      * @param[in] key  tag_key name="key"
      */

     bool has_tag_key(const std::string &key) const;
     const Tag_key& tag_key(const Tag &tag) const;


 private:
     std::map<std::string, Tag_key> m_Tag_keys;
};


}  // end namespace osm2pgr
#endif  // SRC_CONFIGURATION_H_



