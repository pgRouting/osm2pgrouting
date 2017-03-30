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


#ifndef SRC_OSM_TAG_H_
#define SRC_OSM_TAG_H_
#pragma once

#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <string>
#include <map>


namespace osm2pgr {


/**
  @code
  <tag k="highway" v="crossing"/>
  <tag k="exit_to" v="OR 217;Tigard;Salem"/>
  <tag k="highway" v="motorway_junction"/>
  <tag k="ref" v="69A"/>
  <tag k="name" v="Southwest Park &amp; Market"/>
  <tag k="public_transport" v="stop_position"/>
  <tag k="railway" v="tram_stop"/>
  <tag k="ref" v="11011"/>
  <tag k="tram" v="yes"/>
  <tag k="highway" v="turning_circle"/>

  @endcode
  */

class Tag {
 public:
     Tag() = default;
     Tag(const Tag&) = default;
     /**
      *    Constructor
      *    @param atts attributes pointer returned by the XML parser
      */
     explicit Tag(const char **atts);
     Tag(const std::string &k, const std::string &v) {
         m_key = k;
         m_value = v;
     }

     inline std::string key() const {return m_key;}
     inline std::string value() const {return m_value;}
     friend std::ostream& operator<<(std::ostream &os, const Tag& tag);

 private:
     // ! key
     std::string m_key;
     // ! value
     std::string m_value;
};


}  // end namespace osm2pgr
#endif  // SRC_OSM_TAG_H_
