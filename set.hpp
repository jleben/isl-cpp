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
#include <isl/union_set.h>
#include <isl/ilp.h>

namespace isl {

class basic_map;
class map;
class union_map;

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
    basic_set( const space & spc ):
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
    isl::local_space local_space() const
    {
        return isl_basic_set_get_local_space(get());
    }
    unsigned dimensions() const
    {
        return isl_basic_set_dim( get(), isl_dim_set );
    }

    bool is_empty() const
    {
        return isl_basic_set_is_empty(get());
    }
    void insert_dimensions( space::dimension_type t, unsigned i, unsigned n=1 )
    {
        m_object = isl_basic_set_insert_dims(m_object, (isl_dim_type)t, i, n);
    }
    void add_dimensions( space::dimension_type t, unsigned n=1 )
    {
        m_object = isl_basic_set_add_dims(m_object, (isl_dim_type)t, n);
    }
    void project_out_dimensions( space::dimension_type t, unsigned i, unsigned n=1 )
    {
        m_object = isl_basic_set_project_out(m_object, (isl_dim_type)t, i, n);
    }
    void add_constraint( const constraint & c)
    {
        m_object = isl_basic_set_add_constraint(m_object, c.copy());
    }
    void drop_constraints_with( space::dimension_type t, unsigned i, unsigned n=1)
    {
        m_object = isl_basic_set_drop_constraints_involving_dims
                (m_object, (isl_dim_type) t, i, n);
    }
    basic_map unwrapped();

    basic_set lifted() const
    {
        return isl_basic_set_lift(copy());
    }

    basic_set flattened() const
    {
        return isl_basic_set_flatten(copy());
    }

    static bool are_disjoint( const basic_set & a, const basic_set & b )
    {
        return isl_basic_set_is_disjoint(a.get(), b.get());
    }

    bool is_disjoint(const basic_set & b) const
    {
        return are_disjoint(*this, b);
    }

    point single_point() const
    {
        isl_point *p = isl_basic_set_sample_point(copy());
        if (!p)
            throw error("No single point.");
        return p;
    }

    value maximum( const expression & expr ) const
    {
        isl_val *v = isl_basic_set_max_val(get(), expr.get());
        if (!v)
            throw error("No solution.");
        return v;
    }

#if 0 // Not available?
    basic_set & limit_above(isl::space::dimension_type dim, unsigned pos, int value)
    {
        m_object = isl_basic_set_upper_bound_si(m_object, (isl_dim_type)dim, pos, value);
        return *this;
    }
    basic_set & limit_below(isl::space::dimension_type dim, unsigned pos, int value)
    {
        m_object = isl_basic_set_lower_bound_si(m_object, (isl_dim_type)dim, pos, value);
        return *this;
    }
#endif
};

class set : public object<isl_set>
{
public:
    set( isl_set * ptr ): object(ptr) {}
    set( const space & spc ):
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

    unsigned dimensions() const
    {
        return isl_set_dim( get(), isl_dim_set );
    }

    identifier id() const
    {
        isl_id *c_id = isl_set_get_tuple_id(get());
        identifier id(c_id);
        isl_id_free(c_id);
        return id;
    }
    void set_id(const identifier & id )
    {
        isl_id *c_id = id.c_id(m_ctx.get());
        if (c_id)
            m_object = isl_set_set_tuple_id(get(), c_id);
    }
    void clear_id()
    {
        m_object = isl_set_reset_tuple_id(m_object);
    }
    string name() const
    {
        return isl_set_get_tuple_name(get());
    }
    void set_name( const string & name )
    {
        m_object = isl_set_set_tuple_name(m_object, name.c_str());
    }

    bool is_empty() const
    {
        return isl_set_is_empty(get());
    }

    void add_dimensions( space::dimension_type t, unsigned n=1 )
    {
        m_object = isl_set_add_dims(m_object, (isl_dim_type) t, n);
    }
    void project_out_dimensions( space::dimension_type t, unsigned i, unsigned n=1 )
    {
        m_object = isl_set_project_out(m_object, (isl_dim_type)t, i, n);
    }

