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

#include <cstdio> 
#include <errno.h>
#include <string.h>

#include "./stdafx.h"
#include "./XMLParser.h"


namespace xml {

//------------------------------------- global Expat Callbacks:

static void startElement(void *userData, const char *name, const char **atts) {
    XMLParserCallback* pCallback = (XMLParserCallback*) userData;
    if (pCallback) pCallback->StartElement(name, atts);
}

static void endElement(void *userData, const char *name) {
    XMLParserCallback* pCallback = (XMLParserCallback*) userData;
    if (pCallback) pCallback->EndElement(name);
}




int XMLParser::Parse(XMLParserCallback& rCallback, const char* chFileName) {
  int ret = 1;  // File not found

  FILE* fp = fopen(chFileName, "rb");
  if (fp) {
    XML_Parser parser = XML_ParserCreate(NULL);

    XML_SetUserData(parser, (void*)&rCallback);

    //! register Callbacks for start- and end-element events of the parser:
    XML_SetElementHandler(parser, startElement, endElement);

    int done;
    do {  //! loop over whole file content
      char buf[BUFSIZ];
      size_t len = fread(buf, 1, sizeof(buf), fp);    //! read chunk of data
      done = len < sizeof(buf);    //! end of file reached if buffer not completely filled
      if (!XML_Parse(parser, buf, (int)len, done)) {
        //! a parse error occured:
        fprintf(stderr,
            "%s at line %d\n",
            XML_ErrorString(XML_GetErrorCode(parser)), (int)
            XML_GetCurrentLineNumber(parser));
             fclose(fp);
        ret = 2;    //! quit, return = 2 indicating parsing error
        done = 1;
      }
    } while (!done);

    XML_ParserFree(parser);
    fclose(fp);
    ret = 0;
  } else {
    fprintf(stderr, "Error opening %s: %s\n", chFileName, strerror(errno));
  }
  return ret;  //! return = 0 indicating success
}



}  // end namespace xml
//! \endcond
