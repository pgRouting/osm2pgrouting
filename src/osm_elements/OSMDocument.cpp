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

template <typename T>
static
bool
do_export_osm(const T &container, const po::variables_map &vm) {
    return vm.count("addnodes") && (container.size() % vm["chunk"].as<size_t>()) == 0; 
}



void
OSMDocument::AddNode(const Node &n) {
    m_nodes.push_back(n);
    if (do_export_osm(m_nodes, m_vm)) {
        wait_child();
        export_nodes();
    }
}

void OSMDocument::AddWay(const Way &w) {
    if (m_vm.count("addnodes") && m_ways.empty()) {
        wait_child();
        export_nodes();
        std::cout << "\nSaving first way\n\n\n";
    }

    m_ways.push_back(w);
    // if (m_vm.count("addnodes") && (m_ways.size() % m_vm["chunk"].as<size_t>()) == 0) {
    if (do_export_osm(m_ways, m_vm)) {
        wait_child();
        export_ways();
    }
}

void
OSMDocument::AddRelation(const Relation &r) {
    if (m_vm.count("addnodes") && m_relations.empty()) {
        wait_child();
        export_ways();
        std::cout << "\nSaving first relation\n\n\n";
    }

    m_relations.push_back(r);
    if (do_export_osm(m_relations, m_vm)) {
        wait_child();
        export_relations();
    }
}

void
OSMDocument::endOfFile() const {
    if (m_vm.count("addnodes")) {
        wait_child();
        export_relations();
        std::cout << "\nEnd Of file\n\n\n";
    }
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
 * max_speed currently ignored for nodes
 */

void
OSMDocument::add_config(Node &node, const Tag &tag) const {
    auto  k = tag.key();
    auto  v = tag.value();
    if (config_has_tag(tag)) {
        if (!(node.is_tag_configured())
                || (config_has_tag(tag)
                    && config_has_tag(node.tag_config())
                    && m_rConfig.class_priority(tag)
                    < m_rConfig.class_priority(node.tag_config())
                   )) {
            node.tag_config(tag);
        }
    }
}

void
OSMDocument::add_config(Way &way, const Tag &tag) const {
    auto  k = tag.key();
    auto  v = tag.value();
    if (config_has_tag(tag)) {
        if (!(way.is_tag_configured()) 
                || (config_has_tag(tag)
                    && config_has_tag(way.tag_config())
                    && m_rConfig.class_priority(tag) < m_rConfig.class_priority(way.tag_config())
                   )
           ) {
            way.tag_config(tag);

#if 0
            if (config_has_tag(way.tag_config())) {

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

void
OSMDocument::add_config(Relation &relation, const Tag &tag) const {
    auto  k = tag.key();
    auto  v = tag.value();
    if (config_has_tag(tag)) {
        if (!(relation.is_tag_configured())
                || (config_has_tag(tag)
                    && config_has_tag(relation.tag_config())
                    && m_rConfig.class_priority(tag)
                    < m_rConfig.class_priority(relation.tag_config())
                   )) {
            relation.tag_config(tag);
        }
    }
}

/* @brief fork process to export a chunk of nodes 
 *
 */
void
OSMDocument::export_nodes() const {
    if (m_nodes.empty()) return;
    auto pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork" << endl;
        // TODO throw
        exit(1);
    }
    if (pid > 0) return;
    /*
     * TODO make it a member
     */
    size_t m_chunk_size =  m_vm["chunk"].as<size_t>();
    /* todo end */

    auto residue = m_nodes.size() % m_chunk_size;
    size_t start = residue? m_nodes.size() - residue : m_nodes.size() - m_chunk_size; 
#if 0
    std::cout << "\n\t" << getpid() << "\t\texporting nodes" << start << " to " << m_nodes.size() << "\t Total" << (m_nodes.size() - start) << "\n";
#endif
    auto nodes = Nodes(m_nodes.begin() + start, m_nodes.end());

    m_db_conn.export_osm(nodes, "osm_nodes");

    /*
     * finish the child process
     */
    exit(0);
}




/* @brief fork process to export a chunk of ways 
 *
 */
void
OSMDocument::export_ways() const {
    if (m_ways.empty()) return;
    auto pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork" << endl;
        // TODO throw
        exit(1);
    }
    if (pid > 0) return;
    /*
     * TODO make it a member
     */
    size_t m_chunk_size =  m_vm["chunk"].as<size_t>();
    /* todo end */

    auto residue = m_ways.size() % m_chunk_size;
    size_t start = residue? m_ways.size() - residue : m_ways.size() - m_chunk_size; 
#if 0
    std::cout << "\n\t" << getpid() << "\t\texporting nodes" << start << " to " << m_nodes.size() << "\t Total" << (m_nodes.size() - start) << "\n";
#endif
    auto ways = Ways(m_ways.begin() + start, m_ways.end());

    m_db_conn.export_osm(ways, "osm_ways");

    /*
     * finish the child process
     */
    exit(0);
}

/* @brief fork process to export a chunk of relations 
 *
 */
void
OSMDocument::export_relations() const {
    if (m_relations.empty()) return;
    auto pid = fork();
    if (pid < 0) {
        std::cerr << "Failed to fork" << endl;
        // TODO throw
        exit(1);
    }
    if (pid > 0) return;
    /*
     * TODO make it a member
     */
    size_t m_chunk_size =  m_vm["chunk"].as<size_t>();
    /* todo end */

    auto residue = m_relations.size() % m_chunk_size;
    size_t start = residue? m_relations.size() - residue : m_relations.size() - m_chunk_size; 
#if 1
    std::cout << "\n\t" << getpid() << "\t\texporting relations" << start << " to " << m_relations.size() << "\t Total" << (m_relations.size() - start) << "\n";
#endif
    auto relations = Relations(m_relations.begin() + start, m_relations.end());

    m_db_conn.export_osm(relations, std::string("osm_relations"));

    /*
     * finish the child process
     */
    exit(0);
}




}  // namespace osm2pgr
