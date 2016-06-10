# Ideas

## How to Find All Paths

- Do depth first traverse from all input pins toward output pins.

- Add the path to path list every time you reach an output pin.

#### Time Complexity

- ?

## How to Find All Paths Within Constraint

- Do depth first traverse from all input pins toward output pins.

- While doing depth first traverse, calculate arrival time at each
  node. If arrival time is greater than or equal to time constraint,
  stop moving down this path.

- Calculate slack everytime you reach output pin. If slack is greater
  than or equal to slack constraint, do not add this path into path
  list.

#### Time Complexity

- ?

## How to Calculate Max Arrival Time of All Gates

- From input pins, do breadth first traverse toward output pins. Add
  a node to queue only if its fan-in has all arrived. Its max arrival
  time is known if it can be added into queue.

#### Time Complexity

- O(|E|).

## How to Calculate Value of All Points

- Same process as calculating arrival time of all gates.

#### Time Complexity

- O(|E|).

## How to Find Sensitizable Paths

### Method 1 (Brute Force)

- Try all possible permutation of input vectors.

- For each input vector, simulate it and note down which gate is
  part of a true path. Then, do depth first traverse from PI to PO,
  walking through only gate that is part of true path. If you reach
  PO, add (without duplication) this path into true path set.

#### Time Complexity

- Try all possible permutation: O(2^|PI|).

- Simulation: O(|E|).

- Depth first traverse: ?

- For each true path found, add to true path set: O(|P| * |p|) where
  |P| is number of true path found and |p| is average path length.

- Overall time complexity is O(2^|PI| * (|E| + ? + |P| * |p|)).

#### Note

- This method will find all sensitizable paths.

### Method 2 (Backtracking)

- Calculate min and max arrival time of all gates.

- Basically the idea is trace from output pins toward input pins. Try
  every possibility (condition) that make a path become a true path.
  Check whether our assumptions has any contradiction.

- Monitor slack constraint and time constraint while tracing.

How many state do we need to check?

Assume that, given this gate's value, for each true path that pass
through this gate, the PI of that true path can only take one
value (if it can take 1, then it cannot take 0; if it can take 0,
then it cannot take 1).

State table:

Gate |True |Y    |AB   |First |Note
---- |---- |---- |---- |----- |----
NAND |A    |0    |00   |A     |imp
NAND |A    |0    |00   |B     |imp
NAND |A    |0    |00   |AB    |imp
NAND |A    |0    |01   |A     |imp
NAND |A    |0    |01   |B     |imp
NAND |A    |0    |01   |AB    |imp
NAND |A    |0    |10   |A     |imp
NAND |A    |0    |10   |B     |imp
NAND |A    |0    |10   |AB    |imp
NAND |A    |0    |11   |A     |no
NAND |A    |0    |11   |B     |yes
NAND |A    |0    |11   |AB    |yes
NAND |A    |1    |00   |A     |yes
NAND |A    |1    |00   |B     |no
NAND |A    |1    |00   |AB    |yes
NAND |A    |1    |01   |A     |yes
NAND |A    |1    |01   |B     |yes
NAND |A    |1    |01   |AB    |yes
NAND |A    |1    |10   |A     |no
NAND |A    |1    |10   |B     |no
NAND |A    |1    |10   |AB    |no
NAND |A    |1    |11   |A     |imp
NAND |A    |1    |11   |B     |imp
NAND |A    |1    |11   |AB    |imp

