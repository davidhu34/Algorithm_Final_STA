% Algorithm Final Project: Static Timing Analysis
% dianhenglau
% 4/26/2016

# Problem

**2016 D. Static Timing Analysis**

Design a parallel STA program for combinational logic circuit under
multicore environment.

## Input

Input will be a verilog file describing gate-level netlist.

## Output

a list of true paths.

## Specification

-   Assume wire does not cause delay.
-   Only 3 types of gate will be used in input: NAND, NOR, NOT.
-   All gate has delay of 1ns.

# Definition

Static Timing Analysis (STA)

:   is a process of checking whether propagation time from input to
    output is within a certain time constraint. It is an IC design
    process.

Critical Path

:   TODO

Timing Violation

:   TODO

False Path Problem

:   TODO

Floating-mode

:   is a state of a point. It means that the value of the point is 
    unknown. Point can be input or output of any elements in the
    circuit.

Slack

:   TODO

True Path

:   is a path from an input I to an output O, that change of I from
    floating-mode to a certain value, causes a change of O from
    floating-mode to a certain value. It depends on the relative
    arrival time of this path's input, the delay of each elements
    in the circuit, the value of this path's input and other paths'
    input, and the type of gates along the path.

False Path

:   is a path that is not true path.

Sensitizing A Path

:   is an action of changing the value of source from floating-mode to
    a certain value, and cause the value of destination change from 
    floating-mode to certain value. In other word, make that path become
    a true path.

Controlling Value

:   is a value that its existance in input will guarantee a certain
    output. E.g. Controlling value for AND and NAND gate is 0,
    Controlling value for OR and NOR gate is 1.

Online Signal

:   is the input we are focusing right now.

Side Input

:   is an input that is not online signal.

# True Path of Gates

Consider a 2-input gate. Let A and B be its two input. There are two
paths in this case: A --- gate --- output, B --- gate --- output. Now
we are going to find out which path is true path for specific type
of gates, at different arrival time of A and B. Let current state
of A and B be floating-mode.

Table: A arrives *earlier* than B.

Next State  True Path of NAND  True Path of NOR
----------  -----------------  ----------------
00          A                  B
01          A                  B
10          B                  A
11          B                  A

Table: A arrives *later* than B.

Next State  True Path of NAND  True Path of NOR
----------  -----------------  ----------------
00          B                  A
01          A                  B
10          B                  A
11          A                  B

Table: A and B arrive *together*.

Next State  True Path of NAND  True Path of NOR
----------  -----------------  ----------------
00          A and B            A and B
01          A                  B
10          B                  A
11          A and B            A and B

# Team

Group 24

-   劉典恆 b01901185
-   胡明衛 b01901133
-   許睿中 b01502119
-   張漢維 b01901181

# Schedule

Date   Item
-----  ----
5/3    Apply for contest.
5/4    Contest application deadline.
5/5    Start implementing it.
5/13   Finish basic function.
5/14   Start progress report.
5/17   Finish progress report.
5/18   Start optimization (parallelize it).
6/8    Alpha test.
6/10   Finish optimization.
6/11   Start documentation and presentation.
6/13   Finish documentation and presentation.
6/14   Project deadline.

# Some Ideas

Use a directed graph to represent the circuit.
Vertices represent gates, primary inputs and primary outputs.
Edges represent wires.
Edges has weights, represent delays.

# Bibliography

-   [ICCAD 2016 Contest Problem D. Static Timing Analysis Description, by Chung-Ming Huang and Wei-Chang Tsai](cad-contest-2016.el.cycu.edu.tw/Problem_D/default.html)
-   [Fundamental Algorithms for System Modeling, Analysis, and Optimization, by Edward, Jaijeet, Sanjit and Stavros](embedded.eecs.berkeley.edu/eecsx44/lectures/Spring2013/timingAnalysis.pdf)