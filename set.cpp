#include "set.hpp"
#include "expression.hpp"
#include "constraint.hpp"

namespace isl {

value basic_set::maximum( const expression & expr ) const
{
    isl_val *v = isl_basic_set_max_val(get(), expr.get());
    if (!v)
        throw error("No solution.");
    return v;
}

value set::minimum( const expression & expr ) const
{
    isl_val *v = isl_set_min_val(get(), expr.get());
    if (!v)
        throw error("No solution.");
    return v;
}

value set::maximum( const expression & expr ) const
{
    isl_val *v = isl_set_max_val(get(), expr.get());
    if (!v)
        throw error("No solution.");
    return v;
}

void basic_set::add_constraint( const constraint & c)
{
    m_object = isl_basic_set_add_constraint(m_object, c.copy());
}

void set::add_constraint( const constraint & c)
{
    m_object = isl_set_add_constraint(m_object, c.copy());
}

}
