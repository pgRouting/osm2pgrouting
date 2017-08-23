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


#include "parser/XMLParser.h"

#include <errno.h>
#include <string.h>
#include <iostream>
#include <cstdio>



namespace xml {

//------------------------------------- global Expat Callbacks:

static void startElement(void *userData, const char *name, const char **atts) {
    XMLParserCallback* pCallback =
        reinterpret_cast<XMLParserCallback*>(userData);
    if (pCallback) pCallback->StartElement(name, atts);
}

static void endElement(void *userData, const char *name) {
    XMLParserCallback* pCallback =
        reinterpret_cast<XMLParserCallback*>(userData);
    if (pCallback) pCallback->EndElement(name);
}




int XMLParser::Parse(XMLParserCallback& rCallback, const char* chFileName) {
  int ret = 1;  // File not found

  FILE* fp = fopen(chFileName, "rb");
  if (fp) {
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetUserData(parser, static_cast<void*>(&rCallback));

    // register Callbacks for start- and end-element events of the parser:
    XML_SetElementHandler(parser, startElement, endElement);

    int done;
    do {  // loop over whole file content
      char buf[BUFSIZ];
      size_t len = fread(buf, 1, sizeof(buf), fp);    // read chunk of data
      // end of file reached if buffer not completely filled
      done = len < sizeof(buf);
      if (!XML_Parse(parser, buf, static_cast<int>(len), done)) {
        // a parse error occurred:
          std::cerr <<
            XML_ErrorString(XML_GetErrorCode(parser))
            << " at line "
            << static_cast<int>(XML_GetCurrentLineNumber(parser));
        fclose(fp);
        ret = 2;    // quit, return = 2 indicating parsing error
        done = 1;
        return ret;
      }
    } while (!done);

    XML_ParserFree(parser);
    fclose(fp);
    ret = 0;
  } else {
      std::cerr <<  "Error opening " << chFileName << ":" << strerror(errno);
  }
  return ret;  // return = 0 indicating success
}



}  // end namespace xml
//! \endcond
