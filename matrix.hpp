#ifndef ISL_CPP_MATRIX_INCLUDED
#define ISL_CPP_MATRIX_INCLUDED

#include "object.hpp"
#include "context.hpp"
#include "value.hpp"
#include "printer.hpp"

#include <isl/mat.h>

namespace isl {

template<>
struct object_behavior<isl_mat>
{
    static isl_mat * copy( isl_mat * obj )
    {
        return isl_mat_copy(obj);
    }
    static void destroy( isl_mat *obj )
    {
        isl_mat_free(obj);
    }
    static isl_ctx * get_context( isl_mat * obj )
    {
        return isl_mat_get_ctx(obj);
    }
};

class matrix : public object<isl_mat>
{
public:
    class element
    {
        friend class matrix;

        isl_mat *matrix;
        int row;
        int column;
    public:
        element & operator=( const value & v )
        {
            isl_mat_set_element_val(matrix, row, column, v.copy());
        }
        isl::value value() const
        {
            return isl_mat_get_element_val(matrix, row, column);
        }
    private:
        element( isl_mat *matrix, int row, int column ):
            matrix(matrix), row(row), column(column)
        {}
    };

    matrix( const context & ctx, unsigned rows, unsigned columns ):
        object(ctx, isl_mat_alloc(ctx.get(), rows, columns))
    {}
    int row_count() const { return isl_mat_rows(get()); }
    int column_count() const { return isl_mat_cols(get()); }

    element operator() (int row, int column)
    {
        return element(get(), row, column);
    }

    const element operator() (int row, int column) const
    {
        return element(get(), row, column);
    }
};

void print( const matrix & m, int field_width = 4 );

}

#endif // ISL_CPP_MATRIX_INCLUDED
