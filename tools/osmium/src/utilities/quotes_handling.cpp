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
#include <string>

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


std::string
pg_null(const std::ostringstream &original) {
    return original.str().empty() ? "\\N" : original.str();
};

std::string
pg_null_array(const std::ostringstream &original) {
    return original.str().empty() ? "\\N" : "{" + original.str() + "}";
};

