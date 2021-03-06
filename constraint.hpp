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

#ifndef ISL_CPP_CONSTRAINT_INCLUDED
#define ISL_CPP_CONSTRAINT_INCLUDED

#include "object.hpp"
#include "space.hpp"
#include "value.hpp"
#include "expression.hpp"

#include <isl/constraint.h>

namespace isl {

template<>
struct object_behavior<isl_constraint>
{
    static isl_constraint * copy( isl_constraint * obj )
    {
        return isl_constraint_copy(obj);
    }
    static void destroy( isl_constraint *obj )
    {
        isl_constraint_free(obj);
    }
    static isl_ctx * get_context( isl_constraint * obj )
    {
        return isl_constraint_get_ctx(obj);
    }
};

class constraint : public object<isl_constraint>
{
public:
    constraint(isl_constraint * ptr): object(ptr) {}

    static constraint equality( const local_space & spc )
    {
        return isl_equality_alloc(spc.copy());
    }
    static constraint inequality( const local_space & spc )
    {
        return isl_inequality_alloc(spc.copy());
    }
    static constraint equality( const expression & expr )
    {
        return isl_equality_from_aff(expr.copy());
    }
    static constraint inequality( const expression & expr )
    {
        return isl_inequality_from_aff(expr.copy());
    }

    bool is_equality() const
    {
        return isl_constraint_is_equality(get());
    }

    void set_coefficient( space::dimension_type type, int pos, int val )
    {
        m_object = isl_constraint_set_coefficient_si
                (m_object, (isl_dim_type) type, pos, val);
    }
    void set_coefficient( space::dimension_type type, int pos, const value & val )
    {
        m_object = isl_constraint_set_coefficient_val
                (m_object, (isl_dim_type) type, pos, val.copy());
    }
    void set_constant( int val )
    {
        m_object = isl_constraint_set_constant_si(m_object, val);
    }
    void set_constant( const value & val )
    {
        m_object = isl_constraint_set_constant_val(m_object, val.copy());
    }
    isl::expression expression() const
    {
        return isl_constraint_get_aff(get());
    }
    isl::local_space local_space() const
    {
        return isl_constraint_get_local_space(get());
    }
    constraint & unwrap_space()
    {
        m_object = isl_constraint_unwrap_local_space(m_object);
        return *this;
    }
};

inline
constraint operator>= (const expression &lhs, const expression &rhs)
{
    return constraint::inequality(lhs - rhs);
}

inline
constraint operator>= (const expression & lhs, int rhs_int)
{
    return constraint::inequality(lhs - rhs_int);
}

inline
constraint operator> (const expression & lhs, const expression &rhs)
{
    return constraint::inequality(lhs - rhs - 1);
}

inline
constraint operator> (const expression & lhs, int rhs_int)
{
    return constraint::inequality(lhs - (rhs_int + 1));
}

inline
constraint operator<= (const expression &lhs, const expression &rhs)
{
    return constraint::inequality(rhs - lhs);
}

inline
constraint operator<= (const expression &lhs, int rhs_int)
{
    return constraint::inequality(rhs_int - lhs);
}

inline
constraint operator< (const expression &lhs, const expression &rhs)
{
    return constraint::inequality(rhs - lhs - 1);
}

inline
constraint operator< (const expression &lhs, int rhs_int)
{
    return constraint::inequality((rhs_int - 1) - lhs);
}

inline
constraint operator== (const expression &lhs, const expression &rhs)
{
    return constraint::equality(lhs - rhs);
}

inline
constraint operator== (const expression &lhs, const value &rhs)
{
    return constraint::equality(lhs - rhs);
}

inline
constraint operator== (const expression &lhs, int rhs_int)
{
    return constraint::equality(lhs - rhs_int);
}

template <> inline
void printer::print<constraint>( const constraint & s )
{
    m_printer = isl_printer_print_constraint(m_printer, s.get());
}

}

#endif // ISL_CPP_CONSTRAINT_INCLUDED
