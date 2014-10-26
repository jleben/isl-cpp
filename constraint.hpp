#ifndef ISL_CPP_CONSTRAINT_INCLUDED
#define ISL_CPP_CONSTRAINT_INCLUDED

#include "object.hpp"
#include "space.hpp"
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

    static constraint equality( const expression & expr )
    {
        return isl_equality_from_aff(expr.copy());
    }

    static constraint inequality( const expression & expr )
    {
        return isl_inequality_from_aff(expr.copy());
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
constraint operator== (const expression &lhs, int rhs_int)
{
    return constraint::equality(lhs - rhs_int);
}

}

#endif // ISL_CPP_CONSTRAINT_INCLUDED
