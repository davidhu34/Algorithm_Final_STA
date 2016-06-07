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

## How to Calculate Arrival Time of All Gates

- From input pins, do breadth first traverse toward output pins. Add
  a node to queue only if its fan-in has all arrived. Its arrival time
  is known if it can be added into queue.

#### Time Complexity

- O(|E|).

## How to Calculate Value of All Points

- Same process as calculating arrival time of all gates.

#### Time Complexity

- O(|E|).

## How to Find Sensitizable Paths

### Method 1 (Brute Force)

- Find all paths within constraint and collect them in a path list.

- Try all possible permutation of input vectors.

- For each input vector, assume circuit has become stable (ignoring
  delay), then find value of all points in the circuit.

- With arrival time and value at each point, we can know whether a
  path is sensitizable. For each path in path list, check whether it
  is sensitizable.

#### Time Complexity

- Find all paths within constaint: ?

- Try all possible permutation: O(2^|PI|).

- Find value of all points for an input vector: O(|E|).

- Check all paths in path list: O(|P| * |p|) where |P| is number of
  paths and |p| is average number of node in a path.

- Overall time complexity is ? + O(2^|PI| * |E|) + O(|P| * |p|), and 
  O(2^|PI| * |E|) will probably dominate.

#### Note

- This method will find all sensitizable paths.

### Method 2 (Backtracking)

- Calculate min arrival time of all gates.

- Basically the idea is trace from output pins toward input pins. Try
  every possibility (condition) that make a path become a true path.
  Check whether our assumptions has any contradiction.

- Monitor slack constraint and time constraint while tracing.

How many state do we need to check?

Assume that, given this gate's value, for each true path that pass
through this gate, the input of that true path can only take one
value (if it can take 1, then it cannot take 0; if it can take 0,
then it cannot take 1).

Gate |True |This |First |Prev |Needed?
---- |---- |---- |----- |---- |-------
NAND |A    |1    |A     |0X   |
NAND |A    |1    |B     |01   |sup
NAND |A    |1    |AB    |0X   |sup
NAND |A    |1    |AB    |01   |sup
NAND |A    |0    |A     |0X   |imp
NAND |A    |0    |B     |11   |
NAND |A    |0    |AB    |0X   |imp
NAND |A    |0    |AB    |11   |sup
NOR  |A    |1    |A     |1X   |imp
NOR  |A    |1    |B     |00   |
NOR  |A    |1    |AB    |1X   |imp
NOR  |A    |1    |AB    |00   |sup
NOR  |A    |0    |A     |1X   |
NOR  |A    |0    |B     |10   |sup
NOR  |A    |0    |AB    |1X   |sup
NOR  |A    |0    |AB    |10   |sup

- Gate : Type of gate.
- True : Make which input pin to become true path?
- This : Assume this gate has this value.
- First: Which input pin arrive first?
- Prev : What value should each input pin has if I want to make
         specified pin to become a true path? 1X means A = 1 and
         B can be any value.
- sup  : Superfluous, the state of this Prev is checked in other row.
- imp  : Impossible, the state of this Prev is impossible for given
         value of This.

So, we need to check 4 states for NAND gate, (2 to make pin A become
true path, another 2 to make pin B become true path), 4 states for
NOR gate, and 2 states for NOT gate.

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


