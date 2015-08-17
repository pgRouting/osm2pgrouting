/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt       							   *
 *   gentoo.murray@gmail.com   											   *
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

#ifndef SRC_OSMDOCUMENT_H_
#define SRC_OSMDOCUMENT_H_

#include <map>
#include <vector>
#include "./Configuration.h"

namespace osm {


class Node;
class Way;
class Relation;

/**
	An osm-document.
*/
class OSMDocument {
 public:
    // ! Map, which saves the parsed nodes
    std::map<long long, Node*> m_Nodes;
    //! parsed ways
    std::vector<Way*> m_Ways;
    //! spitted ways
    std::vector<Way*> m_SplittedWays;
    //! relation between geometries
    std::vector<Relation*> m_Relations;


    Configuration& m_rConfig;

 public:
    //! Constructor
    OSMDocument(Configuration& config);
    //! Destructor
    virtual ~OSMDocument();
    //! add node to the map
    void AddNode(Node* n);
    //! add way to the map
    void AddWay(Way* w);
    //! find node by using an ID
    Node* FindNode(long long nodeRefId) const;
    //! split the ways
    void SplitWays();
    //  Node* getNode(long long nodeRefId);
    void AddRelation(Relation* r);
};


}  // end namespace osm
#endif  // SRC_OSMDOCUMENT_H_
