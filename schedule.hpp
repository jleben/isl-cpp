/*
isl-cpp: C++ bindings to the ISL (Integer Set Library)

Copyright (C) 2014-2016  Jakob Leben <jakob.leben@gmail.com>

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

#ifndef ISL_CPP_SCHEDULE_INCLUDED
#define ISL_CPP_SCHEDULE_INCLUDED

#include "context.hpp"
#include "object.hpp"
#include "map.hpp"
#include "printer.hpp"

#include <isl/schedule.h>
#include <isl/schedule_node.h>

namespace isl {

template<>
struct object_behavior<isl_schedule>
{
    static isl_schedule * copy( isl_schedule * obj )
    {
        return isl_schedule_copy(obj);
    }
    static void destroy( isl_schedule *obj )
    {
        isl_schedule_free(obj);
    }
    static isl_ctx * get_context( isl_schedule * obj )
    {
        return isl_schedule_get_ctx(obj);
    }
};

template<>
struct object_behavior<isl_schedule_node>
{
    static isl_schedule_node * copy( isl_schedule_node * obj )
    {
        return isl_schedule_node_copy(obj);
    }
    static void destroy( isl_schedule_node *obj )
    {
        isl_schedule_node_free(obj);
    }
    static isl_ctx * get_context( isl_schedule_node * obj )
    {
        return isl_schedule_node_get_ctx(obj);
    }
};

class schedule : public object<isl_schedule>
{
public:
    schedule( isl_schedule * ptr ): object( ptr ) {}
    union_set domain() const
    {
      return isl_schedule_get_domain(get());
    }
    union_map map() const
    {
        return isl_schedule_get_map(get());
    }
    union_map map_on_domain() const
    {
      return map().in_domain(domain());
    }
    schedule & intersect_domain(const union_set & domain)
    {
        m_object = isl_schedule_intersect_domain(m_object, domain.copy());
        return *this;
    }
};

class schedule_node : public object<isl_schedule_node>
{
public:
    using object<isl_schedule_node>::object;

    isl_schedule_node_type type() const
    {
        return isl_schedule_node_get_type(m_object);
    }

    int child_count() const
    {
        return isl_schedule_node_n_children(m_object);
    }

    schedule_node child(int pos) const
    {
        return isl_schedule_node_get_child(m_object, pos);
    }

    void to_child(int pos)
    {
        m_object = isl_schedule_node_child(m_object, pos);
    }

    void to_parent()
    {
        m_object = isl_schedule_node_parent(m_object);
    }

    void remove()
    {
        m_object = isl_schedule_node_delete(m_object);
    }
};

template <> inline
void printer::print<schedule>( const schedule & s )
{
    m_printer = isl_printer_print_schedule(m_printer, s.get());
}

}

#endif // ISL_CPP_SCHEDULE_INCLUDED

