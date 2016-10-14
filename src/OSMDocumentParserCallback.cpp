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


#include "./OSMDocumentParserCallback.h"

#include <math.h>
#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include "./OSMDocument.h"
#include "./print_progress.h"
#include "./Relation.h"
#include "./Way.h"
#include "./Node.h"

// define here, which streetstype you want to parse
// for applying this filter, compile with "DISTRICT" as flag (g++ -DRESTRICT)
// #define _FILTER if (m_pActWay->highway == "motorway" || m_pActWay->highway == "primary" || m_pActWay->highway == "secondary")

namespace osm2pgr {

/*
<relation id ="147411" version="5" timestamp="2010-01-22T17:02:14Z" uid ="24299" user="james_hiebert" changeset="3684904">
    <member type="way" ref="25584788" role=""/>
    <member type="way" ref="35064036" role=""/>
    <member type="way" ref="35064035" role=""/>
    <member type="way" ref="35064037" role=""/>
    <member type="way" ref="35064038" role=""/>
    <member type="way" ref="35065746" role=""/>
    <member type="way" ref="48690404" role=""/>
    <member type="way" ref="24221632" role=""/>
    <tag k="name" v="Mt. Douglas Park Local Connector"/>
    <tag k="network" v="rcn"/>
    <tag k="route" v="bicycle"/>
    <tag k="type" v="route"/>
  </relation>
 */

void
OSMDocumentParserCallback::show_progress() {
    if ((++m_line % (m_rDocument.m_lines / 100)) == 0) {
        print_progress(m_rDocument.m_lines, m_line);
        std::cout << " Total Processed: " << m_line;
    }
}


/**
  Parser callback for OSMDocument files
  */
void
OSMDocumentParserCallback::StartElement(
        const char *name,
        const char** atts) {

    show_progress();
    if ((m_section == 1 && (strcmp(name, "way") == 0))
            || (m_section == 2 && (strcmp(name, "relation") == 0))) {
        ++m_section;
    }


    if (m_section == 1) {
        if (strcmp(name, "node") == 0) {
            last_node = new Node(atts);
        };
#if 0
        // TODO to be used in V.2.2 for a hstore column
        if (strcmp(name, "tag") == 0) {
            last_node->add_tag(atts);
        }
#endif
        return;
    }

    if (m_section == 2) {
        if (strcmp(name, "way") == 0) {
            last_way = new Way(atts);
        };
        if (strcmp(name, "tag") == 0) {
            std::string k;
            std::string v;
            last_way->add_tag(atts, k, v);
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
            if (m_rDocument.m_rConfig.has_class(k, v)) {
                if ((last_way->type().compare("") == 0 && last_way->clss().compare("") == 0)
                        || (
                            m_rDocument.m_rConfig.has_class(k, v) // k name of the type, v name of the class
                            && m_rDocument.m_rConfig.has_class(last_way->type(), last_way->clss())
                            && m_rDocument.m_rConfig.class_priority(k, v)
                            < m_rDocument.m_rConfig.class_priority(last_way->type(), last_way->clss())
                           )
                   ) {
                    last_way->type(k);
                    last_way->clss(v);

                    if (m_rDocument.m_rConfig.has_class(last_way->type(), last_way->clss())) {
                        last_way->AddTag(k, v);


                        // set default maxspeed values from classes, if not set previously (default: -1)
                        auto newValue = m_rDocument.m_rConfig.class_default_maxspeed(last_way->type(), last_way->clss());
                        if (last_way->maxspeed_forward() <= 0) {
                            last_way->maxspeed_forward(newValue);
                        }
                        if (last_way->maxspeed_backward() <= 0) {
                            last_way->maxspeed_backward(newValue);
                        }
                    }
                }
            }
        }

        if (strcmp(name, "nd") == 0) {
            auto nodeRefId = last_way->add_node(atts);
            last_way->AddNodeRef(m_rDocument.FindNode(nodeRefId));
            auto node = m_rDocument.FindNode(nodeRefId);
            if (node != 0) {
                node->incrementUse();
            } else {
                std::cout << "Reference nd=" << nodeRefId
                    << " has no corresponding Node Entry (Maybe Node entry after Reference?)" << std::endl;
            }
        }    
        return;
    }

    if (m_section == 3) {
        /*
         *  START RELATIONS CODE
         */
        if (strcmp(name, "member") == 0) {
            /*
               <member type="node" ref="721818679" role="label"/>
               <member type="way" ref="173424370" role=""/>
               <member type="way" ref="48435091" role="link"/>
               */
            auto way_id = m_pActRelation->add_member(atts);
            if (way_id == -1) return;
            assert(!m_pActRelation->m_WayRefs.empty());
            if (m_rDocument.has_way(way_id)) {
                Way* way_ptr = m_rDocument.FindWay(way_id);
                way_ptr->m_Tags.insert(m_pActRelation->m_Tags.begin(),  m_pActRelation->m_Tags.end());
            } else {
                assert(!m_pActRelation->m_WayRefs.empty());
                m_pActRelation->m_WayRefs.pop_back();
            }

            return;
        }


        if (strcmp(name, "relation") == 0) {   // THIS IS THE RELATION CODE...
            m_pActRelation = new Relation(atts);
            return;
        }
        else if (strcmp(name, "tag") == 0) {  // END OF THE RELATIONS CODE
            if (atts != NULL) {
                std::string k;
                std::string v;
                m_pActRelation->add_tag(atts, k, v);
                if (!k.empty()) {
                    if ((m_pActRelation->type().compare("") == 0 && m_pActRelation->clss().compare("") == 0)
                            || (
                                m_rDocument.m_rConfig.has_class(k, v) // k name of the type, v name of the class
                                && m_rDocument.m_rConfig.has_class(m_pActRelation->type(), m_pActRelation->clss())
                                && m_rDocument.m_rConfig.class_priority(k, v)
                                < m_rDocument.m_rConfig.class_priority(m_pActRelation->type(), m_pActRelation->clss())
                               )
                       ) {
                        m_pActRelation->type(k);
                        m_pActRelation->clss(v);
                    }
                    if (m_pActRelation && k == "name") {
                        m_pActRelation->name = v;
                    }
                }
            }
        }
    } else if (strcmp(name, "osm") == 0) {
    }
}

void OSMDocumentParserCallback::EndElement(const char* name) {
    if (strcmp(name, "node") == 0) {
        m_rDocument.AddNode(*last_node);
        delete last_node;
        return;
    }
    if (strcmp(name, "way") == 0) {
        m_rDocument.AddWay(*last_way);
        delete last_way;
        last_way = nullptr;
        return;

    } else if (strcmp(name, "relation") == 0) {
        if (m_rDocument.m_rConfig.has_class(m_pActRelation->type(), m_pActRelation->clss())) {
            for (const auto &way_id: m_pActRelation->m_WayRefs) {

                assert(m_rDocument.has_way(way_id));
                if (m_rDocument.has_way(way_id)) {
                    Way* way_ptr = m_rDocument.FindWay(way_id);
                    way_ptr->clss(m_pActRelation->clss());
                    way_ptr->type(m_pActRelation->type());
                    auto newValue = m_rDocument.m_rConfig.class_default_maxspeed(m_pActRelation->type(), m_pActRelation->clss());
                    if (way_ptr->maxspeed_forward() <= 0) {
                        way_ptr->maxspeed_forward(newValue);
                    }
                    if (way_ptr->maxspeed_backward() <= 0) {
                        way_ptr->maxspeed_backward(newValue);
                    }
                }
            }
            m_rDocument.AddRelation(m_pActRelation);
        }
        m_pActRelation = 0;

    } else if (strcmp(name, "osm") == 0) {
        show_progress();
    }
}

}  // end namespace osm2pgr
