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

#ifndef ISL_UTILITY_INCLUDED
#define ISL_UTILITY_INCLUDED

#include "context.hpp"
#include "space.hpp"
#include "map.hpp"

#include <exception>

namespace isl {

inline
map order_less_than( const space & s )
{
    return isl_map_lex_lt( s.copy() );
}
inline
map order_less_than_or_equal( const space & s )
{
    return isl_map_lex_le( s.copy() );
}
inline
map order_greater_than( const space & s )
{
    return isl_map_lex_gt( s.copy() );
}
inline
map order_greater_than_or_equal( const space & s )
{
    return isl_map_lex_ge( s.copy() );
}

}

#endif // ISL_UTILITY_INCLUDED
