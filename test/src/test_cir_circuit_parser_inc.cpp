#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>

#include "sta/test/src/util/util.h"
#include "sta/src/cir/circuit_parser.inc"

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

    using namespace Sta;

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

    Cir::Circuit cir;

    ASSERT(parse_module_NAND2(sin, cir), );

    ASSERT(cir.modules[Cir::Module::NAND2].name == "NAND2",
        << "cir.modules[Cir::Module::NAND2].name = "
        << cir.modules[Cir::Module::NAND2].name);
           
    ASSERT(cir.modules[Cir::Module::NAND2].input_names.size() == 2,
        << "cir.modules[Cir::Module::NAND2].input_names.size() = "
        << cir.modules[Cir::Module::NAND2].input_names.size());

    ASSERT(cir.modules[Cir::Module::NAND2].input_names[0] == "A",
        << "cir.modules[Cir::Module::NAND2].input_names[0] = "
        << cir.modules[Cir::Module::NAND2].input_names[0]);

    ASSERT(cir.modules[Cir::Module::NAND2].input_names[1] == "B",
        << "cir.modules[Cir::Module::NAND2].input_names[1] = "
        << cir.modules[Cir::Module::NAND2].input_names[1]);

    ASSERT(cir.modules[Cir::Module::NAND2].output_name == "Y",
        << "cir.modules[Cir::Module::NAND2].output_name = "
        << cir.modules[Cir::Module::NAND2].output_name);

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_parse_module_NOR2(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta;

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

    Cir::Circuit cir;

    ASSERT(parse_module_NOR2(sin, cir), );

    ASSERT(cir.modules[Cir::Module::NOR2].name == "NOR2",
        << "cir.modules[Cir::Module::NOR2].name = "
        << cir.modules[Cir::Module::NOR2].name);
           
    ASSERT(cir.modules[Cir::Module::NOR2].input_names.size() == 2,
        << "cir.modules[Cir::Module::NOR2].input_names.size() = "
        << cir.modules[Cir::Module::NOR2].input_names.size());

    ASSERT(cir.modules[Cir::Module::NOR2].input_names[0] == "A",
        << "cir.modules[Cir::Module::NOR2].input_names[0] = "
        << cir.modules[Cir::Module::NOR2].input_names[0]);

    ASSERT(cir.modules[Cir::Module::NOR2].input_names[1] == "B",
        << "cir.modules[Cir::Module::NOR2].input_names[1] = "
        << cir.modules[Cir::Module::NOR2].input_names[1]);

    ASSERT(cir.modules[Cir::Module::NOR2].output_name == "Y",
        << "cir.modules[Cir::Module::NOR2].output_name = "
        << cir.modules[Cir::Module::NOR2].output_name);

    std::cerr << __FUNCTION__ << "() passed.\n";
}

void test_parse_module_NOT1(void) {
    std::cerr << __FUNCTION__ << "():\n";

    using namespace Sta;

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

    Cir::Circuit cir;

    ASSERT(parse_module_NOT1(sin, cir), );

    ASSERT(cir.modules[Cir::Module::NOT1].name == "NOT1",
        << "cir.modules[Cir::Module::NOT1].name = " 
        << cir.modules[Cir::Module::NOT1].name);
           
    ASSERT(cir.modules[Cir::Module::NOT1].input_names.size() == 1,
        << "cir.modules[Cir::Module::NOT1].input_names.size() = "
        << cir.modules[Cir::Module::NOT1].input_names.size());

    ASSERT(cir.modules[Cir::Module::NOT1].input_names[0] == "A",
        << "cir.modules[Cir::Module::NOT1].input_names[0] = "
        << cir.modules[Cir::Module::NOT1].input_names[0]);

    ASSERT(cir.modules[Cir::Module::NOT1].output_name == "Y",
        << "cir.modules[Cir::Module::NOT1].output_name = "
        << cir.modules[Cir::Module::NOT1].output_name);

    std::cerr << __FUNCTION__ << "() passed.\n";
}

