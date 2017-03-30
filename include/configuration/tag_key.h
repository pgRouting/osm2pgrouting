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


#ifndef SRC_TYPE_H_
#define SRC_TYPE_H_

#include <map>
#include <cstdint>
#include <cassert>
#include <string>
#include "./tag_value.h"

namespace osm2pgr {

class Tag_key : Element {
 public:
    Tag_key() = default;
    Tag_key(const Tag_key &) = default;
    /** @brief build it
     *    @param atts attributes read py the parser
     */
    explicit Tag_key(const char **atts);
    void add_tag_value(const Tag_value &p_values);


    /* to have or not to have */
    bool has(const Tag &tag, const std::string &str) const;
    bool has_tag_value(const Tag &tag) const;

    /* get it*/
    std::string get(const Tag &tag, const std::string &str) const;
    const Tag_value& tag_value(const Tag &tag) const;
    inline int64_t id() const {return osm_id();}
    inline std::string name() const {return get_attribute("name");}

    /* used in the export function */
    std::vector<std::string> values(
            const std::vector<std::string> &columns) const;

 private:
    std::map<std::string, Tag_value> m_Tag_values;
};

}  // namespace osm2pgr
#endif  // SRC_TYPE_H_
