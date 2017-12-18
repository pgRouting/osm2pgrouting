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


#include "osm_elements/osm_tag.h"
#include <string>

namespace osm2pgr {


Tag::Tag(const char **atts) {
    auto **attribut = atts;
    while (*attribut != NULL) {
        std::string name = *attribut++;
        std::string value = *attribut++;
        if (name  == "k") {
            std::transform(value.begin(), value.end(), value.begin(), [](char ch) {
                    return ch == ' ' ? '_' : ch;});
            m_key = value;
        } else if (name == "v") {
            m_value = value;
        }
    }
}

std::ostream& operator<<(std::ostream &os, const Tag& tag) {
    os << tag.m_key << "=>" << tag.m_value;
    return os;
}

}  // namespace osm2pgr
