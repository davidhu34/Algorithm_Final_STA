# 2016 D. Static Timing Analysis

## Problem Description

Design a parallel STA program for combinational logic circuit under
multicore environment.

It take a verilog file describing gate-level netlist as input, and output
a true path set. See Input and Output section below.

For complete problem description, refer to [Official Problem Specification][].

[Official Problem Specification]: <http://cad-contest-2016.el.cycu.edu.tw/Problem_D/default.html>

## Definition

<dl>

<dt>Static Timing Analysis (STA)</dt>
<dd><p> is a process of checking whether propagation time from input to
    output is within time constraint and slack constraint. It is an IC
    design process.</p></dd>

<dt>Time Constraint</dt>
<dd><p>is a requirement. "Time" refers to propagation time of a signal
    from input pin to output pin. Actual propagation time must lower
    than this value.</p></dd>

<dt>Slack</dt>
<dd><p>= time constraint - actual propagation time.</p></dd>

<dt>Slack Constraint</dt>
<dd><p>is a requirement. Actual slack must lower than this value.
    In other word, actual propagation time cannot shorter than
    or equal to time constraint - slack constraint.</p></dd>

<dt>Timing Violation</dt>
<dd><p>happens when actual propagation time violates time constraint or
    slack constraint, i.e. too long or too short.</p></dd>

<dt>Floating-mode</dt>
<dd><p>is a state of a point. It means that the value of the point is 
    unknown. Point can be input or output of any elements in the
    circuit.</p></dd>

<dt>True Path</dt>
<dd><p>is a path from an input I to an output O, that change of I from
    floating-mode to a certain value, causes a change of O from
    floating-mode to a certain value. It depends on the relative
    arrival time of this path's input, the delay of each elements
    in the circuit, the value of this path's input and other paths'
    inputs, and the type of gates along the path.</p></dd>

<dt>False Path</dt>
<dd><p>is a path that is not true path, i.e. when its input change from
    floating-mode to a certain value, its output doesn't change.</p></dd>

<dt>Critical Path</dt>
<dd><p>TODO</p></dd>

<dt>Sensitizing A Path</dt>
<dd><p>is an action of changing the path input from floating-mode to
    a certain value, and cause the path output change from floating-mode
    to certain value. In other word, make that path become a true path.
    </p></dd>

<dt>Sensitizable Path</dt>
<dd><p>is a path capable of becoming a true path.</p></dd>

<dt>False Path Problem</dt>
<dd><p>is a problem of finding longest sensitizable path.</p></dd>

<dt>Controlling Value</dt>
<dd><p>is a value that its existance in input will guarantee a certain
    output. E.g. Controlling value for AND and NAND gate is 0,
    Controlling value for OR and NOR gate is 1.</p></dd>

<dt>Online Signal</dt>
<dd><p>is the input we are focusing right now.</p></dd>

<dt>Side Input</dt>
<dd><p>is an input that is not online signal.</p></dd>

</dl>

## True Path of Gates

Consider a 2-input gate. Let A and B be its two input. There are two
paths in this case: A --- gate --- output, B --- gate --- output. Now
we are going to find out which path is true path for specific type
of gates, at different arrival time of A and B. Let current state
of A and B be floating-mode.

Table: A arrives *earlier* than B.

Next State |True Path of NAND |True Path of NOR
---------- |----------------- |----------------
00         |A                 |B
01         |A                 |B
10         |B                 |A
11         |B                 |A

Table: A arrives *later* than B.

Next State |True Path of NAND |True Path of NOR
---------- |----------------- |----------------
00         |B                 |A
01         |A                 |B
10         |B                 |A
11         |A                 |B

Table: A and B arrive *together*.

Next State |True Path of NAND |True Path of NOR
---------- |----------------- |----------------
00         |A and B           |A and B
01         |A                 |B
10         |B                 |A
11         |A and B           |A and B

```
A_is_true_path_of_nand(A, B, A_arrival_time, B_arrival_time)
    if A_arrival_time < B_arrival_time and A == 0
        return true
    else if A_arrival_time > B_arrival_time and B == 1
        return true
    else if A_arrival_time == B_arrival_time and (A == 0 or B == 1)
        return true
    else
        return false
```

```
A_is_true_path_of_nor(A, B, A_arrival_time, B_arrival_time)
    if A_arrival_time < B_arrival_time and A == 1
        return true
    else if A_arrival_time > B_arrival_time and B == 0
        return true
    else if A_arrival_time == B_arrival_time and (A == 1 or B == 0)
        return true
    else
        return false
```

## Input

There are 3 inputs per case:

1.  File `cadcontest.v` contains basic models.

```verilog
// cadcontest.v

module NAND2(Y, A, B);
    input    A, B;
    output    Y;
    nand(Y, A, B);
endmodule

module NOR2 (Y, A, B);
    output    Y;
    input    A, B;
    nor    (Y, A, B);
endmodule

module NOT1 (Y, A);
    output    Y;
    input    A;
    not    I0(Y, A);
endmodule
```

