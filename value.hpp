#ifndef ISL_CPP_VALUE_INCLUDED
#define ISL_CPP_VALUE_INCLUDED

#include "object.hpp"

#include <isl/val.h>

namespace isl {

template<>
struct object_behavior<isl_val>
{
    static isl_val * copy( isl_val * obj )
    {
        return isl_val_copy(obj);
    }
    static void destroy( isl_val *obj )
    {
        isl_val_free(obj);
    }
    static isl_ctx * get_context( isl_val * obj )
    {
        return isl_val_get_ctx(obj);
    }
};

class value : public object<isl_val>
{
public:
    value( isl_val *v ): object(v) {}
    value( const context & ctx, long v ):
        object(ctx, isl_val_int_from_si(ctx.get(), v)) {}
    value( const context & ctx, unsigned long v ):
        object(ctx, isl_val_int_from_ui(ctx.get(), v)) {}
    value( const context & ctx, int v ):
        object(ctx, isl_val_int_from_si(ctx.get(), v)) {}
    value( const context & ctx, unsigned int v ):
        object(ctx, isl_val_int_from_ui(ctx.get(), v)) {}
    long numerator() { return isl_val_get_num_si(get()); }
    long denominator() { return isl_val_get_den_si(get()); }
    double real() { return isl_val_get_d(get()); }
    bool is_integer() { return isl_val_is_int(get()); }
};

}

#endif
