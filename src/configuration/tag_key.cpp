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

void Tag_key::AddTag_value(const Tag_value &pTag_value) {
    std::cout << "\nadding " << pTag_value.name() << "\t" << m_Tag_values.size();;
    m_Tag_values[pTag_value.name()] = pTag_value;
    std::cout << "\nadding " << "\t" << m_Tag_values.size();;
}


Tag_key::Tag_key(const char **atts) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        std::string name = *attribut++;
        std::string value = *attribut++;
        if (name == "id") {
            m_id = boost::lexical_cast<int64_t>(value);
        } else if (name == "name") {
            m_name = value;
        } else {
            auto tag_key = boost::lexical_cast<std::string>(name);
            auto tag_value = boost::lexical_cast<std::string>(value);
            m_tags[tag_key] = tag_value;
        }
    }
}

void
Tag_key::add_class(const Tag_value &p_values) {
    std::cout << "\nadding " << p_values.name() << "\t before:" << m_Tag_values.size();;
    m_Tag_values[ p_values.name()] =  p_values;
    std::cout << "\t after:" << m_Tag_values.size();;
}


std::vector<std::string> 
Tag_key::values(const std::vector<std::string> &columns) const {
    std::vector<std::string> export_values;

    for (const auto &item : m_Tag_values) {
        auto row = item.second.values(columns, true);
        row[1] = m_name;
        export_values.push_back(tab_separated(row));
        std::cout << tab_separated(row);
    }
    return export_values;
}


}  // end namespace osm2pgr
