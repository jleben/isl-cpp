/*
isl-cpp: C++ bindings to the ISL (Integer Set Library)

Copyright (C) 2014  Jakob Leben <jakob.leben@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "matrix.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace isl {

void print( const matrix & m, int field_width )
{
    using namespace std;

    int rows = m.row_count();
    int cols = m.column_count();

    for(int r = 0; r < rows; ++r)
    {
        for(int c = 0; c < cols; ++c)
        {
            if (c > 0)
                cout << " ";

            auto val = m(r, c).value();

            ostringstream text;
            text << val.numerator();
            if (val.denominator() != 1)
                text << "/" << val.denominator();

            string str = text.str();
            if (str.size() <= field_width)
                cout << setw(field_width) << str;
            else
                cout << string(field_width, '.');
        }
        cout << endl;
    }
}

}
