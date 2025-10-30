/*PGR-GNU*****************************************************************

 Copyright (c) 2017 pgRouting developers
 Mail: project@pgrouting.org

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

#include "boost/lexical_cast.hpp"
#include "database/table_management.h"
#include "utilities/utilities.h"
#include <string>

namespace osm2pgr {


/*
 * configuring TABLE vertices_pgr
 */
Table
Tables::ways_vertices_pgr_config() const {
    Table table(
            /* name */
            "ways_vertices_pgr",

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            std::string(
                m_vm["prefix"].as<std::string>()
                + "ways"
                + m_vm["suffix"].as<std::string>()
                + "_vertices_pgr"),

            /* standard column creation string */
            std::string(
                " id BIGINT GENERATED ALWAYS AS IDENTITY PRIMARY KEY"
                ", in_edges bigint[]"
                ", out_edges bigint[]"
                ", x decimal(11,8) GENERATED ALWAYS AS (ST_X(geom)) STORED"
                ", y decimal(11,8) GENERATED ALWAYS AS (ST_Y(geom)) STORED"
                ", osm_id BIGINT"
                ", geom GEOMETRY(POINT, 4326)"

#if 0
                + (m_vm.count("attributes") ?
                    (std::string(", attributes ") + (m_vm.count("hstore") ? "hstore" : "json"))
                    : "")
                + (m_vm.count("tags") ?
                    (std::string(", tags ") + (m_vm.count("hstore") ? "hstore" : "json"))
                    : "")
#endif
                ),

                /* other columns */
                "",

                /* geometry */
                "");

            return table;
}


} //namespace osm2pgr
