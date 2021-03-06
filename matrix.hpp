/*
isl-cpp: C++ bindings to the ISL (Integer Set Library)

Copyright (C) 2014  Jakob Leben <jakob.leben@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
            return *this;
        }
        element & operator=( int i )
        {
            isl_mat_set_element_si(matrix, row, column, i);
            return *this;
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

    matrix(isl_mat *ptr): object(ptr) {}
    // Does not initialize!
    matrix( const context & ctx, unsigned rows, unsigned columns ):
        object(ctx, isl_mat_alloc(ctx.get(), rows, columns))
    {}
    matrix( const context & ctx, unsigned rows, unsigned columns, int val ):
        object(ctx, isl_mat_alloc(ctx.get(), rows, columns))
    {
        for(int row = 0; row < row_count(); ++row)
        {
            for (int col = 0; col < column_count(); ++col)
            {
                isl_mat_set_element_si(get(), row, col, val);
            }
        }
    }

    context get_context() const { return isl_mat_get_ctx(get()); }

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

    matrix right_kernel() const
    {
        return isl_mat_right_kernel(copy());
    }

    matrix nullspace() const
    {
        return right_kernel();
    }

    void drop_column(int col)
    {
        int n_rows = row_count();
        int n_cols = column_count() - 1;

        auto result = isl_mat_alloc(ctx().get(),
                                    n_rows, n_cols);

        for (int r = 0; r < n_rows; ++r)
        {
            for (int c = 0; c < col; ++c)
            {
                auto val = isl_mat_get_element_val(get(), r, c);
                result = isl_mat_set_element_val(result, r, c, val);
            }

            for (int c = col; c < n_cols; ++c)
            {
                auto val = isl_mat_get_element_val(get(), r, c+1);
                result = isl_mat_set_element_val(result, r, c, val);
            }
        }

        isl_mat_free(m_object);

        m_object = result;
    }

    static matrix concatenate_vertically(const matrix & a, const matrix & b)
    {
        if (a.column_count() != b.column_count())
            throw error("Matrices do not have equal number of columns");

        auto result = isl::matrix(a.get_context(),
                                  a.row_count() + b.row_count(),
                                  a.column_count());

        for (int row = 0; row < a.row_count(); ++row)
        {
            for (int col = 0; col < a.column_count(); ++col)
            {
                result(row,col) = a(row,col).value();
            }
        }
        for (int row = 0; row < b.row_count(); ++row)
        {
            for (int col = 0; col < b.column_count(); ++col)
            {
                result(a.row_count() + row, col) = b(row,col).value();
            }
        }
        return result;
    }
};

void print( const matrix & m, int field_width = 4 );

}

#endif // ISL_CPP_MATRIX_INCLUDED
