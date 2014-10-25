#ifndef ISL_CPP_SET_INCLUDED
#define ISL_CPP_SET_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "point.hpp"
#include "space.hpp"
#include "constraint.hpp"
#include "printer.hpp"

#include <isl/set.h>

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
        object(spc.ctx(), isl_basic_set_empty(spc.get()))
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
};

class set : public object<isl_set>
{
public:
    set( isl_set * ptr ): object(ptr) {}
    set( space & spc ):
        object(spc.ctx(), isl_set_empty(spc.get()))
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
        isl_point *p = isl_set_sample_point(get());
        if (!p)
            throw error("No single point.");
        return p;
    }
};

class union_set : public object<isl_union_set>
{
public:
    union_set( isl_union_set * ptr ): object(ptr) {}
    union_set( context & ctx, const string & text ):
        object(ctx, isl_union_set_read_from_str(ctx.get(), text.c_str()))
    {}
    union_set( space & spc ):
        object(spc.ctx(), isl_union_set_empty(spc.get()))
    {}
    space get_space() const
    {
        return space( isl_union_set_get_space(get()) );
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
