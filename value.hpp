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

#ifndef ISL_CPP_VALUE_INCLUDED
#define ISL_CPP_VALUE_INCLUDED

#include "object.hpp"
#include "printer.hpp"

#include <isl/val.h>
#include <cassert>

namespace isl {

template<>
struct object_behavior<isl_val>
{
    static isl_val * copy( isl_val * obj )
    {
        return isl_val_copy(obj);
    }
    static void destroy( isl_val *obj )
    {
        isl_val_free(obj);
    }
    static isl_ctx * get_context( isl_val * obj )
    {
        return isl_val_get_ctx(obj);
    }
};

class value : public object<isl_val>
{
public:
    value( isl_val *v ): object(v) {}
    value( const context & ctx, long v ):
        object(ctx, isl_val_int_from_si(ctx.get(), v)) {}
    value( const context & ctx, unsigned long v ):
        object(ctx, isl_val_int_from_ui(ctx.get(), v)) {}
    value( const context & ctx, int v ):
        object(ctx, isl_val_int_from_si(ctx.get(), v)) {}
    value( const context & ctx, unsigned int v ):
        object(ctx, isl_val_int_from_ui(ctx.get(), v)) {}
    long numerator() const { return isl_val_get_num_si(get()); }
    long denominator() const { return isl_val_get_den_si(get()); }
    double real() const { return isl_val_get_d(get()); }
    bool is_integer() const { return isl_val_is_int(get()); }
    long integer() const { assert(is_integer()); return numerator(); }
};

template<> inline
void printer::print<value>( const value & v )
{
    m_printer = isl_printer_print_val(m_printer, v.get());
}

}

#endif
