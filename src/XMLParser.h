/***************************************************************************
 *   Copyright (C) 2008 by Daniel Wendt   								   *
 *   gentoo.murray@gmail.com   											   *
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

#ifndef XMLPARSERT_H
#define XMLPARSERT_H

#include <expat.h>


namespace xml
{

/**
    Callback to be used with XMLParser 
 */
class XMLParserCallback
{
public:

  //! Constructor_
  XMLParserCallback(){}
  //! Destructor
  virtual ~XMLParserCallback(){}

  /**
    Implement to construct an element with the given name,
    call back for parser event "start element"
    
    \param name  [IN]  element name
    \param atts  [IN]  the attributes
   */  
	virtual void StartElement( const char *name, const char** atts )=0;

  /**
    Implement to process parser event "end element"
    */
	virtual void EndElement( const char *elementName )=0;


};

/**
  XML-Parser based on expat library by 
  James Clark http://www.jclark.com/xml/expat.html.
  
  Fast, event driven, non-validating parser
  
  Dependencies:
  - link with xmlparse.lib
  - uses xmlparse.dll
*/
class XMLParser
{

public:
  //! Constructor
	XMLParser(){}
	//! Destructor
	virtual ~XMLParser(){}

  /**
    Parse a file from the file system-
    
    \param rCallback [IN] the parser callback
    \param chFileName [IN] name of the file to be parsed  
    
    \return 0: everything ok, 1: file not found, 2: parsing error
   */  
	int Parse( XMLParserCallback& rCallback, const char* chFileName );

private:
	//! the expat parser object / imported from „expat.h“
	XML_Parser			m_ParserCtxt;

};

} // end namespace xml
#endif
