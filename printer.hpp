#ifndef ISL_CPP_PRINTER_INCLUDED
#define ISL_CPP_PRINTER_INCLUDED

#include "context.hpp"
#include "set.hpp"
#include "map.hpp"

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

    void print( const basic_set & s )
    {
        m_printer = isl_printer_print_basic_set(m_printer, s.get());
    }
    void print( const set & s )
    {
        m_printer = isl_printer_print_set(m_printer, s.get());
    }
    void print( const union_set & s )
    {
        m_printer = isl_printer_print_union_set(m_printer, s.get());
    }
    void print( const basic_map & m )
    {
        m_printer = isl_printer_print_basic_map(m_printer, m.get());
    }
    void print( const map & m )
    {
        m_printer = isl_printer_print_map(m_printer, m.get());
    }
    void print( const union_map & m )
    {
        m_printer = isl_printer_print_union_map(m_printer, m.get());
    }
    isl_printer *get() const { return m_printer; }

private:
    isl_printer *m_printer;
    context m_ctx;
};

/*
printer & operator<< ( printer & p, const set & s )
{
    isl_printer_print_set(p.get(), s.get());
}*/

}

#endif // ISL_CPP_PRINTER_INCLUDED
