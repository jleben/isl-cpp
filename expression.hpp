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

class expression : public object<isl_aff>
{
public:
    expression(isl_aff *ptr): object(ptr) {}

    static
    expression variable( const local_space & spc,
                         space::dimension_type type, unsigned index )
    {
        return isl_aff_var_on_domain(spc.copy(), (isl_dim_type) type, index);
    }

    static
    expression value( const local_space & spc,
                      const value & val )
    {
        // FIXME: use isl_aff_val_on_domain when available
        isl_aff *expr = isl_aff_zero_on_domain(spc.copy());
        expr = isl_aff_set_constant_val(expr, val.copy());
        return expr;
    }

    space get_space() const
    {
        return isl_aff_get_space(get());
    }

    local_space get_local_space() const
    {
        return isl_aff_get_local_space(get());
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
    auto rhs = expression::value
            (lhs.get_local_space().domain(), value(lhs.ctx(), rhs_int));

    return lhs + rhs;
}

inline
expression operator+( int lhs_int, const expression & rhs )
{
    return rhs + lhs_int;
}

inline
expression operator-( const expression & lhs, const expression & rhs)
{
    return isl_aff_sub(lhs.copy(), rhs.copy());
}

inline
expression operator-( const expression & lhs, int rhs_int)
{
    auto rhs = expression::value
            (lhs.get_local_space().domain(), value(lhs.ctx(), rhs_int));

    return lhs - rhs;
}

inline
expression operator-( int lhs_val, const expression & rhs )
{
    auto lhs = expression::value
            (rhs.get_local_space().domain(), value(rhs.ctx(), lhs_val));
    return lhs - rhs;
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
expression operator*(  int lhs_int, const expression & rhs )
{
    return rhs * lhs_int;
}

template <> inline
void printer::print<expression>( const expression & expr )
{
    m_printer = isl_printer_print_aff(m_printer, expr.get());
}

}
#endif // ISL_CPP_EXPRESSION_INCLUDED
