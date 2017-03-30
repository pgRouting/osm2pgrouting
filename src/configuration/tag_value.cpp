/***************************************************************************
 *   Copyright (C) 2016 by pgRouting developers                            *
 *   project@pgrouting.org                                                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License t &or more details.                        *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "configuration/tag_value.h"
#include <boost/lexical_cast.hpp>
#include <string>
#include <cassert>



namespace osm2pgr {


Tag_value::Tag_value(const char **atts) :
    Element(atts){
        assert(has_attribute("name"));
}

std::string
Tag_value::get(const std::string &str) const {
    assert(has_attribute(str));
    return  get_attribute(str);
}

#if 0
std::string
Tag_value::priority() const {
    assert(has_attribute("priority"));
    return  get_attribute("priority");
}

std::string
Tag_value::maxspeed() const {
    assert(has_attribute("maxspeed"));
    return  get_attribute("maxspeed");
}
#endif

std::string
Tag_value::name() const {
    assert(has_attribute("name"));
    return get_attribute("name");
}




}  // end namespace osm2pgr
