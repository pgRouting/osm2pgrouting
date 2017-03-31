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


#include "configuration/configuration.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>

namespace osm2pgr {

void Configuration::add_tag_key(const Tag_key &t_key) {
    if (has_tag_key(t_key.name())) {
        std::cerr << "Duplicate Tag_key found in condfiguration file"
            << t_key.name() << " ....  sikipping\n";
        return;
    }
    m_Tag_keys[t_key.name()] = t_key;
}


bool 
Configuration::has_tag_key(const std::string &key) const {
        return m_Tag_keys.count(key) != 0; 
}                      


bool
Configuration::has_tag(const Tag &tag) const {
    return has_tag_key(tag.key())
        && tag_key(tag).has_tag_value(tag);
}


const Tag_value& 
Configuration::tag_value(const Tag &tag) const {
    return tag_key(tag).tag_value(tag);
}                      


const Tag_key& 
Configuration::tag_key(const Tag &tag) const {
    return m_Tag_keys.at(tag.key()); 
}                      


double
Configuration::maxspeed(const Tag &tag) const {
    if (tag_key(tag).has(tag, "maxspeed"))
        return boost::lexical_cast<double>(tag_key(tag).get(tag, "maxspeed"));
    return 50;
}

double
Configuration::maxspeed_forward(const Tag &tag) const {
    if (tag_key(tag).has(tag, "maxspeed:backward"))
        return boost::lexical_cast<double>(tag_key(tag).get(tag, "maxspeed:backward"));
    return maxspeed(tag);
}

double
Configuration::maxspeed_backward(const Tag &tag) const {
    if (tag_key(tag).has(tag, "maxspeed:forward"))
        return boost::lexical_cast<double>(tag_key(tag).get(tag, "maxspeed:backward"));
    return maxspeed(tag);
}

double
Configuration::priority(const Tag &tag) const {
    if (tag_key(tag).has(tag, "priority"))
        return boost::lexical_cast<double>(tag_key(tag).get(tag, "priority"));
    return 0;
}


}  // end namespace osm2pgr
