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

#ifndef ISL_CPP_EXPRESSION_INCLUDED
#define ISL_CPP_EXPRESSION_INCLUDED

#include "value.hpp"
#include "space.hpp"
#include "printer.hpp"

#include <isl/aff.h>
#include <iostream>

namespace isl {

template<>
struct object_behavior<isl_aff>
{
    static isl_aff * copy( isl_aff * obj )
    {
        return isl_aff_copy(obj);
    }
    static void destroy( isl_aff *obj )
    {
        isl_aff_free(obj);
    }
    static isl_ctx * get_context( isl_aff * obj )
    {
        return isl_aff_get_ctx(obj);
    }
};

template<>
struct object_behavior<isl_multi_aff>
{
    static isl_multi_aff * copy( isl_multi_aff * obj )
    {
        return isl_multi_aff_copy(obj);
    }
    static void destroy( isl_multi_aff *obj )
    {
        isl_multi_aff_free(obj);
    }
    static isl_ctx * get_context( isl_multi_aff * obj )
    {
        return isl_multi_aff_get_ctx(obj);
    }
};

class expression : public object<isl_aff>
{
public:
    expression(isl_aff *ptr): object(ptr) {}

    static
    expression variable( const local_space & spc,
                         space::dimension_type type, unsigned index )
    {
        local_space expr_space(spc);
        wrap_space(expr_space, type, index);
        return isl_aff_var_on_domain(expr_space.copy(), (isl_dim_type) type, index);
    }

    static
    expression value( const local_space & spc,
                      const value & val )
    {
        isl_local_space *ls = spc.copy();
        if (!isl_local_space_is_set(ls))
            ls = isl_local_space_wrap(ls);

        // FIXME: use isl_aff_val_on_domain when available
        isl_aff *expr = isl_aff_zero_on_domain(ls);
        expr = isl_aff_set_constant_val(expr, val.copy());
        return expr;
    }

    static
    expression value( const local_space & spc, int i )
    {
        isl_local_space *ls = spc.copy();
        if (!isl_local_space_is_set(ls))
            ls = isl_local_space_wrap(ls);

        // FIXME: use isl_aff_val_on_domain when available
        isl_aff *expr = isl_aff_zero_on_domain(ls);
        expr = isl_aff_set_constant_si(expr, i);
        return expr;
    }

    space domain_space() const
    {
        return isl_aff_get_domain_space(get());
    }

    space get_space() const
    {
        return isl_aff_get_space(get());
    }

    local_space get_local_space() const
    {
        return isl_aff_get_local_space(get());
    }

    bool is_constant() const
    {
        return isl_aff_is_cst(get());
    }

    isl::value constant() const
    {
        return isl_aff_get_constant_val(get());
    }

    isl::value coefficient(space::dimension_type type, int index) const
    {
        return isl_aff_get_coefficient_val(get(), (isl_dim_type)type, index);
    }

private:
    static void wrap_space(local_space & spc,
                           space::dimension_type & type, unsigned & index )
    {
        if (spc.space().is_map())
        {
            switch(type)
            {
            case space::input:
                type = space::variable;
                break;
            case space::output:
                type = space::variable;
                index = index + spc.dimension(space::input);
                break;
            default:;
            }
            spc = spc.wrapped();
        }
    }
};

class multi_expression : public object<isl_multi_aff>
{
public:
    multi_expression(isl_multi_aff * p): object(p) {}

    multi_expression(const expression & e):
        object(isl_multi_aff_from_aff(e.copy()))
    {}

    static multi_expression zero(const space & spc, int count)
    {
        auto in_space = spc;
        if (in_space.is_map())
            in_space = in_space.wrapped();

        auto out_space = space(spc.get_context(), set_tuple(count));

        auto expr_space = space::from(in_space, out_space);
        return isl_multi_aff_zero(expr_space.copy());
    }

    static multi_expression identity(const space & spc)
    {
        auto in_space = spc;
        if (in_space.is_map())
            in_space = in_space.wrapped();

        auto n_dim = in_space.dimension(space::variable);
        auto out_space = space(spc.get_context(), set_tuple(n_dim));

        auto expr_space = space::from(in_space, out_space);
        return isl_multi_aff_identity(expr_space.copy());
    }

