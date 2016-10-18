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
#include <string>
#include "./Relation.h"

namespace osm2pgr {


Relation::Relation(const char **atts) :
    Element(atts) { }



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
        }
        if (key == "role") {
            role = value;
        }
    }
    m_WayRefs.push_back(osm_id);
    return osm_id;
}


}  // end namespace osm2pgr

