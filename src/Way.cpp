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


#include "Way.h"
#include <string>
#include "boost/lexical_cast.hpp"
#include "OSMDocument.h"
#include "Node.h"



namespace osm2pgr {


Way::Way(const char **atts) :
    m_visible(true),
    m_name(""),
    m_type(""),
    m_clss(""),
    m_maxspeed_forward(-1),
    m_maxspeed_backward(-1),
    m_oneWay("UNKNOWN") {
        auto **attribut = atts;
        while (*attribut != NULL) {
            std::string key = *attribut++;
            std::string value = *attribut++;
            if (key == "id") {
                m_osm_id = boost::lexical_cast<int64_t>(value);
            } else if (key == "visible") {
                m_visible = boost::lexical_cast<bool>(value);
            } else {
                m_attributes[key] = value;
            }
        }
    }


int64_t
Way::add_node(const char **atts) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        std::string key = *attribut++;
        std::string value = *attribut++;
        if (key == "ref") {
            m_node_osm_id.push_back(boost::lexical_cast<int64_t>(value));
            return  boost::lexical_cast<int64_t>(value);
        };
    }
    return 0;
}

void
Way::add_tag(const char **atts, std::string &key, std::string &value) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        std::string k = *attribut++;
        std::string v = *attribut++;
        if (k == "k") {
            key = v;
        } if (k == "v") {
            value = v;
        }
    }
    /* store the tag as originaly recieved*/
    m_Tags[key] = value;
    oneWay(key, value);
    max_speed(key, value);
    name(key,value);
}


void
Way::name(const std::string key, const std::string value) {
    if (key == "name") {
        m_name = value;
    }
}




void
Way::AddNodeRef(Node* pNode) {
    if (pNode) m_NodeRefs.push_back(pNode);
}

void
Way::AddTag(std::string key, std::string value) {
    m_Tags[key] = value;
}

bool
Way::HasTag(std::string key) {
    return (m_Tags.count(key) > 0);
}


std::string
Way::geometry_str() const {
    return geometry_str(m_NodeRefs);
}

std::string
Way::length_str() const {
    return length_str(m_NodeRefs);
}

std::string
Way::length_str(size_t i) const {
    return length_str(m_split_ways[i]);
}

std::string
Way::geometry_str(size_t i) const {
    return geometry_str(m_split_ways[i]);
}

std::string
Way::geometry_str(const std::vector<Node*> &nodeRefs) const {
    std::string geometry("LINESTRING(");

    for (auto it = nodeRefs.begin();
            it != nodeRefs.end();
            ++it) {
        auto node_ptr = *it;

        geometry += node_ptr->geom_str(" ");
        geometry += ", ";

    }
    geometry[geometry.size() - 2] = ')';
    return geometry;
}


std::string
Way::length_str(const std::vector<Node*> &nodeRefs) const {
    double length = 0;
    auto prev_node_ptr = nodeRefs.front();

    for (auto it = nodeRefs.begin();
            it != nodeRefs.end();
            ++it) {
        auto node_ptr = *it;

        length  += node_ptr->getLength(*prev_node_ptr);
        prev_node_ptr = node_ptr;

    }

    return boost::lexical_cast<std::string>(length);
}



void
Way::split_me() {
    if (nodeRefs().size() < 2) {
        /*
         * The way is ill formed
         */
        return;
    };

    m_split_ways.clear(); // in case its called twice

    auto it_node(nodeRefs().begin());
    auto last_node(nodeRefs().end());

    while (it_node != last_node) {
        /*
         * starting a new split
         */
        std::vector<Node*> split_way;
        split_way.push_back(*it_node);

        ++it_node;

        if (it_node != last_node) {
            while (it_node != last_node) {
                split_way.push_back(*it_node);

                if ((*it_node)->numsOfUse() > 1) {
                    break;
                }
                ++it_node;
            }
        }

        if (split_way.size() > 1) m_split_ways.push_back(split_way);
    }
}



std::string
Way::oneWay() const {
    return m_oneWay;
}

void
Way::oneWay(const std::string &key, const std::string &p_one_way) {      
    if (key != "oneway") {
        implied_oneWay(key, p_one_way);
        return;
    }

    // one way tag
    if ((p_one_way == "yes") || p_one_way == "true" || p_one_way == "1") {
        m_oneWay = "YES";
    }

    // check false conditions: 0, no, false
    if ((p_one_way == "no") || p_one_way == "false" || p_one_way == "1") {
        m_oneWay = "NO";
    }

    // check reversible condition
    if (p_one_way == "reversible") {
        m_oneWay = "REVERSIBLE";
    } 

    // check revers conditions: -1
    if (p_one_way == "-1") {
        m_oneWay = "REVERSED";
    }
}

