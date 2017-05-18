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
#include "osm_elements/Relation.h"

namespace osm2pgr {


Relation::Relation(const char **atts) :
    Element(atts) { }



int64_t
Relation::add_member(const char **atts) {
#if 0
    std::cout << " --> " << __PRETTY_FUNCTION__ << "\n";
#endif
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
#if 0
    std::cout << "members" << members_str() << "\n";
    std::cout << " <-- " << __PRETTY_FUNCTION__ << "\n";
#endif
    return osm_id;
}

std::string
Relation::members_str() const {
    std::string way_list("");
    for (const auto &way_ref : m_WayRefs) {
        way_list += boost::lexical_cast<std::string>(way_ref)
        /*
         * currently only adding way
         */
            + "=>\"type=>way\",";
    }
    way_list[way_list.size() -1] = ' ';

    return way_list;
}

std::ostream& operator<<(std::ostream &os, const Relation &r) {
    os << r.members_str();
    return os;
}

}  // end namespace osm2pgr

