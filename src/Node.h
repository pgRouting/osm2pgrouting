/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt       							   *
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

#ifndef SRC_NODE_H_
#define SRC_NODE_H_

namespace osm {

/**
\code
  <node id="122603925" 
		lat="53.0780875" 
		lon="8.1351704" 
		user="artus70" visible="true" timestamp="2007-11-18T22:18:59+00:00"/>
\endcode
*/
class Node {
 public:
    // ! ID of the node
    long long id;
    // ! latitude coordinate
    double lat;
    // ! longitude coordinate
    double lon;
    /**
     *    counts the rate, how much this node is used in different ways
     */
    unsigned short numsOfUse;
 public:
    /**
     *    Constructor
     *     @param id ID of the node
     *    @param lat latitude
     *    @param lon longitude
     */
    Node(long long id = -1, double lat = 0, double lon = 0);
    //! Destructor
    virtual ~Node();
};


}  // end namespace osm
#endif  // SRC_NODE_H_
