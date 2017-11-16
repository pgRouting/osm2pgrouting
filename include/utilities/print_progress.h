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

#ifndef SRC_PRINT_PROGRESS_H_
#define SRC_PRINT_PROGRESS_H_
#pragma once


#include <iostream>
#include <string>

template < typename T1 , typename T2>
void
print_progress(T1 wantProgress, T2 currentProgress) {
    int length = 50;
    double percent = static_cast <double> (currentProgress) /  static_cast <double>(wantProgress);
    int fillerLength = static_cast<int>(percent * length);

    std::string bar = "[";
    for (int i = 0; i < fillerLength; i++) {
        bar += "*";
    }

    bar += "|";
    for (int i = 0; i < length - fillerLength; i++) {
        bar += " ";
    }

    bar += "]";
    std::cout << "\r"
        << bar
        << " (" << static_cast<int>(100 * percent) << "%)"
        << " Total processed: " << currentProgress << std::flush;
}
#endif  // SRC_PRINT_PROGRESS_H_
