#ifndef ISL_CPP_CONTEXT_INCLUDED
#define ISL_CPP_CONTEXT_INCLUDED

#include <isl/ctx.h>

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
    context(): d( new data() )
    {
        m_store.emplace(d.get()->ctx, d);
    }
    context( const context & other ):
        d(other.d)
    {}

    context( isl_ctx * ctx )
    {
        auto iter = m_store.find(ctx);
        if (iter != m_store.end())
        {
            d = iter->second.lock();
        }
        else
        {
            d = std::shared_ptr<data>( new data() );
            m_store.emplace(d.get()->ctx, d);
        }
    }

    isl_ctx *get() const { return d->ctx; }

private:

    struct data
    {
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
