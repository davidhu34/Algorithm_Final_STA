#include "sta/src/ana/analyzer.h"

#include <cassert>
#include <ctime>
#include <iostream>
#include <queue>
#include <stack>

//#include "minisat_blbd/src/core/Solver.h"
#include "minisat/src/core/Solver.h"

#ifndef NDEBUG
#include "sta/src/util/converter.h"

static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;
#endif

// Calculate maximum arrival time.
//
// #### Input
//
// - `cir`
//
static void calculate_max_arrival_time(Sta::Cir::Circuit& cir) {
    using Sta::Cir::Gate;

    // Assign 0 to all gate's _tag. It means the number of fan-in of
    // that gate which has arrived.
    //
    for (size_t i = 0; i < cir.getOutputs().size(); ++i) {
        cir.getOutputs()[i]->_tag = 0;
    }
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        cir.getLogicGates()[i]->_tag = 0;
    }

    // For all gate inside queue, all of its fan-in has arrived.
    // Every gate inside queue has already known arrival time.
    // This queue should be sorted according to arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    typedef std::map<std::string, Gate*>::iterator Iter;

    for (Iter it = cir.getInputs().begin();
         it != cir.getInputs().end();
         ++it                               ) {

        Gate* pi = it->second;
        pi->_arrivalTime = 0;
        q.push(pi);
    }

    // While queue is not empty
    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        
        // Tell gate's fan-outs that it has arrived.
        for (size_t i = 0; i < gate->getFanOut().size(); ++i) {
            Gate* fanout = gate->getFanOut()[i];
            fanout->_tag += 1;
            
            // If that fan-out's fan-ins have all arrived
            if (fanout->_tag == fanout->getFanIn().size()) {
                
                // Assign arrival time to that fan-out.
                if (fanout->getModel() == "PO") {
                    fanout->_arrivalTime = gate->_arrivalTime;

                    // No need to put PO into queue, because PO does
                    // not have fan-out.
                }
                else {
                    fanout->_arrivalTime = gate->_arrivalTime + 1;

                    // Add that fan-out to queue.
                    q.push(fanout);
                }
            }
        }
    }
}

// Calculate minimum arrival time.
//
// #### Input
//
// - `cir`
//
static void calculate_min_arrival_time(Sta::Cir::Circuit& cir) {
    using Sta::Cir::Gate;

    // Assign 0 to all gate's _tag.
    // 0: Not yet visited, not in queue.
    // 1: In queue, not visited.
    // 2: Visited, not in queue.
    //
    for (size_t i = 0; i < cir.getOutputs().size(); ++i) {
        cir.getOutputs()[i]->_tag = 0;
    }
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        cir.getLogicGates()[i]->_tag = 0;
    }

    // For all gate inside queue, at least 1 of its fan-in has arrived.
    // Every gate inside queue has already known min arrival time.
    // This queue should be sorted according to min arrival time.
    //
    std::queue<Gate*> q;
    
    // Add primary_inputs into queue.
    typedef std::map<std::string, Gate*>::iterator Iter;

    for (Iter it = cir.getInputs().begin();
         it != cir.getInputs().end();
         ++it                               ) {

        Gate* pi = it->second;
        pi->_arrivalTime = 0;
        q.push(pi);
        pi->_tag = 1;
    }

    // While queue is not empty
    while (!q.empty()) {
        Gate* gate = q.front();
        q.pop();
        
        for (size_t i = 0; i < gate->getFanOut().size(); ++i) {
            Gate* fanout = gate->getFanOut()[i];
            
            // If fanout is not visited nor in queue
            if (fanout->_tag == 0) {
                
                // Assign arrival time to that fan-out.
                if (fanout->getModel() == "PO") {
                    fanout->_arrivalTime = gate->_arrivalTime;

                    // No need to put PO into queue, because PO does
                    // not have fan-out.
                    fanout->_tag = 2;
                }
                else {
                    fanout->_arrivalTime = gate->_arrivalTime + 1;

                    // Add that fan-out to queue.
                    q.push(fanout);
                    fanout->_tag = 1;
                }

            }
        }
    }
}

