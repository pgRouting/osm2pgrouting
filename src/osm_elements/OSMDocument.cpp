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
#include "configuration/Configuration.h"
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


void OSMDocument::AddNode(const Node &n) {
    m_nodes.push_back(n);
    if (m_vm.count("addnodes") && (m_nodes.size() % m_vm["chunk"].as<size_t>()) == 0) {
        /*
         * before starting next export wait for last export
         */
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
        export_nodes();
    }
}

void OSMDocument::AddWay(Way w) {
    if (m_vm.count("addnodes") && m_Ways.empty()) {
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
        export_nodes();
        std::cout << "\nSaving first way\n\n\n";
    }
    m_Ways[w.osm_id()] = w;
}

void OSMDocument::AddRelation(const Relation &r) {
    m_Relations.push_back(r);
}

static
bool
less(const Node n, const int64_t &id) {
    return n.osm_id() < id;
}


Node*
OSMDocument::FindNode(int64_t nodeRefId) {
    auto it = std::lower_bound(m_nodes.begin(), m_nodes.end(), nodeRefId, less); 
    return &*it;
}

bool
OSMDocument::has_node(int64_t nodeRefId) const {
    auto it = std::lower_bound(m_nodes.begin(), m_nodes.end(), nodeRefId, less); 
    return (it != m_nodes.end());
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
        ++m_nodeErrs;
    } else {
        auto node = FindNode(node_id);
        node->incrementUse();
        way.add_node(node);
    }
}

void
OSMDocument::add_config(Node &node, const Tag &tag) const {
    auto  k = tag.key();
    auto  v = tag.value();
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
    if (m_rConfig.has_class(tag)) {
        if ((node.tag_config().key() == "" && node.tag_config().value() == "")
                || (
                    m_rConfig.has_class(tag)
                    && m_rConfig.has_class(node.tag_config())
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
        }
    }
}

/* @brief fork process to export a chink of nodes 
 *
 * TODO steps
 *
 * 1) connect to the database using the string connection
 * 2) copy the code from Export
 * 3) export finishes then start exit
 *
 * @params[in] nodes the chunk of nodes to be exported
 */
void
OSMDocument::export_nodes() const {
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

    m_db_conn.exportNodes(nodes);



    /*
     * finish the child process
     */
    exit(0);
}




}  // namespace osm2pgr
