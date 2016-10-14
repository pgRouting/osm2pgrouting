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

#include <boost/lexical_cast.hpp>
#include <string>
#include "./Relation.h"

namespace osm2pgr {

#if 0
Relation::Relation(int64_t id)
:
    m_osm_id(id) {
}
#endif

Relation::Relation(const char **atts) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        std::string key = *attribut++;
        std::string value = *attribut++;
        if (key == "id") {
            m_osm_id = boost::lexical_cast<int64_t>(value);
        } else if (key == "visible") {
            m_visible = boost::lexical_cast<bool>(value);
        } else {
            m_attributes[key] = value;
        }
    }
}


#if 0
Relation::~Relation() {
}
#endif

int64_t
Relation::add_member(const char **atts) {
    auto **attribut = atts;
    std::string type;
    int64_t osm_id(0);
    std::string role;
    while (*attribut != NULL) {
        std::string key = *attribut++;
        std::string value = *attribut++;
        /*
         * currently only adding way
         */
        if (key == "type") {
            if (value != "way") return -1;
        }
        if (key == "ref") {
            osm_id = boost::lexical_cast<int64_t>(value);
        };
        if (key == "role") {
            role = value;
        };
    }
    AddWayRef(osm_id, "way", role);
    return osm_id;
}


void
Relation::add_tag(const char **atts, std::string &key, std::string &value) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        std::string k = *attribut++;
        std::string v = *attribut++;
        if (k == "k") {
            key = v;
        } if (k == "v") {
            value = v;
        }
    }
    /* store the tag as originaly recieved*/
    m_Tags[key] = value;
}


void Relation::AddWayRef(int64_t osm_id, const std::string &k, const std::string &v) {
    if (k == "way") {
        m_WayRefs.push_back(osm_id);
    }
}

void Relation::AddWayRef(int64_t pID) {
    if (pID) m_WayRefs.push_back(pID);
}

void Relation::AddTag(std::string key, std::string value) {
    m_Tags[key] = value;
}

}  // end namespace osm2pgr

