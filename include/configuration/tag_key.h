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
#include <string>
#include "./tag_value.h"

namespace osm2pgr {

class Tag_key {
 public:
    Tag_key() = default;
    Tag_key(const Tag_key &) = default;
    /**
     *    @param atts attributes read py the parser
     */
    explicit Tag_key(const char **atts);

    inline int64_t id() const {return m_id;}

    inline std::string name() const {return m_name;}
    void add_class(const char **atts);
    std::map<std::string, Tag_value> classes() const {
        return m_Tag_values;
    }
    std::map<std::string, Tag_value>& classes() {
        return m_Tag_values;
    }

    inline bool has_class(const std::string &class_name) const {
        return m_Tag_values.count(class_name);
    }

 private:
    /**
     *    saves the classes of the type
     *    @param pTag_value class
     */
    void AddTag_value(const Tag_value &pTag_value);

 private:
    std::map<std::string, Tag_value> m_Tag_values;
    int64_t m_id;
    std::string m_name;
    std::map<std::string, std::string> m_tags;
};

}  // namespace osm2pgr
#endif  // SRC_TYPE_H_