    value minimum( const expression & expr ) const
    {
        isl_val *v = isl_set_min_val(get(), expr.get());
        if (!v)
            throw error("No solution.");
        return v;
    }
    value maximum( const expression & expr ) const
    {
        isl_val *v = isl_set_max_val(get(), expr.get());
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
    void insert_dimensions( unsigned pos, unsigned count )
    {
        m_object = isl_set_insert_dims(m_object, isl_dim_set, pos, count);
    }
    void add_constraint( const constraint & c)
    {
        m_object = isl_set_add_constraint(m_object, c.copy());
    }
    void drop_constraints_with( space::dimension_type t, unsigned i, unsigned n=1)
    {
        m_object = isl_set_drop_constraints_involving_dims
                (m_object, (isl_dim_type) t, i, n);
    }

    set & limit_above(isl::space::dimension_type dim, unsigned pos, int value)
    {
        m_object = isl_set_upper_bound_si(m_object, (isl_dim_type)dim, pos, value);
        return *this;
    }
    set & limit_below(isl::space::dimension_type dim, unsigned pos, int value)
    {
        m_object = isl_set_lower_bound_si(m_object, (isl_dim_type)dim, pos, value);
        return *this;
    }

    map unwrapped();

    set lifted() const
    {
        return isl_set_lift(copy());
    }

    set flattened() const
    {
        return isl_set_flatten(copy());
    }

    set parameters() const
    {
        return isl_set_params(copy());
    }

    basic_set convex_hull() const
    {
        return isl_set_convex_hull(copy());
    }
    basic_set simple_hull() const
    {
        return isl_set_simple_hull(copy());
    }

    bool is_singleton() const
    {
        return isl_set_is_singleton(get());
    }

    point single_point() const
    {
        isl_point *p = isl_set_sample_point(copy());
        if (!p)
            throw error("No single point.");
        return p;
    }

    static bool are_disjoint( const set & a, const set & b )
    {
        return isl_set_is_disjoint(a.get(), b.get());
    }

    bool is_disjoint(const set & b) const
    {
        return are_disjoint(*this, b);
    }
    template <typename F>
    void for_each( F f ) const
    {
        isl_set_foreach_basic_set(get(), &for_each_basic_set_helper<F>, &f);
    }

private:
    template <typename F>
    static isl_stat for_each_basic_set_helper(isl_basic_set *bs_ptr, void *data_ptr)
    {
        auto f_ptr = reinterpret_cast<F*>(data_ptr);
        basic_set bs(bs_ptr);
        bool result = (*f_ptr)(bs);
        return result ? isl_stat_ok : isl_stat_error;
    }
};

class union_set : public object<isl_union_set>
{
public:
    union_set( isl_union_set * ptr ): object(ptr) {}
    union_set( const context & ctx ):
        object(ctx, isl_union_set_empty(isl_space_params_alloc(ctx.get(),0)))
    {}
    union_set( const space & param_space ):
        object(param_space.ctx(), isl_union_set_empty(param_space.copy()))
    {}
    union_set( context & ctx, const string & text ):
        object(ctx, isl_union_set_read_from_str(ctx.get(), text.c_str()))
    {}
    union_set( const basic_set & bs ):
        object(bs.ctx(), isl_union_set_from_basic_set(bs.copy()))
    {}
    union_set( const set & s ):
        object(s.ctx(), isl_union_set_from_set(s.copy()))
    {}
    space get_space() const
    {
        return space( isl_union_set_get_space(get()) );
    }
    bool is_empty() const
    {
        return isl_union_set_is_empty(get());
    }

    union_map unwrapped();

    union_set lifted() const
    {
        return isl_union_set_lift(copy());
    }

    union_set universe() const
    {
        return isl_union_set_universe(copy());
    }

