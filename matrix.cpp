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
