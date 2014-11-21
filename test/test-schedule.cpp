#include "set.hpp"
#include "map.hpp"
#include <isl/schedule.h>
#include <isl/ast_build.h>
#include <cstdio>

using namespace std;

int main()
{
    isl::context ctx;
    isl::printer printer(ctx);
    ctx.set_error_action(isl::context::abort_on_error);

    isl_printer *p = isl_printer_to_file(ctx.get(), stdout);

    //isl::union_set domains(ctx, "{ A[t] : 0 <= t < 20; B[t]:0 <= t < 20 }");
    isl::union_set domains(ctx, "{ A[t]; B[t] }");
    isl::union_map deps(ctx,
                        "{ A[a] -> B[b] :"
                        " a * 2 >= b * 3 "
                        " and a * 2 < b * 3 + 5 }");
    //isl::union_map deps(ctx, "{ A[a] -> B[b] }");
    isl::union_map empty_deps(deps.get_space());

    cout << "domains:" << endl;
    printer.print(domains); cout << endl;
    cout << "dependencies:" << endl;
    printer.print(deps); cout << endl;


    isl_schedule_constraints *constr =
            isl_schedule_constraints_on_domain(domains.copy());

    constr = isl_schedule_constraints_set_validity(constr, deps.copy());

    isl_schedule * sched =
            isl_schedule_constraints_compute_schedule(constr);
    isl_union_map *sched_map =
            isl_schedule_get_map(sched);

    cout << "schedule:" << endl;
    p = isl_printer_print_schedule(p, sched); cout << endl;
    cout << "schedule map:" << endl;
    p = isl_printer_print_union_map(p, sched_map); cout << endl;

    isl::union_set period(ctx, "{ [t0,t1] : t0 >= 0 and t0 < 6 }");

    //sched_map = isl_union_map_intersect_domain(sched_map, domains.copy());
    sched_map = isl_union_map_intersect_range(sched_map, period.copy());

    cout << "bounded schedule map:" << endl;
    p = isl_printer_print_union_map(p, sched_map); cout << endl;

    isl_set *ast_ctx = isl_set_read_from_str(ctx.get(), "{:}");
    isl_ast_build *ast_build = isl_ast_build_from_context(ast_ctx);
    isl_ast_node * ast = isl_ast_build_ast_from_schedule(ast_build, sched_map);

    p = isl_printer_set_output_format(p, ISL_FORMAT_C);

    cout << "AST:" << endl;
    p = isl_printer_print_ast_node(p, ast); cout << endl;

    isl_ast_node_free(ast);
    isl_ast_build_free(ast_build);
    isl_schedule_free(sched);
    isl_printer_free(p);

    return 0;
}
