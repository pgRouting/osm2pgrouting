/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt                                    *
 *   gentoo.murray@gmail.com                                               *
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
#ifndef SRC_RELATION_H_
#define SRC_RELATION_H_

#include <vector>
#include <map>
#include <string>

namespace osm {
class Way;

/**
\code
 <way id="20215432" visible="true" timestamp="2008-01-09T22:35:16+00:00" user="Pferdo">
    <nd ref="213794929"/>
    <nd ref="213795470"/>
    <nd ref="213795483"/>
    <nd ref="213795493"/>
    <nd ref="213795506"/>
    <nd ref="213795517"/>
    <nd ref="213795527"/>
    <nd ref="213795541"/>
    <nd ref="213795552"/>
    <nd ref="213795561"/>
    <nd ref="213795571"/>
    <tag k="name" v="Pf�nderweg"/>
    <tag k="created_by" v="JOSM"/>
    <tag k="highway" v="residential"/>
  </way>
\endcode
*/
/**
 * @brief The class for a Relation
 * @details Contains the variable for the constructor and a destructor to delete then
 * 
 */
class Relation {
 public:
    long long id;
    std::string name;
    std::vector<long long> m_WayRefs;
    std::map<std::string, std::string> m_Tags;

 public:
    /** 
     *    Constructor
     *    @param id ID of the way
     */
    Relation(long long id);
    //! Destructor
    ~Relation();
    /**
     *    saves the nodes of the way  
     *    @param pNode node
     */
    void AddWayRef(long long pID);
    void AddTag(std::string key, std::string value);
};


}  // end namespace osm
#endif  // SRC_RELATION_H_
