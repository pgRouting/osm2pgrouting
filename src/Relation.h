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
#ifndef SRC_RELATION_H_
#define SRC_RELATION_H_

#include <vector>
#include <map>
#include <string>

namespace osm2pgr {
class Way;

/**
\code

<relation id="2781938" version="1" timestamp="2013-02-24T05:24:08Z" changeset="15143772" uid="621319" user="hayashi">
<member type="way" ref="206946707" role="outer"/>
<member type="way" ref="206946714" role="inner"/>
<member type="way" ref="206946713" role="inner"/>
<member type="way" ref="206946715" role="inner"/>
<member type="way" ref="206946721" role="inner"/>
<member type="way" ref="206946723" role="inner"/>
<tag k="building" v="yes"/>
<tag k="type" v="multipolygon"/>
</relation>

<relation id="6127135" version="2" timestamp="2016-04-11T03:26:53Z" changeset="38464643" uid="624003" user="eugenebata">
<member type="way" ref="184275824" role="link"/>
<member type="way" ref="184275806" role="link"/>
<member type="way" ref="184275771" role="link"/>
<member type="way" ref="374341622" role=""/>
<member type="way" ref="374341625" role=""/>
<member type="way" ref="374341511" role=""/>
<member type="way" ref="374341509" role=""/>
<member type="way" ref="48434343" role=""/>
<member type="way" ref="48434229" role=""/>
<member type="way" ref="184275832" role=""/>
<member type="way" ref="48434228" role=""/>
<member type="way" ref="48434218" role=""/>
<member type="way" ref="48434217" role=""/>
<member type="way" ref="48434216" role=""/>
<member type="way" ref="48434215" role=""/>
<member type="way" ref="374341621" role=""/>
<member type="way" ref="48434190" role="link"/>
<member type="way" ref="93148473" role=""/>
<member type="way" ref="93148470" role=""/>
<member type="way" ref="93148447" role=""/>
<member type="way" ref="185567427" role=""/>
<member type="way" ref="93148469" role="link"/>
<member type="way" ref="48437504" role=""/>
<member type="way" ref="93148452" role=""/>
<member type="way" ref="48437501" role=""/>
<member type="way" ref="48437503" role=""/>
<member type="way" ref="48437502" role=""/>
<member type="way" ref="184275829" role=""/>
<member type="way" ref="184275830" role=""/>
<member type="way" ref="184275826" role=""/>
<member type="way" ref="48437500" role=""/>
<member type="way" ref="48437506" role=""/>
<member type="way" ref="184275822" role="link"/>
<member type="way" ref="184275810" role="link"/>
<member type="way" ref="184275776" role="link"/>
<member type="way" ref="93148444" role=""/>
<member type="way" ref="93148467" role=""/>
<member type="way" ref="48434207" role=""/>
<member type="way" ref="48434208" role=""/>
<member type="way" ref="251901729" role="link"/>
<member type="way" ref="251901695" role="link"/>
<member type="way" ref="251901733" role="link"/>
<member type="way" ref="251901727" role="link"/>
<tag k="name" v="阪神高速31号神戸山手線"/>
<tag k="name:en" v="Hanshin Expressway Route 31"/>
<tag k="ref" v="31"/>
<tag k="route" v="road"/>
<tag k="type" v="route"/>
</relation>

\endcode
*/
class Relation {
    public:
        /** 
         *    Constructor
         *    @param id ID of the way
         */
#if 0
        explicit Relation(int64_t id);
#endif
        explicit Relation(const char ** atts);
        Relation() = delete;
        Relation(const Relation&) = default;
#if 0
        //! Destructor
        ~Relation();
#endif
        /**
         *    saves the nodes of the way  
         *    @param pNode node
         */
        void AddWayRef(int64_t osm_id,  const std::string &k, const std::string &v);
        void AddWayRef(int64_t wID);
        int64_t add_member(const char **atts);
        void add_tag(const char **atts, std::string &key, std::string &value);
        void AddTag(std::string key, std::string value);
        inline int64_t osm_id() const {return m_osm_id;}
        inline int64_t visible(bool p_visible) {return m_visible = p_visible;}

        inline void type(std::string p_type) {m_type = p_type;}
        inline void clss(std::string p_clss) {m_clss = p_clss;}
        inline std::string type() const {return m_type;}
        inline std::string clss() const {return m_clss;}

    public:
        int64_t m_osm_id;
        bool m_visible;
        std::string m_type;
        std::string m_clss;

        std::string name;
        std::vector<int64_t> m_WayRefs;
        std::map<std::string, std::string> m_Tags;
        std::map<std::string, std::string> m_attributes;

};


}  // end namespace osm2pgr
#endif  // SRC_RELATION_H_
