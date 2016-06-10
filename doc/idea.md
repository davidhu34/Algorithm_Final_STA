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

```
function calculate_max_arrival_time(cir)
    for gate in cir.primary_inputs
        gate.max_arrival_time = 0

    for gate in cir.primary_outputs
        // tag = number of input that has arrived.
        gate.tag = 0

    for gate in cir.logic_gates
        gate.tag = 0

    // For all gate inside queue, all of its fan-in has arrived.
    // Every gate inside queue has already known arrival time.
    // This queue should be sorted according to arrival time.
    //
    q = queue(cir.primary_inputs)

    while q is not empty
        gate = q.front()
        q.pop()

        for fanout in gate.tos
            fanout.tag += 1
            if fanout.tag == fanout.froms.size
                if fanout.type == PO
                    fanout.max_arrival_time = gate.max_arrival_time

                else
                    fanout.max_arrival_time = gate.max_arrival_time + 1
                    q.push(fanout)
```

#### Time Complexity

- O(|E|).

## How to Calculate Min Arrival Time of All Gates

```
function calculate_min_arrival_time(cir)
    for gate in cir.primary_inputs
        gate.min_arrival_time = 0

    for gate in cir.primary_outputs
        // tag = 0 means not touched.
        gate.tag = 0

    for gate in cir.logic_gates
        gate.tag = 0

    // For all gate inside queue, at least 1 of its fan-in has arrived.
    // Every gate inside queue has already known min arrival time.
    // This queue should be sorted according to min arrival time.
    //
    q = queue(cir.primary_inputs)

    while q is not empty
        gate = q.front()
        q.pop()

        for fanout in gate.tos
            if fanout.tag == 0
                fanout.tag = 1

                if fanout.type == PO
                    fanout.min_arrival_time = gate.min_arrival_time

                else
                    fanout.min_arrival_time = gate.min_arrival_time + 1
                    q.push(fanout)
```

## How to Calculate Value of All Points

```
// Assume all PI has been assigned a value.
// Assume all other gates has value of X.
//
function calculate_value_and_arrival_time(cir)
    for gate in cir.primary_inputs
        gate.arrival_time = 0

    for gate in cir.primary_outputs
        gate.arrival_time = UNKNOWN

    for gate in cir.logic_gates
        gate.arrival_time = UNKNOWN

    // This is the front wave. Every gate inside queue has known 
    // arrival time. This queue should be sorted according to
    // arrival time.
    //
    q = queue(cir.primary_inputs)

    while q is not empty
        gate = q.front()
        q.pop()

        // Let all of gate's fan-out that does not in queue nor
        // visited check their inputs' value and arrival time. If
        // a fan-out decide that it knows its value and arrival
        // time now, add it into queue.
        //
        for fanout in gate.tos
            if fanout.value == X
                if fanout.type == NAND

                #define FANOUT_CHECK_ITS_INPUT(v0, v1)
                    if gate.value == v0
                        fanout.value = v1
                        fanout.arrival_time = gate.arrival_time + 1
                        q.push(fanout)
                    else
                        if fanout.from_a.value == v1 and
                           fanout.from_b.value == v1
                            fanout.value = v0
                            fanout.arrival_time = gate.arrival_time + 1
                            q.push(fanout)

                    FANOUT_CHECK_ITS_INPUT(0, 1)

                else if fanout.type == NOR
                    FANOUT_CHECK_ITS_INPUT(1, 0)

                else if fanout.type == NOT
                    fanout.value = !gate.value
                    fanout.arrival_time = gate.arrival_time + 1
                    q.push(fanout)

                else 
                    assert(fanout.type == PO)
                    fanout.value = gate.value
                    fanout.arrival_time = gate.arrival_time
```


#### Time Complexity

- O(|E|).

## How to Find Sensitizable Paths

### Method 1: Brute Force

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

### Method 2: Backtracking

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

path          = vector()
path_value    = vector()
subpath       = vector()
subpath_value = vector()

gate  = null
slack = null
cir   = null

function find_true_paths(_cir, time_constraint, slack_constraint)
    slack = time_constraint.copy()

    calculate_max_arrival_time(cir)
    calculate_min_arrival_time(cir)
    reset_gate_value_to_undef(cir)
    reset_arrival_time_to_unknown(cir)

    cir = _cir

    for po in cir.primary_outputs
        path.push(po)
        trace()
        path.pop()

    return tuple(sensitizable_paths, values, input_vecs)

#define PUSH_PATH(gate)
    path.push(gate)
    path_value.push(gate.value.copy())
    slack -= 1

