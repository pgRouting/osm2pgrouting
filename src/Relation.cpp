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
#include <string>
#include "./stdafx.h"
#include "./Relation.h"
#include "./Way.h"


namespace osm {

/**
 * @brief Constructor
 * @details [long description]
 * 
 * @param long Id for the Relation
 */
Relation::Relation(long long id)
:
    id(id) {
}

/**
 * @brief Destructor
 * @details Deletes the relations
 * @return [description]
 */
Relation::~Relation() {
}

void Relation::AddWayRef(long long pID) {
    if (pID) m_WayRefs.push_back(pID);
}

void Relation::AddTag(std::string key, std::string value) {
    m_Tags[key] = value;
}

}  // end namespace osm

