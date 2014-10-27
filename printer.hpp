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

#ifndef ISL_CPP_PRINTER_INCLUDED
#define ISL_CPP_PRINTER_INCLUDED

#include "context.hpp"

#include <isl/printer.h>

#include <memory>

namespace isl {

class printer
{
public:
    printer(const context & ctx):
        m_ctx(ctx)
    {
        m_printer = isl_printer_to_file(ctx.get(), stdout);
    }

    printer(const printer & other) = delete;

    ~printer()
    {
        isl_printer_free(m_printer);
    }

    isl_printer *get() const { return m_printer; }

    template <typename T>
    void print( const T & );

private:
    isl_printer *m_printer;
    context m_ctx;
};

}

#endif // ISL_CPP_PRINTER_INCLUDED
