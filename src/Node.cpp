/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt   								   *
 *   gentoo.murray@gmail.com   											   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "./stdafx.h"
#include "./Node.h"

namespace osm {

/**
 * @brief Constructor for a node
 * @details Initializes a node on the graph with some provided values, which will be later required for routing
 * 
 * @param long The unique id of the node
 * @param lat latitude of the point
 * @param lon  longitude of the point
 */
Node::Node(long long id, double lat, double lon)
:
    id(id),
    lat(lat),
    lon(lon) {
    this->numsOfUse = 0;
}

/**
 * @brief Destructor
 * @details Deleted the node from the DB
 * @return [description]
 */
Node::~Node() {
}

}  // end namespace osm
