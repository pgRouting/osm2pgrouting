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


#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <map>
#include "./osm_tag.h"
#include "./Node.h"

namespace osm2pgr {


Node::Node(const char **atts) :
    Element(atts),
    m_numsOfUse(0) {
        assert(has_attribute("lat"));
        assert(has_attribute("lon"));
    }


double 
    Node::getLength(const Node &previous) const {
        typedef boost::geometry::model::d2::point_xy<double> point_type;

        /* converted point to fit boost.geomtery
         *      * (`p` and `q` are same as `a ` and `b`)
         *           */
        point_type p(
                boost::lexical_cast<double>(get_attribute("lat")),
                boost::lexical_cast<double>(get_attribute("lon")));

        point_type q(
                boost::lexical_cast<double>(previous.get_attribute("lat")),
                boost::lexical_cast<double>(previous.get_attribute("lon")));

        return boost::geometry::distance(p, q);
    }

}  // namespace osm2pgr
