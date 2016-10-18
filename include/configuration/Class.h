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

#ifndef SRC_CLASS_H_
#define SRC_CLASS_H_

#include <map>
#include <string>

namespace osm2pgr {

class Class {
 public:
     Class() = default;
     Class(const Class &) = default;
     explicit Class(const char ** attributes);


     inline int64_t id() const {return m_id;}
     inline std::string name() const {return m_name;}
     inline double priority() const {return m_priority;}
     inline double default_maxspeed() const {return m_default_maxspeed;}

 private:
    int64_t m_id;
    std::string m_name;
    double m_priority;
    double m_default_maxspeed;
    std::map<std::string, std::string> m_tags;
};


}  // end namespace osm2pgr
#endif  // SRC_CLASS_H_
