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
#if 0
#include "osm_elements/osm_tag.h"
#include "configuration/Tag_key.h"
#include "configuration/Class.h"
#endif

namespace osm2pgr {


void Configuration::AddTag_key(Tag_key t) {
    if (has_type(t.name())) {
        std::cerr << "duplicate Tag_key found in condfiguration file"
            << t.name() << "\n";
        return;
    }
    m_Tag_keys[t.name()] = t;
}

Tag_key& Configuration::FindTag_key(std::string name) {
    return m_Tag_keys.at(name);
}
Tag_key Configuration::FindTag_key(std::string name) const {
    return m_Tag_keys.at(name);
}

Tag_value Configuration::FindTag_value(const Tag &tag) const {
    return m_Tag_keys.at(tag.key()).classes()[tag.value()];
}

std::string Configuration::priority_str(const Tag &tag) const {
    return  boost::lexical_cast<std::string>(FindTag_value(tag).priority());
}



}  // end namespace osm2pgr
