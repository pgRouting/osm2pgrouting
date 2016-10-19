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
#include <map>
#include <string>
#include "./osm_tag.h"
#include "./osm_element.h"

namespace osm2pgr {


Element::Element(const char **atts) :
    m_visible(true) {
        auto **attribut = atts;
        while (*attribut != NULL) {
            std::string name = *attribut++;
            std::string value = *attribut++;
            if (name == "id") {
                m_osm_id = boost::lexical_cast<int64_t>(value);
            }
            if (name == "visible") {
                m_visible = (value == "true")? true : false;
            }
            m_attributes[name] = value;
        }
    }

Tag
    Element::add_tag(const Tag &tag) {
        m_tags[tag.key()] = tag.value();
        return tag;
    }

bool
    Element::has_tag(const std::string& key) const {
        return m_tags.find(key) != m_tags.end();
    }

std::string
    Element::get_tag(const std::string& key) const {
        return m_tags.find(key)->second;
    }

bool
    Element::has_attribute(const std::string& key) const {
        return m_attributes.find(key) != m_attributes.end();
    }

std::string
    Element::get_attribute(const std::string& key) const {
        return m_attributes.find(key)->second;
    }

std::string Element::attributes_str() const {
    if (m_tags.empty()) return "\"\"";
    std::string str("\"");
    for (auto it = m_attributes.begin(); it != m_attributes.end(); ++it) {
        auto attribute = *it;
        str +=  attribute.first + "=>" + attribute.second + ",";
    }
#if 0
    str.pop_back();
    str += "\"";
#else
    str[str.size()-1] = '\"';
#endif
    return str;
}

std::string Element::tags_str() const {
    if (m_tags.empty()) return "";
    std::string str("\"");
    for (auto it = m_tags.begin(); it != m_tags.end(); ++it) {
        auto tag = *it;
        str +=  tag.first + "=>" + tag.second + ",";
    }
#if 0
    str.pop_back();
    str += "\"";
#else
    str[str.size()-1] = '\"';
#endif
    return str;
}

}  // namespace osm2pgr
