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
#include "utilities/utilities.h"
#include "configuration/Configuration.h"
#include "utilities/prog_options.h"
#include "database/Export2DB.h"

namespace osm2pgr {


class Node;
class Way;
class Relation;

/**
    An osm-document.
*/
class OSMDocument {
 public:
    typedef std::vector<Node> Nodes;
    typedef std::vector<Way> Ways;

    //! Constructor
    OSMDocument(
            const Configuration& config,
            const po::variables_map &vm,
            const Export2DB &db_conn,
            size_t lines);

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

    const Nodes& nodes() const {return m_nodes;}
    const std::map<int64_t, Way>& ways() const {return m_Ways;}
    const std::vector<Relation>& relations() const {return m_Relations;}

    //! add node to the map
    void AddNode(const Node &n);

    //! add way to the map
    void AddWay(Way w);

    //! find node by using an ID
    bool has_node(int64_t nodeRefId) const;
    Node* FindNode(int64_t nodeRefId);

    bool has_way(int64_t way_id) const;
    Way* FindWay(int64_t way_id);

    void AddRelation(const Relation &r);

    void add_node(Way &way, const char **atts);

    /**
     * add the configuration tag used for the speeds
     */
    void add_config(Node &node, const Tag &tag) const;
    void add_config(Way &way, const Tag &tag) const;
    inline uint16_t nodeErrs() const {return m_nodeErrs;}

 private:
    void export_nodes() const;
    void export_ways() const;
    void wait_child() const;


 private:
    // ! parsed nodes TODO change to sorted vector
    Nodes m_nodes;
    //! parsed ways
    Ways m_ways;
    std::map<int64_t, Way> m_Ways;
    //! parsed relations
    std::vector<Relation> m_Relations;

    const Configuration& m_rConfig;
    po::variables_map m_vm;
    const Export2DB &m_db_conn;

    uint16_t m_nodeErrs;
    size_t m_lines;
};

}  // end namespace osm2pgr
#endif  // SRC_OSMDOCUMENT_H_
