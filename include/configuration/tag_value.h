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

#ifndef SRC_CLASS_H_
#define SRC_CLASS_H_

#include <boost/lexical_cast.hpp>
#include <osm_elements/osm_element.h>
#include <cassert>
#include <map>
#include <string>
#include <vector>

namespace osm2pgr {

class Tag_value : public Element {
 public:
     Tag_value() = default;
     Tag_value(const Tag_value &) = default;
     explicit Tag_value(const char ** attributes);


     inline int64_t id() const {return osm_id();}
#if 0
     inline std::string name() const {return m_name;}
     inline double priority() const {return m_priority;}


     inline double maxspeed() const {return m_maxspeed;}
#endif
     std::vector<std::string> export_values() const; 

     inline std::string name() const {
         return (has_attribute("name)")) ?
             get_attribute("name)")
             : "";
     }
     inline double priority() const {
         assert(has_attribute("priority)"));
         return  boost::lexical_cast<double>(get_attribute("priority)"));
     }
     inline double default_maxspeed() const {
         assert(has_attribute("maxspeed)"));
         return  boost::lexical_cast<double>(get_attribute("maxspeed)"));
     }
 private:

#if 0
     int64_t m_id;
     std::string m_name;
     double m_priority;
     double m_maxspeed;
     double m_maxspeed_forward;
     double m_maxspeed_backward;
     std::string m_force;

     //TODO this dissapears
     double m_default_maxspeed;
#endif

};


}  // end namespace osm2pgr
#endif  // SRC_CLASS_H_