Please see `case1/input/cadcontest.v` for more verilog constructs.
This is a simplified example.

2.  File `case*` where `*` = case number, contains netlist.

```verilog
// case0

module mul2 ( M, A, B );     output [3:0] M;
    input [1:0] A;
    input [1:0] B;
    wire n1, n2, n3, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14;

    NOT1 U1 ( .A(n13), .Y(n1) );
    NOT1 U2 ( .A(n11), .Y(n2) );
    NOT1 U3 ( .A(n9), .Y(n3) );
    NOT1 U4 ( .A(n14), .Y(M[0]) );
    NOT1 U5 ( .A(B[1]), .Y(n5) );
    NOR2 U6 ( .A(n6), .B(n7), .Y(M[3]) );
    NAND2 U7 ( .A(B[1]), .B(B[0]), .Y(n7) );
    NAND2 U8 ( .A(A[1]), .B(A[0]), .Y(n6) );
    NOR2 U9 ( .A(n5), .B(n8), .Y(M[2]) );
    NAND2 U10 ( .A(A[1]), .B(n3), .Y(n8) );
    NOR2 U11 ( .A(n10), .B(n11), .Y(n9) );
    NAND2 U12 ( .A(n12), .B(n1), .Y(M[1]) );
    NOR2 U13 ( .A(n10), .B(n2), .Y(n13) );
    NAND2 U14 ( .A(n2), .B(n10), .Y(n12) );
    NAND2 U15 ( .A(B[1]), .B(A[0]), .Y(n10) );
    NAND2 U16 ( .A(B[0]), .B(A[1]), .Y(n11) );
    NAND2 U17 ( .A(A[0]), .B(B[0]), .Y(n14) );
endmodule
```

