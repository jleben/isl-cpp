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

#ifndef ISL_CPP_MAP_INCLUDED
#define ISL_CPP_MAP_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "space.hpp"
#include "set.hpp"
#include "expression.hpp"
#include "matrix.hpp"
#include "printer.hpp"

#include <isl/map.h>

namespace isl {

template<>
struct object_behavior<isl_basic_map>
{
    static isl_basic_map * copy( isl_basic_map * obj )
    {
        return isl_basic_map_copy(obj);
    }
    static void destroy( isl_basic_map *obj )
    {
        isl_basic_map_free(obj);
    }
    static isl_ctx * get_context( isl_basic_map * obj )
    {
        return isl_basic_map_get_ctx(obj);
    }
};

template<>
struct object_behavior<isl_map>
{
    static isl_map * copy( isl_map * obj )
    {
        return isl_map_copy(obj);
    }
    static void destroy( isl_map *obj )
    {
        isl_map_free(obj);
    }
    static isl_ctx * get_context( isl_map * obj )
    {
        return isl_map_get_ctx(obj);
    }
};

template<>
struct object_behavior<isl_union_map>
{
    static isl_union_map * copy( isl_union_map * obj )
    {
        return isl_union_map_copy(obj);
    }
    static void destroy( isl_union_map *obj )
    {
        isl_union_map_free(obj);
    }
    static isl_ctx * get_context( isl_union_map * obj )
    {
        return isl_union_map_get_ctx(obj);
    }
};

class basic_map : public object<isl_basic_map>
{
public:
    basic_map( isl_basic_map * map ): object(map) {}
    basic_map( const space & spc ):
        object(spc.ctx(), isl_basic_map_empty(spc.copy()))
    {}
    basic_map( context & ctx, const string & text ):
        object(isl_basic_map_read_from_str(ctx.get(), text.c_str()))
    {}
    basic_map( const space & spc,
               const matrix & equalities, const matrix & inequalities ):
        object(isl_basic_map_from_constraint_matrices(spc.copy(),
                                                      equalities.copy(),
                                                      inequalities.copy(),
                                                      isl_dim_param,
                                                      isl_dim_div,
                                                      isl_dim_in,
                                                      isl_dim_out,
                                                      isl_dim_cst))
    {}
    basic_map( const expression & expr ):
        object(expr.ctx(), isl_basic_map_from_aff(expr.copy()))
    {}
    static basic_map universe( const space & s )
    {
        return isl_basic_map_universe(s.copy());
    }
    space get_space() const
    {
        return space( isl_basic_map_get_space(get()) );
    }
    isl::local_space local_space() const
    {
        return isl_basic_map_get_local_space(get());
    }
    basic_map inverse() const
    {
        return basic_map( isl_basic_map_reverse(copy()) );
    }
    void add_constraint( const constraint & c )
    {
        m_object = isl_basic_map_add_constraint(m_object, c.copy());
    }
    basic_map in_domain( const basic_set & domain )
    {
        return isl_basic_map_intersect_domain(copy(), domain.copy());
    }
    basic_map in_range( const basic_set & range )
    {
        return isl_basic_map_intersect_range(copy(), range.copy());
    }
};

class map : public object<isl_map>
{
public:
    map( isl_map * ptr ): object(ptr) {}
    map( const space & spc ):
        object(spc.ctx(), isl_map_empty(spc.copy()))
    {}
    map( context & ctx, const string & text ):
        object(ctx, isl_map_read_from_str(ctx.get(), text.c_str()))
    {}
    map( const expression & expr ):
        object(expr.ctx(), isl_map_from_aff(expr.copy()))
    {}
    map( const basic_map &bm ):
        object(bm.ctx(), isl_map_from_basic_map(bm.copy()))
    {}
    static map universe( const space & s )
    {
        return isl_map_universe(s.copy());
    }
    space get_space() const
    {
        return space( isl_map_get_space(get()) );
    }
#if 0
    space domain_space()
    {
        return space( isl_space_domain_map(isl_map_get_space(get())) );
    }
    space range_space()
    {
        return space( isl_space_range_map(isl_map_get_space(get())) );
    }
#endif
    set range() const
    {
        return isl_map_range(copy());
    }
    set domain() const
    {
        return isl_map_domain(copy());
    }
    bool is_single_valued() const
    {
        return isl_map_is_single_valued(get());
    }
    map inverse() const
    {
        return map( isl_map_reverse(copy()) );
    }
    map lex_minimum() const
    {
        return isl_map_lexmin(copy());
    }
    map lex_maximum() const
    {
        return isl_map_lexmax(copy());
    }
    void coalesce()
    {
        m_object = isl_map_coalesce(m_object);
    }
    map in_domain( const set & domain )
    {
        return isl_map_intersect_domain(copy(), domain.copy());
    }
    map in_range( const set & range )
    {
        return isl_map_intersect_range(copy(), range.copy());
    }
    set operator() ( const set & arg ) const
    {
        return isl_set_apply( arg.copy(), copy() );
    }
    map operator() ( const map & arg ) const
    {
        return isl_map_apply_range( arg.copy(), copy() );
    }
    void map_domain_through( const map & other )
    {
        m_object = isl_map_apply_domain(m_object, other.copy());
    }
    void map_range_through( const map & other )
    {
        m_object = isl_map_apply_range(m_object, other.copy());
    }
    string name( space::dimension_type type )
    {
        return isl_map_get_tuple_name(get(), (isl_dim_type) type);
    }
    void insert_dimensions( space::dimension_type type, unsigned pos, unsigned count )
    {
        m_object = isl_map_insert_dims(m_object, (isl_dim_type) type, pos, count);
    }
    void add_constraint( const constraint & c )
    {
        m_object = isl_map_add_constraint(m_object, c.copy());
    }
};

class union_map : public object<isl_union_map>
{
public:
    union_map( isl_union_map * ptr ): object(ptr) {}
    union_map( const context & ctx ):
        object(ctx, isl_union_map_empty(isl_space_params_alloc(ctx.get(),0)))
    {}
    union_map( const space & param_space ):
        object(param_space.ctx(), isl_union_map_empty(param_space.copy()))
    {}
    union_map( context & ctx, const string & text ):
        object(ctx, isl_union_map_read_from_str(ctx.get(), text.c_str()))
    {}
    union_map( const basic_map & bm ):
        object(bm.ctx(), isl_union_map_from_basic_map(bm.copy()))
    {}
    union_map( const map & m ):
        object(m.ctx(), isl_union_map_from_map(m.copy()))
    {}
    space get_space() const
    {
        return space( isl_union_map_get_space(get()) );
    }
    union_map inverse() const
    {
        return union_map( isl_union_map_reverse(copy()) );
    }
    map map_for( space & spc ) const
    {
        return isl_union_map_extract_map(get(), spc.copy());
    }
    union_map in_domain( const union_set & domain )
    {
        return isl_union_map_intersect_domain(copy(), domain.copy());
    }
    union_map in_range( const union_set & range )
    {
        return isl_union_map_intersect_range(copy(), range.copy());
    }
    void map_domain_through( const union_map & other )
    {
        m_object = isl_union_map_apply_domain(m_object, other.copy());
    }
    void map_range_through( const union_map & other )
    {
        m_object = isl_union_map_apply_range(m_object, other.copy());
    }
    template <typename F>
    void for_each( F f ) const
    {
        isl_union_map_foreach_map(get(), &for_each_helper<F>, &f);
    }
private:
    template <typename F>
    static int for_each_helper(isl_map *map_ptr, void *data_ptr)
    {
        auto f_ptr = reinterpret_cast<F*>(data_ptr);
        map m(map_ptr);
        bool result = (*f_ptr)(m);
        return result ? 0 : -1;
    }
};

union_map operator| (const union_map &lhs, const union_map & rhs)
{
    return isl_union_map_union(lhs.copy(), rhs.copy());
}

union_map operator| (const union_map &lhs, const map & rhs)
{
    return isl_union_map_union(lhs.copy(), isl_union_map_from_map(rhs.copy()));
}

union_map operator| (const union_map &lhs, const basic_map & rhs)
{
    return isl_union_map_union(lhs.copy(), isl_union_map_from_basic_map(rhs.copy()));
}

map operator* ( const map & lhs, const map & rhs )
{
    return isl_map_range_product(lhs.copy(), rhs.copy());
}

template <> inline
void printer::print<basic_map>( const basic_map & m )
{
    m_printer = isl_printer_print_basic_map(m_printer, m.get());
}
template <> inline
void printer::print<map>( const map & m )
{
    m_printer = isl_printer_print_map(m_printer, m.get());
}
template <> inline
void printer::print<union_map>( const union_map & m )
{
    m_printer = isl_printer_print_union_map(m_printer, m.get());
}

}
#endif // ISL_CPP_MAP_INCLUDED
