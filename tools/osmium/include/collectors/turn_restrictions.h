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

#ifndef TOOLS_OSMIUM_INCLUDE_COLLECTORS_TURN_RESTRICTIONS_H_
#define TOOLS_OSMIUM_INCLUDE_COLLECTORS_TURN_RESTRICTIONS_H_
#pragma once

#include <string>
#include <iostream>  // for std::cout, std::cerr


// For the NodeLocationForWays handler
#include <osmium/handler/node_locations_for_ways.hpp>
#include <osmium/relations/collector.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>



class MyRelCollector :
    public osmium::relations::Collector<MyRelCollector, true, true, true> {
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
            const osmium::RelationMember& member) const;

    /*
     * (2654080,'no_right_turn',30513235,30513221,336812979,'n','version=>1,timestamp=>2012-12-22T17:01:50Z,changeset=>14368535,uid=>381316,user=>Schermy'::hstore,'except=>hgv,restriction=>no_right_turn,type=>restriction'::hstore)
     */
    std::string attributes_str(
            const osmium::Relation& relation) const;


    std::string tags_str(
            const osmium::Relation& relation) const;

    /** A Restriction:
     *
     * from: is of type way
     * to: is of type way
     * via: can be of type way or node
     * can not have a member relation
     *
     * Overwritten from the base class.
     */
    void complete_relation(osmium::relations::RelationMeta& relation_meta);

    void flush();

 private:
    std::ostream &m_file;
};


#endif  // TOOLS_OSMIUM_INCLUDE_COLLECTORS_TURN_RESTRICTIONS_H_
