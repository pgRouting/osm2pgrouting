/*PGR-GNU*****************************************************************
 * File: turn_restrictions.cpp
 *
 * Copyright(c) 2017 pgRouting developers
 *
 * File developer: Celia Virginia Vergara Castillo (2017)
 *
 *
 * ------
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *  ********************************************************************PGR-GNU*/

/*! @file */


#include "collectors/turn_restrictions.h"

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include "utilities/quotes_handling.h"


MyRelCollector::MyRelCollector() :
    m_file(std::cout) {
    }

MyRelCollector::MyRelCollector(std::ostream &file) :
    m_file(file) {
    }

/**
 * Interested in all relations tagged with type=restriction
 *
 * OSM WIKI about restrictions
 * http://wiki.openstreetmap.org/wiki/Relation:restriction
 *
 * Overwritten from the base class.
 */
bool MyRelCollector::keep_relation(const osmium::Relation& relation) const {
    const char* type = relation.tags().get_value_by_key("type");
    //std::cout <<  "my function name is: " << __FUNCTION__ << "\n";
    /*
     * known transportation modes
     * TODO save in a configuration file
     */
    std::vector<std::string> transportation_mode{
        "hgv", "caravan", "motorcar", "bus",
            "agricultural", "bicycle", "hazmat", "psv", "emergency"};

    /*
     *  ignore relations without "type" tag
     */
    if (!type) {
        return false;
    }

    if ((!std::strcmp(type, "restriction"))) {
        return true;
    }

#if 0
    for (const auto& tm : transportation_mode) {
        if ((std::string("restriction:") + tm) == std::string(type)) {
            return true;
        }
    }
#endif
    return false;
}

/**
 * Overwritten from the base class.
 */
bool MyRelCollector::keep_member(
        const osmium::relations::RelationMeta&,
        const osmium::RelationMember &member) const {
    //std::cout <<  "my function name is: " << __FUNCTION__ << "\n";
#if 0
    /*
     * Interested in members of type way & node.
     */
    return member.type() == osmium::item_type::way ||
        member.type() == osmium::item_type::node;
#else
    return true;
#endif
}

/*
 * (2654080,'no_right_turn',30513235,30513221,336812979,'n','version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>Schermy'::hstore,'except=>hgv,restriction=>no_right_turn,type=>restriction'::hstore)
 */
std::string MyRelCollector::attributes_str(
        const osmium::Relation& relation) const {
    //std::cout <<  "my function name is: " << __FUNCTION__ << "\n";
    std::string user = add_quotes(std::string(relation.user()));
    std::string str("");
    str += "version=>" + std::to_string(relation.version()) + ",";
    str += "timestamp=>" + relation.timestamp().to_iso() + ",";
    str += "changeset=>" + std::to_string(relation.changeset()) + ",";
    str += "uid=>" + std::to_string(relation.uid()) + ",";
    str += "user=>" + add_quotes(std::string(relation.user()));
    return str;
}


std::string MyRelCollector::tags_str(
        const osmium::Relation& relation) const {
    std::string str("");
    for (const osmium::Tag& tag : relation.tags()) {
        str += std::string(tag.key()) + "=>" +  add_quotes(tag.value()) + ',';
    }
    str[str.size()-1] = ' ';
    return str;
}

/** A Restriction:
 *
 * from: is of type way
 * to: is of type way
 * via: can be of type way or node
 * can not have a member relation
 *
 * Overwritten from the base class.
 */
void MyRelCollector::complete_relation(
        osmium::relations::RelationMeta& relation_meta) {
    const osmium::Relation& relation = this->get_relation(relation_meta);
    //std::cout <<  "my function name is: " << __FUNCTION__ << "\n";

    /*
     * http://wiki.openstreetmap.org/wiki/Relation:restriction#Members
     *
     * From:  A no_entry restriction can have more than 1 from member, all others have exactly 1 from.
     * to:   A no_exit restriction can have more than 1 to member, all others have exactly 1 to.
     * Via: One node
     * Via: One or more ways
     */
     std::ostringstream from;
     std::ostringstream to;
     std::ostringstream via;
     std::ostringstream location_hint;
     osmium::item_type via_type;

    for (const auto &member : relation.members()) {
        if (!std::strcmp(member.role(), "via")) {
            if (via.str().empty()) {
                /*
                 * Via: One or more ways with a via role
                 * Catching only the first via_type (n, w)
                 * - all the others (if any) must be the same
                 *   - Not checking here
                 *
                 *
                 * On the database:
                 * - if via_type is n:
                 *   - the array must be of size == 1
                 *   - the array[i] contains node ID
                 * - if via_type is w:
                 *   - the array can be of size >= 1
                 *   - the array[i] contains ways ID
                 */
                via_type = member.type();
            } else {
                via << ",";
            }

            via << member.ref();

        } else if (!std::strcmp(member.role(), "from")) {
            if (!from.str().empty()) {
                from << ",";
            }
            from << member.ref();
        } else if (!std::strcmp(member.role(), "to")) {
            if (!to.str().empty()) {
                to << ",";
            }
            to << member.ref();
        } else if (!std::strcmp(member.role(), "location_hint")) {
            location_hint << member.ref();
        } else {
            std::cerr
                << "Found currently unsuported member role: '"
                << member.role()
                << "' on restriction: " << relation.id() << "\n";
        }
    }
#if 0
    std::cout 
        << relation.id() << "\t"
        << pg_null_array(from) << "\t"
        << pg_null_array(to) << "\t"
        << pg_null_array(via) << "\t"
        << via_type << "\t"
        << pg_null(location_hint) << "\t"
        << attributes_str(relation) << "\t"
        << tags_str(relation)
        << "\n";
#endif
    m_file
        << relation.id() << "\t"
        << pg_null_array(from) << "\t"
        << pg_null_array(to) << "\t"
        << pg_null_array(via) << "\t"
        << via_type << "\t"
        << pg_null(location_hint) << "\t"
        << attributes_str(relation) << "\t"
        << tags_str(relation)
        << "\n";
}

void MyRelCollector::flush() {
    this->callback();
}
