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


#ifndef SRC_CONFIGURATIONPARSERCALLBACK_H_
#define SRC_CONFIGURATIONPARSERCALLBACK_H_
#pragma once

#ifdef BOOST_NO_CXX11_NULLPTR
#define nullptr NULL
#endif

#include <string.h>
#include "XMLParser.h"


namespace osm2pgr {

class Configuration;
class Tag_key;

/**
  Parser callback for configuration files
  */
class ConfigurationParserCallback : public xml::XMLParserCallback {
 public:
    /**
     *    Constructor
     */
    explicit ConfigurationParserCallback(Configuration& doc) :
        m_config(doc),
        m_current(nullptr) {
        }
 private:
    //! reference to a Configuration object
    Configuration& m_config;

    //! current type, which will be parsed
    Tag_key* m_current;

    virtual void StartElement(const char *name, const char** atts);

    virtual void EndElement(const char* name);
};

}  // namespace osm2pgr

#endif  // SRC_CONFIGURATIONPARSERCALLBACK_H_
