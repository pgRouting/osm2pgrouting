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

#ifndef SRC_WAY_H_
#define SRC_WAY_H_

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include <string>
#include "./osm_element.h"
#include "./Node.h"

namespace osm2pgr {


/**
  \code
  <way id="20215432" visible="true" timestamp="2008-01-09T22:35:16+00:00" user="Pferdo">
  <nd ref="213794929"/>
  <nd ref="213795470"/>
  <nd ref="213795483"/>
  <nd ref="213795493"/>
  <nd ref="213795506"/>
  <nd ref="213795517"/>
  <nd ref="213795527"/>
  <nd ref="213795541"/>
  <nd ref="213795552"/>
  <nd ref="213795561"/>
  <nd ref="213795571"/>
  <tag k="name" v="Pfnderweg"/>
  <tag k="created_by" v="JOSM"/>
  <tag k="highway" v="residential"/>
  </way>
  \endcode
  */
class Way : public Element {
 public:
     Way() = default;
     ~Way() {};

     /**
      *  @param atts attributes read py the parser
      */
     explicit Way(const char **atts);
     Tag add_tag(const Tag &tag);
     void add_node(Node* node);
     void add_node(int64_t node_id);

     std::vector<Node*>& nodeRefs() {return m_NodeRefs;}
     const std::vector<Node*> nodeRefs() const {return m_NodeRefs;}


     std::string members_str() const;

 public:
     inline void maxspeed_forward(double p_max) {m_maxspeed_forward = p_max;}
     inline void maxspeed_backward(double p_max) {m_maxspeed_backward = p_max;}

     inline std::string name() const {return has_tag("name")? get_tag("name") : "";}


     std::string oneWay() const;
     std::string oneWayType_str() const;
     inline bool is_oneway() const { return m_oneWay == "YES";}
     inline bool is_reversed() const { return m_oneWay == "REVERSED";}

     inline double maxspeed_forward() const {return m_maxspeed_forward;}
     inline double maxspeed_backward() const { return m_maxspeed_backward;}

     std::string get_geometry() const;
     std::string length_str() const;


     inline std::string maxspeed_forward_str() const {
         return boost::lexical_cast<std::string>(m_maxspeed_forward);
     }
     inline std::string maxspeed_backward_str() const {
         return boost::lexical_cast<std::string>(m_maxspeed_backward);
     }


     //! splits the way
     std::vector<std::vector<Node*>> split_me();
     std::string geometry_str(const std::vector<Node*> &) const;
     std::string length_str(const std::vector<Node*> &) const;

     /**
      * to insert the relations tags
      */
     void insert_tags(const std::map<std::string, std::string> &tags);

#ifndef NDEBUG
     friend
     std::ostream& operator<<(std::ostream &, const Way &);
#endif


 private:
     bool is_number(const std::string& s) const;
     double get_kph(const std::string &value) const;
     void max_speed(const Tag& tag);
     void oneWay(const Tag& tag);
     void implied_oneWay(const Tag& tag);


 private:
     /** references to node that its on the file */
     std::vector<Node*> m_NodeRefs;

     /** node identifiers found as part of the way */
     std::vector<int64_t> m_node_ids;

     double m_maxspeed_forward;
     double m_maxspeed_backward;
     std::string m_oneWay;
};


}  // end namespace osm2pgr
#endif  // SRC_WAY_H_
