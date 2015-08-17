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
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include "./stdafx.h"
#include "./OSMDocument.h"
#include "./Configuration.h"
#include "./Node.h"
#include "./Relation.h"
#include "./Way.h"
#include "./math_functions.h"

namespace osm {

/**
 * @brief Constructor
 * @details Deals with the OSM config file 
 * 
 * @param config the osm config file
 */
OSMDocument::OSMDocument(Configuration &config) : m_rConfig(config) {
}

/**
 * @brief Destructor
 */
OSMDocument::~OSMDocument() {
    ez_mapdelete(m_Nodes);
    ez_vectordelete(m_Ways);
    ez_vectordelete(m_Relations);
    ez_vectordelete(m_SplittedWays);
}
void OSMDocument::AddNode(Node* n) {
    m_Nodes[n->id] = n;
}

void OSMDocument::AddWay(Way* w) {
    m_Ways.push_back(w);
}

void OSMDocument::AddRelation(Relation* r) {
    m_Relations.push_back(r);
}

Node* OSMDocument::FindNode(long long nodeRefId)
const {
    std::map<long long, Node*>::const_iterator  it = m_Nodes.find(nodeRefId);
    return (it != m_Nodes.end()) ? it->second : 0;
}

void OSMDocument::SplitWays() {
    std::vector<Way*>::const_iterator it(m_Ways.begin());
    std::vector<Way*>::const_iterator last(m_Ways.end());

    //  splitted ways get a new ID
    long long id = 0;

    while (it != last) {
        Way* currentWay = *it++;

        // ITERATE THROUGH THE NODES
        std::vector<Node*>::const_iterator it_node(currentWay->m_NodeRefs.begin());
        std::vector<Node*>::const_iterator last_node(currentWay->m_NodeRefs.end());

        Node* backNode = currentWay->m_NodeRefs.back();

        while (it_node != last_node) {
            Node* node = *it_node++;
            Node* secondNode = 0;
            Node* lastNode = 0;

            Way* splitted_way = new Way(++id, currentWay->visible,
                currentWay->osm_id,
                currentWay->maxspeed_forward,
                currentWay->maxspeed_backward);
            splitted_way->name = currentWay->name;
            splitted_way->type = currentWay->type;
            splitted_way->clss = currentWay->clss;
            splitted_way->oneWayType = currentWay->oneWayType;

            std::map<std::string, std::string>::iterator it_tag(currentWay->m_Tags.begin());
            std::map<std::string, std::string>::iterator last_tag(currentWay->m_Tags.end());
//            std::cout << "Number of tags: " << currentWay->m_Tags.size() << std::endl;
//            std::cout << "First tag: " << currentWay->m_Tags.front()->key << ":" << currentWay->m_Tags.front()->value << std::endl;

            // ITERATE THROUGH THE TAGS

            while (it_tag != last_tag) {
                std::pair<std::string, std::string> pair = *it_tag++;

                splitted_way->AddTag(pair.first, pair.second);
            }

    // GeometryFromText('LINESTRING('||x1||' '||y1||','||x2||' '||y2||')',4326);

            splitted_way->geom = "LINESTRING("+ boost::lexical_cast<std::string>(node->lon) + " " + boost::lexical_cast<std::string>(node->lat) +",";

            splitted_way->AddNodeRef(node);

            bool found = false;

            if (it_node != last_node) {
                while (it_node != last_node && !found) {
                    splitted_way->AddNodeRef(*it_node);
                    if ((*it_node)->numsOfUse > 1) {
                        found = true;
                        secondNode = *it_node;
                        splitted_way->AddNodeRef(secondNode);
                        double length = getLength(node, secondNode);
                        if (length < 0)
                            length*=-1;
                        splitted_way->length+=length;
                        splitted_way->geom+= boost::lexical_cast<std::string>(secondNode->lon) + " " + boost::lexical_cast<std::string>(secondNode->lat) + ")";
                    } else if (backNode == (*it_node)) {
                        lastNode =*it_node++;
                        splitted_way->AddNodeRef(lastNode);
                        double length = getLength(node, lastNode);
                        if (length < 0)
                            length*=-1;
                        splitted_way->length+=length;
                        splitted_way->geom+= boost::lexical_cast<std::string>(lastNode->lon) + " " + boost::lexical_cast<std::string>(lastNode->lat) + ")";
                    } else {
                        splitted_way->geom+= boost::lexical_cast<std::string>((*it_node)->lon) + " " + boost::lexical_cast<std::string>((*it_node)->lat) + ",";
                        *it_node++;
                    }
                }
            }

            if (splitted_way->m_NodeRefs.front() != splitted_way->m_NodeRefs.back()) {
                m_SplittedWays.push_back(splitted_way);
            } else {
                delete splitted_way;
                splitted_way = 0;
            }
        }
    }
}  // end SplitWays

}  // end namespace osm