// Add NAND clause into solver.
//
// #### Input
//
// - `A`
// - `B`
// - `C`
// - `solver`
//
// #### Output
//
// True if success, false otherwise.
//
static bool add_NAND_clause(Minisat::Var A, 
                            Minisat::Var B, 
                            Minisat::Var C, 
                            Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (!solver.addClause(mkLit(A), mkLit(C))) {
        return false;
    }
    if (!solver.addClause(mkLit(B), mkLit(C))) {
        return false;
    }
    if (!solver.addClause(mkLit(A, 1), mkLit(B, 1), mkLit(C, 1))) {
        return false;
    }

    return true;
}

// Add NOR clause into solver.
//
// #### Input
//
// - `A`
// - `B`
// - `C`
// - `solver`
//
// #### Output
//
// True if success, false otherwise.
//
static bool add_NOR_clause(Minisat::Var A, 
                           Minisat::Var B, 
                           Minisat::Var C, 
                           Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (!solver.addClause(mkLit(A, 1), mkLit(C, 1))) {
        return false;
    }
    if (!solver.addClause(mkLit(B, 1), mkLit(C, 1))) {
        return false;
    }
    if (!solver.addClause(mkLit(A), mkLit(B), mkLit(C))) {
        return false;
    }

    return true;
}

// Add NOR clause into solver.
//
// #### Input
//
// - `A`
// - `C`
// - `solver`
//
// #### Output
//
// True if success, false otherwise.
//
static bool add_NOT_clause(Minisat::Var A, 
                           Minisat::Var C, 
                           Minisat::Solver& solver) {
    using Minisat::mkLit;

    if (!solver.addClause(mkLit(A), mkLit(C))) {
        return false;
    }
    if (!solver.addClause(mkLit(A, 1), mkLit(C, 1))) {
        return false;
    }

    return true;
}

// Call function and create a return point. `n` is the return point
// index. It must be a literal number without any operation. Each
// return point index must be unique. Please do not put semicolon
// after calling this function.
//
#define CALL_FUNCTION(n)     \
    continue_point.push(n);  \
    goto start_function;     \
    case n:

// Perform code substitution. Please do not put semicolon after
// calling these functions.
//
// CAUTION: SURROUND THEM WITH BRACES WHEN USE WITH BRANCH OR LOOP.
//
#define ASSIGN(gate, v)                    \
    gate->_value = v;                      \
    assumptions.push(mkLit(gate->_var, !v));

#define UNASSIGN(gate)                     \
    assumptions.pop();                     \
    gate->_value = 2;

#define PUSH_GATE(gate)                    \
    path.push_back(gate);                  \
    slack -= 1;

#define POP_GATE_1()                       \
    slack += 1;                            \
    path.pop_back();                       \
    gate = path.back();                    

#define POP_GATE_2()                       \
    slack += 1;                            \
    path.pop_back();                       \
    gate = path.back();                    \
    gA = gate->getFanIn()[0];              \
    gB = gate->getFanIn()[1];                   