Gate |True |Y    |AB   |First |Note
---- |---- |---- |---- |----- |----
NAND |B    |0    |00   |A     |imp
NAND |B    |0    |00   |B     |imp
NAND |B    |0    |00   |AB    |imp
NAND |B    |0    |01   |A     |imp
NAND |B    |0    |01   |B     |imp
NAND |B    |0    |01   |AB    |imp
NAND |B    |0    |10   |A     |imp
NAND |B    |0    |10   |B     |imp
NAND |B    |0    |10   |AB    |imp
NAND |B    |0    |11   |A     |yes
NAND |B    |0    |11   |B     |no 
NAND |B    |0    |11   |AB    |yes
NAND |B    |1    |00   |A     |no 
NAND |B    |1    |00   |B     |yes 
NAND |B    |1    |00   |AB    |yes
NAND |B    |1    |01   |A     |no 
NAND |B    |1    |01   |B     |no 
NAND |B    |1    |01   |AB    |no 
NAND |B    |1    |10   |A     |yes
NAND |B    |1    |10   |B     |yes
NAND |B    |1    |10   |AB    |yes
NAND |B    |1    |11   |A     |imp
NAND |B    |1    |11   |B     |imp
NAND |B    |1    |11   |AB    |imp

Gate |True |Y    |AB   |First |Note
---- |---- |---- |---- |----- |----
NOR  |A    |0    |00   |A     |imp
NOR  |A    |0    |00   |B     |imp
NOR  |A    |0    |00   |AB    |imp
NOR  |A    |0    |01   |A     |no 
NOR  |A    |0    |01   |B     |no
NOR  |A    |0    |01   |AB    |no
NOR  |A    |0    |10   |A     |yes
NOR  |A    |0    |10   |B     |yes
NOR  |A    |0    |10   |AB    |yes
NOR  |A    |0    |11   |A     |yes
NOR  |A    |0    |11   |B     |no 
NOR  |A    |0    |11   |AB    |yes
NOR  |A    |1    |00   |A     |no 
NOR  |A    |1    |00   |B     |yes
NOR  |A    |1    |00   |AB    |yes
NOR  |A    |1    |01   |A     |imp
NOR  |A    |1    |01   |B     |imp
NOR  |A    |1    |01   |AB    |imp
NOR  |A    |1    |10   |A     |imp
NOR  |A    |1    |10   |B     |imp
NOR  |A    |1    |10   |AB    |imp
NOR  |A    |1    |11   |A     |imp
NOR  |A    |1    |11   |B     |imp
NOR  |A    |1    |11   |AB    |imp

Gate |True |Y    |AB   |First |Note
---- |---- |---- |---- |----- |----
NOR  |B    |0    |00   |A     |imp
NOR  |B    |0    |00   |B     |imp
NOR  |B    |0    |00   |AB    |imp
NOR  |B    |0    |01   |A     |yes
NOR  |B    |0    |01   |B     |yes
NOR  |B    |0    |01   |AB    |yes
NOR  |B    |0    |10   |A     |no
NOR  |B    |0    |10   |B     |no
NOR  |B    |0    |10   |AB    |no
NOR  |B    |0    |11   |A     |no
NOR  |B    |0    |11   |B     |yes
NOR  |B    |0    |11   |AB    |yes
NOR  |B    |1    |00   |A     |yes
NOR  |B    |1    |00   |B     |no 
NOR  |B    |1    |00   |AB    |yes
NOR  |B    |1    |01   |A     |imp
NOR  |B    |1    |01   |B     |imp
NOR  |B    |1    |01   |AB    |imp
NOR  |B    |1    |10   |A     |imp
NOR  |B    |1    |10   |B     |imp
NOR  |B    |1    |10   |AB    |imp
NOR  |B    |1    |11   |A     |imp
NOR  |B    |1    |11   |B     |imp
NOR  |B    |1    |11   |AB    |imp

- Gate : Type of gate.
- True : Make which input pin to become true path?
- Y, AB: Assume this gate has this value.
- First: Which input pin arrive first?
- imp  : Impossible, this state does not match gate type.

- Pseudo code:

TODO: Parallelize it.