3.  Time constraint and slack constraint. They will not be given
    in the file. You can find them at official page or section
    [Time and Slack Constraint](#time-and-slack-constraint) below.
    Please set them in your Makefile using compiler option, e.g.
    `-DTIME_CONSTRAINT=<value> -DSLACK_CONSTRAINT=<value>`.

5 cases will be given (`case1` to `case5`). You can download them
at [Official Problem Specification][].

Some specifications:

-   Only 3 types of gate will be used in input: NAND2, NOR2, NOT1.
    NAND2 and NOR2 are 2-input gates, while NOT1 is 1-input gate.

-   Assume wire does not cause delay.

-   All gate has delay of 1ns.

-   The input and output pins of NAND2, NOR2, NOT1 will be named
    as above (`A`, `B`, `Y`) in all cases. But to immitate real world
    scenario, please get those names by parsing them.

-   The naming of each object (wire and gate) is random, not
    necessarily (`U1`, `U2`, ...) and (`n1`, `n2`, ...).

-   While create object from module, "naming association" is the
    only method we use in all cases. We won't use "positional
    association" method to pass arguments.

Please see [IEEE std 1346-1995][] for verilog's syntax. 

[IEEE std 1346-1995]: <http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=803556&isnumber=12005&tag=1>

There are some open source verilog parser outside. You can refer to them.

-   Icarus <http://icarus.com/eda/verilog>
-   VBS <http://www.geda.seul.org/index.html>

## Output

Output a true path set file, e.g.

```
Header  {  A True Path Set  }

  Benchmark  {  case1  }

  Path  {  1  }

  A True Path List
  {
  ---------------------------------------------------------------------------
  Pin    type                                Incr        Path delay
  ---------------------------------------------------------------------------
  A[1] (in)                                   0          0 f
  U16/B (NAND2)                               0          0 f
  U16/Y (NAND2)                               1          1 r
  U2/A (NOT1)                                 0          1 r
  U2/Y (NOT1)                                 1          2 f
  U13/B (NOR2)                                0          2 f
  U13/Y (NOR2)                                1          3 r
  U1/A (NOT1)                                 0          3 r
  U1/Y (NOT1)                                 1          4 f
  U12/B (NAND2)                               0          4 f
  U12/Y (NAND2)                               1          5 r
  M[1] (out)                                  0          5 r
  --------------------------------------------------------------------------
  Data Required Time            10
  Data Arrival Time              5
  --------------------------------------------------------------------------
  Slack                          5
  }

  Input Vector
  {
    A[0]  =  1
    A[1]  =  f
    B[0]  =  1
    B[1]  =  1
     }

  Path  {  2  }

  A True Path List
  {
  ---------------------------------------------------------------------------
  Pin     type                               Incr     Path delay
  ---------------------------------------------------------------------------
  A[0] (in)                                   0          0 r
  U17/A (NAND2)                               0          0 r
  U17/Y (NAND2)                               1          1 f
  U4/A (NOT1)                                 0          1 f
  U4/Y (NOT1)                                 1          2 r
  M[0] (out)                                  0          2 r
  --------------------------------------------------------------------------
    Data Required Time          10
    Data Arrival Time            2
    --------------------------------------------------------------------------
    Slack                        8
  }

  Input Vector
  {
    A[0]  =  r
    A[1]  =  1
    B[0]  =  1
    B[1]  =  r
  }

  Path  {  3  }

  A True Path List
  {
  ---------------------------------------------------------------------------
  Pin  type                                  Incr   Path delay
  ---------------------------------------------------------------------------
  B[1] (in)                                   0          0 r
  U7/A (NAND2)                                0          0 r
  U7/Y (NAND2)                                1          1 f
  U6/B (NOR2)                                 0          1 f
  U6/Y (NOR2)                                 1          2 r
  M[3] (out)                                  0          2 r
  --------------------------------------------------------------------------
    Data Required Time          10
    Data Arrival Time            2
    --------------------------------------------------------------------------
    Slack                        8
  }

   {
    A[0]  =  1
    A[1]  =  1
    B[0]  =  1
    B[1]  =  r
  }

    ...


}
```

Header block always gives "A True Path Set".

Benchmark block gives the name of running case (input file).

Path block gives the order of true path start from 1.

'A True Path List' block give a sensitizable path and its STA result.

-   The "Pin" column lists input pin and output pin of all gates along
    that true path. It starts from input port and ends at output port. 
-   The "type" column lists the type of gate of first column.
-   The "Incr" column lists the incremental delay.
-   The "Path delay" lists accumulated delay.
-   The fifth column lists logical value of that pin, r = 1 and f = 0.
-   "Data Required Time" gives the timing constraint.
-   "Data Arrival Time" is arrival time of signal at output pin.
-   Slack = Required - Arrival.

'Input Vector' block is the input that result in this true path.

Some requirements:

-   One 'Input Vector' block for each 'A True Path List' block.
-   No duplicate 'A True Path List' block. Two 'A True Path List'
    blocks is same if they have identical path with identical input.
    (If their input is not same, then these two blocks are not same
    and both of them must be listed.)
-   Output filename should be `case*_true_path_set` where `*` is the
    case number.
    
You can verify your output using a program provided by CIC.

## General Procedure

1.  Read in an verilog file describing gate level netlist. In this case,
    it is a 2-bit multiplier.

2.  Build a directed acyclic graph from it.

3.  Find all paths that are within time constraint and slack constraint.
    In this case, time constraint is 10ns and slack constraint is 7ns.

4.  Find all sensitizable paths from result of 3. For each sensitizable
    path, record the input vector that result in that path being true.

5.  Output sensitizable path and its input vector.

## Evaluation

Two stages: Correctness and performance.

### Correctness

Procedure of evaluating correctness:

1.  Under project root, check whether `Makefile` and `case1` to `case5`
    exists.

2.  Type `make sta`. Your program should be compiled automatically.
    The executable should be located at project root directory.

3.  Type `time run_sta*`. `*` = 1 to 5. `run_sta*` is a script containing
    only one line: `make run_case*`. It will be provided. The output file
    (`case*_true_path_set`) of this run should be located at project
    root directory.

4.  Check the format and slack constraint of `case*_true_path_set` file
    using `Verify` provided by CIC. Output of verification will be stored
    at `case*.verify.rpt`. If success, keyword `pass` will appear
    in that file.

5.  Check the true path list of `case*_true_path_set` file using
    `Justify` provided by CIC. Output of verification will be stored
    at `case*.justify.rpt`.

6.  Repeat steps 3 to 5 using private input files.

We will classify the result of correctness into grades:

-   Grade 1: All correct.
-   Grade 2: Contain false path in one case.
-   Grade 3: Contain false path in two cases.
-   Grade 4: Contain false path in three cases.
-   Grade 5: Contain false path in four cases.
-   and so on...

Grade 1 is better than Grade 2, no matter how they perform in performance.

### Performance

Not important since we are not participating the contest :wink: .

## Time and Slack Constraint

File  |Gate Num |Input Num |Output Num |Time Constraint |Slack Constraint
----- |--------:|---------:|----------:|---------------:|----------------:
case1 |    1145 |       20 |        20 |             45 |               4
case2 |     413 |       60 |        26 |             43 |              10
case3 |      95 |        8 |         9 |             31 |               6
case4 |     276 |       41 |        21 |             45 |               6
case5 |  127389 |       64 |        32 |             47 |              10

## How to Use `Verify` and `Justify`

1.  Run your program and create an output file `case1_true_path_set`.

2.  Put it under `case1/true_path/` directory.

3.  Under `case1/true_path/` directory, execute
    `path/to/Verify case1_true_path_set`. It will create a report
    `case1.verify.rpt` at `case1/output/`. If you pass the verification,
    it will show the keyword "pass".

Same steps for `Justify`.