void
Way::implied_oneWay(const std::string &key, const std::string &value) {
    /*
     * was tagged, so not using implied tagging
     */
    if (m_oneWay != "UNKNOWN") return;

    if ((key == "junction" && value == "roundabout")
            || (key == "highway"
                && (value == "motorway"
                    || value == "trunk") )) {
        m_oneWay == "YES";
        return;
    }

    if (key == "highway"
            && (value == "primary"
                || value == "secondary"
                || value == "tertiary")) {
        m_oneWay == "NO";
        return;
    }
}

#if 0
void
Way::pedestrian(const std::string &key, const std::string &value) {
    // TODO
    // m_pedestrian("UNKNOWN") <-- the default in the constructor
    if ((key == "sidewak" && value == "no")
            || (key == "foot" && value == "no")) {
        m_pedestrian = "NO";
    }

    if ((key == "highway" && value == "pedestrian") {
            || (key == "highway" && value == "footway") 
            || (key == "highway" && value == "cycleway") 
            || (key == "highway" && value == "bridleway") 
            || (key == "highway" && value == "track") 
            || (key == "sidewak" && value != "no")  )
        || (key == "foot" && value != "no")  )
            || (key == "highway" && value == "steps") { 
                m_pedestrian = "YES";
                return
            }
    }
}
#endif

bool
Way::is_number(const std::string& s) const{
    auto str = s;
    remove_if(str.begin(), str.end(), isspace);
    auto it = str.begin();
    for (; it != str.end() && std::isdigit(*it);
            ++it) {};
    return !str.empty() && it == s.end();
}


/*
 * takes the fist value found
 */
double
Way::get_kph(const std::string &value) const {
    auto mph_pos = value.find("mph");
    if (mph_pos != std::string::npos) {
        auto newstr = value;
        newstr.erase(mph_pos, std::string::npos);
        if (is_number(newstr)) {
            return boost::lexical_cast<double>(newstr) * 1.609346;
        }
    }

    mph_pos = value.find("knots");
    if (mph_pos != std::string::npos) {
        auto newstr = value;
        newstr.erase(mph_pos, std::string::npos);
        if (is_number(newstr)) {
            return boost::lexical_cast<double>(newstr) * 1.852;
        }
    }
    if (is_number(value)) { 
        return boost::lexical_cast<double>(value);
    }
    // TODO(vicky): handle non-numeric values, ex.: RO:urban
    // maybe using a configuration option
    // http://wiki.openstreetmap.org/wiki/Speed_limits
    //
    // TODO(vicky): handle multiple values for lanes
    // the way with N lanes generates N ways that have to be split
    // with the different Speeds ???
    return 50;
}




void
Way::max_speed(const std::string &key, const std::string &value) {
    if (key == "maxspeed:forward") {
        m_maxspeed_forward = get_kph(value);
        return;
    }
    if (key == "maxspeed:backward") {
        m_maxspeed_backward = get_kph(value);
        return;
    }
    if (key == "maxspeed") {
        m_maxspeed_backward =  m_maxspeed_forward = get_kph(value);
        return;
    }
}


std::string
Way::oneWayType_str() const{  
    if (m_oneWay == "YES") return "1";
    if (m_oneWay == "NO") return  "2";
    if (m_oneWay == "REVERSIBLE") return  "3";
    if (m_oneWay == "REVERSED") return "-1";
    if (m_oneWay == "UNKNOWN") return "0";
    return "0";
}

std::ostream& operator<<(std::ostream &os, const Way &way) {
    std::cout << "\nWay"
        << "\t m_id: " << way.m_id
        << "\t m_type: " << way.m_type
        << "\t m_class: " << way.m_clss
        << "\t m_name: " << way.m_name
        << "\t m_osm_id: " << way.m_osm_id
        << "\t m_visible: " << way.m_visible
        << "\t m_maxspeed_forward: " << way.m_maxspeed_forward
        << "\t m_maxspeed_backward: " << way.m_maxspeed_backward
        << "\t m_oneWay: " << way.m_oneWay;
    std::cout << "\n\n ************ attributes:\n";
    for (auto const &e : way.m_attributes) {
        std::cout << "(" << e.first << "-> " << e.second << "), ";
    }
    std::cout << "\n\n ************ tags \n";
    for (auto const &e : way.m_Tags) {
        std::cout << "(" << e.first << "-> " << e.second << "), ";;
    }
    std::cout << "\n nodes: \n";
    for (auto const &e : way.m_node_osm_id) {
        std::cout << e << ", ";
    }

    return os;
}


}  // namespace osm2pgr

