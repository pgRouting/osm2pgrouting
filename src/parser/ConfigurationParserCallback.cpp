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

#include "parser/ConfigurationParserCallback.h"

#include <boost/lexical_cast.hpp>
#include <string>

#include "osm_elements/OSMDocument.h"
#include "configuration/configuration.h"

namespace osm2pgr {

/*!
    Parser callback for configuration files
*/
void ConfigurationParserCallback::StartElement(
        const char *name,
        const char** atts) {
    /* the type is the tag_key */
    if (strcmp(name, "tag_name") == 0) {
        m_current = new Tag_key(atts);
    /* the class is the tag_value */
    } else if (strcmp(name, "tag_value") == 0) {
         m_current->add_tag_value(Tag_value(atts));
    } else if (strcmp(name, "configuration") == 0) {
    }
}


void ConfigurationParserCallback::EndElement(const char* name) {
    /* the type is the tag_key */
    if (strcmp(name, "tag_name") == 0) {
        m_config.add_tag_key(*m_current);
        delete m_current;
    }
}

}  // namespace osm2pgr
