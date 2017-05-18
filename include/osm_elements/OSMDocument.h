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

#include <iostream>
#include <map>
#include <vector>
#include "utilities/utilities.h"
#include "configuration/configuration.h"
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
    typedef std::vector<Relation> Relations;

    //! Constructor
    OSMDocument(
            const Configuration& config,
            const po::variables_map &vm,
            const Export2DB &db_conn,
            size_t lines);

    inline size_t lines() const {return m_lines;}

    //! Do the configuration has the @b tag ?
    inline bool config_has_tag(const Tag &tag) const {
        return m_rConfig.has_tag(tag);
    }

    inline double priority(const Tag &tag) const {
        return m_rConfig.priority(tag);
    }

    inline double maxspeed(const Tag &tag) const {
        return m_rConfig.maxspeed(tag);
    }

    const Nodes& nodes() const {return m_nodes;}
    const Ways& ways() const {return m_ways;}
    const Relations& relations() const {return m_relations;}

    void AddNode(const Node &n);
    void AddWay(const Way &w);
    void AddRelation(const Relation &r);
    void endOfFile() const;

    //! find node by using an ID
    bool has_node(int64_t nodeRefId) const;
    Node* FindNode(int64_t nodeRefId);

    bool has_way(int64_t way_id) const;
    Way* FindWay(int64_t way_id);


    void add_node(Way &way, const char **atts);

    /**
     * add the configuration tag used for the speeds
     */
    void add_config(Element *osm_element, const Tag &tag) const;

    inline uint16_t nodeErrs() const {return m_nodeErrs;}

 private:
    template <typename T>
        bool
        do_export_osm(const T &container) {
            return m_vm.count("addnodes") && (container.size() % m_chunk_size) == 0;
        }

    void wait_child() const;

    template <typename T>
        void
        osm_table_export(const T &osm_items, const std::string &table) const {
            if (osm_items.empty()) return;

            if (m_vm.count("addnodes")) {
                auto pid = fork();
                if (pid < 0) {
                    std::cerr << "Failed to fork" << endl;
                    exit(1);
                }
                if (pid > 0) {
                    return;
                }
            }
            auto residue = osm_items.size() % m_chunk_size;
            size_t start = residue? osm_items.size() - residue : osm_items.size() - m_chunk_size;
            auto export_items = T(osm_items.begin() + start, osm_items.end());

            m_db_conn.export_osm(export_items, table);

            if (m_vm.count("addnodes")) {
                /*
                 * finish the child process
                 */
                _exit(0);
            }
        }

   void export_pois() const;


 private:
    // ! parsed nodes TODO change to sorted vector
    Nodes m_nodes;
    //! parsed ways
    Ways m_ways;
    //! parsed relations
    Relations  m_relations;

    const Configuration& m_rConfig;
    po::variables_map m_vm;
    const Export2DB &m_db_conn;

    size_t m_chunk_size;
    uint16_t m_nodeErrs;
    size_t m_lines;
};

}  // end namespace osm2pgr
#endif  // SRC_OSMDOCUMENT_H_
