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

#if __GNUC__ > 4 || \
            (__GNUC__ == 4 && (__GNUC_MINOR__ >= 6))
#define WITH_RANGE_LOOP
#endif

#include "Way.h"
#include <string>
#include "Node.h"
#include "./math_functions.h"



namespace osm2pgr {

Way::Way(int64_t id,
        bool visible,
        int64_t osm_id,
        double maxspeed_forward,
        double maxspeed_backward) :
    m_id(id),
    m_visible(visible),
    m_maxspeed_forward(maxspeed_forward),
    m_maxspeed_backward(maxspeed_backward),
    m_oneWayType(UNKNOWN),
    m_osm_id(osm_id) {
    }

void Way::AddNodeRef(Node* pNode) {
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

#ifdef WITH_RANGE_LOOP
    for (const auto &node_ptr : nodeRefs) {
#else
        for (auto it = nodeRefs.begin();
                it != nodeRefs.end();
                ++it) {
            auto node_ptr = *it;
#endif

            geometry += node_ptr->geom_str(" ");
            geometry += ", ";

#ifdef WITH_RANGE_LOOP
        }
#else
    }
#endif
    geometry[geometry.size() - 2] = ')';
    return geometry;
}


std::string
Way::length_str(const std::vector<Node*> &nodeRefs) const {
    double length = 0;
    auto prev_node_ptr = nodeRefs.front();

#ifdef WITH_RANGE_LOOP
    for (const auto &node_ptr : nodeRefs) {
#else
        for (auto it = nodeRefs.begin();
                it != nodeRefs.end();
                ++it) {
            auto node_ptr = *it;
#endif

            length  += getLength(*prev_node_ptr, *node_ptr);
            prev_node_ptr = node_ptr;

#ifdef WITH_RANGE_LOOP
        }
#else
    }
#endif

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






}  // end namespace osm2pgr