```
sensitizable_paths = vector()
values             = vector()
input_vecs         = vector()

path        = vector()
assumptions = vector()

gate  = null
slack = time_constraint

function main()
    for po in output_pins
        path.push(po)
        trace()
        path.pop()

#define ASSIGN(gate, val)
    gate->value = val
    assumptions.push(literal(gate->var, val))

#define UNASSIGN(gate)
    assumptions.pop()
    gate->value = X

#define PUSH_GATE(gate)
    path.push(gate)
    slack -= 1

#define POP_GATE()
    slack += 1
    path.pop()
    gate = path.back()

function trace()
    gate = path.back()

    if gate->min_arrival_time > slack
        return

    if slack == 0 and gate->type != PI
        return

    if gate->value == X
        assert(gate->type == PO)

        gate->value = 0
        ASSIGN(gate->from, 0)
        PUSH_GATE(gate->from)
        trace()
        POP_GATE()
        UNASSIGN(gate->from)
        gate->value = X

        gate->value = 1
        ASSIGN(gate->from, 1)
        PUSH_GATE(gate->from)
        trace()
        POP_GATE()
        UNASSIGN(gate->from)
        gate->value = X

    else if gate.type == NAND
        // Try to make gate->from_a become a true path.

        if gate->value == 1
            if gate->from_a->value == X
                ASSIGN(gate->from_a, 0)
                PUSH_GATE(gate->from_a)
                trace()
                POP_GATE()
                UNASSIGN(gate->from_a)

        else // gate->value == 0
            if gate->from_a->value == X and gate->fromm_b->value == X
                ASSIGN(gate->from_a, 1)
                ASSIGN(gate->from_b, 1)
                PUSH_GATE(gate->from_a)
                trace()
                POP_GATE()
                UNASSIGN(gate->from_b)
                UNASSIGN(gate->from_a)
            
        // Try to make gate->from_b become a true path.

        // Same logic as above, just swap "from_a" and "from_b".

    else if gate.type == NOR
        // Try to make gate->from_a become a true path.

        // Same logic as above, just swap "0" and "1".

        // Try to make gate->from_b become a true path.

        // Same logic as above, just swap "from_a" and "from_b".

    else if gate.type == NOT
        if gate.from.value == X
            ASSIGN(gate.from, !gate->value)
            PUSH_GATE(gate.from)
            trace()
            POP_GATE()
            UNASSIGN(gate.from)

    else if gate.type == PI 
        if slack < slack_constraint
            if no_conflict(assumptions)
                sensitizable_paths.push(path)

                path_value = vector()
                for g in path
                    path_value.push(g.value)
                values.push(path_value)

                input_vec = vector()
                for pi in input_pins
                    input_vec.push(pi.value)
                input_vecs.push(input_vec)
        
    else
        print("Error: Unknown gate type.\n")
        exit(1)
```

#### Time Complexity

- Calculate arrival time: O(|E|).

- Tracing: ?

- Check for confliction: ?

#### Note

- This method find all sensitizable paths too.

- To adapt parallel execution, every thread must have their own copy
  of `gate->value`. Other gate attribute can be shared.

### Method 3 (UI-Timer)

- TODO

### Method 4 (Wave)

