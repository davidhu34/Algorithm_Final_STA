#include "sta/src/ana/analyzer.h"

#include <ctime>
#include <iostream>

#ifndef NDEBUG
#include "sta/src/util/converter.h"

static time_t      start_time;
static double      time_difference;
static double      time_step;
static std::string buffer;
#endif

// Wrap verify_true_path() to output error messages.
//
static int verify_true_path_wrapper(
    Sta::Cir::Circuit&         cir,
    const Sta::Cir::Path&      path,
    size_t                     path_idx,
    const Sta::Cir::PathValue& path_value,
    const Sta::Cir::InputVec&  input_vec  ) {

    using Sta::Cir::Gate;
    using Sta::Cir::Module;

    const Gate* gA = 0;
    const Gate* gB = 0;
    const Gate* gY = 0;

    int return_code = Sta::Ana::verify_true_path(
                          cir, path, input_vec, gA, gB, gY);

    if (return_code != 0) {
        std::cout << "\nError: Gate '" << gA->name << "' in Path { "      
                  << path_idx + 1 << " } is not part of true path."       
                  << gY->name
                  << "\n- module       = " << cir.modules[gY->module].name 
                  << "\n- value        = " << (int)(gY->value)             
                  << "\n- arrival_time = " << gY->arrival_time             
                  << gA->name                       
                  << "\n- value        = " << (int)(gA->value)               
                  << "\n- arrival_time = " << gA->arrival_time               
                  << gB->name                       
                  << "\n- value        = " << (int)(gB->value)               
                  << "\n- arrival_time = " << gB->arrival_time;

        std::cerr << "\nNumber of examined path: " << buffer;             

        return return_code;
    }
    
    for (size_t i = 0; i < path.size(); ++i) {
        const Gate* g = path[i];
        
        if (g->value != path_value[i]) {
            std::cerr << "Error: Gate " << g->name << "'s value "
                      << "does not match with simulated value.\n";
            return 1;
        }
    }

    return 0;
}

int Sta::Ana::verify_true_path_set(
    Cir::Circuit&                      cir,
    int                                time_constraint,
    int                                slack_constraint,
    const std::vector<Cir::Path>&      paths,
    const std::vector<Cir::PathValue>& values,
    const std::vector<Cir::InputVec>&  input_vecs) {

    #ifndef NDEBUG
    std::cerr << "Number of examined path: ";

    start_time       = 0;
    time_difference  = 0.0;
    time_step        = 1.0;
    buffer           = "";
    #endif

    int return_code = 0;

    for (size_t i = 0; i < paths.size(); ++i) {
        return_code |= verify_true_path_wrapper(
                           cir, 
                           paths[i], 
                           i,
                           values[i], 
                           input_vecs[i]);

        #ifndef NDEBUG
        time_difference = difftime(time(0), start_time);

        if (time_difference > time_step) {
            time_step = time_difference + 1.0;
            std::cerr << std::string(buffer.size(), '\b');

            buffer = Sta::Util::to_str(i);
            std::cerr << buffer;
        }
        #endif
    }

    #ifndef NDEBUG
    std::cerr << std::string(buffer.size(), '\b');
    std::cerr << paths.size() << "\n";
    #endif

    return return_code;
}
