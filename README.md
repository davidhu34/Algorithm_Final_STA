# Algorithm Final Project: Static Timing Analysis

## Team

Group 24

- 劉典恆 b01901185
- 胡明衛 b01901133
- 許睿中 b01502119
- 張漢維 b01901181

## Schedule

Date  |Item
----- |----
5/4   |Contest application deadline.
5/24  |Progress report deadline.
6/7   |Project deadline. Presentation.

## Problem

**2016 D. Static Timing Analysis**

Find as many sensitizable paths as possible for combinational logic
circuit using shortest time.

See `doc/requirement.md` for more information.

## Synopsys

```
sta -t <time_constraint> 
    -s <slack_constraint> 
    [-o <output_file>] 
    <input_files>
```

## Description

It will print a true path set. If option `-o` is given, it will print
into `<output_file>`. For more details about input and output,
see `doc/requirement.md`.

## Options

<dl>

<dt><code>-o</code></dt>
<dd><p>Output to this file.</p></dd>

<dt><code>-t &lt;time_constraint&gt;</code></dt>
<dd><p>Specify time constraint.</p></dd>

<dt><code>-s &lt;slack_constraint&gt;</code></dt>
<dd><p>Specify slack constraint.</p></dd>

<dt><code>&lt;output_file&gt;</code></dt>
<dd><p>If given, a true path set will be printed into it.</p></dd>

<dt><code>&lt;input_file&gt;</code></dt>
<dd><p>Verilog file describing a gate-level netlist. Files will
    concatenated before being parsed. </p></dd>

</dl>

## Directory

<table>

<tr><th>Directory</th>
<th>Put what kind of file?</th></tr>

<tr><td>bin/</td>
<td>Executable, library, product.</td></tr>

<tr><td>doc/</td>
<td>Documentation, design, reference.</td>

<tr><td>lib/</td>
<td>Library needed for this program. That include headers of that 
library.</td>

<tr><td>res/</td>
<td>Resources needed for this program, such as images.</td>

<tr><td>src/</td>
<td>Source code, grouped into modules.</td>

<tr><td>src/main/</td>
<td>Main module, should contain only main function and utilities
that don't need to perform unit test.</td>

<tr><td>src/util/</td>
<td>Utility module. Code inside here should not include other modules,
but other module can include this this module. E.g. some container
classes or general algorithms.</td>

<tr><td>src/cir/</td>
<td>Circuit module, contain parser, writer and circuits elements.</td>

<tr><td>src/ana/</td>
<td>Analyzer module, contain anything about STA algorithms.</td>

<tr><td>test/</td>
<td>Everything related to testing.</td>

<tr><td>test/doc/</td>
<td>Testing documentation and results, performance report.</td>

<tr><td>test/cases/</td>
<td>Input and output of all cases.</td>

<tr><td>test/lib/</td>
<td>Library needed for testing.</td>

<tr><td>test/src/</td>
<td>Source code for unit testing.</td>

<tr><td>test/src/util/</td>
<td>Some utilities for testing.</td>

<tr><td>test/scripts/</td>
<td>Scripts for testing.</td>

<tr><td>test/cic_program/</td> 
<td>Contain verification programs provided by Cadence.</td>

</table>

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
module. Please put all these functions and classes under project
namespace e.g. `Sta`, then under namespace with the name same as
your module, e.g. `Foo`.

To include a header file, always use path start from project, e.g.
`#include "sta/src/foo/parser.h"`.

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
    `utest` section. If you want to exclude other module, just
    delete it from that line.

6.  At project root directory, type `make`. If will compile
    all necessary files and create an executable: `bin/unit_test`.
    It will execute it automatically at the end of make.

In the implementation or debugging process, some people will write
many small `main()` function to test their class or function, and
dump them away after their test complete. This is a kind of waste.
Try to write those functions in `test/src` so we can reuse them
everytime after we modify our source code.

If you don't want to compile certain test file, just add it into
`test/src/module.mk` at `EXCLUDE` line.

Please write a `print()` member function for each class you defined
that print all data members' state. This is for debugging purpose.

If you want your static (intenal linkage) functions to be testable,
write them in a file end with `.inc`, then include it into your `.cpp`.
Then you can include that `.inc` file to test it.

### Integration Testing

Integration testing if for whole project. To run integration test,
see scripts under `test/scripts`.

## How to Compile

Utility required to compile this project:

- g++ (or maybe other C++ compiler?)
- gnumake (or maybe other make?)
- fmt (\*nix utility)
- sed (\*nix utility)

At project root directory, type `make ver=rel`. This will compile with
optimization `-O3`. You can use `make ver=dbg` to compile with no
optimization. If you type `make` only, it will only perform unit test.

## How to Run

See sypnosis.

