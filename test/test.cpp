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
    using isl::tuple;

    cout << "-- Testing matrix --" << endl;

    {
        cout << "Some matrix:" << endl;
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

    {
        space spc(ctx, tuple(), tuple("A",2));

        matrix eq(ctx, 1, 3);
        eq(0,0) = 1;
        eq(0,1) = 2;
        eq(0,2) = -4;

        matrix ineq(ctx, 1, 3);
        ineq(0,0) = 2;
        ineq(0,1) = 3;
        ineq(0,2) = 5;

        basic_set st(spc, eq, ineq);

        cout << "Equalities:" << endl;
        print(eq);
        cout << "Inequalities:" << endl;
        print(ineq);
        cout << "Set:" << endl;
        p.print(st); cout << endl;
    }
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

void test_dataflow_counts(context & ctx, printer &p)
{
    /*
    p(a) = 2

    o(b) = 1
    e(b) = 3
    p(b) = 3

    o(c) = 2
    e(c) = 4
    */

    space spc(ctx, isl::tuple(), isl::tuple("",3));
    auto a = expression::variable(spc, space::variable, 0);
    auto b = expression::variable(spc, space::variable, 1);
    auto c = expression::variable(spc, space::variable, 2);

    // Flow counts:

    int a_col = 0;
    int b_col = 1;
    int c_col = 2;

    matrix flow(ctx, 2, 3, 0);
    flow(0,a_col) = 2;
    flow(0,b_col) = -1;
    flow(1,b_col) = 3;
    flow(1,c_col ) = -2;

    matrix steady_counts = flow.nullspace();
    cout << "Steady:" << endl;
    print(steady_counts);

    // Init counts:

    // p(a)*i(a) - o(b)*i(b) + [p(a)*s(a) - o(b)*s(b) - e(b) - o(b)] >= 0

    int s_a = steady_counts(a_col,0).value().numerator();
    int s_b = steady_counts(b_col,0).value().numerator();
    int s_c = steady_counts(c_col ,0).value().numerator();

    auto init_counts = set::universe(spc);
    init_counts.add_constraint( 2*a - 1*b + (2*s_a - 1*s_b - 3 - 1) >= 0 );
    init_counts.add_constraint( 3*b - 2*c + (3*s_b - 2*s_c - 4 - 2) >= 0 );
    init_counts.add_constraint(a >= 0);
    init_counts.add_constraint(b >= 0);
    init_counts.add_constraint(c >= 0);
#if 0
    matrix init_flow(ctx, 2, 4, 0);

    init_flow(0, a_col) = 2;
    init_flow(0, b_col) = -1;
    init_flow(0, const_col) = 2*s_a - 1*s_b - 3 - 1;

    init_flow(1, b_col) = 3;
    init_flow(1, c_col ) = -2;
    init_flow(1, const_col) = 3*s_b - 2*s_c - 4 - 2;

    set init_possibilities(basic_set(spc, matrix(ctx,0,4), init_flow));
#endif
    cout << "Init:" << endl;
    p.print(init_counts); cout << endl;

    auto cost = a + b + c;
    auto optimum = init_counts.minimum(cost);

    cout << "Optimium Value:" << endl;
    p.print(optimum); cout << endl;

    init_counts.add_constraint( cost == optimum );

    cout << "Optimum Set:" << endl;
    p.print(init_counts); cout << endl;

    point optimum_point = init_counts.single_point();
    cout << "Optimum Point:" << endl;
    p.print(optimum_point); cout << endl;
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
    cout << endl;
    test_dataflow_counts(ctx, p);
    //cout << endl;
    //test_buffer_size(ctx, p);

    return 0;
}
