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
#include "Way.h"


namespace osm2pgr {

Way::Way(int64_t id, bool visible, int64_t osm_id, int maxspeed_forward, int maxspeed_backward)
:
	id(id),
	visible(visible),
	length(0),
	maxspeed_forward(maxspeed_forward),
	maxspeed_backward(maxspeed_backward),
	oneWayType(UNKNOWN),
	osm_id(osm_id) {
}

#if 0
Way::~Way()
{
}
#endif

void Way::AddNodeRef( Node* pNode )
{
	if( pNode ) m_NodeRefs.push_back(pNode);
}

void Way::AddTag( std::string key , std::string value)
{
	m_Tags[key] = value;
}

bool Way::HasTag(std::string key)
{
	return (m_Tags.count(key)>0);
}



} // end namespace osm2pgr

