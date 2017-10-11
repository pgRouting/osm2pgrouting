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

#include "configuration/tag_key.h"
#include "utilities/utilities.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <map>

namespace osm2pgr {


Tag_key::Tag_key(const char **atts) 
    : Element(atts) {
}

void
Tag_key::add_tag_value(const Tag_value &value) {
    m_Tag_values[value.name()] =  value;
}

bool
Tag_key::has_tag_value(const Tag &tag) const {
    return m_Tag_values.count(tag.value());
}

const
Tag_value&
Tag_key::tag_value(
        const Tag &tag) const {
    return m_Tag_values.at(tag.value());
}

bool
Tag_key::has(const Tag &tag, const std::string &str) const {
    return tag_value(tag).has_attribute(str)
           || this->has_attribute(str);
}

std::string
Tag_key::get(const Tag &tag, const std::string &str) const {
    assert(this->has(tag, str));
    return (tag_value(tag).has_attribute(str)) ?
        tag_value(tag).get(str)
        : this->get_attribute(str);
}


std::vector<std::string> 
Tag_key::values(const std::vector<std::string> &columns) const {
    std::vector<std::string> export_values;

    for (const auto &item : m_Tag_values) {
        auto row = item.second.values(columns, true);
        row[1] = name();
        row[2] = item.second.get_attribute("name"); 
        // row[3] has priority
        if (row[4] == "") row[4] = "40"; // max_speed
        if (row[5] == "") row[5] = row[4]; 
        if (row[6] == "") row[6] = row[4];
        if (row[7] == "") row[7] = "N"; 
        export_values.push_back(tab_separated(row));
    }
    return export_values;
}


}  // end namespace osm2pgr
