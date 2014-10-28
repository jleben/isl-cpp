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

#ifndef ISL_CPP_SET_INCLUDED
#define ISL_CPP_SET_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "point.hpp"
#include "space.hpp"
#include "expression.hpp"
#include "constraint.hpp"
#include "matrix.hpp"
#include "printer.hpp"

#include <isl/set.h>
#include <isl/ilp.h>

namespace isl {

template<>
struct object_behavior<isl_basic_set>
{
    static isl_basic_set * copy( isl_basic_set * obj )
    {
        return isl_basic_set_copy(obj);
    }
    static void destroy( isl_basic_set *obj )
    {
        isl_basic_set_free(obj);
    }
    static isl_ctx * get_context( isl_basic_set * obj )
    {
        return isl_basic_set_get_ctx(obj);
    }
};

template<>
struct object_behavior<isl_set>
{
    static isl_set * copy( isl_set * obj )
    {
        return isl_set_copy(obj);
    }
    static void destroy( isl_set *obj )
    {
        isl_set_free(obj);
    }
    static isl_ctx * get_context( isl_set * obj )
    {
        return isl_set_get_ctx(obj);
    }
};

template<>
struct object_behavior<isl_union_set>
{
    static isl_union_set * copy( isl_union_set * obj )
    {
        return isl_union_set_copy(obj);
    }
    static void destroy( isl_union_set *obj )
    {
        isl_union_set_free(obj);
    }
    static isl_ctx * get_context( isl_union_set * obj )
    {
        return isl_union_set_get_ctx(obj);
    }
};

class basic_set : public object<isl_basic_set>
{
public:
    basic_set( isl_basic_set * ptr ): object( ptr ) {}
    basic_set( space & spc ):
        object(spc.ctx(), isl_basic_set_empty(spc.copy()))
    {}
    basic_set( const space & spc,
               const matrix & equalities, const matrix & inequalities ):
        object(isl_basic_set_from_constraint_matrices(spc.copy(),
                                                      equalities.copy(),
                                                      inequalities.copy(),
                                                      isl_dim_param,
                                                      isl_dim_div,
                                                      isl_dim_set,
                                                      isl_dim_cst))
    {}
    basic_set( context & ctx, const string & text ):
        object(ctx, isl_basic_set_read_from_str(ctx.get(), text.c_str()))
    {}
    static basic_set universe( const space & s )
    {
        return isl_basic_set_universe(s.copy());
    }
    space get_space() const
    {
        return space( isl_basic_set_get_space(get()) );
    }
    void add_constraint( const constraint & c)
    {
        m_object = isl_basic_set_add_constraint(m_object, c.copy());
    }
};

class set : public object<isl_set>
{
public:
    set( isl_set * ptr ): object(ptr) {}
    set( space & spc ):
        object(spc.ctx(), isl_set_empty(spc.copy()))
    {}
    set( const basic_set & bset ):
        object(bset.ctx(), isl_set_from_basic_set(bset.copy()))
    {}
    set( context & ctx, const string & text ):
        object(ctx, isl_set_read_from_str(ctx.get(), text.c_str()))
    {}
    static set universe( const space & s )
    {
        return isl_set_universe(s.copy());
    }
    space get_space() const
    {
        return space( isl_set_get_space(get()) );
    }
    value minimum( const expression & expr )
    {
        isl_val *v = isl_set_min_val(get(), expr.get());
        if (!v)
            throw error("No solution.");
        return v;
    }
    set lex_minimum() const
    {
        return isl_set_lexmin(copy());
    }
    set lex_maximum() const
    {
        return isl_set_lexmax(copy());
    }
    void coalesce()
    {
        m_object = isl_set_coalesce(m_object);
    }
    void add_constraint( const constraint & c)
    {
        m_object = isl_set_add_constraint(m_object, c.copy());
    }

    point single_point() const
    {
        isl_point *p = isl_set_sample_point(copy());
        if (!p)
            throw error("No single point.");
        return p;
    }
};

class union_set : public object<isl_union_set>
{
public:
    union_set( isl_union_set * ptr ): object(ptr) {}
    union_set( const context & ctx ):
        object(ctx, isl_union_set_empty(isl_space_params_alloc(ctx.get(),0)))
    {}
    union_set( space & param_space ):
        object(param_space.ctx(), isl_union_set_empty(param_space.copy()))
    {}
    union_set( context & ctx, const string & text ):
        object(ctx, isl_union_set_read_from_str(ctx.get(), text.c_str()))
    {}
    space get_space() const
    {
        return space( isl_union_set_get_space(get()) );
    }
    template <typename F>
    void for_each( F f ) const
    {
        isl_union_set_foreach_set(get(), &for_each_helper<F>, &f);
    }
private:
    template <typename F>
    static int for_each_helper(isl_set *set_ptr, void *data_ptr)
    {
        auto f_ptr = reinterpret_cast<F*>(data_ptr);
        bool result = (*f_ptr)(set(set_ptr));
        return result ? 0 : -1;
    }
};

set operator&( const set & lhs, const set & rhs )
{
    isl_set *x = isl_set_intersect(lhs.copy(), rhs.copy());
    return set(x);
}
basic_set operator&( const basic_set & lhs, const basic_set & rhs )
{
    isl_basic_set *x = isl_basic_set_intersect(lhs.copy(), rhs.copy());
    return basic_set(x);
}

set operator|( const set & lhs, const set & rhs )
{
    isl_set *u = isl_set_union(lhs.copy(), rhs.copy());
    return set(u);
}
set operator|( const basic_set & lhs, const basic_set & rhs )
{
    isl_set *u = isl_basic_set_union(lhs.copy(), rhs.copy());
    return set(u);
}
union_set operator| (const union_set &lhs, const union_set & rhs)
{
    return isl_union_set_union(lhs.copy(), rhs.copy());
}
union_set operator| (const union_set &lhs, const set & rhs)
{
    return isl_union_set_union(lhs.copy(), isl_union_set_from_set(rhs.copy()));
}
union_set operator| (const union_set &lhs, const basic_set & rhs)
{
    return isl_union_set_union(lhs.copy(), isl_union_set_from_basic_set(rhs.copy()));
}

template <> inline
void printer::print<basic_set>( const basic_set & s )
{
    m_printer = isl_printer_print_basic_set(m_printer, s.get());
}
template <> inline
void printer::print<set>( const set & s )
{
    m_printer = isl_printer_print_set(m_printer, s.get());
}
template <> inline
void printer::print<union_set>( const union_set & s )
{
    m_printer = isl_printer_print_union_set(m_printer, s.get());
}

}


#endif // ISL_CPP_SET_INCLUDED
