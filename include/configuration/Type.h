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
#include "./Class.h"

namespace osm2pgr {

class Type {
 public:
    Type() = default;
    Type(const Type &) = default;
    /**
     *    @param atts attributes read py the parser
     */
    explicit Type(const char **atts);
    inline int64_t id() const {return m_id;}
    inline std::string name() const {return m_name;}
    void add_class(const char **atts);
    std::map<std::string, Class> classes() const {
        return m_Classes;
    }
    std::map<std::string, Class>& classes() {
        return m_Classes;
    }

    inline bool has_class(const std::string &class_name) const {
        return m_Classes.count(class_name);
    }

 private:
    /**
     *    saves the classes of the type
     *    @param pClass class
     */
    void AddClass(const Class &pClass);

 private:
    std::map<std::string, Class> m_Classes;
    int64_t m_id;
    std::string m_name;
    std::map<std::string, std::string> m_tags;
};

}  // namespace osm2pgr
#endif  // SRC_TYPE_H_
