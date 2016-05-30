# How to Use MiniSat

## Boolean Satisfiability Problem (SAT)

<dl><dt>Boolean Satisfiability Problem</dt>
    <dd><p>It asks, given a boolean formula, is there a way to assign
    value to variables of that formula, so that the formula evaluate
    to 1. It is a NP-complete problem. </p></dd>

<dt>Satisfiable</dt>
    <dd><p>If there is a way to assign value to variables of a formula,
    such that the formula evaluate to 1, then the formula is
    satisfiable. E.g. "AB'" is satisfiable, since when we assign
    (A, B) = (1, 0), that formula evaluate to 1.</p></dd>

<dt>Unsatisfiable</dt>
    <dd><p>If there is no way to assign value to variables of a formula,
    such that the formula evaluate to 1, then the formula is
    unsatisfiable. E.g. "AA'" is not satisfiable.</p></dd>

<dt>Boolean Formula</dt>
    <dd><p>It is build from variables, operator AND, operator OR,
    operator NOT, and parenthesis.</p></dd>

<dt>Conjunction</dt>
    <dd><p>It is synonym of AND.</p></dd>

<dt>Disjunction</dt>
    <dd><p>It is synonym of OR.</p></dd>

<dt>Negation</dt>
    <dd><p>It is synonym of NOT.</p></dd>

<dt>Literal</dt>
    <dd><p>It is either a variable (then called positive literal), or
    the negation of a variable (then called negative literal). E.g.
    "A" is a positive literal, "A'" is a negative literal.</dd></p>

<dt>Clause</dt>
    <dd><p>It is either many literals ORed together, or one literal
    (then called unit clause). E.g. "X1 + X2'", "X1".</dd></p>

<dt>Conjunctive Normal Form</dt>
    <dd><p>It is a format of a formula. The format is, either many
    clauses ANDed together, or one clause. E.g. "(A + B')(A' +
    B + C)A'". This example is satisfiable. We can assign (A, B, C)
    = (0, 0, X) where X can be 1 or 0. This will make the formula
    evaluate to 1.</dd></p>

<dt>Model</dt>
    <dd><p>It is an assignment of variable that can make the formula
    to evaluate to true.</p></dd></dl>

## Circuit SAT

<dl><dt>Circuit SAT</dt>
    <dd><p>It asks, given a boolean circuit, is there a way to assign
    value to input pin, so that output pin's value is 1.</p></dd></dl>

Circuit SAT problem can be expressed as a SAT problem. Consider a
circuit with only one AND gate.

```
      -------
A-----|      \
      |      |-----C
B-----|      /
      -------
```

We want to know is there a way to assign value to A, B, so that C = 1.
To transform it to SAT, first, form a boolean formula that describe
this AND gate:

AB    |C
----- |-----
00    |0
01    |0
10    |0
11    |1

ABC   |Is AND Gate
----- |-----------
000   |1
001   |0
010   |1
011   |0
100   |1
101   |0
110   |0
111   |1

