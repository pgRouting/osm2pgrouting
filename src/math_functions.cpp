
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

#include "./math_functions.h"
#include <math.h>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>


// boost fucntion to calculate the distance
double getLength(Node* a, Node *b) {
    typedef boost::geometry::model::d2::point_xy<double> point_type;

    /* converted point to fit boost.geomtery
     * (`p` and `q` are same as `a ` and `b`)
     */
    point_type p(a->lat , a->lon);
    point_type q(b->lat , b->lon);

    return boost::geometry::distance(p, q);
}
