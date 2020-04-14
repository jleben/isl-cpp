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

#ifndef ISL_CPP_CONTEXT_INCLUDED
#define ISL_CPP_CONTEXT_INCLUDED

#include <isl/ctx.h>
#include <isl/options.h>

#include <memory>
#include <unordered_map>
#include <string>
#include <exception>

namespace isl {

using std::string;

class matrix;
class set;
class map;
class printer;

class context
{
public:
    enum error_action
    {
        warn_on_error = ISL_ON_ERROR_WARN,
        continue_on_error = ISL_ON_ERROR_CONTINUE,
        abort_on_error = ISL_ON_ERROR_ABORT
    };

    context(): d( new data() )
    {
        m_store.emplace(d.get()->ctx, d);
    }
    context( const context & other ):
        d(other.d)
    {}

    context( isl_ctx * ctx )
    {
        if (ctx == nullptr)
            return;

        auto iter = m_store.find(ctx);
        if (iter != m_store.end())
        {
            d = iter->second.lock();
        }
        else
        {
            d = std::shared_ptr<data>( new data(ctx) );
            m_store.emplace(ctx, d);
        }
    }

    void set_error_action( int action )
    {
        isl_options_set_on_error(get(), action);
    }

    int error_action() const
    {
        return isl_options_get_on_error(get());
    }

    isl_ctx *get() const { return d->ctx; }

private:

    struct data
    {
        data(isl_ctx * ctx): ctx(ctx) {}

        data()
        {
            ctx = isl_ctx_alloc();
            //isl_options_set_on_error(ctx, ISL_ON_ERROR_CONTINUE);
        }

        ~data()
        {
            context::m_store.erase(ctx);
            isl_ctx_free(ctx);
        }

        isl_ctx *ctx;
    };

    friend class data;

    std::shared_ptr<data> d;

    static std::unordered_map<isl_ctx*, std::weak_ptr<data>> m_store;
};

class error : public std::exception
{
public:
    error() {}
    error( const string & what ): m_what(what) {}
    virtual const char *what()
    {
        return m_what.c_str();
    }
private:
    string m_what;
};

}

#endif // ISL_CPP_CONTEXT_INCLUDED
