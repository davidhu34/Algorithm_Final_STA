#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "test/src/util/util.h"
#include "cir/parser.inc"

void test_get_token(void) {
    std::cerr << __FUNCTION__ << "():\n";

    std::istringstream sin(
        "haha.hoho, :;(what,who$1,sum_m&er\n)[/]=`*//hello\n*>//");

    std::string ans1 = 
        "haha . hoho , : ; ( what , who$1 , sum_m er ) [ / ] = ` * *> ";

    std::string ans2 =
        "haha . hoho , : ; ( what , who$1 , sum_m er \n ) [ / ] = ` * *> ";

    std::string out1, out2;

    std::string token;
    while ((token = get_token(sin)) != "") {
        out1 += token;
        out1 += " ";
    }

    ASSERT(out1 == ans1,
        << "\nout1: \"" << TestUtil::escape(out1) << "\""
        << "\nans1: \"" << TestUtil::escape(ans1) << "\"");

    sin.clear();
    sin.seekg(0);

    while ((token = get_token(sin, 1)) != "") {
        out2 += token;
        out2 += " ";
    }

    ASSERT(out2 == ans2,
        << "\nout2 = \"" << TestUtil::escape(out2) << "\""
        << "\nans2 = \"" << TestUtil::escape(ans2) << "\"");

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_parse_module_NAND2(void) {
    std::cerr << __FUNCTION__ << "():\n";

    std::istringstream sin(
        "NAND2 (Y, A, B);\n"
        "output Y;\n"
        "input A, B;\n"
        "\n"
        "  nand (Y, A, B);\n"
        "\n"
        "  specify\n"
        "    \n"
        "    specparam\n"
        "      tplh$A$Y = 1,\n"
        "      tphl$A$Y = 1,\n"
        "      tplh$B$Y = 1,\n"
        "      tphl$B$Y = 1;\n"
        "\n"
        "    \n"
        "    (A *> Y) = (tplh$A$Y, tphl$A$Y);\n"
        "    (B *> Y) = (tplh$B$Y, tphl$B$Y);\n"
        "  endspecify\n"
        "\n"
        "endmodule \n"
        "`endcelldefine\n");

    Cir::Circuit circuit;

    ASSERT(parse_module_NAND2(sin, circuit) == 0, );

    ASSERT(circuit.module_NAND2.type == Cir::Module::NAND2,
        << "circuit.module_NAND2.type = " << circuit.module_NAND2.type);
           
    ASSERT(circuit.module_NAND2.input_names.size() == 2,
        << "circuit.module_NAND2.input_names.size() = "
        << circuit.module_NAND2.input_names.size());

    ASSERT(circuit.module_NAND2.input_names[0] == "A",
        << "circuit.module_NAND2.input_names[0] = "
        << circuit.module_NAND2.input_names[0]);

    ASSERT(circuit.module_NAND2.input_names[1] == "B",
        << "circuit.module_NAND2.input_names[1] = "
        << circuit.module_NAND2.input_names[1]);

    ASSERT(circuit.module_NAND2.output_name == "Y",
        << "circuit.module_NAND2.output_name = "
        << circuit.module_NAND2.output_name);

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_parse_module_NOR2(void) {
    std::cerr << __FUNCTION__ << "():\n";

    std::istringstream sin(
        "NOR2 (Y, A, B);\n"
        "output Y;\n"
        "input A, B;\n"
        "\n"
        "  nor (Y, A, B);\n"
        "\n"
        "  specify\n"
        "    \n"
        "    specparam\n"
        "      tplh$A$Y = 1,\n"
        "      tphl$A$Y = 1,\n"
        "      tplh$B$Y = 1,\n"
        "      tphl$B$Y = 1;\n"
        "\n"
        "    \n"
        "    (A *> Y) = (tplh$A$Y, tphl$A$Y);\n"
        "    (B *> Y) = (tplh$B$Y, tphl$B$Y);\n"
        "  endspecify\n"
        "\n"
        "endmodule \n"
        "`endcelldefine\n");

    Cir::Circuit circuit;

    ASSERT(parse_module_NOR2(sin, circuit) == 0, );

    ASSERT(circuit.module_NOR2.type == Cir::Module::NOR2,
        << "circuit.module_NOR2.type = " << circuit.module_NOR2.type);
           
    ASSERT(circuit.module_NOR2.input_names.size() == 2,
        << "circuit.module_NOR2.input_names.size() = "
        << circuit.module_NOR2.input_names.size());

    ASSERT(circuit.module_NOR2.input_names[0] == "A",
        << "circuit.module_NOR2.input_names[0] = "
        << circuit.module_NOR2.input_names[0]);

    ASSERT(circuit.module_NOR2.input_names[1] == "B",
        << "circuit.module_NOR2.input_names[1] = "
        << circuit.module_NOR2.input_names[1]);

    ASSERT(circuit.module_NOR2.output_name == "Y",
        << "circuit.module_NOR2.output_name = "
        << circuit.module_NOR2.output_name);

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_parse_module_NOT1(void) {
    std::cerr << __FUNCTION__ << "():\n";

    std::istringstream sin(
        "module NOT1 (Y, A);\n"
        "output Y;\n"
        "input A;\n"
        "\n"
        "  not I0(Y, A);\n"
        "  specify\n"
        "    \n"
        "    specparam\n"
        "      tplh$A$Y = 1,\n"
        "      tphl$A$Y = 1;\n"
        "\n"
        "    \n"
        "    (A *> Y) = (tplh$A$Y, tphl$A$Y);\n"
        "  endspecify\n"
        "\n"
        "endmodule \n"
        "`endcelldefine\n");

    Cir::Circuit circuit;

    ASSERT(parse_module_NOT1(sin, circuit) == 0, );

    ASSERT(circuit.module_NOT1.type == Cir::Module::NOT1,
        << "circuit.module_NOT1.type = " << circuit.module_NOT1.type);
           
    ASSERT(circuit.module_NOT1.input_names.size() == 1,
        << "circuit.module_NOT1.input_names.size() = "
        << circuit.module_NOT1.input_names.size());

    ASSERT(circuit.module_NOT1.input_names[0] == "A",
        << "circuit.module_NOT1.input_names[0] = "
        << circuit.module_NOT1.input_names[0]);

    ASSERT(circuit.module_NOT1.output_name == "Y",
        << "circuit.module_NOT1.output_name = "
        << circuit.module_NOT1.output_name);

    std::cerr << __FUNCTION__ << "() passed.\n";
}

