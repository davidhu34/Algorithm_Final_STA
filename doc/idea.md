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

- Calculate arrival time of all gates.

- Basically the idea is trace from output pins toward input pins. Try
  every possibility (condition) that make a path become a true path.
  Check whether our assumption has any contradiction.

- Monitor slack constraint and time constraint while tracing.

- Pseudo code:

TODO: Parallelize it.

```
sensitizable_paths = vector()
input_vecs = vector()

path = vector()
assumption = vector()

gate = null

function main()
    for po in output_pins
        slack = time_constrain - gate.arrival_time

        if po.arrival_time < time_constrain and slack < slack_constrain
            path.push(po)
            trace()
            path.pop()

function trace()
    gate = path.back()

    if gate.value == X
        assert(gate.type == PO)

        gate.value = 0
        gate.from.value = 0
        assumption.push(-gate.from.var)
        path.push(gate.from)
        trace()
        path.pop()
        gate = path.back()
        assumption.pop()
        gate.from.value = X
        gate.value = X

        gate.value = 1
        gate.from.value = 1
        assumption.push(gate.from.var)
        path.push(gate.from)
        trace()
        path.pop()
        gate = path.back()
        assumption.pop()
        gate.from.value = X
        gate.value = X

    else if gate.type == NAND
        # Try to make gate.from_a become a true path.

        `new_code_block(nand_make_from_a_true_path)
        if gate.from_a.arrival_time < gate.from_b.arrival_time
            if gate.from_a.value == X
                if gate.value == 1
                    gate.from_a.value = 0
                    assumption.push(-gate.from_a.var)
                    path.push(gate.from_a)
                    trace()
                    path.pop()
                    gate = path.back()
                    assumption.pop()
                    gate.from_a.value = X

        else if gate.from_a.arrival_time > gate.from_b.arrival_time
            if gate.from_b.value == X
                gate.from_b.value = 1
                assumption.push(gate.from_b.var)

                if gate.value == 1
                    gate.from_a.value = 0
                    assumption.push(-gate.from_a.var)
                else # gate.value == 0
                    gate.from_a.value = 1
                    assumption.push(gate.from_a.var)

                path.push(gate.from_a)
                trace()
                path.pop()
                gate = path.back()
                assumption.pop()
                gate.from_a.value = X
                assumption.pop()
                gate.from_b.value = X

        else # Both of them have same arrival time.
            if gate.from_a.value == X
                if gate.value == 1
                    gate.from_a.value = 0
                    assumption.push(-gate.from_a.var)
                    path.push(gate.from_a)
                    trace()
                    path.pop()
                    gate = path.back()
                    assumption.pop()
                    gate.from_a.value = X

            if gate.from_b.value == X
                gate.from_b.value = 1
                assumption.push(gate.from_b.var)

                if gate.value == 1
                    gate.from_a.value = 0
                    assumption.push(-gate.from_a.var)
                else # gate.value == 0
                    gate.from_a.value = 1
                    assumption.push(gate.from_a.var)

                path.push(gate.from_a)
                trace()
                path.pop()
                gate = path.back()
                assumption.pop()
                gate.from_a.value = X
                assumption.pop()
                gate.from_b.value = X
        `end_code_block(basic_logic)

        # Try to make gate.from_b become a true path.

        `print(swap("from_a", "from_b", nand_make_from_a_true_path))

    else if gate.type == NOR
        # Try to make gate.from_a become a true path.

        `new_code_block(nor_make_from_a_true_path)
        `print(swap("assumption.push(-gate", "assumption.push(gate", \
               swap("0", "1", basic_logic)))
        `end_code_block(nor_make_from_a_true_path)

        # Try to make gate.from_b become a true path.

        `print(swap("from_a", "from_b", nor_make_from_a_true_path))

    else if gate.type == NOT
        if gate.from.value == X
            if gate.value == 1
                gate.from.value = 0
                assumption.push(-gate.from.var)
            else # gate.value == 0
                gate.from.value = 1
                assumption.push(gate.from.var)

            path.push(gate.from)
            trace()
            path.pop()
            gate = path.back()
            assumption.pop()
            gate.from.value = X

    else if gate.type == PI and no_conflict(assumption)
        sensitizable_paths.push(reverse(path))
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
  of `gate.value`. Other gate attribute can be shared.

### Method 3 (UI-Timer)

- TODO


