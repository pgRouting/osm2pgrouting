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
#include "osm_elements/osm_tag.h"
#include "osm_elements/osm_element.h"

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

void 
Element::tag_config(const Tag &tag) {
    m_tag_config = tag;
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
Element::is_tag_configured() const {
        return (m_tag_config.key() != "" && m_tag_config.value() != "");
}


bool
Element::has_attribute(const std::string& key) const {
    return m_attributes.find(key) != m_attributes.end();
}

std::string
Element::get_attribute(const std::string& key) const {
    return m_attributes.find(key)->second;
}

std::string
Element::attributes_str() const {
    if (m_tags.empty()) return "\"\"";
    std::string str("\"");
    for (auto it = m_attributes.begin(); it != m_attributes.end(); ++it) {
        auto attribute = *it;
        str +=  attribute.first + "=>" + attribute.second + ",";
    }
    str[str.size()-1] = '\"';
    return str;
}

std::string
Element::tags_str() const {
    if (m_tags.empty()) return "";
    std::string str("\"");
    for (auto it = m_tags.begin(); it != m_tags.end(); ++it) {
        auto tag = *it;
        str +=  tag.first + "=>" + tag.second + ",";
    }
    str[str.size()-1] = '\"';
    return str;
}

static
std::string
addquotes(const std::string str, bool force) {
    std::string result("");

    for (auto c : str) {
        if ( c == '"' ) {
            /*
             * To avoid problems with json & hstore
             * all quotes are converted to single quotes
             */
            result += "\'\'";
            continue;
        } else if ( c == '\\' ) {
            result += '\\';
        } else if (c == '\'') {
            result += '\'';
        } else if (c == '\n') {
            result += "\\n";
            continue;
        } else if (c == '\r') {
            result += "\\r";
            continue;
        } else if (c == '\t') {
            result += "\\t";
            continue;
        }
        result += c;
    }
    if (!force) {
        for (auto c : result) {
            if  (c == ' ' || c == ',' || c == '=' || c == '>' || c == ':') {
                return std::string("\"") + result + "\"";
            }
        }
        return result;
    }
    return std::string("\"") + result + "\"";
}


static
std::string
getHstore(const std::map<std::string, std::string> &values) {
    std::string hstore;
    if (values.empty()) return std::string();

    for (const auto item : values) {
        hstore +=
            addquotes(item.first, true)
            + " => "
            + addquotes(item.second, true)  + ",";
    }
    hstore[hstore.size() - 1] = ' ';
    hstore += "";
    return hstore;
}

#if 0
static
std::string
getJSON(const std::map<std::string, std::string> &values) {
    if (values.empty()) return std::string("{}");
    std::string json("{");
    for (const auto item : values) {
        json += addquotes(item.first, true) 
            + ":"
            + addquotes(item.second, true)  + ",";
    }
    json[json.size() - 1] = '}';
    json += "";
    return json;
}
#endif

std::vector<std::string>
Element::values(const std::vector<std::string> &columns, bool is_hstore) const {
    std::vector<std::string> values;
    for (const auto column : columns) {
        if (column == "osm_id" || column == "tag_id") { 
            values.push_back(boost::lexical_cast<std::string>(osm_id()));
            continue;
        }   
        if (column == "tag_name") {
            values.push_back(m_tag_config.key());
            continue;
        }
        if (column == "tag_value") {
            values.push_back(m_tag_config.value());
            continue;
        }
        if (column == "the_geom") {
            values.push_back(get_geometry());
            continue;
        }

        if (column == "members") {
            values.push_back(members_str());
            continue;
        }

        if (column == "attributes") {
            values.push_back(getHstore(m_attributes));
            continue;
        }   
        if (column == "tags") {
            values.push_back(getHstore(m_tags));
            if (is_hstore) {};

            continue;
        }   
        if (has_attribute(column)) {
            values.push_back(get_attribute(column));
            continue;
        }
        if (has_tag(column)) {
            values.push_back(get_tag(column));
            continue;
        }
        values.push_back(std::string(""));
    }
    return values;
}

}  // namespace osm2pgr
