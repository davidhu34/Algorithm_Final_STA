#include <cassert>
#include <iostream>
#include <vector>

#include "minisat/src/core/Solver.h"

using namespace Minisat;

static bool add_AND_clause(Var A, Var B, Var C, Solver& solver) {
    vec<Lit> clause;
    clause.push(mkLit(A));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(B));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(A, true));
    clause.push(mkLit(B, true));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    return true;
}

static bool add_OR_clause(Var A, Var B, Var C, Solver& solver) {
    vec<Lit> clause;
    clause.push(mkLit(A, true));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(B, true));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(A));
    clause.push(mkLit(B));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    return true;
}

static bool add_NOT_clause(Var A, Var C, Solver& solver) {
    vec<Lit> clause;
    clause.push(mkLit(A));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(A, true));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    return true;
}

static bool add_NAND_clause(Var A, Var B, Var C, Solver& solver) {
    vec<Lit> clause;
    clause.push(mkLit(A));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(B));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(A, true));
    clause.push(mkLit(B, true));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    return true;
}

static bool add_NOR_clause(Var A, Var B, Var C, Solver& solver) {
    vec<Lit> clause;
    clause.push(mkLit(A, true));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(B, true));
    clause.push(mkLit(C, true));
    if (!solver.addClause(clause)) {
        return false;
    }

    clause.clear();
    clause.push(mkLit(A));
    clause.push(mkLit(B));
    clause.push(mkLit(C));
    if (!solver.addClause(clause)) {
        return false;
    }

    return true;
}

void test_minisat(void) {
    std::cerr << __FUNCTION__ << "():\n";

    Solver solver;
    Var    A = solver.newVar(), 
           B = solver.newVar(), 
           C = solver.newVar(), 
           D = solver.newVar(), 
           E = solver.newVar(), 
           F = solver.newVar(), 
           G = solver.newVar(), 
           H = solver.newVar(), 
           I = solver.newVar(),
           J = solver.newVar();

    if (!add_OR_clause  (B, C, F, solver) ||
        !add_NAND_clause(D, E, G, solver) ||
        !add_AND_clause (A, F, H, solver) ||
        !add_AND_clause (H, G, I, solver)   ) {

        std::cout << "Clause conflicts while adding clause.\n";
        return;
    }
    
    if (!solver.simplify()) {
        std::cout << "Clause conflicts while simplifying.\n";
        return;
    }

    vec<Lit> assumptions;
    assumptions.push(mkLit(I));

    if (solver.solve(assumptions)) {
        std::cout << "Satisfiable. Model is:\n"
                  << "A = " << toInt(solver.model[A]) << "\n"
                  << "B = " << toInt(solver.model[B]) << "\n"
                  << "C = " << toInt(solver.model[C]) << "\n"
                  << "D = " << toInt(solver.model[D]) << "\n"
                  << "E = " << toInt(solver.model[E]) << "\n"
                  << "J = " << toInt(solver.model[J]) << "\n";
    }
    else {
        std::cout << "Unsatisfiable.\n";
    }
        
    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_minisat_2(void) {
    std::cerr << __FUNCTION__ << "():\n";

    Solver solver;
    Var    A = solver.newVar(), 
           B = solver.newVar();

    assert(solver.addClause(mkLit(A), mkLit(B)));
    assert(solver.addClause(mkLit(A)));

    std::cout << "Number of clause = " << solver.nClauses() << "\n";

    assert(solver.simplify());

    std::cout << "Number of clause = " << solver.nClauses() << "\n";

    std::cerr << __FUNCTION__ << "() passed.\n";
}
