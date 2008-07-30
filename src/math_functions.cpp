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

#include "stdafx.h"
#include "math_functions.h"
#include <math.h>

// calculation of the lenght is correct???
double getLength(Node* a, Node* b)
{
	int R = 6371; //km
	double PI =3.141592653589793238462643;

	double dLat=(b->lat -a->lat)*PI/180;
	double dLon=(b->lon -a->lon)*PI/180;

	double c = sin(dLat/2) * sin(dLat/2) + cos(a->lat*PI/180) * cos(b->lat*PI/180) * sin(dLon/2) * sin(dLon/2);

	double d = 2 * atan2(sqrt(c), sqrt(1-c)); 

	return R*d;

}

