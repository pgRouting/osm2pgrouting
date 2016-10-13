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
        if (strcmp(name, "tag") == 0) {
            last_node->add_tag(atts);
        }
#endif
        return;
    }


    // START RELATIONS CODE
    if (strcmp(name, "member") == 0) {
        // std::cout << "In member..." << std::endl;

        if (m_pActRelation && atts != NULL) {
            const char** attribut = (const char**)atts;
            while (*attribut != NULL) {
                const char* name = *attribut++;
                const char* value = *attribut++;
                if (strcmp(name, "ref") == 0) {
                    int64_t wayRefId = atoll(value);
                    m_pActRelation->AddWayRef(wayRefId);
                }
            }
        }
    }
    // END RELATIONS CODE


    if (strcmp(name, "nd") == 0) {
        if (m_pActWay && atts != NULL) {
            const char* name = *atts++;
            const char* value = *atts++;
            if (strcmp(name, "ref") == 0) {
                int64_t nodeRefId = atoll(value);
                m_pActWay->AddNodeRef(m_rDocument.FindNode(nodeRefId));
                Node * node = m_rDocument.FindNode(nodeRefId);
                if (node != 0) {
                    node->incrementUse();
                } else {
                    std::cout << "Reference nd=" << nodeRefId
                        << " has no corresponding Node Entry (Maybe Node entry after Reference?)" << std::endl;
                }
            }
        }
    } else if (strcmp(name, "relation") == 0) {   // THIS IS THE RELATION CODE...
        if (atts != NULL) {
            int64_t id =-1;
            const char** attribut = (const char**)atts;
            while (*attribut != NULL) {
                const char* name = *attribut++;
                const char* value = *attribut++;
                if (strcmp(name, "id") == 0) {
                    id = atoll(value);
                }
            }
            if (id > 0) m_pActRelation = new Relation(id);
            // std::cout<<"Starting relation: "<<id<<std::endl;
        }
    } else if (strcmp(name, "tag") == 0) {  // END OF THE RELATIONS CODE
        // <tag k="name" v="Pfnderweg"/>
        if (atts != NULL) {
            std::string k;
            std::string v;
            const char** attribut = (const char**)atts;
            while (*attribut != NULL) {
                const char* name = *attribut++;
                const char* value = *attribut++;
                if (strcmp(name, "k") == 0) {
                    k = value;
                } else if (strcmp(name, "v") == 0) {
                    v = value;
                }
            }
            if (!k.empty()) {
                if (m_pActWay) {
                  m_pActWay->oneWay(k, v);
                  m_pActWay->max_speed(k, v);
                }
                //  CHECKING OUT SOME DATA...
                // std::cout<<"k: "<<k<<", v: "<<v<<std::endl;
                // std::cout<<"m_pActWay: "<<m_rDocument.m_rConfig.m_Types.count(k)<<std::endl;
                // std::cout<<"thecount: "<<m_rDocument.m_rConfig.m_Types.count(k)<<std::endl;
                if (m_pActWay && k.compare("name") == 0) {
                    m_pActWay->name(v);
                }
                else if (m_pActWay && m_rDocument.m_rConfig.has_class(k, v)) {
                    if ((m_pActWay->type().compare("") == 0 && m_pActWay->clss().compare("") == 0)
                            || (
                                m_rDocument.m_rConfig.has_class(k, v) // k name of the type, v name of the class
                                && m_rDocument.m_rConfig.has_class(m_pActWay->type(), m_pActWay->clss())
                                && m_rDocument.m_rConfig.class_priority(k, v)
                                < m_rDocument.m_rConfig.class_priority(m_pActWay->type(), m_pActWay->clss())
                               )
                       ) {
                        m_pActWay->type(k);
                        m_pActWay->clss(v);

                        if (m_rDocument.m_rConfig.has_class(m_pActWay->type(), m_pActWay->clss())) {
                            m_pActWay->AddTag(k, v);


                            // set default maxspeed values from classes, if not set previously (default: -1)
                            auto newValue = m_rDocument.m_rConfig.class_default_maxspeed(m_pActWay->type(), m_pActWay->clss());
                            if (m_pActWay->maxspeed_forward() <= 0) {
                                m_pActWay->maxspeed_forward(newValue);
                            }
                            if (m_pActWay->maxspeed_backward() <= 0) {
                                m_pActWay->maxspeed_backward(newValue);
                            }
                        }
                    }
                } else if (m_pActRelation && m_rDocument.m_rConfig.has_class(k, v) )  {
                    m_pActRelation->AddTag(k, v);
                }

                if (m_pActRelation && k.compare("name") == 0) {
                    m_pActRelation->name = v;
                }
                // END TAG FOR RELATION
            }
        }
    } else if (strcmp(name, "way") == 0) {
        m_pActWay = new Way(atts);
    } else if (strcmp(name, "osm") == 0) {
    }
}

void OSMDocumentParserCallback::EndElement(const char* name) {
    if (strcmp(name, "node") == 0) {
        m_rDocument.AddNode(*last_node);
        std::cout << "\nadded Node: " << last_node->osm_id();
        delete last_node;
        return;
    }
    if (strcmp(name, "way") == 0) {

        if (m_rDocument.m_rConfig.has_class(m_pActWay->type(), m_pActWay->clss())) {

            m_rDocument.AddWay(*m_pActWay);
            std::cout << "\nadded Way: " << m_pActWay->osm_id();

        }
        delete m_pActWay;
        m_pActWay = nullptr;
        return;
    } else if (strcmp(name, "relation") == 0) {
        // THIS IS THE RELATION CODE...
        m_rDocument.AddRelation(m_pActRelation);
        m_pActRelation = 0;

        // std::cout<<"Adding relation: "<<m_pActRelation->id<<std::endl;
    } else if (strcmp(name, "osm") == 0) {
        show_progress();
    }
    // END OF THE RELATIONS CODE
}

}  // end namespace osm2pgr
