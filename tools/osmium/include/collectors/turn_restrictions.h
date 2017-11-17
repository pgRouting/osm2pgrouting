/*PGR-GNU*****************************************************************
File: turn_restrictions.h

Copyright (c) 2017 pgRouting developers

File developer: Celia Virginia Vergara Castillo (2017)


------
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
********************************************************************PGR-GNU*/

/*! @file */

#include <fstream>
#include <iostream> // for std::cout, std::cerr
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
#endif
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/relations/collector.hpp>

#if 0
#include <cstdlib>  // for std::exit
#include <getopt.h> // for getopt_long
#endif

#if 0
// For assembling multipolygons
#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#endif

// For the DynamicHandler class
#include <osmium/dynamic_handler.hpp>

#if 0
// For the WKT factory
#include <osmium/geom/wkt.hpp>
#endif

#if 0
// For the Dump handler
#include <osmium/handler/dump.hpp>
#endif

#if 1
// For the NodeLocationForWays handler
#include <osmium/handler/node_locations_for_ways.hpp>
#endif

// Allow any format of input files (XML, PBF, ...)
#include <osmium/io/any_input.hpp>

#if 0
// For osmium::apply()
#include <osmium/visitor.hpp>
#endif

#if 0
// For the location index. There are different types of indexes available.
// This will work for small and medium sized input files.
#include <osmium/index/map/sparse_mem_array.hpp>
#endif


using index_type = osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location>;
using location_handler_type = osmium::handler::NodeLocationsForWays<index_type>;


class MyRelCollector : public osmium::relations::Collector<MyRelCollector, true, true, true> {

    public:

    MyRelCollector();
    explicit MyRelCollector(std::ostream &file);

    /**
     * Interested in all relations tagged with type=restriction
     *
     * OSM WIKI about restrictions
     * http://wiki.openstreetmap.org/wiki/Relation:restriction
     *
     * Overwritten from the base class.
     */
    bool keep_relation(const osmium::Relation& relation) const;

    /**
     * Overwritten from the base class.
     */
    bool keep_member(
            const osmium::relations::RelationMeta&,
            const osmium::RelationMember& member) const ;

    /*
     * (2654080,'no_right_turn',30513235,30513221,336812979,'n','version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>Schermy'::hstore,'except=>hgv,restriction=>no_right_turn,type=>restriction'::hstore)
     */
    std::string attributes_str(
            const osmium::Relation& relation) const ;


    std::string tags_str(
            const osmium::Relation& relation) const ;

    /** A Restriction:
     *
     * from: is of type way
     * to: is of type way
     * via: can be of type way or node
     * can not have a member relation
     *
     * Overwritten from the base class.
     */
    void complete_relation(osmium::relations::RelationMeta& relation_meta) ;

    void flush();

  private:

    std::ostream &m_file;
};


