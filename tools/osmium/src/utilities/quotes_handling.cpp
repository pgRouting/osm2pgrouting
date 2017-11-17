/*PGR-GNU*****************************************************************
 * File: quotes_handling.cpp
 *
 * Copyright (c) 2017 pgRouting developers
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


#include "utilities/quotes_handling.h"

std::string
add_quotes(const std::string str, bool force) {
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


#if 0
class MyRelCollector : public osmium::relations::Collector<MyRelCollector, true, true, true> {

    public:

    MyRelCollector() :
        m_file(std::cout) {
    }

    MyRelCollector(std::ostream &file) :
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
    bool keep_relation(const osmium::Relation& relation) const {
        const char* type = relation.tags().get_value_by_key("type");
        /*
         * known transportation modes
         * TODO save in a configuration file
         */
        std::vector<std::string> transportation_mode{"hgv","caravan","motorcar","bus","agricultural","bicycle","hazmat","psv","emergency"}; 

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
            if ((std::string("restriction:") + tm) == std::string(type)) return true;
        }
        return false;
    }

    /**
     * Overwritten from the base class.
     */
    bool keep_member(
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
    std::string attributes_str(
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


    std::string tags_str(
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
    void complete_relation(osmium::relations::RelationMeta& relation_meta) {
        const osmium::Relation& relation = this->get_relation(relation_meta);

        osmium::object_id_type from;
        osmium::object_id_type to;
        osmium::object_id_type via;
        osmium::item_type via_type;

        for (const auto& member : relation.members()) {
            if  (!std::strcmp(member.role(),"via")) {
                via = member.ref();
                via_type = member.type();
            } else if  (!std::strcmp(member.role(),"from")) {
                from = member.ref();
            } else if  (!std::strcmp(member.role(),"to")) {
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

    void flush() {
        this->callback();
    }
  private:

    std::ostream &m_file;
};




main() {
    /*
     * The output file
     */
    std::ofstream of("restrictions_output.sql");

    /*
     * Reading the create table query
     */
    std::ifstream f("../restrictions.sql");
    std::stringstream buffer;

    of  << f.rdbuf();
    std::string str = buffer.str();
    std::cout << str << "\n";
    f.close();

    osmium::handler::DynamicHandler handler;
    osmium::relations::RelationMeta relation_meta;
    MyRelCollector collector(of);
    std::cerr << "Pass 1...\n";
    osmium::io::Reader reader1{"../../../tools/data/restrictions.osm", osmium::osm_entity_bits::relation};
    collector.read_relations(reader1);
    reader1.close();
    std::cerr << "Pass 1 done\n";


    // Output the amount of main memory used so far. All multipolygon relations
    // are in memory now.
    std::cerr << "Memory:\n";
    collector.used_memory();

    // The index storing all node locations.
    index_type index;

    // The handler that stores all node locations in the index and adds them
    // to the ways.
    location_handler_type location_handler{index};

    // If a location is not available in the index, we ignore it. It might
    // not be needed (if it is not part of a multipolygon relation), so why
    // create an error?
    location_handler.ignore_errors();

    // On the second pass we read all objects and run them first through the
    // node location handler and then the multipolygon collector. The collector
    // will put the areas it has created into the "buffer" which are then
    // fed through our "handler".
    std::cerr << "Pass 2...\n";
    osmium::io::Reader reader2{"../../../tools/data/restrictions.osm"};
    osmium::apply(reader2, location_handler, collector.handler([&handler](osmium::memory::Buffer&& buffer) {
                osmium::apply(buffer, handler);
                }));
    reader2.close();
    std::cout << "\\.";
    std::cerr << "Pass 2 done\n";
    std::ifstream l("../restrictions_end.sql");
    of  << "\\.";
    of  << l.rdbuf();
    l.close();
    of.close();

    // Output the amount of main memory used so far. All complete multipolygon
    // relations have been cleaned up.
    std::cerr << "Memory:\n";
    collector.used_memory();
}
#endif
