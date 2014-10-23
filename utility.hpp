#ifndef ISL_UTILITY_INCLUDED
#define ISL_UTILITY_INCLUDED

#include "context.hpp"
#include "space.hpp"
#include "map.hpp"

#include <exception>

namespace isl {

map order_less_than( const space & s )
{
    return isl_map_lex_lt( s.copy() );
}
map order_less_than_or_equal( const space & s )
{
    return isl_map_lex_le( s.copy() );
}
map order_greater_than( const space & s )
{
    return isl_map_lex_gt( s.copy() );
}
map order_greater_than_or_equal( const space & s )
{
    return isl_map_lex_ge( s.copy() );
}

}

#endif // ISL_UTILITY_INCLUDED