#define POP_PATH()
    slack += 1
    path_value.pop()
    path.pop()
    gate = path.back()

#define PUSH_SUBPATH(gate)
    subpath.push(gate)
    subpath_value.push(gate.value.copy())

#define POP_PATH()
    subpath_value.pop()
    subpath.pop()

function trace()
    gate = path.back()

    if gate.min_arrival_time > slack
        return

    if slack == 0 and gate.type != PI
        return

    if gate.value == X
        assert(gate.type == PO)

        gate.value = 0
        gate.from.value = 0
        PUSH_PATH(gate.from)
        trace()
        POP_PATH()
        gate.from.value = X
        gate.value = X

        gate.value = 1
        gate.from.value = 1
        PUSH_PATH(gate.from)
        trace()
        POP_PATH()
        gate.from.value = X
        gate.value = X

    else if gate.type == NAND
        // Try to make gate.from_a become a true path.

        if gate.value == 1
            if gate.from_a.value == X
                gate.from_a.value = 0
                PUSH_PATH(gate.from_a)
                trace()
                POP_PATH()
                gate.from_a.value = X

        else // gate.value == 0
            if gate.from_a.value == X and gate.from_b.value == X
                gate.from_a.value = 1
                gate.from_b.value = 1
                PUSH_SUBPATH(gate.from_b)
                PUSH_PATH(gate.from_a)
                trace()
                POP_PATH()
                POP_SUBPATH()
                gate.from_b.value = X
                gate.from_a.value = X
            
        // Try to make gate.from_b become a true path.

        // Same logic as above, just swap "from_a" and "from_b".

    else if gate.type == NOR
        // Try to make gate.from_a become a true path.

        // Same logic as above, just swap "0" and "1".

        // Try to make gate.from_b become a true path.

        // Same logic as above, just swap "from_a" and "from_b".

    else if gate.type == NOT
        if gate.from.value == X
            gate.from.value = !gate.value
            PUSH_PATH(gate.from)
            trace()
            POP_PATH()
            gate.from.value = X

    else if gate.type == PI 
        if slack < slack_constraint
            if no_conflict(cir, path, subpath)
                sensitizable_paths.push(path)
                values.push(path_value)

                input_vec = vector()
                for pi in cir.primary_inputs
                    input_vec.push(pi.value)
                input_vecs.push(input_vec)

            // Restore cir to state before passing to no_conflict()
            reset_gate_value_to_undef(cir)
            reset_arrival_time_to_unknown(cir)
            
            for i = 0 to path.size - 1
                path[i].value = path_value[i].copy()
            for i = 0 to subpath.size - 1
                subpath[i].value = subpath_value[i].copy()

    else
        print("Error: Unknown gate type.\n")
        exit(1)
