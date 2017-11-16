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

namespace osm2pgr {


/*
 * configuring TABLE configuration
 */ 


Table
Tables::configuration_config() const {
    Table table(
            /* name */
            "configuration",

            /* schema */
            m_vm["schema"].as<std::string>(),

            /* full name */
            "configuration",

            /* standard column creation string */
            std::string(
                " id serial"
                ", tag_id INTEGER"
                ", tag_key TEXT"
                ", tag_value TEXT"
                ", priority double precision"
                ", maxspeed double precision"
                ", maxspeed_forward double precision"
                ", maxspeed_backward double precision"
                ", force char"),

            /* other columns */
            "",

            /* geometry */
            "");



    std::vector<std::string> columns;
    columns.push_back("tag_id");
    columns.push_back("tag_key");
    columns.push_back("tag_value");
    columns.push_back("priority");
    columns.push_back("maxspeed");
    columns.push_back("maxspeed_forward");
    columns.push_back("maxspeed_backward");
    columns.push_back("force");

    table.set_columns(columns);

    return table;
}


} //namespace osm2pgr
