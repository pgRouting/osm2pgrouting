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

#if 0
#include <osmium/osm/types.hpp>
#include <osmium/osm/location.hpp>
#include <osmium/osm/segment.hpp>
#include <osmium/osm/undirected_segment.hpp>
#include <osmium/osm/box.hpp>
#include <osmium/osm/object.hpp>
#include <osmium/builder/builder.hpp>
#include <osmium/builder/osm_object_builder.hpp>
#include <osmium/io/any_compression.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/relations/collector.hpp>

#include <cstdlib>  // for std::exit
#include <getopt.h> // for getopt_long

// For assembling multipolygons
#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#endif

// For the DynamicHandler class
#include <osmium/dynamic_handler.hpp>

#if 0
// For the WKT factory
#include <osmium/geom/wkt.hpp>

// For the Dump handler
#include <osmium/handler/dump.hpp>
#endif

// For the NodeLocationForWays handler
#include <osmium/handler/node_locations_for_ways.hpp>

// Allow any format of input files (XML, PBF, ...)
#include <osmium/io/any_input.hpp>

#if 0
// For osmium::apply()
#include <osmium/visitor.hpp>

// For the location index. There are different types of indexes available.
// This will work for small and medium sized input files.
#include <osmium/index/map/sparse_mem_array.hpp>
#endif






main() {
    using index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
    using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;

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
