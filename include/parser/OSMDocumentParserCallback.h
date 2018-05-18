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


#ifndef SRC_OSMDOCUMENTPARSERCALLBACK_H_
#define SRC_OSMDOCUMENTPARSERCALLBACK_H_
#pragma once

#ifdef BOOST_NO_CXX11_NULLPTR
#define nullptr NULL
#endif


#include <string.h>
#include "./XMLParser.h"

namespace osm2pgr {

class OSMDocument;
class Node;
class Way;
class Relation;

/**
    Parser callback for OSMDocument files
*/
class OSMDocumentParserCallback :
  public xml::XMLParserCallback {
    //! reference to a OSMDocument object
    OSMDocument& m_rDocument;
    //! current way, which will be parsed
    // Way* m_pActWay;
    Relation* m_pActRelation;

    virtual void StartElement(const char *name, const char** atts);

    virtual void EndElement(const char* name);

 public:
    /**
     *    Constructor
     */
    explicit OSMDocumentParserCallback(OSMDocument& doc) :
        m_rDocument(doc),
        m_pActRelation(0),
        last_node(nullptr),
        last_way(nullptr),
        last_relation(nullptr),
        m_line(0),
        m_section(1) {
    }
 private:
    void show_progress();

 private:
    Node *last_node;
    Way *last_way;
    Relation* last_relation;
    size_t m_line;
    int m_section;
};  // class OSMDocumentParserCallback

}  // end namespace osm2pgr

#endif  // SRC_OSMDOCUMENTPARSERCALLBACK_H_
