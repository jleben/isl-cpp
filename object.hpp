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

    bool is_valid() const { return m_object != nullptr; }
    T * get() const { return m_object; }
    T * copy() const { return object_behavior<T>::copy(m_object); }

    object<T> & operator= ( const object<T> & other )
    {
        if (m_object != other.m_object)
        {
            object_behavior<T>::destroy(m_object);
            m_ctx = other.m_ctx;
            m_object = other.copy();
        }
        return *this;
    }


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

    struct copy_of {};

    object(copy_of, T * p):
        m_ctx(object_behavior<T>::get_context(p)),
        m_object(object_behavior<T>::copy(p))
    {}

protected:
    context m_ctx;
    T* m_object;
};

}
#endif // ISL_CPP_OBJECT_INCLUDED