    set set_for( const space & spc ) const
    {
        return isl_union_set_extract_set(get(), spc.copy());
    }
    template <typename F>
    void for_each( F f ) const
    {
        isl_union_set_foreach_set(get(), &for_each_helper<F>, &f);
    }

private:
    template <typename F>
    static isl_stat for_each_helper(isl_set *set_ptr, void *data_ptr)
    {
        auto f_ptr = reinterpret_cast<F*>(data_ptr);
        set s(set_ptr);
        bool result = (*f_ptr)(s);
        return result ? isl_stat_ok : isl_stat_error;
    }
};

inline
set operator!( const set & s )
{
    isl_set *x = isl_set_complement(s.copy());
    return set(x);
}

inline
set operator&( const set & lhs, const set & rhs )
{
    isl_set *x = isl_set_intersect(lhs.copy(), rhs.copy());
    return set(x);
}
inline
basic_set operator&( const basic_set & lhs, const basic_set & rhs )
{
    isl_basic_set *x = isl_basic_set_intersect(lhs.copy(), rhs.copy());
    return basic_set(x);
}
inline
union_set operator&( const union_set & lhs, const union_set & rhs )
{
    return isl_union_set_intersect(lhs.copy(), rhs.copy());
}
inline
set & operator&=(set & lhs, const set & rhs )
{
    lhs = lhs & rhs;
    return lhs;
}
inline
union_set & operator&=(union_set & lhs, const union_set & rhs )
{
    lhs = lhs & rhs;
    return lhs;
}

inline
set operator|( const set & lhs, const set & rhs )
{
    isl_set *u = isl_set_union(lhs.copy(), rhs.copy());
    return set(u);
}
inline
set operator|( const basic_set & lhs, const basic_set & rhs )
{
    isl_set *u = isl_basic_set_union(lhs.copy(), rhs.copy());
    return set(u);
}
inline
union_set operator| (const union_set &lhs, const union_set & rhs)
{
    return isl_union_set_union(lhs.copy(), rhs.copy());
}
inline
union_set operator| (const union_set &lhs, const set & rhs)
{
    return isl_union_set_union(lhs.copy(), isl_union_set_from_set(rhs.copy()));
}
inline
union_set operator| (const union_set &lhs, const basic_set & rhs)
{
    return isl_union_set_union(lhs.copy(), isl_union_set_from_basic_set(rhs.copy()));
}
inline
set & operator|=(set & lhs, const set & rhs )
{
    lhs = lhs | rhs;
    return lhs;
}
inline
union_set & operator|=(union_set & lhs, const union_set & rhs )
{
    lhs = lhs | rhs;
    return lhs;
}

inline
set operator* ( const set & lhs, const set & rhs )
{
    return isl_set_product(lhs.copy(), rhs.copy());
}
inline
union_set operator* ( const union_set & lhs, const union_set & rhs )
{
    return isl_union_set_product(lhs.copy(), rhs.copy());
}

inline
set operator- (const set & lhs, const set & rhs)
{
    return isl_set_subtract(lhs.copy(), rhs.copy());
}

inline
union_set operator- (const union_set & lhs, const union_set & rhs)
{
    return isl_union_set_subtract(lhs.copy(), rhs.copy());
}

inline
bool operator==( const basic_set & lhs, const basic_set & rhs)
{
    return isl_basic_set_is_equal(lhs.get(), rhs.get());
}

inline
bool operator==( const set & lhs, const set & rhs)
{
    return isl_set_is_equal(lhs.get(), rhs.get());
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

template <> inline
void printer::print_each_in<set>(const set & u)
{
    u.for_each([this](const basic_set & s){
        print(s); std::cout << std::endl;
        return true;
    });
}

template <> inline
void printer::print_each_in<union_set>(const union_set & us)
{
    us.for_each([this](const set & s){
        print(s); std::cout << std::endl;
        return true;
    });
}
}


#endif // ISL_CPP_SET_INCLUDED
