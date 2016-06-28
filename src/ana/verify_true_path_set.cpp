#include "sta/src/ana/analyzer.h"

#include <time.h>
#include <iostream>

#include "sta/src/util/converter.h"

// Timing variables.
static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;

static void reset_timing_variables() {
    start_time      = time(0);
    time_difference = 0.0;
    time_step       = 1.0;
    buffer          = "";
}

static void update_number_of_examined_path(size_t n, bool force) {
    time_difference = difftime(time(0), start_time);

    if (force || time_difference > time_step) {
        time_step = time_difference + 1.0;
        std::cerr << std::string(buffer.size(), '\b');

        buffer = Sta::Util::to_str(n);
        std::cerr << buffer;
    }
}

// Wrap verify_true_path() to output error messages.
static bool verify_true_path_wrapper(
    const Sta::Cir::Circuit&   cir,
    const Sta::Cir::Path&      path,
    size_t                     path_idx,
    const Sta::Cir::PathValue& path_value,
    const Sta::Cir::InputVec&  input_vec  ) {

    using Sta::Cir::Gate;
    using Sta::Cir::Module;

    const Gate* g1 = 0;
    const Gate* g2 = 0;
    const Gate* gY = 0;

    bool is_true_path = Sta::Ana::verify_true_path(
                            cir, path, input_vec, g1, g2, gY);

    for (size_t i = 0; i < path.size(); ++i) {
        const Gate* g = path[i];
        
        if (g->value != path_value[i]) {
            std::cerr << "Error: Gate " << g->name << "'s value "
                      << "(" << path_value[i] << ") "
                      << "in Path { " << path_idx + 1 << " } does "
                      << "not match with simulated value "
                      << "(" << (int)(g->value) << ").\n";
            return false;
        }
    }

    if (!is_true_path) {
        std::cout << "\nError: Gate '" << g1->name << "' in Path { "      
                  << path_idx + 1 << " } is not part of true path."       
                  << "\n" << gY->name
                  << "\n- module       = " << cir.modules[gY->module].name 
                  << "\n- value        = " << (int)(gY->value)             
                  << "\n- arrival_time = " << gY->arrival_time             
                  << "\n" << g1->name                       
                  << "\n- value        = " << (int)(g1->value)               
                  << "\n- arrival_time = " << g1->arrival_time               
                  << "\n" << g2->name                       
                  << "\n- value        = " << (int)(g2->value)               
                  << "\n- arrival_time = " << g2->arrival_time;

        std::cerr << "\nNumber of examined path: " << buffer;             

        return false;
    }
    
    return true;
}

bool Sta::Ana::verify_true_path_set(
    const Cir::Circuit&                cir,
    int                                time_constraint,
    int                                slack_constraint,
    const std::vector<Cir::Path>&      paths,
    const std::vector<Cir::PathValue>& values,
    const std::vector<Cir::InputVec>&  input_vecs) {


    reset_timing_variables();
    std::cerr << "Number of examined path: ";

    bool passed = true;

    for (size_t i = 0; i < paths.size(); ++i) {
        if (!verify_true_path_wrapper(cir, 
                                      paths[i], 
                                      i,
                                      values[i], 
                                      input_vecs[i])) {
            passed = false;
        }

        update_number_of_examined_path(i, false);
    }

    update_number_of_examined_path(paths.size(), true);

    return passed;
}