```

#### Time Complexity

- Calculate arrival time: O(|E|).

- Tracing: ?

- Check for confliction: ?

### Method 3: UI-Timer

- TODO

## How to Check For Confliction?

### Method 1: SAT Solver

- TODO

#### Time Complexity

### Method 2: Propagation

```
function no_conflict(cir, path, subpath)
    // tag = 0 if not in queue.
    // tag = 1 if in queue.
    //
    for gate in cir.all_gates
        gate.tag = 0

    for i = 1 to path.size
        path[i].arrival_time = path.size - i - 1
    
    q = queue()

    for gate in subpath
        gate.tag = 1
        q.push(gate)

    for gate in cir.primary_inputs
        if gate.tag == 0
            gate.tag = 1
            q.push(gate)

    while q is not empty
        gate = q.front()
        q.pop()
        gate.tag = 0

        if gate.type == NAND
            // 0 = falling
            // 1 = rising
            // 2 = undefined
            //
            if      (Y, A, B) == (0, 0, 0) then return false
            else if (Y, A, B) == (0, 0, 1) then return false
            else if (Y, A, B) == (0, 0, 2) then return false
            else if (Y, A, B) == (0, 1, 0) then return false
            else if (Y, A, B) == (0, 1, 1)
            else if (Y, A, B) == (0, 1, 2) then B = 1
            else if (Y, A, B) == (0, 2, 0) then return false
            else if (Y, A, B) == (0, 2, 1) then A = 1
            else if (Y, A, B) == (0, 2, 2) then A = 1, B = 1

            else if (Y, A, B) == (1, 0, 0)
            else if (Y, A, B) == (1, 0, 1)
            else if (Y, A, B) == (1, 0, 2)
            else if (Y, A, B) == (1, 1, 0)
            else if (Y, A, B) == (1, 1, 1) then return false
            else if (Y, A, B) == (1, 1, 2) then B = 0
            else if (Y, A, B) == (1, 2, 0)
            else if (Y, A, B) == (1, 2, 1) then A = 0
            else if (Y, A, B) == (1, 2, 2)

            else if (Y, A, B) == (2, 0, 0) then Y = 1
            else if (Y, A, B) == (2, 0, 1) then Y = 1
            else if (Y, A, B) == (2, 0, 2) then Y = 1
            else if (Y, A, B) == (2, 1, 0) then Y = 1
            else if (Y, A, B) == (2, 1, 1) then Y = 0
            else if (Y, A, B) == (2, 1, 2)
            else if (Y, A, B) == (2, 2, 0) then Y = 1
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
                if      (Y, A, B) == (0, 1, 1) then Ytt = Btt + 1
                else if (Y, A, B) == (1, 0, 0) then Ytt = Att + 1
                else if (Y, A, B) == (1, 0, 1) then Ytt = Att + 1
                else if (Y, A, B) == (1, 0, 2) then Ytt = Att + 1
                else if (Y, A, B) == (1, 1, 0) then Ytt = Btt + 1
                else if (Y, A, B) == (1, 2, 0) 
                else if (Y, A, B) == (1, 2, 2)
                else if (Y, A, B) == (2, 1, 2) then Ytt = Btt + 1
                else if (Y, A, B) == (2, 2, 1)
                else if (Y, A, B) == (2, 2, 2)
            else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 2)
                if      (Y, A, B) == (0, 1, 1) then Ytt = Att + 1
                else if (Y, A, B) == (1, 0, 0) then Ytt = Btt + 1
                else if (Y, A, B) == (1, 0, 1) then Ytt = Att + 1
                else if (Y, A, B) == (1, 0, 2) 
                else if (Y, A, B) == (1, 1, 0) then Ytt = Btt + 1
                else if (Y, A, B) == (1, 2, 0) then Ytt = Btt + 1
                else if (Y, A, B) == (1, 2, 2)
                else if (Y, A, B) == (2, 1, 2)
                else if (Y, A, B) == (2, 2, 1) then Ytt = Att + 1
                else if (Y, A, B) == (2, 2, 2)
            else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 3) then Ytt = Att + 1
            else if (T, Yt, At, Bt, Q) == (X, 1, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 1) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 3) then assertion failed

            else if (T, Yt, At, Bt, Q) == (A, 0, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 1) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 3) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 0, 0)
                if      (Y, A, B) == (0, 1, 1) then Att must >= Btt
                    if !(Bmint <= Att) then return false
                else if (Y, A, B) == (1, 0, 0) then Att must <= Btt
                    if !(Bmaxt >= Att) then return false
                else if (Y, A, B) == (1, 0, 1)
                else if (Y, A, B) == (1, 0, 2) then Att must <= Btt or B = 1
                    if !(Bmaxt >= Att) then B = 1
                else if (Y, A, B) == (1, 1, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 1)
                if      (Y, A, B) == (0, 1, 1) then return false
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (1, 0, 1)
                else if (Y, A, B) == (1, 0, 2)
                else if (Y, A, B) == (1, 1, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 2)
                if      (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (1, 0, 0) then return false
                else if (Y, A, B) == (1, 0, 1)
                else if (Y, A, B) == (1, 0, 2) then B = 1
                else if (Y, A, B) == (1, 1, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 3)
                if      (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (1, 0, 1)
                else if (Y, A, B) == (1, 0, 2)
                else if (Y, A, B) == (1, 1, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 0) then assertion failed
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed

            else if (T, Yt, At, Bt, Q) == (B, 0, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 1) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 3) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 0, 1, 0)
                if      (Y, A, B) == (0, 1, 1) then Att must <= Btt
                    if !(Amint <= Btt) then return false
                else if (Y, A, B) == (1, 0, 0) then Att must >= Btt
                    if !(Amaxt >= Btt) then return false
                else if (Y, A, B) == (1, 0, 1) then assertion failed
                else if (Y, A, B) == (1, 0, 2) then assertion failed
                else if (Y, A, B) == (1, 1, 0) 
                else if (Y, A, B) == (1, 2, 0) then Att must >= Btt or A = 1
                    if !(Amaxt >= Btt) then A = 1
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 1)
                if      (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (1, 0, 0) then return false
                else if (Y, A, B) == (1, 0, 1) then assertion failed
                else if (Y, A, B) == (1, 0, 2) then assertion failed
                else if (Y, A, B) == (1, 1, 0) 
                else if (Y, A, B) == (1, 2, 0) 
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 2)
                if      (Y, A, B) == (0, 1, 1) then return false
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (1, 0, 1) then assertion failed
                else if (Y, A, B) == (1, 0, 2) then assertion failed
                else if (Y, A, B) == (1, 1, 0)
                else if (Y, A, B) == (1, 2, 0) then A = 1
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 3)
                if      (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (1, 0, 1) then assertion failed
                else if (Y, A, B) == (1, 0, 2) then assertion failed
                else if (Y, A, B) == (1, 1, 0)
                else if (Y, A, B) == (1, 2, 0)
                else if (Y, A, B) == (1, 2, 2) then assertion failed
                else if (Y, A, B) == (2, 1, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 1) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed

        else if gate.type == NOR
            // 0 = falling
            // 1 = rising
            // 2 = undefined
            //
            if      (Y, A, B) == (0, 0, 0) then return false
            else if (Y, A, B) == (0, 0, 1)
            else if (Y, A, B) == (0, 0, 2) then B = 1
            else if (Y, A, B) == (0, 1, 0)
            else if (Y, A, B) == (0, 1, 1)
            else if (Y, A, B) == (0, 1, 2)
            else if (Y, A, B) == (0, 2, 0) then A = 1
            else if (Y, A, B) == (0, 2, 1)
            else if (Y, A, B) == (0, 2, 2)

            else if (Y, A, B) == (1, 0, 0)
            else if (Y, A, B) == (1, 0, 1) then return false
            else if (Y, A, B) == (1, 0, 2) then B = 0
            else if (Y, A, B) == (1, 1, 0) then return false
            else if (Y, A, B) == (1, 1, 1) then return false
            else if (Y, A, B) == (1, 1, 2) then return false
            else if (Y, A, B) == (1, 2, 0) then A = 0
            else if (Y, A, B) == (1, 2, 1) then return false
            else if (Y, A, B) == (1, 2, 2) then A = 0, B = 0

            else if (Y, A, B) == (2, 0, 0) then Y = 1
            else if (Y, A, B) == (2, 0, 1) then Y = 0
            else if (Y, A, B) == (2, 0, 2)
            else if (Y, A, B) == (2, 1, 0) then Y = 0
            else if (Y, A, B) == (2, 1, 1) then Y = 0
            else if (Y, A, B) == (2, 1, 2) then Y = 0
            else if (Y, A, B) == (2, 2, 0) 
            else if (Y, A, B) == (2, 2, 1) then Y = 0
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
                if      (Y, A, B) == (0, 0, 1) then Ytt = Btt + 1
                else if (Y, A, B) == (0, 1, 0) then Ytt = Att + 1
                else if (Y, A, B) == (0, 1, 1) then Ytt = Att + 1
                else if (Y, A, B) == (0, 1, 2) then Ytt = Att + 1
                else if (Y, A, B) == (0, 2, 1)
                else if (Y, A, B) == (0, 2, 2)
                else if (Y, A, B) == (1, 0, 0) then Ytt = Btt + 1
                else if (Y, A, B) == (2, 0, 2) then Ytt = Btt + 1
                else if (Y, A, B) == (2, 2, 0) 
                else if (Y, A, B) == (2, 2, 2)
            else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 2)
                if      (Y, A, B) == (0, 0, 1) then Ytt = Btt + 1
                else if (Y, A, B) == (0, 1, 0) then Ytt = Att + 1
                else if (Y, A, B) == (0, 1, 1) then Ytt = Btt + 1
                else if (Y, A, B) == (0, 1, 2)
                else if (Y, A, B) == (0, 2, 1) then Ytt = Btt + 1
                else if (Y, A, B) == (0, 2, 2)
                else if (Y, A, B) == (1, 0, 0) then Ytt = Att + 1
                else if (Y, A, B) == (2, 0, 2)
                else if (Y, A, B) == (2, 2, 0) then Ytt = Att + 1
                else if (Y, A, B) == (2, 2, 2)
            else if (T, Yt, At, Bt, Q) == (X, 0, 1, 1, 3) then Ytt = Att + 1
            else if (T, Yt, At, Bt, Q) == (X, 1, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 1) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (X, 1, 1, 1, 3) then assertion failed

            else if (T, Yt, At, Bt, Q) == (A, 0, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 1) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 0, 1, 1, 3) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 0, 0)
                if      (Y, A, B) == (0, 0, 1) then assertion failed
                else if (Y, A, B) == (0, 1, 0)
                else if (Y, A, B) == (0, 1, 1) then Att must <= Btt
                    if !(Bmaxt >= Att) then return false
                else if (Y, A, B) == (0, 1, 2) then Att must <= Btt or B = 0
                    if !(Bmaxt >= Att) then B = 0
                else if (Y, A, B) == (0, 2, 1) then assertion failed
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0) then Att must >= Btt
                    if !(Bmint <= Att) then return false
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 1)
                if      (Y, A, B) == (0, 0, 1) then assertion failed
                else if (Y, A, B) == (0, 1, 0)
                else if (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (0, 1, 2)
                else if (Y, A, B) == (0, 2, 1) then assertion failed
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0) then return false
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 2)
                if      (Y, A, B) == (0, 0, 1) then assertion failed
                else if (Y, A, B) == (0, 1, 0)
                else if (Y, A, B) == (0, 1, 1) then return false
                else if (Y, A, B) == (0, 1, 2) then B = 0
                else if (Y, A, B) == (0, 2, 1) then assertion failed
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed  
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (A, 1, 1, 1, 3)
                if      (Y, A, B) == (0, 0, 1) then assertion failed
                else if (Y, A, B) == (0, 1, 0)
                else if (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (0, 1, 2)
                else if (Y, A, B) == (0, 2, 1) then assertion failed
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed  
                else if (Y, A, B) == (2, 2, 2) then assertion failed

            else if (T, Yt, At, Bt, Q) == (B, 0, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 0, 1, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 1) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 0, 1, 1, 3) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 0, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 0, 1, 0)
                if      (Y, A, B) == (0, 0, 1)
                else if (Y, A, B) == (0, 1, 0) then assertion failed
                else if (Y, A, B) == (0, 1, 1) then Att must >= Btt
                    if !(Amaxt >= Btt) then return false
                else if (Y, A, B) == (0, 1, 2) then assertion failed
                else if (Y, A, B) == (0, 2, 1) then Att must >= Btt or A = 0
                    if !(Amaxt >= Btt) then A = 0
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0) then Att must <= Btt
                    if !(Amint <= Btt) then return false
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 0, 0) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 1)
                if      (Y, A, B) == (0, 0, 1)
                else if (Y, A, B) == (0, 1, 0) then assertion failed
                else if (Y, A, B) == (0, 1, 1) then return false
                else if (Y, A, B) == (0, 1, 2) then assertion failed
                else if (Y, A, B) == (0, 2, 1) then A = 0
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 2)
                if      (Y, A, B) == (0, 0, 1)
                else if (Y, A, B) == (0, 1, 0) then assertion failed
                else if (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (0, 1, 2) then assertion failed
                else if (Y, A, B) == (0, 2, 1)
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0) then return false
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed 
                else if (Y, A, B) == (2, 2, 2) then assertion failed
            else if (T, Yt, At, Bt, Q) == (B, 1, 1, 1, 3)
                if      (Y, A, B) == (0, 0, 1)
                else if (Y, A, B) == (0, 1, 0) then assertion failed
                else if (Y, A, B) == (0, 1, 1)
                else if (Y, A, B) == (0, 1, 2) then assertion failed
                else if (Y, A, B) == (0, 2, 1)
                else if (Y, A, B) == (0, 2, 2) then assertion failed
                else if (Y, A, B) == (1, 0, 0)
                else if (Y, A, B) == (2, 0, 2) then assertion failed
                else if (Y, A, B) == (2, 2, 0) then assertion failed 
                else if (Y, A, B) == (2, 2, 2) then assertion failed

        else if gate.type == NOT
            // 0 = falling
            // 1 = rising
            // 2 = undefined
            //
            if      (Y, A) == (0, 0) then return false
            else if (Y, A) == (0, 1)
            else if (Y, A) == (0, 2) then A = 1
            else if (Y, A) == (1, 0)
            else if (Y, A) == (1, 1) then return false
            else if (Y, A) == (1, 2) then A = 0
            else if (Y, A) == (2, 0) then Y = 1
            else if (Y, A) == (2, 1) then Y = 0
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
            else if (T, Yt, At) == (X, 0, 1) then Ytt = Att + 1
            else if (T, Yt, At) == (A, 1, 0) then assertion failed
            else if (T, Yt, At) == (A, 1, 1)

        else if gate.type == PI
            // T: True path
            // - X: No
            // - A
            //
            // Yt: Arrival time known
            // - 0 = unknown
            // - 1 = known
            //
            if      (T, Yt) == (X, 0) then Ytt = 0
            else if (T, Yt) == (X, 1)
            else if (T, Yt) == (A, 0) then assertion failed
            else if (T, Yt) == (A, 1)

        else // gate.type == PO
            Ytt = Att
            Y = A
    
    for pi in cir.primary_inputs
        if pi.value == X
            pi.value = 0

    return true
```

#### Time Complexity

- ?

