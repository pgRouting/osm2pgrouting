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


#include "./Configuration.h"
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <string>
#include "./osm_tag.h"
#include "./Type.h"
#include "./Class.h"


namespace osm2pgr {


void Configuration::AddType(Type t) {
    if (has_type(t.name())) {
        std::cerr << "duplicate Type found in condfiguration file"
            << t.name() << "\n";
        return;
    }
    m_Types[t.name()] = t;
}

Type& Configuration::FindType(std::string name) {
    return m_Types.at(name);
}
Type Configuration::FindType(std::string name) const {
    return m_Types.at(name);
}

Class Configuration::FindClass(const Tag &tag) const {
    return m_Types.at(tag.key()).classes()[tag.value()];
}

std::string Configuration::priority_str(const Tag &tag) const {
    return  boost::lexical_cast<std::string>(FindClass(tag).priority());
}

}  // end namespace osm2pgr
