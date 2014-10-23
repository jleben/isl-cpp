#include "../context.hpp"
#include "../space.hpp"
#include "../set.hpp"
#include "../map.hpp"
#include "../expression.hpp"
#include "../utility.hpp"
#include "../printer.hpp"

#include <iostream>

using namespace isl;
using namespace std;

void test_expr(context & ctx, printer &p)
{
    set s(ctx, "{[[a,b] -> [c,d]]}");
    auto loc_space = local_space(s.get_space());
    auto a = expression::variable(loc_space, space::variable, 0);
    auto b = expression::variable(loc_space, space::variable, 2);
    auto c = a - b;
    auto m = map(c);
    p.print(c); cout << endl;
    p.print(m); cout << endl;
}

int main()
{
    using isl::tuple;

    context ctx;
    printer p(ctx);

    test_expr(ctx, p);

    space time_space(ctx, tuple(), tuple("T",1));

    map xy_dep(ctx, "{ X[i] -> Y[i] } ");
    map y_schedule(ctx, "{ Y[i] -> T[i + 3] }");
    map x_schedule(ctx, "{ X[i] -> T[i + 7] }");

    cout << "X-Y dependency := ";
    p.print(xy_dep);
    cout << endl;

    cout << "Y schedule := ";
    p.print(y_schedule);
    cout << endl;

    cout << "X schedule := ";
    p.print(x_schedule);
    cout << endl;


    map not_later = order_greater_than_or_equal(time_space);
    map y_not_later = y_schedule.inverse()( not_later );
    cout << "Y before := ";
    p.print(y_not_later);
    cout << endl;

    map not_earlier = order_less_than_or_equal(time_space);
    map x_not_earlier = x_schedule.inverse()( not_earlier ) ;
    map y_consumed_not_earlier = xy_dep( x_not_earlier );

    cout << "X after := ";
    p.print(x_not_earlier);
    cout << endl;

    cout << "Y consumed after := ";
    p.print(y_consumed_not_earlier);
    cout << endl;

    cout << "Y combined := ";
    p.print(y_not_later * y_consumed_not_earlier);
    cout << endl;

    map difference(ctx, "{ [Y[a] -> Y[b]] -> [a-b] }");
    map delta = difference(y_not_later * y_consumed_not_earlier);
    cout << "delta := ";
    p.print(delta);
    cout << endl;

    set delta_range = delta.range();
    cout << "range := "; p.print(delta_range); cout << endl;

    set max_delta = delta_range.lex_maximum();
    cout << "max delta := ";
    p.print(max_delta);
    cout << endl;

    point pt = max_delta.single_point();
    cout << pt(space::variable, 0).real() << endl;

    return 0;
}
