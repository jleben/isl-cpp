#ifndef ISL_CPP_POINT_INCLUDED
#define ISL_CPP_POINT_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "space.hpp"
#include "value.hpp"

#include <isl/point.h>

namespace isl {

template<>
struct object_behavior<isl_point>
{
    static isl_point * copy( isl_point * obj )
    {
        return isl_point_copy(obj);
    }
    static void destroy( isl_point *obj )
    {
        isl_point_free(obj);
    }
    static isl_ctx * get_context( isl_point * obj )
    {
        return isl_point_get_ctx(obj);
    }
};

class point : public object<isl_point>
{
public:
    point( isl_point * pt ): object(pt) {}

    value operator()( space::dimension_type type, int dim )
    {
        return isl_point_get_coordinate_val
                ( get(), (isl_dim_type) type, dim );
    }
};

}

#endif // ISL_CPP_POINT_INCLUDED
