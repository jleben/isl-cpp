#include "set.hpp"
#include "map.hpp"
#include <isl/schedule.h>
#include <isl/ast_build.h>
#include <cstdio>
#include <algorithm>

using namespace std;

int gcd(int a, int b)
{
    for (;;)
    {
        if (a == 0) return b;
        b %= a;
        if (b == 0) return a;
        a %= b;
    }
}

int lcm(int a, int b)
{
    int div = gcd(a, b);
    return div ? (a * b / div) : 0;
}

int main()
{
    isl::context ctx;
    isl::printer printer(ctx);
    ctx.set_error_action(isl::context::abort_on_error);

    isl_printer *p = isl_printer_to_file(ctx.get(), stdout);

#if 0
    isl::union_set domains(ctx,
                           "{ A[a0,a2] : 0 <= a2 < 10; "
                           "  B[b0,b1,b2] : 0 <= b1 < 3 and 0 <= b2 < 10; }");
    isl::union_map deps(ctx,
                        "{ A[a0,a2] -> B[b0,b1,a2] :"
                        " a0 = 2 * b0 + b1 }");
#endif

#if 1
    isl::union_set domains(ctx,
                           "{ A[a0]; "
                           "  B[b0,b1] : 0 <= b1 < 3 }");
    isl::union_map deps(ctx,
                        "{ A[a0] -> B[b0,b1] :"
                        " a0 = 2 * b0 + b1 }");
#endif

    cout << "domains:" << endl;
    printer.print(domains); cout << endl;
    cout << "dependencies:" << endl;
    printer.print(deps); cout << endl;


    isl_schedule_constraints *constr =
            isl_schedule_constraints_on_domain(domains.copy());

    constr = isl_schedule_constraints_set_validity(constr, deps.copy());
    constr = isl_schedule_constraints_set_proximity(constr, deps.copy());

    isl_schedule * sched =
            isl_schedule_constraints_compute_schedule(constr);
    isl::union_map sched_map(isl_schedule_get_map(sched));

    cout << "schedule:" << endl;
    p = isl_printer_print_schedule(p, sched); cout << endl;
    cout << "schedule map:" << endl;
    p = isl_printer_print_union_map(p, sched_map.get()); cout << endl;

    int period;
    int offset;

    {
        using namespace isl;

        vector<int> ks;

        sched_map.for_each( [&] (const isl::map & m)
        {
            string name = m.id(isl::space::input).name;
            m.for_each( [&] (const isl::basic_map & bm)
            {
                // Find first output dimension which iterates
                // the infinite input dimension.

                auto space = bm.get_space();
                int in_dims = space.dimension(space::input);
                int out_dims = space.dimension(space::output);

                auto eq = bm.equalities_matrix();
                int rows = eq.row_count();

                int first_out_dim = out_dims;
                int k = 0;

                for (int r = 0; r < rows; ++r)
                {
                    int in0_k = eq(r, 0).value().integer();
                    if (in0_k)
                    {
                        for (int out = 0; out < out_dims; ++out)
                        {
                            int out_k = eq(r, out + in_dims).value().integer();
                            if (out_k)
                            {
                                if (out < first_out_dim)
                                {
                                    first_out_dim = out;
                                    k = in0_k;
                                }
                                break;
                            }
                        }
                    }
                }

                k = std::abs(k);
                ks.push_back(k);

                cout << name << "@" << first_out_dim << " = " << k << endl;

                return true;
            });
            return true;
        });

        period = std::accumulate(ks.begin(), ks.end(), 1, lcm);
        offset = *std::max_element(ks.begin(), ks.end());

        cout << "period = " << period << endl;
        cout << "offset = " << offset << endl;
    }

#if 1
    isl::union_map period_sched (sched_map.ctx());
    {
        using namespace isl;

        auto sched_in_domain = sched_map.in_domain(domains);

        sched_in_domain.for_each( [&](map & m)
        {
           local_space cnstr_space(m.get_space());
           auto dim0 = cnstr_space(space::input, 0);
           m.add_constraint(dim0 >= offset);
           m.add_constraint(dim0 < (offset + period));
           period_sched = period_sched | m;
           return true;
        });
    }

    cout << "period schedule map:" << endl;
    p = isl_printer_print_union_map(p, period_sched.get()); cout << endl;

    isl_set *ast_ctx = isl_set_read_from_str(ctx.get(), "{:}");
    isl_ast_build *ast_build = isl_ast_build_from_context(ast_ctx);
    isl_ast_node * ast = isl_ast_build_ast_from_schedule(ast_build, period_sched.get());

    p = isl_printer_set_output_format(p, ISL_FORMAT_C);

    cout << "AST:" << endl;
    p = isl_printer_print_ast_node(p, ast); cout << endl;

    isl_ast_node_free(ast);
    isl_ast_build_free(ast_build);
    isl_schedule_free(sched);
    isl_printer_free(p);
#endif
    return 0;
}
