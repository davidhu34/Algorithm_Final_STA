# Ideas

## How to Find All Paths

-   Do depth first search from all input pins toward output pins.

-   Add the path to path list every time you reach an output pin.

-   Time complexity = O(2^|E|) where |E| is number of edges.

## How to Find All Paths Within Constraint

-   Do depth first search from all input pins toward output pins.

-   While doing depth first search, calculate arrival time at each
    node. If arrival time is greater than or equal to time constraint,
    stop moving down this path.

-   Calculate slack everytime you reach output pin. If slack is greater
    than or equal to slack constraint, do not add this path into path
    list.

-   Time complexity = O(2^|E|).

Is there any better method?

## How to Find Sensitizable Paths

### Method 1

-   Find all paths within constraint and collect them in a path list.
    Time complexity = O(2^|E|).

-   Try all input vectors. Time complexity = O(2^|PI|) where |PI| is
    number of input pins.

-   For each input vector, assume circuit has become stable (ignoring
    delay), then find out value of all points in the circuit. Time
    complexity = O(|N|) where |N| is number of nodes.

-   With arrival time and value at each point, we can know whether a
    path is sensitizable. For each path in path list, check whether it
    is sensitizable. Time complexity = O(2^|E| * |p|) where |p| is
    average path length.

-   Overall time complexity = O(2^|PI| * 2^|E| * |p|)

-   This method will find all sensitizable paths.

### Method 2

-   

## How to Find the Longest Sensitizable Path

### Method 1


