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



#include "parser/OSMDocumentParserCallback.h"

#include <math.h>
#include <string>
#include <cassert>
#include <iostream>
#include <sstream>
#include "osm_elements/OSMDocument.h"
#include "osm_elements/Relation.h"
#include "osm_elements/osm_tag.h"
#include "osm_elements/Way.h"
#include "osm_elements/Node.h"
#include "utilities/print_progress.h"


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
    try {
        if (m_line == 0) return;
        if (m_rDocument.lines() == 0) return;
        if (((++m_line) % (m_rDocument.lines() / 100)) == 0) {
            print_progress(m_rDocument.lines(), m_line);
        }
    } catch(...) {
        m_line = 1;
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

    if (strcmp(name, "osm") == 0) {
        m_section = 1;
    }

    if ((m_section == 1 && (strcmp(name, "way") == 0))
            || (m_section == 2 && (strcmp(name, "relation") == 0))) {
        ++m_section;
    }


    if (m_section == 1) {
        if (strcmp(name, "node") == 0) {
            last_node = new Node(atts);
        }
        if (strcmp(name, "tag") == 0) {
            auto tag = last_node->add_tag(Tag(atts));
            m_rDocument.add_config(last_node, tag);
        }
        return;
    }

    if (m_section == 2) {
        if (strcmp(name, "way") == 0) {
            last_way = new Way(atts);
        }
        if (strcmp(name, "tag") == 0) {
            auto tag = last_way->add_tag(Tag(atts));
            m_rDocument.add_config(last_way, tag);
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
        if (strcmp(name, "relation") == 0) {
            last_relation = new Relation(atts);
            return;
        }

        if (strcmp(name, "member") == 0) {
            /*
               <member type="node" ref="721818679" role="label"/>
               <member type="way" ref="173424370" role=""/>
               <member type="way" ref="48435091" role="link"/>
               */
            auto way_id = last_relation->add_member(atts);
            if (way_id == -1) return;
            assert(!last_relation->way_refs().empty());
            if (m_rDocument.has_way(way_id)) {
                Way* way_ptr = m_rDocument.FindWay(way_id);
                way_ptr->insert_tags(last_relation->tags());
            } else {
                assert(!last_relation->way_refs().empty());
                last_relation->way_refs().pop_back();
            }

            return;
        }
        if (strcmp(name, "tag") == 0) {
            auto tag = last_relation->add_tag(Tag(atts));
            m_rDocument.add_config(last_relation, tag);
        }
        return;
    }
}

void OSMDocumentParserCallback::EndElement(const char* name) {
    if (strcmp(name, "osm") == 0) {
        m_rDocument.endOfFile();
        show_progress();
        return;
    }

    if (strcmp(name, "node") == 0) {
        m_rDocument.AddNode(*last_node);
        delete last_node;
        return;
    }
    if (strcmp(name, "way") == 0) {
        m_rDocument.AddWay(*last_way);
        if (m_rDocument.config_has_tag(last_way->tag_config())) {

            auto maxspeed = m_rDocument.maxspeed(last_way->tag_config());
            if (last_way->maxspeed_forward() <= 0) {
                last_way->maxspeed_forward(maxspeed);
            }
            if (last_way->maxspeed_backward() <= 0) {
                last_way->maxspeed_backward(maxspeed);
            }
        }
        delete last_way;
        return;
    }

    if (strcmp(name, "relation") == 0) {
        m_rDocument.AddRelation(*last_relation);
        if (m_rDocument.config_has_tag(last_relation->tag_config())) {
            for (auto it = last_relation->way_refs().begin();  it != last_relation->way_refs().end(); ++it) {
                auto way_id = *it;
                assert(m_rDocument.has_way(way_id));
                if (m_rDocument.has_way(way_id)) {
                    Way* way_ptr = m_rDocument.FindWay(way_id);
                    way_ptr->tag_config(last_relation->tag_config());
                    auto newValue = m_rDocument.maxspeed(
                            last_relation->tag_config());
                    if (way_ptr->maxspeed_forward() <= 0) {
                        way_ptr->maxspeed_forward(newValue);
                    }
                    if (way_ptr->maxspeed_backward() <= 0) {
                        way_ptr->maxspeed_backward(newValue);
                    }
                }
            }
        }
        delete last_relation;
        return;
    } 
}

}  // end namespace osm2pgr
