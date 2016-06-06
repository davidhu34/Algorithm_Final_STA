#include "sta/src/cir/parser.h"

#include <cctype>   // isalnum
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

#include "sta/src/util/hash_map.h"
#include "sta/src/util/prime.h"
#include "sta/src/util/hasher.h"
#include "sta/src/util/converter.h"

namespace {

struct Reader {
    // Data member
    bool          unget_once;
    int           line_no;
    std::istream& in;
    std::string   previous_token;
    std::string   token;

    // Constructor
    explicit Reader(std::istream& _in): 
        unget_once(false),
        line_no   (1),  
        in        (_in) { }

    // Get a token from stream. A token is defined to be:
    //
    // - Consecutive of alphabet or number or underscore.
    // - { } ( ) - / ---------- =
    // 
    // For horizontal line longer or equal to 2 dashes, it will return 2 
    // dashes. Other characters are separator, will be ignored.
    //
    // #### Output
    //
    // Return a token. If any error occur or stream reach the end, return
    // empty string. That token will be stored inside Reader too.
    //
    const std::string& get_token(void) {
        if (unget_once) {
            unget_once = false;
            return token;
        }

        // Make sure you can always get the last token even after stream
        // is ended.
        if (!token.empty()) {
            previous_token = token;
        }
        token.clear();

    reread:
        char c;
        if (in.get(c).good()) {
            switch (c) {
            case '{':
            case '}':
            case '(':
            case ')':
            case '/':
            case '=':
                token.push_back(c);
                break;
            
            case '-':
                token.push_back(c);

                if (in.get(c).good()) {
                    if (c == '-') { // Horizontal line
                        token.push_back(c);

                        while (in.get(c).good() && c == '-') { }

                        if (in.good()) {
                            in.unget();
                        }
                    }
                    else {
                        in.unget();
                    }
                }
                break;

            case ' ':
            case '\t':
            case '\r':
            case '\v':
                goto reread;

            case '\n':
                line_no += 1;
                goto reread;

            default:
                if (isalnum(c) || c == '_') {
                    token.push_back(c);

                   // Read consecutive characters.
                   while (in.get(c).good()) {
                       if (isalnum(c) || c == '_') {
                           token.push_back(c);
                       }
                       else {
                           in.unget();
                           break;
                       }
                   }
                }
                else {
                    goto reread;
                }
                break;
            } // switch (c)
        } // if (in.get(c).good())
        
        return token;
    } // get_token()

    // You can unget token for once only, because reader only store
    // one previous token.
    //
    void unget_token(void) {
        assert(!unget_once);
        unget_once = true;
    }
};

} // namespace

#define ASSERT(condition, errmsg)                \
    if (!(condition)) {                          \
        std::cerr << "Error: " errmsg << "\n";   \
        return 1;                                \
    }

#define EXPECT(str)                                                 \
    reader.get_token();                                             \
    ASSERT(reader.token == str,                                     \
        << "At line " << reader.line_no << ": Expecting '" << str   \
        << "' after '" << reader.previous_token << "' but '"        \
        << reader.token << "' was found instead.")


