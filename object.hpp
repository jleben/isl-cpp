#ifndef ISL_CPP_OBJECT_INCLUDED
#define ISL_CPP_OBJECT_INCLUDED

#include "context.hpp"

namespace isl {

template <typename T>
struct object_behavior
{};

template <typename T>
class object
{
public:
    virtual ~object()
    {
        object_behavior<T>::destroy(m_object);
    }

    const context & ctx() const { return m_ctx; }

    T * get() const { return m_object; }
    T * copy() const { return object_behavior<T>::copy(m_object); }

    object<T> & operator= ( const object<T> & other )
    {
        m_ctx = other.m_ctx;
        m_object = other.copy();
    }

protected:
    object( const context & ctx, T * obj ):
        m_ctx(ctx),
        m_object(obj)
    {}

    object( const object<T> & other ):
        m_ctx(other.m_ctx),
        m_object(other.copy())
    {}

    object( T * other_obj ):
        m_ctx( object_behavior<T>::get_context(other_obj) ),
        m_object( other_obj )
    {}

protected:
    context m_ctx;
    T* m_object;
};

}
#endif // ISL_CPP_OBJECT_INCLUDED
