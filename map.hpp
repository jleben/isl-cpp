#ifndef ISL_CPP_MAP_INCLUDED
#define ISL_CPP_MAP_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "space.hpp"
#include "set.hpp"
#include "expression.hpp"
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
    basic_map inverse() const
    {
        return basic_map( isl_basic_map_reverse(copy()) );
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
    set operator() ( const set & arg ) const
    {
        return isl_set_apply( arg.copy(), copy() );
    }
    map operator() ( const map & arg ) const
    {
        return isl_map_apply_range( arg.copy(), copy() );
    }
};

class union_map : public object<isl_union_map>
{
public:
    union_map( isl_union_map * ptr ): object(ptr) {}
    union_map( const space & param_space ):
        object(param_space.ctx(), isl_union_map_empty(param_space.copy()))
    {}
    union_map( context & ctx, const string & text ):
        object(ctx, isl_union_map_read_from_str(ctx.get(), text.c_str()))
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
};

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
