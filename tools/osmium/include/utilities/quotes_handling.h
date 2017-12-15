/*PGR-GNU*****************************************************************
File: quotes_handling.h

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

#ifndef TOOLS_OSMIUM_INCLUDE_UTILITIES_QUOTES_HANDLING_H_
#define TOOLS_OSMIUM_INCLUDE_UTILITIES_QUOTES_HANDLING_H_

#include <string>
#include <sstream>

std::string
add_quotes(const std::string str, bool force = false);

std::string
pg_null(const std::ostringstream &original);

std::string
pg_null_array(const std::ostringstream &original);

#endif  // TOOLS_OSMIUM_INCLUDE_UTILITIES_QUOTES_HANDLING_H_
