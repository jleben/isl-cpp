#ifndef ISL_CPP_PRINTER_INCLUDED
#define ISL_CPP_PRINTER_INCLUDED

#include "context.hpp"

#include <isl/printer.h>

#include <memory>

namespace isl {

class printer
{
public:
    printer(context & ctx):
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
