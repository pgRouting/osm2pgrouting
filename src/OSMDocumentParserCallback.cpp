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
#include "./osm_tag.h"
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
#if 1
        // TODO to be used in V.2.2 for a hstore column
        if (strcmp(name, "tag") == 0) {
            last_node->add_tag(Tag(atts));
        }
#endif
        return;
    }

    if (m_section == 2) {
        if (strcmp(name, "way") == 0) {
            last_way = new Way(atts);
        };
        if (strcmp(name, "tag") == 0) {
            auto tag = last_way->add_tag(Tag(atts));
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
            if (m_rDocument.m_rConfig.has_class(k, v)) {
                if ((last_way->type().compare("") == 0 && last_way->clss().compare("") == 0)
                        || (
                            m_rDocument.m_rConfig.has_class(k, v) // k name of the type, v name of the class
                            && m_rDocument.m_rConfig.has_class(last_way->type(), last_way->clss())
                            && m_rDocument.m_rConfig.class_priority(k, v)
                            < m_rDocument.m_rConfig.class_priority(last_way->type(), last_way->clss())
                           )
                   ) {
                    last_way->type(k, v);

                    if (m_rDocument.m_rConfig.has_class(last_way->type(), last_way->clss())) {
                        last_way->add_tag(tag);


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
            m_rDocument.add_node(*last_way, atts);
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
            auto way_id = last_relation->add_member(atts);
            if (way_id == -1) return;
            assert(!last_relation->m_WayRefs.empty());
            if (m_rDocument.has_way(way_id)) {
                Way* way_ptr = m_rDocument.FindWay(way_id);
                way_ptr->insert_tags(last_relation->m_Tags);
            } else {
                assert(!last_relation->m_WayRefs.empty());
                last_relation->m_WayRefs.pop_back();
            }

            return;
        }


        if (strcmp(name, "relation") == 0) {   // THIS IS THE RELATION CODE...
            last_relation = new Relation(atts);
            return;
        }
        else if (strcmp(name, "tag") == 0) {  // END OF THE RELATIONS CODE
            if (atts != NULL) {
                std::string k;
                std::string v;
                last_relation->add_tag(atts, k, v);
                if (!k.empty()) {
                    if ((last_relation->type().compare("") == 0 && last_relation->clss().compare("") == 0)
                            || (
                                m_rDocument.m_rConfig.has_class(k, v) // k name of the type, v name of the class
                                && m_rDocument.m_rConfig.has_class(last_relation->type(), last_relation->clss())
                                && m_rDocument.m_rConfig.class_priority(k, v)
                                < m_rDocument.m_rConfig.class_priority(last_relation->type(), last_relation->clss())
                               )
                       ) {
                        last_relation->type(k);
                        last_relation->clss(v);
                    }
                    if (last_relation && k == "name") {
                        last_relation->name = v;
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
        if (m_rDocument.m_rConfig.has_class(last_relation->type(), last_relation->clss())) {
            for (const auto &way_id: last_relation->m_WayRefs) {

                assert(m_rDocument.has_way(way_id));
                if (m_rDocument.has_way(way_id)) {
                    Way* way_ptr = m_rDocument.FindWay(way_id);
                    way_ptr->type(last_relation->type(), last_relation->clss());
                    auto newValue = m_rDocument.m_rConfig.class_default_maxspeed(last_relation->type(), last_relation->clss());
                    if (way_ptr->maxspeed_forward() <= 0) {
                        way_ptr->maxspeed_forward(newValue);
                    }
                    if (way_ptr->maxspeed_backward() <= 0) {
                        way_ptr->maxspeed_backward(newValue);
                    }
                }
            }
            m_rDocument.AddRelation(*last_relation);
        }
        delete last_relation;
        last_relation = nullptr;

    } else if (strcmp(name, "osm") == 0) {
        show_progress();
    }
}

}  // end namespace osm2pgr