// Basically the idea is trace from output pins toward input pins. Try
// every possibility (condition) that make a path become a true path.
// Check whether our assumption has any contradiction.
//
// #### Input
//
// - `po`
// - `cir`
// - `solver`
//
// #### Output
//
// Add true path to `paths` and an input vector that can make
// that path sensitized.
//
static void trace(Sta::Cir::Gate*                   po,
                  Sta::Cir::Circuit&                cir,
                  int                               time_constraint,
                  int                               slack_constraint,
                  Minisat::Solver&                  solver,
                  std::vector<Sta::Cir::Path>&      paths, 
                  std::vector< std::vector<bool> >& values,
                  std::vector<Sta::Cir::InputVec>&  input_vecs) {

    using Sta::Cir::Gate;
    using Sta::Cir::Path;
    using Sta::Cir::InputVec;
    using Minisat::mkLit;
    using Minisat::toInt;

    // Try to mimic recursive function call without using function.
    // Use continue_point and switch statement to make sure function
    // return to correct point.
    std::stack<int> continue_point;

    // Static variable of that fake function.
    Path                       path(1, po);
    Minisat::vec<Minisat::Lit> assumptions;

    // Temporary/helper variables.
    Gate* gate;
    Gate* gA;
    Gate* gB;
    int   point;
    int   slack = time_constraint;

    // Call function
    continue_point.push(0);
    goto start_function;

pop_function:
    point = continue_point.top(); 
    continue_point.pop();

    switch (point) {

start_function:
    gate = path.back();

    if (gate->_arrivalTime > slack) {
        goto pop_function;
    }

    if (slack == 0 && gate->getModel() != "PI") {
        goto pop_function;
    }

    if (gate->_value == 2) { // Floating
        assert(gate->getModel() == "PO");

        gate->_value = 0;
        ASSIGN(gate->getFanIn()[0], 0)
        PUSH_GATE(gate->getFanIn()[0])
        CALL_FUNCTION(1)
        POP_GATE_1()
        UNASSIGN(gate->getFanIn()[0])
        gate->_value = 2;

        gate->_value = 1;
        ASSIGN(gate->getFanIn()[0], 1)
        PUSH_GATE(gate->getFanIn()[0])
        CALL_FUNCTION(2)
        POP_GATE_1()
        UNASSIGN(gate->getFanIn()[0])
        gate->_value = 2;
    }

    else if (gate->getModel() == "NAND2") {
        gA = gate->getFanIn()[0];
        gB = gate->getFanIn()[1];

    #define MAKE_TRUE_PATH(gA, gB, v0, v1, p1, p2, p3, p4)  \
        if (gate->_value == v1) {                           \
            if (gA->_value == 2) {                          \
                ASSIGN(gA, v0)                              \
                PUSH_GATE(gA)                               \
                CALL_FUNCTION(p1)                           \
                POP_GATE_2()                                \
                UNASSIGN(gA)                                \
            }                                               \
        }                                                   \
                                                            \
        else { /* gate->_value == 0 */                      \
            if (gA->_value == 2 && gB->_value == 2) {       \
                ASSIGN(gA, v1)                              \
                ASSIGN(gB, v1)                              \
                PUSH_GATE(gA)                               \
                CALL_FUNCTION(p2)                           \
                POP_GATE_2()                                \
                UNASSIGN(gB)                                \
                UNASSIGN(gA)                                \
            }                                               \
        }                                                   

        // Try to make gA become a true path.
        
        MAKE_TRUE_PATH(gA, gB, 0, 1, 3, 4, 5, 6)

        // Try to make gB become a true path.

        // Same logic as above, just swap "gA" and "gB"
        MAKE_TRUE_PATH(gB, gA, 0, 1, 7, 8, 9, 10)

    } // else if (gate->getModel() == "NAND2")

    else if (gate->getModel() == "NOR2") {
        gA = gate->getFanIn()[0];
        gB = gate->getFanIn()[1];

        // Try to make gA become a true path.
        
        MAKE_TRUE_PATH(gA, gB, 1, 0, 11, 12, 13, 14)

        // Try to make gB become a true path

        // Same logic as above, just swap "gA" and "gB"
        MAKE_TRUE_PATH(gB, gA, 1, 0, 15, 16, 17, 18)
        
    } // else if (gate->getModel() == "NOR2")

    else if (gate->getModel() == "NOT1") {
        if (gate->getFanIn()[0]->_value == 2) {
            if (gate->_value == 1) {
                ASSIGN(gate->getFanIn()[0], 0)
            }
            else { // gate->_value == 0
                ASSIGN(gate->getFanIn()[0], 1)
            }

            PUSH_GATE(gate->getFanIn()[0])
            CALL_FUNCTION(19)
            POP_GATE_1()
            UNASSIGN(gate->getFanIn()[0])
        }
    } // else if (gate->getModel() == "NOT1")

    else if (gate->getModel() == "PI") {
        if (slack + 1 < slack_constraint) {
            if (solver.solve(assumptions)) {
                paths.push_back(path);

                // Record _value of all gate along a path.
                std::vector<bool> path_value(path.size());
                for (size_t i = 0; i < path.size(); ++i) {
                    path_value[i] = path[i]->_value;
                }
                values.push_back(path_value);

                // Record input vector.
                typedef std::map<std::string, Gate*>::iterator Iter;

                InputVec input_vec;
                input_vec.reserve(cir.getInputs().size());
                for (Iter it = cir.getInputs().begin();
                     it != cir.getInputs().end();
                     ++it                               ) {

                    input_vec.push_back( 
                        toInt(solver.model[it->second->_var]) ^ 1);
                }
                input_vecs.push_back(input_vec);

                #ifndef NDEBUG
                time_difference = difftime(time(0), start_time);

                if (time_difference > time_step) {
                    time_step = time_difference + 1.0;
                    std::cerr << std::string(buffer.size(), '\b');

                    buffer = Sta::Util::to_str(input_vecs.size());
                    std::cerr << buffer;
                }
                #endif
            }
        }
    }

    else {
        assert(false && "Error: Unknown gate type.\n");
    }

    goto pop_function;

    case 0: 
        ; // Function returned to root caller.

    } // switch (point)

}