    expression at(int i) const
    {
        return isl_multi_aff_get_aff(get(), i);
    }

    void set(int i, const expression & e)
    {
        m_object = isl_multi_aff_set_aff(m_object, i, e.copy());
    }

    int size() const
    {
        space s = isl_multi_aff_get_space(m_object);
        return s.dimension(isl::space::output);
    }
};

inline
expression operator+( const expression & lhs, const expression & rhs)
{
    return isl_aff_add(lhs.copy(), rhs.copy());
}
inline
expression operator+( const expression & lhs, int rhs_int)
{
    return isl_aff_add_constant_si(lhs.copy(), rhs_int);
}
inline
expression operator+( const expression & lhs, const value & rhs_val)
{
    return isl_aff_add_constant_val(lhs.copy(), rhs_val.copy());
}
template <typename T> inline
expression operator+( const T & lhs, const expression & rhs )
{
    return rhs + lhs;
}

inline
expression operator-( const expression & e)
{
    return isl_aff_neg(e.copy());
}

inline
expression operator-( const expression & lhs, const expression & rhs)
{
    return isl_aff_sub(lhs.copy(), rhs.copy());
}
inline
expression operator-( const expression & lhs, const value & rhs_val)
{
    return isl_aff_add_constant_val(lhs.copy(), isl_val_neg(rhs_val.copy()));
}
inline
expression operator-( const expression & lhs, int rhs_int)
{
    return isl_aff_add_constant_si(lhs.copy(), -rhs_int);
}
inline
expression operator-( int lhs_val, const expression & rhs )
{
    isl_aff *neg_rhs = isl_aff_neg(rhs.copy());
    isl_aff *result = isl_aff_add_constant_si(neg_rhs, lhs_val);
    return result;
}

inline
expression operator*( const expression & lhs, const value & rhs)
{
    return isl_aff_scale_val(lhs.copy(), rhs.copy());
}
inline
expression operator*( const value & lhs, const expression & rhs )
{
    return rhs * lhs;
}
inline
expression operator*( const expression & lhs, int rhs_int)
{
    auto rhs_val = value(lhs.ctx(), rhs_int);
    return lhs * rhs_val;
}
inline
expression operator*( int lhs_int, const expression & rhs )
{
    return rhs * lhs_int;
}

#if 0
inline
expression operator/ ( const expression & lhs, unsigned rhs_uint )
{
    return isl_aff_scale_down_ui(lhs.copy(), rhs_uint);
}
#endif

inline
expression operator/ ( const expression & lhs, const value & rhs )
{
    return isl_aff_scale_down_val(lhs.copy(), rhs.copy());
}

inline
expression operator% ( const expression & lhs, const value & rhs )
{
    return isl_aff_mod_val(lhs.copy(), rhs.copy());
}

inline
expression operator% ( const expression & lhs, int rhs_int )
{
    auto rhs = value(lhs.ctx(), rhs_int);
    return lhs % rhs;
}

inline
expression floor(const expression & e)
{
    return isl_aff_floor(e.copy());
}

template <> inline
void printer::print<expression>( const expression & expr )
{
    m_printer = isl_printer_print_aff(m_printer, expr.get());
}

template <> inline
void printer::print<multi_expression>( const multi_expression & expr )
{
    m_printer = isl_printer_print_multi_aff(m_printer, expr.get());
}

inline
expression space::operator()(dimension_type type, int index)
{
    return expression::variable(local_space(*this), type, index);
}

inline
expression space::param(int index)
{
    return expression::variable(local_space(*this), isl::space::parameter, index);
}

inline
expression space::var(int index)
{
    return expression::variable(local_space(*this), isl::space::variable, index);
}

inline
expression space::in(int index)
{
    return expression::variable(local_space(*this), isl::space::input, index);
}

inline
expression space::out(int index)
{
    return expression::variable(local_space(*this), isl::space::output, index);
}

inline
expression space::val(int v)
{
    return expression::value(local_space(*this), v);
}


inline
expression local_space::operator()(space::dimension_type type, int index)
{
    return expression::variable(*this, type, index);
}

}
#endif // ISL_CPP_EXPRESSION_INCLUDED
