/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt                                      *
 *   gentoo.murray@gmail.com                                                  *
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
#include "./ConfigurationParserCallback.h"
#include "./OSMDocument.h"
#include "./Configuration.h"
#include "./Type.h"
#include "./Class.h"

namespace osm {

/**
    Parser callback for configuration files
*/
void ConfigurationParserCallback::StartElement(
    const char *name,
    const char** atts) {
    // std::cout << "SE for <" << name << ">" << std::endl;
    if (strcmp(name, "class") == 0) {
        if (atts != NULL) {
            long long id = -1;
            std::string name;
            double priority  =  1;
            int maxspeed = 50;
            const char** attribut = (const char**)atts;
            while (*attribut != NULL) {
                const char* key = *attribut++;
                const char* value = *attribut++;
                if (strcmp(key, "id") == 0) {
                    id = atoll(value);
                    // std::cout << "class id = " << id << std::endl;
                } else if (strcmp(key, "name") == 0) {
                    name = value;
                    // std::cout << "class name = " << name << std::endl;
                } else if (strcmp(key, "priority") == 0) {
                    priority = boost::lexical_cast<double>(value);
                } else if (strcmp(key, "maxspeed") == 0) {
                    maxspeed = boost::lexical_cast<int>(value);
                }
            }
            if (id > 0 && !name.empty()) {
                m_pActType->AddClass(new Class(id, name, priority, maxspeed));
            }
        }
    } else if (strcmp(name, "type") == 0) {
        if (atts != NULL) {
            long long id;
            std::string name;
            const char** attribut = (const char**)atts;
            while (*attribut != NULL) {
                const char* key = *attribut++;
                const char* value = *attribut++;
                if (strcmp(key, "id") == 0) {
                    id = atoll(value);

                } else if (strcmp(key, "name") == 0) {
                    name = value;
                }
            }
            if (!name.empty()) {
                m_pActType = new Type(id, name);
            }
        }
    } else if (strcmp(name, "configuration") == 0) {
    }
}

/**
 * @brief Parser callback for the last element of the document
 * 
 */
void ConfigurationParserCallback::EndElement(const char* name) {
    if (strcmp(name, "type") == 0) {
        m_rDocument.AddType(m_pActType);
        m_pActType = 0;
    }
}

};  // end namespace osm
