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
#include "./OSMDocument.h"
#include "./Configuration.h"
#include "./Node.h"
#include "./Relation.h"
#include "./Way.h"
#include "./math_functions.h"

namespace osm2pgr {

OSMDocument::OSMDocument(Configuration &config) : m_rConfig(config) {
}

OSMDocument::~OSMDocument() {
    ez_mapdelete(m_Nodes);
    ez_vectordelete(m_Ways);
    ez_vectordelete(m_Relations);
    ez_vectordelete(m_SplitWays);
}
void OSMDocument::AddNode(Node* n) {
    m_Nodes[n->id()] = n;
}

void OSMDocument::AddWay(Way* w) {
    m_Ways.push_back(w);
}

void OSMDocument::AddRelation(Relation* r) {
    m_Relations.push_back(r);
}

Node* OSMDocument::FindNode(int64_t nodeRefId)
const {
    std::map<int64_t, Node*>::const_iterator  it = m_Nodes.find(nodeRefId);
    return (it != m_Nodes.end()) ? it->second : 0;
}

void OSMDocument::SplitWays() {
    std::vector<Way*>::const_iterator it(m_Ways.begin());
    std::vector<Way*>::const_iterator last(m_Ways.end());
    std::string separator(" ");

    //  split ways get a new ID
    int64_t id = 0;

    while (it != last) {
        Way* currentWay = *it++;

        // ITERATE THROUGH THE NODES
        auto it_node(currentWay->nodeRefs().begin());
        auto last_node(currentWay->nodeRefs().end());

        Node* backNode = currentWay->nodeRefs().back();

        while (it_node != last_node) {
            Node* node = *it_node++;
            Node* secondNode = 0;
            Node* lastNode = 0;

            Way* split_way = new Way(++id, currentWay->visible(),
                currentWay->osm_id(),
                currentWay->maxspeed_forward(),
                currentWay->maxspeed_backward());
            split_way->name(currentWay->name());
            split_way->type(currentWay->type());
            split_way->clss(currentWay->clss());
            split_way->oneWayType(currentWay->oneWayType());

            auto it_tag(currentWay->tags().begin());
            auto last_tag(currentWay->tags().end());
//            std::cout << "Number of tags: " << currentWay->m_Tags.size() << std::endl;
//            std::cout << "First tag: " << currentWay->m_Tags.front()->key << ":" << currentWay->m_Tags.front()->value << std::endl;

            // ITERATE THROUGH THE TAGS

            while (it_tag != last_tag) {
                std::pair<std::string, std::string> pair = *it_tag++;

                split_way->AddTag(pair.first, pair.second);
            }

    // GeometryFromText('LINESTRING('||x1||' '||y1||','||x2||' '||y2||')',4326);

            split_way->geom() = "LINESTRING(" + node->geom_str(separator) + ", ";

            split_way->AddNodeRef(node);

            bool found = false;

            if (it_node != last_node) {
                while (it_node != last_node && !found) {
                    split_way->AddNodeRef(*it_node);
                    if ((*it_node)->numsOfUse() > 1) {
                        found = true;
                        secondNode = *it_node;
                        split_way->AddNodeRef(secondNode);
                        double length = getLength(*node, *secondNode);
                        if (length < 0)
                            length*=-1;
                        split_way->add_length(length);
                        split_way->geom() += secondNode->geom_str(separator) + ")";
                    } else if (backNode == (*it_node)) {
                        lastNode = *it_node++;
                        split_way->AddNodeRef(lastNode);
                        double length = getLength(*node, *lastNode);
                        if (length < 0)
                            length*=-1;
                        split_way->add_length(length);
                        split_way->geom() += lastNode->geom_str(separator) + ")";
                    } else {
                        split_way->geom() += (*it_node)->geom_str(separator)  + ", ";
                        ++it_node;
                    }
                }
            }

            if (split_way->nodeRefs().front() != split_way->nodeRefs().back()) {
                m_SplitWays.push_back(split_way);
            } else {
                delete split_way;
                split_way = 0;
            }
        }
    }
}  // end SplitWays

}  // end namespace osm2pgr
