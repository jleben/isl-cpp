#ifndef ISL_CPP_CONSTRAINT_INCLUDED
#define ISL_CPP_CONSTRAINT_INCLUDED

#include "space.hpp"

#include <isl/constraint.h>

namespace isl {

class constraint
{
public:
    constraint( const space & s )
    {
        isl_local_space *ls = isl_local_space_from_space(s.copy());
        m_equalities = isl_equality_alloc(ls);
        m_inequalities = isl_inequality_alloc(ls);
    }

    ~constraint()
    {
        isl_constraint_free(m_equalities);
        isl_constraint_free(m_inequalities);
    }

private:
    isl_constraint *m_equalities;
    isl_constraint *m_inequalities;
};

}

#endif // ISL_CPP_CONSTRAINT_INCLUDED
