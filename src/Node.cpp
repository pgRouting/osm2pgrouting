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

#include "./Node.h"

namespace osm2pgr {

Node::Node(int64_t p_id, double p_lat, double p_lon) :
    m_id(p_id),
    m_lat(p_lat),
    m_lon(p_lon),
    m_numsOfUse(0) {
}

Node::Node(const char **atts) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        const char* name = *attribut++;
        const char* value = *attribut++;
        if (strcmp(name, "id") == 0) {
            m_id = boost::lexical_cast<int64_t>(value);
        } else if (strcmp(name, "lat") == 0) {
            m_lat = boost::lexical_cast<double>(value);
        } else if (strcmp(name, "lon") == 0) {
            m_lon = boost::lexical_cast<double>(value);
        } else {
            auto tag_key = boost::lexical_cast<std::string>(name);
            auto tag_value = boost::lexical_cast<std::string>(value);
            tags[tag_key] = tag_value;
        }
    }
}

void
Node::add_tag(const char **atts) {
    auto **attribut = atts;
    std::string tag_key, tag_value;
    while (*attribut != NULL) {
        const char* name = *attribut++;
        const char* value = *attribut++;
        if (strcmp(name, "k") == 0) {
            tag_key = boost::lexical_cast<std::string>(value);
        } else if (strcmp(name, "v") == 0) {
            tag_value = boost::lexical_cast<std::string>(value);
        }
    }
    tags[tag_key] = tag_value;
}

}  // namespace osm2pgr
