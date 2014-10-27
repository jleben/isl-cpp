#include "../context.hpp"
#include "../space.hpp"
#include "../set.hpp"
#include "../map.hpp"
#include "../expression.hpp"
#include "../matrix.hpp"
#include "../utility.hpp"
#include "../printer.hpp"

#include <iostream>

using namespace isl;
using namespace std;

void test_space(context & ctx, printer &p)
{
    cout << "-- Testing space products --" << endl;
    {
        map u1(ctx, "{T1[a,b] -> B1[c,d]}");
        map u2(ctx, "{T2[a,b] -> B2[c,d]}");
        space s2 = product(u1.get_space(), u2.get_space());
        map u3 = map::universe(s2);
        p.print(u3); cout << endl;
    }

    {
        set universe(ctx, "{A[a,b]}");
        space s = universe.get_space();
        space s2 = range_product(s, s);
        set u2 = set::universe(s2);
        p.print(u2); cout << endl;
    }
}

void test_set(context & ctx, printer &p)
{
    cout << "-- Testing sets --" << endl;

    {
        union_set u(ctx);
        cout << "empty union_set: "; p.print(u); cout << endl;
        set a(ctx, "{X[a,b]}");
        set b(ctx, "{Y[a,b]}");
        u = u | a | b;
        cout << "{X[a,b]} union {Y[a,b]} = "; p.print(u); cout << endl;
    }
}

void test_map(context & ctx, printer &p)
{
    cout << "-- Testing maps --" << endl;

    {
        union_map u(ctx);
        cout << "empty union_map: "; p.print(u); cout << endl;
        map a(ctx, "{X[a] -> [2]}");
        map b(ctx, "{Y[a] -> [3]}");
        u = u | a | b;
        cout << "{X[a] -> 2} union {Y[a] -> 3} = ";
        p.print(u);
        cout << endl;
    }
}

void test_expr(context & ctx, printer &p)
{
    cout << "-- Testing expression --" << endl;

    set universe(ctx, "{A[a,b]}");
    space s = universe.get_space();
    space s2 = range_product(s, s);
    auto loc_space = local_space(s2);
    auto a = expression::variable(loc_space, space::variable, 0);
    auto b = expression::variable(loc_space, space::variable, 2);
    auto c = expression::value(loc_space, value(ctx,(long)3));
    auto result = a - b + c;
    auto m = map(result);
    p.print(result); cout << endl;
    p.print(m); cout << endl;
}

void test_matrix(context & ctx, printer &p)
{
    cout << "-- Testing matrix --" << endl;

    matrix m(ctx, 3, 4);
    for (int row = 0; row < m.row_count(); ++row)
    {
        for (int col = 0; col < m.column_count(); ++col)
        {
            value v(ctx, row * 10 + col);
            m(row, col) = v;
        }
    }
    print(m);
}

void test_constraint(context & ctx, printer & p)
{
    using isl::tuple;

    cout << "-- Testing constraint --" << endl;

    space s(ctx, tuple(), tuple("",1));
    local_space ls(s);
    auto x = expression::variable(ls, space::variable, 0);

    {
        auto lhs = x + 3;
        auto constr = lhs > 2;
        auto S = set::universe(s);
        S.add_constraint(constr);
        cout << "x + 3 > 2: "; p.print(S); cout << endl;
    }
    {
        auto a = x * 2;
        auto constr = a < 10;
        auto S = set::universe(s);
        S.add_constraint(constr);
        cout << "x * 2 < 10: "; p.print(S); cout << endl;
    }
}

void test_buffer_size(context & ctx, printer &p)
{
    using isl::tuple;

    cout << "-- Testing computation of buffer size --" << endl;

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
}

int main()
{
    context ctx;
    ctx.set_error_action(context::abort_on_error);

    printer p(ctx);

    test_space(ctx, p);
    cout << endl;
    test_set(ctx,p);
    cout << endl;
    test_map(ctx,p);
    cout << endl;
    test_expr(ctx, p);
    cout << endl;
    test_constraint(ctx, p);
    cout << endl;
    test_matrix(ctx, p);
    //cout << endl;
    //test_buffer_size(ctx, p);

    return 0;
}