int Sta::Ana::find_true_paths(
    Cir::Circuit&                     cir,
    int                               time_constraint,
    int                               slack_constraint,
    std::vector<Cir::Path>&           paths,
    std::vector< std::vector<bool> >& values,
    std::vector<Cir::InputVec>&       input_vecs) {

    #ifndef NDEBUG
    std::cerr << "Number of true path found: ";

    start_time      = time(0);
    time_difference = 0.0;
    time_step       = 1.0;
    buffer          = "";
    #endif

    // Calculate minimum arrival time.
    calculate_min_arrival_time(cir);

    // Initialize gate _value to undefined state.
    typedef std::map<std::string, Cir::Gate*>::iterator Iter;

    for (Iter it = cir.getInputs().begin();
         it != cir.getInputs().end();
         ++it                               ) {

        it->second->_value = 2;
    }
    for (size_t i = 0; i < cir.getOutputs().size(); ++i) {
        cir.getOutputs()[i]->_value = 2;
    }
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        cir.getLogicGates()[i]->_value = 2;
    }

    Minisat::Solver solver;

    // Assign SAT variable to gate.
    for (Iter it = cir.getInputs().begin();
         it != cir.getInputs().end();
         ++it                               ) {

        it->second->_var = solver.newVar();
    }
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        cir.getLogicGates()[i]->_var = solver.newVar();
    }

    // Add clauses into solver.
    for (size_t i = 0; i < cir.getLogicGates().size(); ++i) {
        Cir::Gate* gate = cir.getLogicGates()[i];
        bool       success;

        if (gate->getModel() == "NAND2") {
            success = add_NAND_clause(gate->getFanIn()[0]->_var,
                                      gate->getFanIn()[1]->_var,
                                      gate->_var,
                                      solver);
        }
        else if (gate->getModel() == "NOR2") {
            success = add_NOR_clause(gate->getFanIn()[0]->_var,
                                     gate->getFanIn()[1]->_var,
                                     gate->_var,
                                     solver);
        }
        else if (gate->getModel() == "NOT1") {
            success = add_NOT_clause(gate->getFanIn()[0]->_var,
                                     gate->_var,
                                     solver);
        }
        else {
            assert(false && "gate model not recognized.");
        }

        if (!success) {
            std::cerr << "Error: Clause conflicts while adding clause.\n";
            return 1;
        }
    }

    // Simplify
    if (!solver.simplify()) {
        std::cerr << "Error: Clause conflicts while simplifying.\n";
        return 1;
    }

    // Find true path.
    for (size_t i = 0; i < cir.getOutputs().size(); ++i) {
        Cir::Gate* po = cir.getOutputs()[i];
        trace(po, cir, time_constraint, slack_constraint,
              solver, paths, values, input_vecs);
    }

    #ifndef NDEBUG
    std::cerr << std::string(buffer.size(), '\b');
    std::cerr << input_vecs.size() << "\n";
    #endif

    return 0;
}
