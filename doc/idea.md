# Ideas

## How to Find All Paths

-   Do depth first search from all input pins toward output pins.

-   Add the path to path list every time you reach an output pin.

#### Time Complexity

-   Time complexity of doing depth first search on a single input pin
    is O(|E|) where |E| is number of edges of entire circuit. This is
    the worst case.

-   Time complexity of doing depth first search on all input pins is
    O(|PI| * |E|) where |PI| is number of input pins.

## How to Find All Paths Within Constraint

-   Do depth first search from all input pins toward output pins.

-   While doing depth first search, calculate arrival time at each
    node. If arrival time is greater than or equal to time constraint,
    stop moving down this path.

-   Calculate slack everytime you reach output pin. If slack is greater
    than or equal to slack constraint, do not add this path into path
    list.

#### Time Complexity

-   Time complexity is same as depth first search, i.e. O(|PI| * |E|).

## How to Find Out Value of All Points

Given an input vector and a set of nodes sorted according to each node's
arrival time, how to find out value of all nodes.

-   For each node in the sorted set, get its input nodes' value, then
    calculate its value.

#### Time Complexity

-   Since each node need to access its input nodes, the overall time
    complexity should be O(|E|).

## How to Find Sensitizable Paths

### Method 1 (Brute force)

-   Find all paths within constraint and collect them in a path list.

-   Try all possible permutation of input vectors.

-   For each input vector, assume circuit has become stable (ignoring
    delay), then find out value of all points in the circuit.

-   With arrival time and value at each point, we can know whether a
    path is sensitizable. For each path in path list, check whether it
    is sensitizable.

#### Time Complexity

-   Find all paths within constaint: O(|PI| * |E|).

-   Try all possible permutation: O(2^|PI|).

-   Find out value of all points for an input vector: O(|E|).

-   Check all paths in path list: O(|P| * |p|) where |P| is number of
    paths and |p| is average number of node in a path.

-   Overall time complexity is O(|PI| * |E|) + O(2^|PI| * |E|) + 
    O(|P| * |p|), and O(2^|PI| * |E|) will probably dominate.

#### Note

-   This method will find all sensitizable paths.

### Method 2

-   

## How to Find the Longest Sensitizable Path

### Method 1


