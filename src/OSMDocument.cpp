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


#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <iostream>
#include "./OSMDocument.h"
#include "./Configuration.h"
#include "./Node.h"
#include "./Relation.h"
#include "./Way.h"

namespace osm2pgr {

OSMDocument::OSMDocument(const Configuration &config, size_t lines) :
    m_nodesErrs(0),
    m_rConfig(config),
    m_lines(lines) {
}


void OSMDocument::AddNode(Node n) {
    m_Nodes[n.osm_id()] = n;
}

void OSMDocument::AddWay(Way w) {
    m_Ways[w.osm_id()] = w;
}

void OSMDocument::AddRelation(const Relation &r) {
    m_Relations.push_back(r);
}

Node*
OSMDocument::FindNode(int64_t nodeRefId) {
    auto it = m_Nodes.find(nodeRefId);
    return &(it->second);
}

bool
OSMDocument::has_node(int64_t nodeRefId) const {
    auto it = m_Nodes.find(nodeRefId);
    return (it != m_Nodes.end());
}

Way*
OSMDocument::FindWay(int64_t way_id) {
    auto it = m_Ways.find(way_id);
    return &(it->second);
}

bool
OSMDocument::has_way(int64_t way_id) const {
    auto it = m_Ways.find(way_id);
    return (it != m_Ways.end());
}

void
OSMDocument::add_node(Way &way, const char **atts) {
    auto **attribut = atts;
    std::string key = *attribut++;
    std::string value = *attribut++;
    auto node_id =  (key == "ref")?  boost::lexical_cast<int64_t>(value): -1;
    if (!has_node(node_id)) {
        ++m_nodesErrs;
    } else {
        auto node = FindNode(node_id);
        node->incrementUse();
        way.add_node(node);
    }
}

void
OSMDocument::add_config(Way &way, const Tag &tag) const {
    auto  k = tag.key();
    auto  v = tag.value();
    /*
     * for example
     *  <tag highway=motorway>    // k = highway  v = motorway
     *  <tag highway=path>    // k = highway  v = motorway
     *
     * And the configuration file has:
     * <type name="highway" id="1">
     *     <class name="motorway" id="101" priority="1.0" maxspeed="130" />
     *     // there is no class name="path"
     */
    if (m_rConfig.has_class(tag)) {
        if ((way.tag_config().key() == "" && way.tag_config().value() == "")
                || (
                    m_rConfig.has_class(tag)
                    && m_rConfig.has_class(way.tag_config())
                    && m_rConfig.class_priority(tag)
                    < m_rConfig.class_priority(way.tag_config())
                   )
           ) {
            way.tag_config(tag);
#if 0
            if (m_rConfig.has_class(way.tag_config())) {
                way.add_tag(tag);

                auto newValue = m_rConfig.class_default_maxspeed(way.tag_config());
                if (way.maxspeed_forward() <= 0) {
                    way.maxspeed_forward(newValue);
                }
                if (way.maxspeed_backward() <= 0) {
                    way.maxspeed_backward(newValue);
                }
            }
#endif
        }
    }
}


}  // namespace osm2pgr
