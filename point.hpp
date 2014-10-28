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

#ifndef ISL_CPP_POINT_INCLUDED
#define ISL_CPP_POINT_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "space.hpp"
#include "value.hpp"
#include "printer.hpp"

#include <isl/point.h>

namespace isl {

template<>
struct object_behavior<isl_point>
{
    static isl_point * copy( isl_point * obj )
    {
        return isl_point_copy(obj);
    }
    static void destroy( isl_point *obj )
    {
        isl_point_free(obj);
    }
    static isl_ctx * get_context( isl_point * obj )
    {
        return isl_point_get_ctx(obj);
    }
};

class point : public object<isl_point>
{
public:
    point( isl_point * pt ): object(pt) {}

    value operator()( space::dimension_type type, int dim )
    {
        return isl_point_get_coordinate_val
                ( get(), (isl_dim_type) type, dim );
    }
};

template<> inline
void printer::print<point>( const point & pt )
{
    m_printer = isl_printer_print_point(m_printer, pt.get());
}

}

#endif // ISL_CPP_POINT_INCLUDED
