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

#ifndef SRC_RELATION_H_
#define SRC_RELATION_H_

#include <vector>
#include <map>
#include <string>
#include "./osm_element.h"

namespace osm2pgr {
class Way;

/**
\code

<relation id="2781938" version="1" timestamp="2013-02-24T05:24:08Z" changeset="15143772" uid="621319" user="hayashi">
<member type="way" ref="206946707" role="outer"/>
<member type="way" ref="206946714" role="inner"/>
<tag k="building" v="yes"/>
<tag k="type" v="multipolygon"/>
</relation>

<relation id="6127135" version="2" timestamp="2016-04-11T03:26:53Z" changeset="38464643" uid="624003" user="eugenebata">
<member type="way" ref="184275824" role="link"/>
<member type="way" ref="374341622" role=""/>
<tag k="name" v="阪神高速31号神戸山手線"/>
<tag k="name:en" v="Hanshin Expressway Route 31"/>
<tag k="ref" v="31"/>
<tag k="route" v="road"/>
<tag k="type" v="route"/>
</relation>

\endcode
*/
class Relation : public Element{
 public:
     /** 
      *    @param atts attributes read py the parser
      */
     explicit Relation(const char ** atts);
     Relation() = delete;
     ~Relation() {};
     Relation(const Relation&) = default;
     std::vector<int64_t> way_refs() const {return m_WayRefs;}
     std::vector<int64_t>& way_refs() {return m_WayRefs;}
     std::string get_geometry() const {return std::string("");}

     /**
      *    saves the nodes of the way  
      *    @param atts member attributes read py the parser
      */
     int64_t add_member(const char **atts);
     std::string members_str() const;

     friend std::ostream& operator<<(std::ostream &os, const Relation &r);

 private:
     std::vector<int64_t> m_WayRefs;
};


}  // end namespace osm2pgr
#endif  // SRC_RELATION_H_