int Sta::Cir::parse_true_path_set(
    const std::string&                true_path_set_file,
    const Circuit&                    cir,
    int                               time_constraint,
    int                               slack_constraint,
    std::vector<Path>&                paths,
    std::vector< std::vector<bool> >& values,
    std::vector<InputVec>&            input_vecs) {

    // Read file.
    std::ifstream fin(true_path_set_file.c_str());
    if (!fin.good()) {
        std::cerr << "Error: Cannot open file '" << true_path_set_file
                  << "'.\n";
        return 1;
    }

    // Map gate name to gate pointer.
    typedef uint32_t (*Hasher)(const std::string&);
    typedef Util::HashMap<std::string, Gate*, Hasher> GateMap;

    GateMap gate_map(Util::hash_str);

    gate_map.bucket[0].reserve(
        cir.primary_inputs.size()  +
        cir.primary_outputs.size() +
        cir.logic_gates.size());

    for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
        Gate* pi = cir.primary_inputs[i];
        gate_map.bucket[0].push_back((GateMap::Pair(pi->name, pi)));
        gate_map.size += 1;
    }
    for (size_t i = 0; i < cir.primary_outputs.size(); ++i) {
        Gate* po = cir.primary_outputs[i];
        gate_map.bucket[0].push_back((GateMap::Pair(po->name, po)));
        gate_map.size += 1;
    }
    for (size_t i = 0; i < cir.logic_gates.size(); ++i) {
        Gate* g = cir.logic_gates[i];
        gate_map.bucket[0].push_back((GateMap::Pair(g->name, g)));
        gate_map.size += 1;
    }
    gate_map.rehash(Util::prime_gt(gate_map.size));

    // Start reading.
    Reader reader(fin);

    EXPECT("Header")
    EXPECT("{")
    EXPECT("A")
    EXPECT("True")
    EXPECT("Path")
    EXPECT("Set")
    EXPECT("}")
    EXPECT("Benchmark")
    EXPECT("{")
    EXPECT(cir.name)
    EXPECT("}")

    // Read Path
    while (reader.get_token() != "") {
        ASSERT(reader.token == "Path", 
            << "At line " << reader.line_no << ": Expecting 'Path'"
            << " after '" << reader.previous_token << "' but '"
            << reader.token << "' was found instead.")

        EXPECT("{")
        std::string temp = Util::to_str(paths.size() + 1);
        EXPECT(temp)
        EXPECT("}")

        // Read A True Path List
        EXPECT("A")
        EXPECT("True")
        EXPECT("Path")
        EXPECT("List")
        EXPECT("{")
        EXPECT("--")
        EXPECT("Pin")
        EXPECT("Type")
        EXPECT("Incr")
        EXPECT("Path")
        EXPECT("Delay")
        EXPECT("--")

        Path              path;
        std::vector<bool> path_value;
        Gate*             gate = 0;

        while (reader.get_token() != "" && reader.token != "--") {
            // Read two rows.

            // First row

            // Read Gate
            ASSERT(gate_map.contains(reader.token),
                << "At line " << reader.line_no << ": "
                << "Circuit does not contain gate '" << reader.token
                << "'.")

            gate = gate_map[reader.token];
            path.push_back(gate); 

            // Read Input Pin
            reader.get_token();
            if (reader.token == "/") {
                reader.get_token();
            }
            else {
                reader.unget_token();
            }

            // Read Type
            EXPECT("(")
            std::string temp;
            if (gate->module == Module::PI) {
                temp = "in";
            }
            else if (gate->module == Module::PO) {
                temp = "out";
            }
            else {
                temp = cir.modules[gate->module].name;
            }
            EXPECT(temp)
            EXPECT(")")

            // Incr
            if (gate->module == Module::PI) {
                EXPECT("0")
            }
            else {
                EXPECT("1")
            }

            // Path Delay
            temp = Util::to_str(path.size() - 1);
            EXPECT(temp)
            
            // Rising or Falling
            reader.get_token(); 
            ASSERT(reader.token == "r" || reader.token == "f",
                << "At line " << reader.line_no << ": Expecting 'f' or "
                << "'r' after '<path_delay>' but '"
                << reader.token << "' was found instead.")

            if (reader.token == "r") {
                path_value.push_back(true);
            }
            else { // reader.token == "f"
                path_value.push_back(false);
            }

            // Second row
            
            // Read Gate
            reader.get_token();
            ASSERT(gate_map.contains(reader.token),
                << "At line " << reader.line_no << ": "
                << "Circuit does not contain gate '" << reader.token
                << "'.")

            gate = gate_map[reader.token];

            // Read Input Pin
            reader.get_token();
            if (reader.token == "/") {
                reader.get_token();

                // Check whether the connected pin is correct.
                size_t idx = cir.modules[gate->module]
                                .find_input_name(reader.token);

                ASSERT(idx != Module::npos,
                    << "At line " << reader.line_no << ": "
                    << "Gate '" << gate->name << "' does not have "
                    << "input pin named '" << reader.token << "'.")

                ASSERT(gate->froms[idx]->name == path.back()->name,
                    << "At line " << reader.line_no << ": "
                    << "Gate '" << gate->name << "' does not connect "
                    << "to gate '" << path.back()->name << "' through "
                    << "pin '" << reader.token << "'.")
            }
            else { // It is PO
                path.push_back(gate);
                path_value.push_back(path_value.back());
                reader.unget_token();
            }

            // Read Type
            EXPECT("(")
            if (gate->module == Module::PI) {
                temp = "in";
            }
            else if (gate->module == Module::PO) {
                temp = "out";
            }
            else {
                temp = cir.modules[gate->module].name;
            }
            EXPECT(temp)
            EXPECT(")")

            // Incr
            EXPECT("0")

            // Path Delay
            if (gate->module == Module::PO) {
                temp = Util::to_str(path.size() - 2);
            }
            else {
                temp = Util::to_str(path.size() - 1);
            }
            EXPECT(temp)
            
            // Rising or Falling
            if (path_value.back()) {
                EXPECT("r")
            }
            else {
                EXPECT("f")
            }
        }

        ASSERT(path.front()->module == Module::PI,
            << "True Path before line " << reader.line_no << ": "
            << "Gate '" << path.front()->name << "' should be a PI.")

        ASSERT(path.back()->module == Module::PO,
            << "True Path before line " << reader.line_no << ": "
            << "Gate '" << path.back()->name << "' should be a PO.")

        paths.push_back(path);
        values.push_back(path_value);
        
        EXPECT("Data")
        EXPECT("Required")
        EXPECT("Time")
        temp = Util::to_str(time_constraint);
        EXPECT(temp)
        EXPECT("Data")
        EXPECT("Arrival")
        EXPECT("Time")
        temp = Util::to_str(path.size() - 2);
        EXPECT(temp)
        EXPECT("--")
        EXPECT("Slack")

        int slack = time_constraint - (path.size() - 2);
        ASSERT(slack < slack_constraint,
            << "At line " << reader.line_no << ": "
            << "Slack (" << slack << ") > slack constraint ("
            << slack_constraint << ").")

        temp = Util::to_str(slack);
        EXPECT(temp)
        EXPECT("}")

        // Read Input Vector
        EXPECT("Input")
        EXPECT("Vector")
        EXPECT("{")

        InputVec input_vec;

        while (reader.get_token() != "" && reader.token != "}") {

            // Read PI
            ASSERT(gate_map.contains(reader.token),
                << "At line " << reader.line_no << ": "
                << "Circuit does not contain gate '" << reader.token
                << "'.")

            gate = gate_map[reader.token];

            ASSERT(gate->module == Module::PI,
                << "At line " << reader.line_no << ": "
                << "Gate '" << reader.token << "' is not a primary "
                << "input.")

            // Read =
            EXPECT("=")

            // Read value
            reader.get_token();

            if (reader.token == "0" || reader.token == "f") {
                gate->value = 0;
            }
            else if (reader.token == "1" || reader.token == "r") {
                gate->value = 1;
            }
            else {
                ASSERT(false,
                    << "At line " << reader.line_no << ": "
                    << "Expecting '0' or '1' or 'f' or 'r' after '=' "
                    << "but get '" << reader.token << "' instead.")
            }
        }

        for (size_t i = 0; i < cir.primary_inputs.size(); ++i) {
            input_vec.push_back(cir.primary_inputs[i]->value);
        }
        input_vecs.push_back(input_vec);
    }

    return 0;
}
