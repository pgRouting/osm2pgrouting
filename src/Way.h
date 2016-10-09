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
#ifndef SRC_WAY_H_
#define SRC_WAY_H_

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include <string>
#include <Node.h>
namespace osm2pgr {

enum OneWayType{ UNKNOWN = 0, YES = 1, NO = 2, REVERSED = -1, REVERSIBLE = 3};

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
class Way {
 public:
     /** 
      *  Constructor
      *  @param id ID of the way
      */
     Way(int64_t id,
             bool visible,
             int64_t osm_id,
             double maxspeed_forward,
             double maxspeed_backward);
     //! Destructor
     // ~Way();
     /**
      *  saves the nodes of the way  
      *  @param pNode node
      */
     void AddNodeRef(Node* pNode);
     void AddTag(std::string key, std::string value);
     bool HasTag(std::string key);

     std::vector<Node*>& nodeRefs() {return m_NodeRefs;}
     const std::vector<Node*> nodeRefs() const {return m_NodeRefs;}
     std::map<std::string, std::string>& tags() {return m_Tags;}
     const std::map<std::string, std::string> tags() const {return m_Tags;}

     inline int64_t osm_id() const {return m_osm_id;}
     inline bool visible() const {return m_visible;}
     inline int64_t id() const {return m_id;}

     inline void name(std::string p_name) {m_name = p_name;}
     inline void type(std::string p_type) {m_type = p_type;}
     inline void clss(std::string p_clss) {m_clss = p_clss;}
     inline void oneWayType(OneWayType p_one_way) {m_oneWayType = p_one_way;}
     inline void maxspeed_forward(double p_max) {m_maxspeed_forward = p_max;}
     inline void maxspeed_backward(double p_max) {m_maxspeed_backward = p_max;}

     inline std::string name() const {return m_name;}
     inline std::string type() const {return m_type;}
     inline std::string clss() const {return m_clss;}
     inline OneWayType oneWayType() const {return m_oneWayType;}
     inline double maxspeed_forward() const {return m_maxspeed_forward;}
     inline double maxspeed_backward() const { return m_maxspeed_backward;}

     std::string geometry_str() const;
     std::string length_str() const;

     std::string geometry_str(size_t i) const;
     std::string length_str(size_t i) const;


     inline std::string source_osm_id(size_t i) const {
         return m_split_ways[i].front()->osm_id_str();
     }
     inline std::string target_osm_id(size_t i) const {
         return m_split_ways[i].back()->osm_id_str();
     }

     inline std::string first_node_str() const {
         return nodeRefs().front()->geom_str(std::string("\t"));
     }
     inline std::string last_node_str() const {
         return nodeRefs().back()->geom_str("\t");
     }

     inline std::string first_node_str(size_t i) const {
         return m_split_ways[i].front()->geom_str(std::string("\t"));
     }
     inline std::string last_node_str(size_t i) const {
         return m_split_ways[i].back()->geom_str(std::string("\t"));
     }

     inline std::string maxspeed_forward_str() const {
         return boost::lexical_cast<std::string>(m_maxspeed_forward);
     }
     inline std::string maxspeed_backward_str() const {
         return boost::lexical_cast<std::string>(m_maxspeed_backward);
     }

     inline std::string oneWayType_str() const {
         return boost::lexical_cast<std::string>(m_oneWayType);
     }

     //! splits the way
     void split_me();

     //! @brief splits the way
     inline size_t splits() {return m_split_ways.size();}




 private:
     std::string geometry_str(const std::vector<Node*> &) const;
     std::string length_str(const std::vector<Node*> &) const;

     //! Do not delete nodes in this container!
     std::vector<Node*> m_NodeRefs;
     std::map<std::string, std::string> m_Tags;

     /*! the splited ways are a subset of the original way
      *  all split_ways share the information of the original way
      *  - name
      *  - osm id
      *  - type
      *  - class
      *  - max speeds
      *  - one_way_type
      */
     std::vector<std::vector<Node*>> m_split_ways;

     //! ID of the way
     int64_t m_id;
     bool m_visible;
     //! name of the street
     std::string m_name;
     //! type of the street, for example "motorway"

     std::string m_type;
     std::string m_clss;

     double m_maxspeed_forward;
     double m_maxspeed_backward;

     OneWayType m_oneWayType;

     int64_t m_osm_id;
};


}  // end namespace osm2pgr
#endif  // SRC_WAY_H_