```
gate = queue.front()
queue.pop()

if gate->type == NAND
    // 0 = falling
    // 1 = rising
    // 2 = undefined
    //
    if      (Y, A, B) == (0, 0, 0) -> bad
    else if (Y, A, B) == (0, 0, 1) -> bad
    else if (Y, A, B) == (0, 0, 2) -> bad
    else if (Y, A, B) == (0, 1, 0) -> bad
    else if (Y, A, B) == (0, 1, 1)
    else if (Y, A, B) == (0, 1, 2) -> B = 1
    else if (Y, A, B) == (0, 2, 0) -> bad
    else if (Y, A, B) == (0, 2, 1) -> A = 1
    else if (Y, A, B) == (0, 2, 2) -> A = 1, B = 1

    else if (Y, A, B) == (1, 0, 0)
    else if (Y, A, B) == (1, 0, 1)
    else if (Y, A, B) == (1, 0, 2)
    else if (Y, A, B) == (1, 1, 0)
    else if (Y, A, B) == (1, 1, 1) -> bad
    else if (Y, A, B) == (1, 1, 2) -> B = 0
    else if (Y, A, B) == (1, 2, 0)
    else if (Y, A, B) == (1, 2, 1) -> A = 0
    else if (Y, A, B) == (1, 2, 2)

    else if (Y, A, B) == (2, 0, 0) -> Y = 1
    else if (Y, A, B) == (2, 0, 1) -> Y = 1
    else if (Y, A, B) == (2, 0, 2) -> Y = 1
    else if (Y, A, B) == (2, 1, 0) -> Y = 1
    else if (Y, A, B) == (2, 1, 1) -> Y = 0
    else if (Y, A, B) == (2, 1, 2)
    else if (Y, A, B) == (2, 2, 0) -> Y = 1
    else if (Y, A, B) == (2, 2, 1)
    else if (Y, A, B) == (2, 2, 2)

    // T: True path
    // - X: No
    // - A
    // - B
    //
    // Yt, At, Bt: Arrival time known
    // - 0 = unknown
    // - 1 = known
    //
    // Q: Compare arrival time
    // - 0 = unknown
    // - 1 = Att <  Btt
    // - 2 = Att >  Btt
    // - 3 = Att == Btt
    //
    if      (T, Yt, At, Bt, Q) == (X, 0, 0, 0, 0)
    else if (T, Yt, At, Bt, Q) == (X, 0, 0, 1, 0)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 0, 0)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 1)
        if      (Y, A, B) == (0, 1, 1) -> Ytt = Btt + 1
        else if (Y, A, B) == (1, 0, 0) -> Ytt = Att + 1
        else if (Y, A, B) == (1, 0, 1) -> Ytt = Att + 1
        else if (Y, A, B) == (1, 0, 2) -> Ytt = Att + 1
        else if (Y, A, B) == (1, 1, 0) -> Ytt = Btt + 1
        else if (Y, A, B) == (1, 2, 0) 
        else if (Y, A, B) == (1, 2, 2)
        else if (Y, A, B) == (2, 1, 2) -> Ytt = Btt + 1
        else if (Y, A, B) == (2, 2, 1)
        else if (Y, A, B) == (2, 2, 2)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 2)
        if      (Y, A, B) == (0, 1, 1) -> Ytt = Att + 1
        else if (Y, A, B) == (1, 0, 0) -> Ytt = Btt + 1
        else if (Y, A, B) == (1, 0, 1) -> Ytt = Att + 1
        else if (Y, A, B) == (1, 0, 2) 
        else if (Y, A, B) == (1, 1, 0) -> Ytt = Btt + 1
        else if (Y, A, B) == (1, 2, 0) -> Ytt = Btt + 1
        else if (Y, A, B) == (1, 2, 2)
        else if (Y, A, B) == (2, 1, 2)
        else if (Y, A, B) == (2, 2, 1) -> Ytt = Att + 1
        else if (Y, A, B) == (2, 2, 2)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 3) -> Ytt = Att + 1
    else if (T, Yt, At, Bt, Q) == (X, 1, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 1) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 3) -> wont

    else if (T, Yt, At, Bt, Q) == (A, 0, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 1) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 3) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 0, 0)
        if      (Y, A, B) == (0, 1, 1) -> Att must >= Btt
            if !(Bmint <= Att) -> bad
        else if (Y, A, B) == (1, 0, 0) -> Att must <= Btt
            if !(Bmaxt >= Att) -> bad
        else if (Y, A, B) == (1, 0, 1)
        else if (Y, A, B) == (1, 0, 2) -> Att must <= Btt or B = 1
            if !(Bmaxt >= Att) -> B = 1
        else if (Y, A, B) == (1, 1, 0) -> wont
        else if (Y, A, B) == (1, 2, 0) -> wont
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 1)
        if      (Y, A, B) == (0, 1, 1) -> bad
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (1, 0, 1)
        else if (Y, A, B) == (1, 0, 2)
        else if (Y, A, B) == (1, 1, 0) -> wont
        else if (Y, A, B) == (1, 2, 0) -> wont
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 2)
        if      (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (1, 0, 0) -> bad
        else if (Y, A, B) == (1, 0, 1)
        else if (Y, A, B) == (1, 0, 2) -> B = 1
        else if (Y, A, B) == (1, 1, 0) -> wont
        else if (Y, A, B) == (1, 2, 0) -> wont
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 3)
        if      (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (1, 0, 1)
        else if (Y, A, B) == (1, 0, 2)
        else if (Y, A, B) == (1, 1, 0) -> wont
        else if (Y, A, B) == (1, 2, 0) -> wont
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont

    else if (T, Yt, At, Bt, Q) == (B, 0, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 1) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 3) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 0, 1, 0)
        if      (Y, A, B) == (0, 1, 1) -> Att must <= Btt
            if !(Amint <= Btt) -> bad
        else if (Y, A, B) == (1, 0, 0) -> Att must >= Btt
            if !(Amaxt >= Btt) -> bad
        else if (Y, A, B) == (1, 0, 1) -> wont
        else if (Y, A, B) == (1, 0, 2) -> wont
        else if (Y, A, B) == (1, 1, 0) 
        else if (Y, A, B) == (1, 2, 0) -> Att must >= Btt or A = 1
            if !(Amaxt >= Btt) -> A = 1
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 1)
        if      (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (1, 0, 0) -> bad
        else if (Y, A, B) == (1, 0, 1) -> wont
        else if (Y, A, B) == (1, 0, 2) -> wont
        else if (Y, A, B) == (1, 1, 0) 
        else if (Y, A, B) == (1, 2, 0) 
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 2)
        if      (Y, A, B) == (0, 1, 1) -> bad
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (1, 0, 1) -> wont
        else if (Y, A, B) == (1, 0, 2) -> wont
        else if (Y, A, B) == (1, 1, 0)
        else if (Y, A, B) == (1, 2, 0) -> A = 1
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 3)
        if      (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (1, 0, 1) -> wont
        else if (Y, A, B) == (1, 0, 2) -> wont
        else if (Y, A, B) == (1, 1, 0)
        else if (Y, A, B) == (1, 2, 0)
        else if (Y, A, B) == (1, 2, 2) -> wont
        else if (Y, A, B) == (2, 1, 2) -> wont
        else if (Y, A, B) == (2, 2, 1) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont

else if gate->type == NOR
    // 0 = falling
    // 1 = rising
    // 2 = undefined
    //
    if      (Y, A, B) == (0, 0, 0) -> bad
    else if (Y, A, B) == (0, 0, 1)
    else if (Y, A, B) == (0, 0, 2) -> B = 1
    else if (Y, A, B) == (0, 1, 0)
    else if (Y, A, B) == (0, 1, 1)
    else if (Y, A, B) == (0, 1, 2)
    else if (Y, A, B) == (0, 2, 0) -> A = 1
    else if (Y, A, B) == (0, 2, 1)
    else if (Y, A, B) == (0, 2, 2)

    else if (Y, A, B) == (1, 0, 0)
    else if (Y, A, B) == (1, 0, 1) -> bad
    else if (Y, A, B) == (1, 0, 2) -> B = 0
    else if (Y, A, B) == (1, 1, 0) -> bad
    else if (Y, A, B) == (1, 1, 1) -> bad
    else if (Y, A, B) == (1, 1, 2) -> bad
    else if (Y, A, B) == (1, 2, 0) -> A = 0
    else if (Y, A, B) == (1, 2, 1) -> bad
    else if (Y, A, B) == (1, 2, 2) -> A = 0, B = 0

    else if (Y, A, B) == (2, 0, 0) -> Y = 1
    else if (Y, A, B) == (2, 0, 1) -> Y = 0
    else if (Y, A, B) == (2, 0, 2)
    else if (Y, A, B) == (2, 1, 0) -> Y = 0
    else if (Y, A, B) == (2, 1, 1) -> Y = 0
    else if (Y, A, B) == (2, 1, 2) -> Y = 0
    else if (Y, A, B) == (2, 2, 0) 
    else if (Y, A, B) == (2, 2, 1) -> Y = 0
    else if (Y, A, B) == (2, 2, 2)

    // T: True path
    // - X: No
    // - A
    // - B
    //
    // Yt, At, Bt: Arrival time known
    // - 0 = unknown
    // - 1 = known
    //
    // Q: Compare arrival time
    // - 0 = unknown
    // - 1 = Att <  Btt
    // - 2 = Att >  Btt
    // - 3 = Att == Btt
    //
    if      (T, Yt, At, Bt, Q) == (X, 0, 0, 0, 0)
    else if (T, Yt, At, Bt, Q) == (X, 0, 0, 1, 0)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 0, 0)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 1)
        if      (Y, A, B) == (0, 0, 1) -> Ytt = Btt + 1
        else if (Y, A, B) == (0, 1, 0) -> Ytt = Att + 1
        else if (Y, A, B) == (0, 1, 1) -> Ytt = Att + 1
        else if (Y, A, B) == (0, 1, 2) -> Ytt = Att + 1
        else if (Y, A, B) == (0, 2, 1)
        else if (Y, A, B) == (0, 2, 2)
        else if (Y, A, B) == (1, 0, 0) -> Ytt = Btt + 1
        else if (Y, A, B) == (2, 0, 2) -> Ytt = Btt + 1
        else if (Y, A, B) == (2, 2, 0) 
        else if (Y, A, B) == (2, 2, 2)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 2)
        if      (Y, A, B) == (0, 0, 1) -> Ytt = Btt + 1
        else if (Y, A, B) == (0, 1, 0) -> Ytt = Att + 1
        else if (Y, A, B) == (0, 1, 1) -> Ytt = Btt + 1
        else if (Y, A, B) == (0, 1, 2)
        else if (Y, A, B) == (0, 2, 1) -> Ytt = Btt + 1
        else if (Y, A, B) == (0, 2, 2)
        else if (Y, A, B) == (1, 0, 0) -> Ytt = Att + 1
        else if (Y, A, B) == (2, 0, 2)
        else if (Y, A, B) == (2, 2, 0) -> Ytt = Att + 1
        else if (Y, A, B) == (2, 2, 2)
    else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 3) -> Ytt = Att + 1
    else if (T, Yt, At, Bt, Q) == (X, 1, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 1) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 3) -> wont

    else if (T, Yt, At, Bt, Q) == (A, 0, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 1) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 3) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 0, 0)
        if      (Y, A, B) == (0, 0, 1) -> wont
        else if (Y, A, B) == (0, 1, 0)
        else if (Y, A, B) == (0, 1, 1) -> Att must <= Btt
            if !(Bmaxt >= Att) -> bad
        else if (Y, A, B) == (0, 1, 2) -> Att must <= Btt or B = 0
            if !(Bmaxt >= Att) -> B = 0
        else if (Y, A, B) == (0, 2, 1) -> wont
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0) -> Att must >= Btt
            if !(Bmint <= Att) -> bad
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 1)
        if      (Y, A, B) == (0, 0, 1) -> wont
        else if (Y, A, B) == (0, 1, 0)
        else if (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (0, 1, 2)
        else if (Y, A, B) == (0, 2, 1) -> wont
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0) -> bad
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 2)
        if      (Y, A, B) == (0, 0, 1) -> wont
        else if (Y, A, B) == (0, 1, 0)
        else if (Y, A, B) == (0, 1, 1) -> bad
        else if (Y, A, B) == (0, 1, 2) -> B = 0
        else if (Y, A, B) == (0, 2, 1) -> wont
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont  
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 3)
        if      (Y, A, B) == (0, 0, 1) -> wont
        else if (Y, A, B) == (0, 1, 0)
        else if (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (0, 1, 2)
        else if (Y, A, B) == (0, 2, 1) -> wont
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont  
        else if (Y, A, B) == (2, 2, 2) -> wont

    else if (T, Yt, At, Bt, Q) == (B, 0, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 0, 1, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 1) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 3) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 0, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 0, 1, 0)
        if      (Y, A, B) == (0, 0, 1)
        else if (Y, A, B) == (0, 1, 0) -> wont
        else if (Y, A, B) == (0, 1, 1) -> Att must >= Btt
            if !(Amaxt >= Btt) -> bad
        else if (Y, A, B) == (0, 1, 2) -> wont
        else if (Y, A, B) == (0, 2, 1) -> Att must >= Btt or A = 0
            if !(Amaxt >= Btt) -> A = 0
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0) -> Att must <= Btt
            if !(Amint <= Btt) -> bad
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 0, 0) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 1)
        if      (Y, A, B) == (0, 0, 1)
        else if (Y, A, B) == (0, 1, 0) -> wont
        else if (Y, A, B) == (0, 1, 1) -> bad
        else if (Y, A, B) == (0, 1, 2) -> wont
        else if (Y, A, B) == (0, 2, 1) -> A = 0
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 2)
        if      (Y, A, B) == (0, 0, 1)
        else if (Y, A, B) == (0, 1, 0) -> wont
        else if (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (0, 1, 2) -> wont
        else if (Y, A, B) == (0, 2, 1)
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0) -> bad
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont 
        else if (Y, A, B) == (2, 2, 2) -> wont
    else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 3)
        if      (Y, A, B) == (0, 0, 1)
        else if (Y, A, B) == (0, 1, 0) -> wont
        else if (Y, A, B) == (0, 1, 1)
        else if (Y, A, B) == (0, 1, 2) -> wont
        else if (Y, A, B) == (0, 2, 1)
        else if (Y, A, B) == (0, 2, 2) -> wont
        else if (Y, A, B) == (1, 0, 0)
        else if (Y, A, B) == (2, 0, 2) -> wont
        else if (Y, A, B) == (2, 2, 0) -> wont 
        else if (Y, A, B) == (2, 2, 2) -> wont

else if gate->type == NOT
    // 0 = falling
    // 1 = rising
    // 2 = undefined
    //
    if      (Y, A) == (0, 0) -> bad
    else if (Y, A) == (0, 1)
    else if (Y, A) == (0, 2) -> A = 1
    else if (Y, A) == (1, 0)
    else if (Y, A) == (1, 1) -> bad
    else if (Y, A) == (1, 2) -> A = 0
    else if (Y, A) == (2, 0) -> Y = 1
    else if (Y, A) == (2, 1) -> Y = 0
    else if (Y, A) == (2, 2)

    // T: True path
    // - X: No
    // - A
    //
    // Yt, At: Arrival time known
    // - 0 = unknown
    // - 1 = known
    //
    if      (T, Yt, At) == (X, 0, 0)
    else if (T, Yt, At) == (X, 0, 1) -> Ytt = Att + 1
    else if (T, Yt, At) == (A, 1, 0) -> wont
    else if (T, Yt, At) == (A, 1, 1)

else if gate->type == PI
    // T: True path
    // - X: No
    // - A
    //
    // Yt: Arrival time known
    // - 0 = unknown
    // - 1 = known
    //
    if      (T, Yt) == (X, 0) -> Ytt = 0
    else if (T, Yt) == (X, 1)
    else if (T, Yt) == (A, 0) -> wont
    else if (T, Yt) == (A, 1)

else // gate->type == PO

```

