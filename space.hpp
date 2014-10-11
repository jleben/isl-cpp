#ifndef ISL_CPP_SPACE_INCLUDED
#define ISL_CPP_SPACE_INCLUDED

#include "context.hpp"

#include <isl/space.h>

#include <string>
#include <vector>

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

class space
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
        div = isl_dim_div
    };

    space( const space & other )
    {
        m_context = other.m_context;
        m_space = other.copy();
    }

    space & operator=( const space & other )
    {
        m_context = other.m_context;
        m_space = other.copy();
        return *this;
    }

    space( context & ctx, const tuple & params ):
        m_context(ctx)
    {
        m_space = isl_space_params_alloc(ctx.get(), params.size());
        set_names(isl_dim_param, params);
    }

    space( context & ctx, const tuple & params, const tuple & vars ):
        m_context(ctx)
    {
        m_space = isl_space_set_alloc(ctx.get(), params.size(), vars.size());
        set_names(isl_dim_param, params);
        set_names(isl_dim_set, vars);
    }

    space( context & ctx, const tuple & params,
           const tuple & in, const tuple & out ):
        m_context(ctx)
    {
        m_space = isl_space_alloc(ctx.get(), params.size(), in.size(), out.size());
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

    ~space()
    {
        isl_space_free(m_space);
    }

    isl_space *get() const { return m_space; }
    isl_space *copy() const { return isl_space_copy(m_space); }

private:
    space( context & ctx, isl_space *space ):
        m_space(space),
        m_context(ctx)
    {}

    void set_names( isl_dim_type type, const tuple & tup )
    {
        if (!tup.size())
            return;

        isl_space_set_tuple_name(m_space, type, tup.name.c_str());

        int dim_idx = 0;
        for (const string & elem_name : tup.elements)
        {
            if (elem_name.empty())
                continue;

            isl_space_set_dim_name(m_space,
                                   type, dim_idx,
                                   elem_name.c_str());
            ++dim_idx;
        }
    }

    context m_context;
    isl_space *m_space;
};

}

#endif // ISL_CPP_SPACE_INCLUDED
