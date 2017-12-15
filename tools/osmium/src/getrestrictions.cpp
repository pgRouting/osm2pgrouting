/*PGR-GNU*****************************************************************
 * File: getrestrictions.cpp
 *
   Copyright (c) 2017 pgRouting developers
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


#include <fstream>
#include <iostream> // for std::cout, std::cerr

#include "utilities/quotes_handling.h"
#include "collectors/turn_restrictions.h"

// For the DynamicHandler class
#include <osmium/dynamic_handler.hpp>


// For the NodeLocationForWays handler
#include <osmium/handler/node_locations_for_ways.hpp>

// Allow any format of input files (XML, PBF, ...)
#include <osmium/io/any_input.hpp>





main(int argc, char *argv[]) {
    using index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

    if (argc != 2) {
        std::cerr << "file to process missing\n";
        exit(1);
    }
    /*
     *  the input file
     */
    std::string in_file_name = argv[1];
    std::string out_file_name = 
        std::string(in_file_name, 0, in_file_name.size()-4) + "_restrictions.sql";

    std::cout << "processing: " << in_file_name << "\n";
    std::cout << "results at: " << out_file_name << "\n";

    /*
     * The output file
     */
    std::ofstream of(out_file_name);

    /*
     * Reading the create table query
     */
    std::ifstream f("../restrictions.sql");
    std::stringstream buffer;

    /*
     * output of the create tables
     */
    of << f.rdbuf();

    f.close();

    std::string str = buffer.str();
    std::cout << str << "\n";

    osmium::handler::DynamicHandler handler;
    osmium::relations::RelationMeta relation_meta;

    MyRelCollector collector(of);
    std::cerr << "Pass 1...\n";
    osmium::io::Reader reader1{in_file_name, osmium::osm_entity_bits::relation};
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
    osmium::io::Reader reader2{in_file_name};
    osmium::apply(reader2, location_handler, collector.handler([&handler](osmium::memory::Buffer&& buffer) {
                osmium::apply(buffer, handler);
                }));
    of  << "\\.";
    std::cerr << "Pass 2 done\n";

    std::ifstream l("../restrictions_end.sql");
    of  << l.rdbuf();
    l.close();

    of.close();

    reader2.close();
    // Output the amount of main memory used so far. All complete multipolygon
    // relations have been cleaned up.
    std::cerr << "Memory:\n";
    collector.used_memory();
}
