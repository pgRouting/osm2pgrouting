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

    for (const auto& tm : transportation_mode) {
        if ((std::string("restriction:") + tm) == std::string(type)) {
            return true;
        }
    }
    return false;
}

/**
 * Overwritten from the base class.
 */
bool MyRelCollector::keep_member(
        const osmium::relations::RelationMeta&,
        const osmium::RelationMember& member) const {
    /*
     * Interested in members of type way & node.
     */
    return member.type() == osmium::item_type::way ||
        member.type() == osmium::item_type::node;
}

/*
 * (2654080,'no_right_turn',30513235,30513221,336812979,'n','version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>Schermy'::hstore,'except=>hgv,restriction=>no_right_turn,type=>restriction'::hstore)
 */
std::string MyRelCollector::attributes_str(
        const osmium::Relation& relation) const {
    std::string user = std::string(relation.user());
    user = add_quotes(user, true);
    std::string str("");
    str += "version=>" + std::to_string(relation.version()) + ",";
    str += "timestamp=>" + relation.timestamp().to_iso() + ",";
    str += "changeset=>" + std::to_string(relation.changeset()) + ",";
    str += "uid=>" + std::to_string(relation.uid()) + ",";
    str += "user=>" + user;
    return str;
}


std::string MyRelCollector::tags_str(
        const osmium::Relation& relation) const {
    std::string str("");
    for (const osmium::Tag& tag : relation.tags()) {
        str += std::string(tag.key()) + "=>" +  tag.value() + ',';
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

    osmium::object_id_type from;
    osmium::object_id_type to;
    osmium::object_id_type via;
    osmium::item_type via_type;

    for (const auto& member : relation.members()) {
        if (!std::strcmp(member.role(), "via")) {
            via = member.ref();
            via_type = member.type();
        } else if (!std::strcmp(member.role(), "from")) {
            from = member.ref();
        } else if (!std::strcmp(member.role(), "to")) {
            to = member.ref();
        } else {
            std::cout << "Found an illegal member relation in restriction\n";
            assert(false);
        }
    }
    m_file
        << relation.id() << "\t"
        << "'" << relation.get_value_by_key("restriction") << "'\t"
        << from << "\t"
        << to << "\t"
        << via << "\t"
        <<  via_type << "\t"
        << attributes_str(relation) << "\t"
        << tags_str(relation)
        << "\n";
}

void MyRelCollector::flush() {
    this->callback();
}
