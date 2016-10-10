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

#include <string>
#include <boost/lexical_cast.hpp>
#include "Configuration.h"
#include "Type.h"
#include "Class.h"


namespace osm2pgr {

Configuration::Configuration() {
}

Configuration::~Configuration() {
    ez_mapdelete(m_Types);
}

void Configuration::AddType(Type* t) {
    m_Types[t->name()] = t;
}

Type* Configuration::FindType(std::string name) const {
    return m_Types.at(name);
}

Class Configuration::FindClass(const std::string &typeName, const std::string &className) const {
    return m_Types.at(typeName)->classes()[className];
}

std::string Configuration::priority_str(const std::string &typeName, const std::string &className) const {
    return  boost::lexical_cast<std::string>(FindClass(typeName, className).priority);
}

}  // end namespace osm2pgr
