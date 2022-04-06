# Algorithme-DPLL


<h1>What is DPLL?</h1>

The **Davis-Putnam-Logemann-Loveland** algorithm is an algorithm for deciding the satisfiablility of propositional logic formulae. Although the SAT problem is NP-complete, it is stxill remarkably amenable to automation, and modern high-performance SAT-solvers are regularly used in software verification, constraint solving and optimization.

<h2>&nbsp; description du projet</h2>

un programme dans le langage de **C** si un ensemble de clauses est satisfaisable ou non à l’aide de l’algorithme dpll.
Les clauses seront codées dans un fichier texte contenant une ligne par clause. Pour faciliter la lecture des données, la première ligne du fichier
contiendra 2 entiers, le premier désignera le nombre de variables, le second le nombre de clauses. Chaque clause sera codée par une séquence d’entiers
relatifs non-nuls séparés par un espace et se terminant par la valeur 0. Si la valeur est positive, il s’agira du littéral positif, sinon du littéral négatif.

<h2>&nbsp; description of project</h2>

a program in the language of **C** whether a set of clauses is satisfiable or not using the dpll algorithm. The clauses will be coded in a text file containing one line per clause. To facilitate the reading of the data, the first line of the file will contain 2 integers, the first will designate the number of variables, the second the number of clauses. Each clause will be encoded by a sequence of non-zero integers separated by a space and ending with the value 0. If the value is positive, it will be the positive literal, otherwise the negative literal.


<h3>&nbsp; backtracking + unit propagation + pure literal rule</h3>

- in general, to find a set of values satisfying some conditions:
- set a variable to each possible value in turn
- for each value, recursively repeat

<h3>&nbsp; Backtracking for satisfiability</h3>

find values of x1, ..., xn satisfying the formula F

- algorithm:
  - choose a variable xi
  - check satisfiability of F + (xi=true)
  - check satisfiability of F + (xi=false)

<h3>&nbsp; Satisfiability: recursive calls</h3>

the two recursive calls are: "check satisfiability of F + (xi=value)"

in general: check satisfiability when some variables already have a value

partial interpretation = assigns true/false to some variables

<h3>&nbsp;Backtracking with partial interpretation </h3>

- algorithm (some parts missing):

- boolean sat(formula F, partial_interpretation I)

  1.choose xi that I does not assign
  
  2.return sat(F, I ∪ { xi=true }) or sat(F, I ∪ { xi=false })
  
  3.satisfiability of F = satisfiability of F with I=∅
missing: base case of recursion, choice of xi

<h3>&nbsp;Base case </h3>

recursion adds a xi=value to I
at some point, all variables are assigned
we can now check whether F is true or false
but:
sometimes, we can check whether F is true or false even if some variables are still unassigned

<h3>&nbsp;Value of formulae under partial interpretations </h3>

in the formula F:

-replace each xi that is assigned in I with its truth value
(e.g. if I contains xi=true replace each occurrence of xi with true)
-simplify using rules:
  -something ∧ true = something
  -something ∧ false = false
  -something ∨ true = true
  -something ∨ false = something
result could be:

  1.true
  
  2.false
  
  3.some formula containing only unassigned variables
in the first two cases, the formula has a value that does not depend on the unassigned variables
