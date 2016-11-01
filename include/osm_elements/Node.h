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


#ifndef SRC_NODE_H_
#define SRC_NODE_H_
#pragma once

#include <boost/lexical_cast.hpp>
#include <cstdint>
#include <string>
#include <map>
#include "./osm_element.h"

namespace osm2pgr {

class Tag;
    /**
      @code
      <node id="122603925" lat="53.0780875" lon="8.1351704"
      user="artus70" visible="true" timestamp="2007-11-18T22:18:59+00:00"/>
      </node>

      <node id="27526559" lat="45.5163189" lon="-122.7922059"
      version="22" timestamp="2012-12-20T03:29:13Z" changeset="14338804" uid="393906" user="Grant Humphries">
      <tag k="highway" v="traffic_signals"/>
      </node>

      <tag k="highway" v="traffic_signals"/>
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

class Node : public Element {
 public:
     Node() = default;
     Node(const Node&) = default;
     /**
      *    @param atts attributes read py the parser
      */
     explicit Node(const char **atts);
     ~Node() {};

     inline std::string geom_str(const std::string separator) const {
         return get_attribute("lon") + separator +  get_attribute("lat");
     }
     inline std::string lat() {return get_attribute("lat");}
     inline std::string lon() {return get_attribute("lon");}

     void tag_config(const Tag &tag);


     std::string get_geometry() const {
         return
             std::string("srid=4326; POINT(")
             + geom_str(" ") + ")";
     }

     inline std::string osm_id_str() {
         return boost::lexical_cast<std::string>(m_osm_id);
     }
     double getLength(const Node &previous) const;


     inline uint16_t incrementUse() {return ++m_numsOfUse;}
     inline uint16_t numsOfUse() const {return m_numsOfUse;}
     inline void numsOfUse(uint16_t val)  {m_numsOfUse = val;}

 private:
     /**
      *    counts the rate, how much this node is used in different ways
      */
     uint16_t m_numsOfUse;
};


}  // end namespace osm2pgr
#endif  // SRC_NODE_H_
