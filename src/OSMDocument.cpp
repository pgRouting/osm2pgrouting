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

OSMDocument::OSMDocument(Configuration &config, size_t lines) :
    m_rConfig(config),
    m_lines(lines){
}

OSMDocument::~OSMDocument() {
    ez_mapdelete(m_Nodes);
    ez_vectordelete(m_Ways);
    ez_vectordelete(m_Relations);
}
void OSMDocument::AddNode(Node* n) {
    m_Nodes[n->osm_id()] = n;
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


}  // end namespace osm2pgr
