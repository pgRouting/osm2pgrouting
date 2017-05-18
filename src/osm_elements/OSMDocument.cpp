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
#include <algorithm>
#include <sys/wait.h>
#include "utilities/utilities.h"
#include "osm_elements/OSMDocument.h"
#include "configuration/configuration.h"
#include "osm_elements/Node.h"
#include "osm_elements/Relation.h"
#include "osm_elements/Way.h"
#include "database/Export2DB.h"

namespace osm2pgr {

OSMDocument::OSMDocument(
        const Configuration &config,
        const po::variables_map &vm,
        const Export2DB &db_conn,
        size_t lines) :
    m_rConfig(config),
    m_vm(vm),
    m_db_conn(db_conn),
    m_chunk_size(vm["chunk"].as<size_t>()),
    m_nodeErrs(0),
    m_lines(lines) {
}


void
OSMDocument::wait_child() const {
    while (true) {
        int status;
        pid_t done = wait(&status);
        if (done == -1) {
            if (errno == ECHILD) break; // no more child processes
        } else {
            if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
                cerr << "pid " << done << " failed" << endl;
                exit(1);
            }
        }
    }
}


void
OSMDocument::AddNode(const Node &n) {
    m_nodes.push_back(n);
    if ((m_nodes.size() % m_chunk_size) == 0) {
        wait_child();
        if (do_export_osm(m_nodes)) {
            osm_table_export(m_nodes, "osm_nodes");
        }
        export_pois();
    }
}

void OSMDocument::AddWay(const Way &w) {
    if (m_ways.empty()) {
        wait_child();
        if (m_vm.count("addnodes")) {
            osm_table_export(m_nodes, "osm_nodes");
        }
        export_pois();
    std::cout << "\nSaving first way\n\n\n";
    }

    m_ways.push_back(w);
    if (do_export_osm(m_ways)) {
        wait_child();
        osm_table_export(m_ways, "osm_ways");
    }
}

void
OSMDocument::AddRelation(const Relation &r) {
    if (m_vm.count("addnodes") && m_relations.empty()) {
        wait_child();
        osm_table_export(m_ways, "osm_ways");
        std::cout << "\nSaving first relation\n\n\n";
    }

    m_relations.push_back(r);
    if (do_export_osm(m_relations)) {
        wait_child();
        osm_table_export(m_relations, "osm_relations");
    }
}

void
OSMDocument::endOfFile() const {
    if (m_vm.count("addnodes")) {
        wait_child();
        osm_table_export(m_relations, "osm_relations");
    }
    std::cout << "\nEnd Of file\n\n\n";
}


template <typename T>
static
bool
less(const T &item, const int64_t &id) {
    return item.osm_id() < id;
}


Node*
OSMDocument::FindNode(int64_t node_id) {
    auto it = std::lower_bound(m_nodes.begin(), m_nodes.end(), node_id, less<Node>); 
    return &*it;
}

bool
OSMDocument::has_node(int64_t node_id) const {
    auto it = std::lower_bound(m_nodes.begin(), m_nodes.end(), node_id, less<Node>); 
    return (it != m_nodes.end());
}

Way*
OSMDocument::FindWay(int64_t way_id) {
    auto it = std::lower_bound(m_ways.begin(), m_ways.end(), way_id, less<Way>); 
    return &*it;
}

bool
OSMDocument::has_way(int64_t way_id) const {
    auto it = std::lower_bound(m_ways.begin(), m_ways.end(), way_id, less<Way>); 
    return (it != m_ways.end());
}

void
OSMDocument::add_node(Way &way, const char **atts) {
    auto **attribut = atts;
    std::string key = *attribut++;
    std::string value = *attribut++;
    auto node_id =  (key == "ref")?  boost::lexical_cast<int64_t>(value): -1;
    way.add_node(node_id);

#if 1
    // TODO leave this when splitting
    if (!has_node(node_id)) {
        ++m_nodeErrs;
    } else {
        auto node = FindNode(node_id);
        node->incrementUse();
        way.add_node(node);
    }
#endif
}

/*
 * for example
 *  <tag highway="kerb">
 *  
 *
 * And the configuration file has:
 * <type name="highway" id="1">
 *     <class name="kerb" id="101" priority="1.0" maxspeed="130" />
 *
 */

void
OSMDocument::add_config(Element *item, const Tag &tag) const {
    auto  k = tag.key();
    auto  v = tag.value();
    if (config_has_tag(tag)) {
        if (!(item->is_tag_configured())
                || (config_has_tag(item->tag_config())
                    && m_rConfig.priority(tag) < m_rConfig.priority(item->tag_config())
                   )) {
            item->tag_config(tag);
        }
    }
}

static
bool
has_no_tags(const Node &node) {
    return !node.has_tags();
}

void
OSMDocument::export_pois() const {
    std::string table("pointsofinterest");
    if (m_nodes.empty()) return;

    if (m_vm.count("fork")) {
        auto pid = fork();
        if (pid < 0) {
            std::cerr << "Failed to fork" << endl;
            exit(1);
        }
        if (pid > 0) return;
    }


    auto residue = m_nodes.size() % m_chunk_size;
    size_t start = residue? m_nodes.size() - residue : m_nodes.size() - m_chunk_size;

    auto export_items = Nodes(m_nodes.begin() + start, m_nodes.end());
    /*
     * deleting nodes with no tag information
     */
    export_items.erase(
            std::remove_if(export_items.begin(), export_items.end(), has_no_tags),
            export_items.end());

    if (!export_items.empty()) {
        m_db_conn.export_osm(export_items, table);
    }

    if (m_vm.count("fork")) {
        /*
         * finish the child process
         */
        _exit(0);
    }
}


}  // namespace osm2pgr
