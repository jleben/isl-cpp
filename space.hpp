#ifndef ISL_CPP_SPACE_INCLUDED
#define ISL_CPP_SPACE_INCLUDED

#include "object.hpp"
#include "context.hpp"

#include <isl/space.h>
#include <isl/map.h>

#include <string>
#include <vector>
#include <cassert>

namespace isl {

using std::string;
using std::vector;

class tuple
{
public:
    tuple() {}
    tuple(const string & name, int size): name(name), elements(size) {}
    tuple(const vector<string> & elements): elements(elements) {}
    tuple(const string & name, const vector<string> & elements):
        name(name), elements(elements) {}
    int size() const { return elements.size(); }
    string name;
    vector<string> elements;
};

template<>
struct object_behavior<isl_space>
{
    static isl_space * copy( isl_space * obj )
    {
        return isl_space_copy(obj);
    }
    static void destroy( isl_space *obj )
    {
        isl_space_free(obj);
    }
    static isl_ctx * get_context( isl_space * obj )
    {
        return isl_space_get_ctx(obj);
    }
};

class space : public object<isl_space>
{
    friend class set;
    friend class map;
    friend class constraint;

public:
    enum dimension_type
    {
        parameter = isl_dim_param,
        input = isl_dim_in,
        output = isl_dim_out,
        variable = isl_dim_set,
        div = isl_dim_div,
        all = isl_dim_all
    };

    space(isl_space *ptr): object(ptr) {}

    space( context & ctx, const tuple & params ):
        object(ctx, isl_space_params_alloc(ctx.get(), params.size()))
    {
        set_names(isl_dim_param, params);
    }

    space( context & ctx, const tuple & params, const tuple & vars ):
        object(ctx, isl_space_set_alloc(ctx.get(), params.size(), vars.size()))
    {
        set_names(isl_dim_param, params);
        set_names(isl_dim_set, vars);
    }

    space( context & ctx, const tuple & params,
           const tuple & in, const tuple & out ):
        object(ctx, isl_space_alloc(ctx.get(), params.size(), in.size(), out.size()))
    {
        set_names(isl_dim_param, params);
        set_names(isl_dim_in, in);
        set_names(isl_dim_out, out);
    }

    static space for_parameters( context & ctx, int param_count )
    {
        isl_space *s =
                isl_space_params_alloc(ctx.get(), param_count);
        return space(ctx, s);
    }

    static space for_set( context & ctx,
                          int param_count,
                          int dim )
    {
        isl_space *s =
                isl_space_set_alloc(ctx.get(),
                                    param_count,
                                    dim);
        return space(ctx, s);
    }

    static space for_map( context & ctx,
                          int param_count,
                          int in_dim,
                          int out_dim )
    {
        isl_space *s =
                isl_space_alloc(ctx.get(),
                                param_count,
                                in_dim,
                                out_dim);
        return space(ctx, s);
    }

    static space from(const space & domain, const space & range)
    {
        return space( isl_space_map_from_domain_and_range(domain.copy(), range.copy()) );
    }

#if 0
    space range()
    {
        assert(isl_space_is_map(get()));
        return isl_space_range_map(get());
    }

    space domain()
    {
        assert(isl_space_is_map(get()));
        return isl_space_domain_map(get());
    }
#endif
    unsigned int dimension( dimension_type type )
    {
        return isl_space_dim(get(), (isl_dim_type) type );
    }

private:
    space( context & ctx, isl_space *space ): object(ctx, space) {}

    void set_names( isl_dim_type type, const tuple & tup )
    {
        if (!tup.size())
            return;

        isl_space_set_tuple_name(get(), type, tup.name.c_str());

        int dim_idx = 0;
        for (const string & elem_name : tup.elements)
        {
            if (elem_name.empty())
                continue;

            isl_space_set_dim_name(get(),
                                   type, dim_idx,
                                   elem_name.c_str());
            ++dim_idx;
        }
    }
};

inline
space product( const space &lhs, const space &rhs)
{
    return isl_space_product(lhs.copy(), rhs.copy());
}

inline
space domain_product( const space &lhs, const space &rhs)
{
    return isl_space_domain_product(lhs.copy(), rhs.copy());
}

inline
space range_product( const space &lhs, const space &rhs)
{
    return isl_space_range_product(lhs.copy(), rhs.copy());
}

inline
space operator*(const space &lhs, const space &rhs)
{
    return product(lhs, rhs);
}

}

#endif // ISL_CPP_SPACE_INCLUDED
