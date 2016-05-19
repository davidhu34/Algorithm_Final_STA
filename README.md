# Algorithm Final Project: Static Timing Analysis

## Team

Group 24

-   劉典恆 b01901185
-   胡明衛 b01901133
-   許睿中 b01502119
-   張漢維 b01901181

## Schedule

Date  |Item
----- |----
5/4   |Contest application deadline.
5/24  |Progress report deadline.
6/8   |Alpha test.
6/14  |Project deadline.

## TODO

- [ ] Finish `requirement.md`.
- [ ] Input parser.
- [ ] Define interfaces.

## Problem

**2016 D. Static Timing Analysis**

Design a parallel STA program for combinational logic circuit under
multicore environment.

## Some Ideas

-   Use a directed graph to represent the circuit.
-   Vertices represent gates, primary inputs and primary outputs.
-   Edges represent wires.
-   Edges has weights, represent delays.

## Synopsys

```
sta <input_file>
```

## Description

It will output a true path set. For more details about input and output,
see `doc/requirement.md`.

## Options

<dl>

<dt><code>input_file</code></dt>
<dd><p>Verilog file that describing a gate-level netlist.</p></dd>

</dl>

## Directory

Directory      |Put what kind of file?
-------------- |--------------------------------------------------
bin/           |Executable, product.
doc/           |Documentation, reports.
lib/           |Library needed for this program.
res/           |Resources needed for this program, such as images.
src/           |Source code, grouped into modules.
src/main/      |Main module.
test/          |Everything related to testing.
test/doc/      |Testing documentation and results.
test/inputs/   |Input for testing.
test/outputs/  |Output for testing.
test/lib/      |Library needed for testing.
test/src/      |Source code of testing.
test/scripts/  |Scripts for testing.

## How to Compile

Type `make`.

You can choose between optimize or debug version in Makefile. Remember
to clear all files before you switch.

## How to Run

See sypnosis.

## How to Test

To run unit test, type `make -f Maketest`, then run `bin/unit_test`.

To run integration test, see scripts under `test/scripts`.

