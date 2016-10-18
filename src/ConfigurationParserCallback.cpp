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

#include "./ConfigurationParserCallback.h"

#include <boost/lexical_cast.hpp>

#include <string>
#include "./OSMDocument.h"
#include "./Configuration.h"
#include "./Type.h"
#include "./Class.h"

namespace osm2pgr {

/*!
    Parser callback for configuration files
*/
void ConfigurationParserCallback::StartElement(
        const char *name,
        const char** atts) {
    if (strcmp(name, "class") == 0) {
         m_current->add_class(atts);
    } else if (strcmp(name, "type") == 0) {
        m_current = new Type(atts);
    } else if (strcmp(name, "configuration") == 0) {
    }
}


void ConfigurationParserCallback::EndElement(const char* name) {
    if (strcmp(name, "type") == 0) {
        m_config.AddType(*m_current);
        delete m_current;
    }
}

}  // namespace osm2pgr
