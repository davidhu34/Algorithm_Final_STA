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

## Problem

**2016 D. Static Timing Analysis**

Find as many sensitizable paths as possible for combinational logic
circuit using shortest time.

See `doc/requirement.md` for more information.

## Synopsys

```
sta [-o <output_file>] <input_file> ...
```

## Description

It will print a true path set. If option `-o` is given, it will print
into `<output_file>`. For more details about input and output,
see `doc/requirement.md`.

## Options

<dl>

<dt><code>-o</code></dt>
<dd><p>Output to this file.</p></dd>

<dt><code>&lt;output_file&gt;</code></dt>
<dd><p>If given, a true path set will be printed into it.</p></dd>

<dt><code>&lt;input_file&gt;</code></dt>
<dd><p>Verilog file describing a gate-level netlist. Files will
    concatenated before being parsed. </p></dd>

</dl>

## Directory

Directory      |Put what kind of file?
-------------- |------------------------------------------------------
bin/           |Executable, product.
doc/           |Documentation, reports.
lib/           |Library needed for this program, e.g. openMP.
res/           |Resources needed for this program, such as images.
src/           |Source code, grouped into modules.
src/main/      |Main module, should contain only main function and utilities don't need to be tested.
src/util/      |Utility module, can be used by other modules.
src/cir/       |Circuit module, contain parser and circuits elements.
src/ana/       |Analyzer module, contain anything about STA algo.
test/          |Everything related to testing.
test/doc/      |Testing documentation and results.
test/cases/    |Input and output of all cases.
test/lib/      |Library needed for testing.
test/src/      |Source code of testing.
test/src/util/ |Some utilities for testing.
test/scripts/  |Scripts for testing.

### How to Work With It

Module is a directory that contains many source and header files.
Use module to group closely related classes and functions.

Procedure for creating a new module:

1.  Create a new directory under `src/`, e.g. `src/foo`.

2.  Copy `src/util/module.mk` to `src/foo/`.

3.  Modify the first line of `src/foo/module.mk`, change it to
    `DIR := src/foo`.

4.  Modify `Makefile`, add your module (`src/foo`) to the line start
    with `MODULES := `. 

That's it. You can add whatever headers and sources in your new
module. Please put all these functions and classes under namespace
with the name same as your module, e.g. `Foo`.

To include a header file, always prefix it with its module name.
E.g. `#include "foo/parser.h"`.

Any files under `src/main/` (e.g. main.cpp) won't be compiled while
doing unit test. Testing has its own main function, so don't put
files that you want to do unit test under `src/main/`.

## How to Test

### Unit Testing

Let's say you have written some classes or functions in `src/foo/bar.cpp`
and `src/foo/bar.h` and you want to test them. Here is the procedure:

1.  Go to `test/src` directory.

2.  Create a test file: `test_foo_bar.cpp`. The naming format is
    `test_<module>_<file>.cpp` or `test_<module>_<class>.cpp`.

3.  Write your test function there. See `test_example.cpp` for examples.
    I've written some random functions to help testing. It's in
    `test/src/util`. You can use it if you need it.

4.  Open `test/src/main.cpp`, add your test functions' prototype before
    `main()`, then call your test functions inside `main()`.

5.  Go to project root directory, open `Makefile` file. Make sure
    your module (`src/foo`) is listed on the `MODULES := ` line in
    `debug` section. If you want to exclude other module, just
    delete it from that line.

6.  At project root directory, type `make`. If will compile
    all necessary files and create an executable: `bin/unit_test`.
    It will execute it automatically at the end of make.

In the implementation or debugging process, some people will write
many small `main()` function to test their class or function, and
dump them away after their test complete. This is a kind of waste.
Try to write those functions in `test/src` so we can reuse them
everytime after we modify our source code.

Please write a `print()` member function for each class you defined
that print all data members' state. This is for debugging purpose.

If you want your static (intenal linkage) functions to be testable,
write them in a file end with `.inc`, then include it into your `.cpp`.
Then you can include that `.inc` file to test it.

### Integration Testing

Integration testing if for whole project. To run integration test,
see scripts under `test/scripts`.

## How to Compile

Type `make mode=release`.

## How to Run

See sypnosis.

