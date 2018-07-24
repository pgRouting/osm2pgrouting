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


#include "osm_elements/Way.h"
#include "boost/lexical_cast.hpp"
#include <algorithm>
#include <string>
#include <cassert>
#include <map>
#include <vector>
#include <iostream>
#include "osm_elements/OSMDocument.h"
#include "osm_elements/osm_tag.h"
#include "osm_elements/Node.h"



namespace osm2pgr {


Way::Way(const char **atts) :
    Element(atts),
    m_maxspeed_forward(-1),
    m_maxspeed_backward(-1),
    m_oneWay("UNKNOWN") {
    }

Tag
Way::add_tag(const Tag &tag) {
    m_tags[tag.key()] = tag.value();
    implied_oneWay(tag);
    oneWay(tag);
    max_speed(tag);
    return tag;
}


void
Way::add_node(int64_t node_id) {
    m_node_ids.push_back(node_id);
}

void
Way::add_node(Node *node) {
    assert(node);
    m_NodeRefs.push_back(node);
}


std::string
Way::get_geometry() const {
    return geometry_str(m_NodeRefs);
}

std::string
Way::length_str() const {
    return length_str(m_NodeRefs);
}


std::string
Way::geometry_str(const std::vector<Node*> &nodeRefs) const {
    if (nodeRefs.size() < 2) return "srid=4326;LINESTRING EMPTY";

    std::string geometry("srid=4326;LINESTRING(");

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



std::vector<std::vector<Node*>>
Way::split_me() {
    if (nodeRefs().size() < 2) {
        /*
         * The way is ill formed
         */
        return std::vector<std::vector<Node*>>();
    }

    std::vector<std::vector<Node*>> m_split_ways;
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
    return m_split_ways;
}



std::string
Way::oneWay() const {
    return static_cast<std::string>(m_oneWay);
}

void
Way::oneWay(const Tag &tag) {
    auto key = tag.key();
    auto value = tag.value();

    if (key != "oneway") {
        return;
    }

    if (m_oneWay != "UNKNOWN") return;

    // one way tag
    if ((value == "yes") || value == "true" || value == "1") {
        m_oneWay = "YES";
    }

    // check false conditions: 0, no, false
    if ((value == "no") || value == "false" || value == "1") {
        m_oneWay = "NO";
    }

    // check reversible condition
    if (value == "reversible") {
        m_oneWay = "REVERSIBLE";
    }

    // check revers conditions: -1
    if (value == "-1") {
        m_oneWay = "REVERSED";
    }
}

void
Way::implied_oneWay(const Tag &tag) {
    auto key = tag.key();
    auto value = tag.value();
    /*
     * was tagged, so not using implied tagging
     */
    if (m_oneWay != "UNKNOWN") return;

    if ((key == "junction" && value == "roundabout")
            || (key == "highway" && value == "motorway")) {
        m_oneWay = "YES";
        return;
    }

}

#if 0
void
Way::pedestrian(const std::string &key, const std::string &value) {
    // TODO(vicky) for 3.0
    // m_pedestrian("UNKNOWN") <-- the default in the constructor
    if ((key == "sidewalk" && value == "no")
            || (key == "foot" && value == "no")) {
        m_pedestrian = "NO";
    }

    if ((key == "highway" && value == "pedestrian") {
            || (key == "highway" && value == "footway")
            || (key == "highway" && value == "cycleway")
            || (key == "highway" && value == "bridleway")
            || (key == "highway" && value == "track")
            || (key == "sidewalk" && value != "no")  )
        || (key == "foot" && value != "no")  )
            || (key == "highway" && value == "steps") {
                m_pedestrian = "YES";
                return
            }
    }
}
#endif

bool
Way::is_number(const std::string& s) const {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}


/*
 * takes the fist value found
 */
double
Way::get_kph(const std::string &value) const {
    auto mph_pos = value.find(" mph");
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
Way::max_speed(const Tag &tag) {
    auto key = tag.key();
    auto value = tag.value();
    if (key == "maxspeed:forward") {
        m_maxspeed_forward = get_kph(value);
        return;
    }
    if (key == "maxspeed:backward") {
        m_maxspeed_backward = get_kph(value);
        return;
    }
    if (key == "maxspeed") {
        m_maxspeed_backward = get_kph(value);
        m_maxspeed_forward = get_kph(value);
        return;
    }
}


std::string
Way::oneWayType_str() const {
    if (m_oneWay == "YES") return "1";
    if (m_oneWay == "NO") return  "2";
    if (m_oneWay == "REVERSIBLE") return  "3";
    if (m_oneWay == "REVERSED") return "-1";
    if (m_oneWay == "UNKNOWN") return "0";
    return "0";
}

void
Way::insert_tags(const std::map<std::string, std::string> &tags) {
    for (auto it = tags.begin(); it != tags.end(); ++it) {
        auto tag = *it;
        m_tags[tag.first] = tag.second;
    }
}

std::string
Way::members_str() const {
    /* this list comes from the node_ids becuase a node might not be on the file */
    std::string node_list("");
    for (const auto &node_id : m_node_ids) {
        node_list += boost::lexical_cast<std::string>(node_id) + "=>\"type=>nd\",";
    }
    node_list[node_list.size() -1] = ' ';

    return node_list;
}



#ifndef NDEBUG
std::ostream& operator<<(std::ostream &os, const Way &way) {
    std::cout << "\nWay"
        << "\t m_osm_id: " << way.m_osm_id
        << "\t m_tag_config: " << way.m_tag_config
        << "\t m_visible: " << way.m_visible
        << "\t m_maxspeed_forward: " << way.m_maxspeed_forward
        << "\t m_maxspeed_backward: " << way.m_maxspeed_backward
        << "\t m_oneWay: " << way.m_oneWay;
    std::cout << "\n\n ************ attributes: " << way.attributes_str();
    std::cout << "\n\n ************ tags: " << way.tags_str();
    std::cout << "\n nodes: \n";
    for (auto it = way.m_NodeRefs.begin(); it != way.m_NodeRefs.end(); ++it) {
        auto e = *it;
        std::cout << e->osm_id() << ", ";
    }

    return os;
}
#endif


}  // namespace osm2pgr