Formula:  
(A'B'C + A'BC + AB'C + ABC')'  
= (A + B + C')(A + B' + C')(A' + B + C')
(A' + B' + C)  
= (A + C')(B + C')(A' + B' + C)

This formula evaluate to 1 only when assignment to A, B, C matches
behavior of an AND gate.

We want C = 1, so we add a clause (C) to that formula:  
(A + C')(B + C')(A' + B' + C)(C)

Now this formula evaluate to 1 only when assignment to A, B, C matches
behavior of an AND gate, and C must equal to 1. So now it has become
a SAT problem: Is there a way we can assign value to A, B, C so that
this formula evaluate to 1?

Truth table for behavior of AND, OR, NOT, NAND, NOR gates:

ABC   |AND   |OR    |NOT   |NAND  |NOR
----- |----- |----- |----- |----- |-----
000   |1     |1     |0     |0     |0
001   |0     |0     |1     |1     |1
010   |1     |0     |      |0     |1
011   |0     |1     |      |1     |0
100   |1     |0     |1     |0     |1
101   |0     |1     |0     |1     |0
110   |0     |0     |      |1     |1
111   |1     |1     |      |0     |0

Formula for AND, OR, NOT, NAND, NOR gates:

Gate  |Formula
----- |-----------------------------------
AND   |(A + C')(B + C')(A' + B' + C)
OR    |(A + B + C')(B' + C)(A' + C)
NOT   |(A + C)(A' + C')
NAND  |(A + C)(B + C)(A' + B' + C')
NOR   |(A + B + C)(B' + C')(A' + C')

Another example:

```
A----------------
                |   -------
      ------    ----|      \
B-----\     \       |      |---H
       |     >--F---|      /   |
C-----/     /       -------    |
      ------                   |  -------
      -------                  ---|      \
D-----|      \                    |      |-----I
      |      |O--G----------------|      /
E-----|      /                    -------
      -------
```

The behavior of this circuit can be expressed with the following
boolean formula:

(B + C + F')(C' + F)(B' + F)  
(D + G)(E + G)(D' + E' + G')  
(A + H')(F + H')(A' + F' + H)  
(H + I')(G + I')(H' + G' + I)  

To ask whether there exist a assignment that can let I = 1, just add
a unit clause (I) to that formula.

## MiniSat

MiniSat is an SAT solver. Given an boolean formula, it can tell you
whether it is satisfiable or not.

### MiniSat API

```c++
template <class T>
class vec {
public:
    // Constructors
    vec         (void);
    explicit vec(int size);
    vec         (int size, const T& pad);

    // Size Operations
    int  size    (void) const;
    void shrink  (int nelems);
    int  capacity(void) const;
    void capacity(int min_cap);
    void growTo  (int size);
    void growTo  (int size, const T& pad);
    void clear   (bool dealloc = false);

    // Stack Interface
    void     push(void);
    void     push(const T& elem);
    void     pop (void);
    const T& last(void) const;
    T&       last(void);

    // Vector interface
    const T& operator[](int index) const;
    T&       operator[](int index);

    // Duplication
    void copyTo(vec<T>& copy) const;
    void moveTo(vec<T>& dest);
};
```

This is a container class defined in MiniSat that you will be using
frequently. Basically it is same as `std::vector`, except `shrink()`
function, which `nelems` represent number of element you want to
delete.

```c++
typedef int Var;
```

In MiniSat, variable is represented by an integer. It start from 0.

```c++
struct Lit {
    int x;

    bool operator==(Lit p) const;
    bool operator!=(Lit p) const;
};

Lit  mkLit    (Var, bool sign = false);
Lit  operator~(Lit p);
Lit  operator^(Lit p, bool b);
bool sign     (Lit p);
int  var      (Lit p);
```

In MiniSat, literal is represented by an integer. It use the last bit
to store sign information. 1 represent positive literal. 0 represent
negative literal. It use other bits to store variable. `operator~`
and `operator^` operate on sign of literal only.

```c++
#define l_True  0
#define l_False 1
#define l_Undef 2

class lbool {
public:
    lbool         (void);
    explicit lbool(uint8_t v);
    explicit lbool(bool x);

    bool  operator==(lbool b) const;
    bool  operator!=(lbool b) const;
    lbool operator^ (bool b) const;
    lbool operator&&(lbool b) const;
    lbool operator||(lbool b) const;
};

int   toInt  (lbool b);
lbool toLbool(int v);
```

`lbool` is lifted boolean. Lifted boolean has 3 possible values: true,
false or undefined.

```c++
class Solver {
public:
    Var  newVar   (bool polarity = true, bool dvar = true);
    bool addClause(const vec<Lit>& literals);
    bool simplify ();
    bool solve    ();
    bool solve    (const vec<Lit>& assumptions);

    vec<lbool> model;
};
```

Use `newVar()` to create new variable. Use `mkLit()` to create literal
from variable.

Use `addClause()` to add clause. Clause is simply `vec` of `Lit`.
Trivial conflict of clauses, such as clause "X" and clause "~X", can
be detect by `addClause()`. If it detect such clauses, it will return
`false`. From this point on, the solver's state is undefined and must
not be used further.

Use `solve()` with no assumption to check whether this formula is
satisfiable. It will return `true` if the formula is satisfiable, and
`false` otherwise. If it return `true`, you can read `model`, and 
continue to add more clauses to it, and then call `solve()` again.

Use `simplify()` before calling `solve()` to simplify the formula. It 
will first propagate all unit clause, then remove all satisfied 
constraints. It return `false` if it detect conflict. In that case,
Solver's state is undefined and must not be used further.

Use `solve()` with assumptions to make solve temporarily assumes
the literals to be true. After finding a model or conflict, these
assumptions are undone, and the solver is returned to usable state,
even when `solve()` return `false`. You must call `simplify()` before
calling `solve()` with assumptions.

