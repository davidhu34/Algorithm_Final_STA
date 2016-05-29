# How to Use MiniSat

## Boolean Satisfiability Problem

<dl>

<dt>Boolean Satisfiability Problem</dt>
    <dd><p>Its abbreviation is SAT. It asks, given a boolean formula, is
    there a way to assign value to variables of that formula,
    so that the formula evaluate to 1. It is a NP-complete problem.
    </p></dd>

<dt>Satisfiable</dt>
<dd><p>If there is a way to assign value to variables of a formula,
    such that the formula evaluate to 1, then the formula is
    satisfiable. E.g. "A & ~B" is satisfiable, since when we assign
    (A, B) = (1, 0), that formula evaluate to 1.</p></dd>

<dt>Unsatisfiable</dt>
<dd><p>If there is no way to assign value to variables of a formula,
    such that the formula evaluate to 1, then the formula is
    unsatisfiable. E.g. "A & ~A" is not satisfiable.</p></dd>

<dt>Boolean Formula</dt>
<dd><p>It is build from variables, operator AND, operator OR, operator
    NOT, and parenthesis.</p></dd>

<dt>Conjunction</dt>
<dd><p>It is synonym of AND.</p></dd>

<dt>Disjunction</dt>
<dd><p>It is synonym of OR.</p></dd>

<dt>Negation</dt>
<dd><p>It is synonym of NOT.</p></dd>

<dt>Literal</dt>
<dd><p>It is either a variable (then called positive literal), or
    the negation of a variable (then called negative literal).</dd></p>
