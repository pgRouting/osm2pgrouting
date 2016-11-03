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


#ifndef SRC_OSMDOCUMENT_H_
#define SRC_OSMDOCUMENT_H_

#include <map>
#include <vector>
#include <string>
#include "./Configuration.h"

namespace osm2pgr {


class Node;
class Way;
class Relation;

/**
    An osm-document.
*/
class OSMDocument {
 public:
    //! Constructor
    OSMDocument(const Configuration& config, size_t lines);

    inline size_t lines() const {return m_lines;}
    inline bool has_class(const Tag &tag) const {
        return m_rConfig.has_class(tag);
    }
    inline double class_priority(const Tag &tag) const {
        return m_rConfig.class_priority(tag);
    }
    inline double class_default_maxspeed(const Tag &tag) const {
        return m_rConfig.class_default_maxspeed(tag);
    }

    const std::map<int64_t, Node>& nodes() const {return m_Nodes;}
    const std::map<int64_t, Way>& ways() const {return m_Ways;}
    const std::vector<Relation>& relations() const {return m_Relations;}

    //! add node to the map
    void AddNode(Node n);

    //! add way to the map
    void AddWay(Way w);

    //! find node by using an ID
    bool has_node(int64_t nodeRefId) const;
    Node* FindNode(int64_t nodeRefId);

    bool has_way(int64_t way_id) const;
    Way* FindWay(int64_t way_id);

    void AddRelation(const Relation &r);

    void add_node(Way &way, const char **atts);
    size_t nodesErrs() {return m_nodesErrs;}

    /**
     * add the configuration tag used for the speeds
     */
    void add_config(Way &way, const Tag &tag) const;

 private:
    // ! parsed nodes
    std::map<int64_t, Node> m_Nodes;
    //! parsed ways
    std::map<int64_t, Way> m_Ways;
    //! parsed relations
    std::vector<Relation> m_Relations;


    size_t m_nodesErrs;
 public:
    const Configuration& m_rConfig;
 private:
    size_t m_lines;
};

}  // end namespace osm2pgr
#endif  // SRC_OSMDOCUMENT_H_
