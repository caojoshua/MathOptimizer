### Design

MathOptimizer is designed like any other language processor, consisting of a scanner, parser, optimizer, and code generator.

## Scanner

The [scanner](https://github.com/caojoshua/MathOptimizer/blob/main/src/Scanner.cpp) is like any other plain 'ol scanner. It takes the math expression character stream as input, and outputs a token stream. I chose not to use any scanner libraries like GNU flex because I can learn more from implementing from scratch.

## Parser

The [parser](https://github.com/caojoshua/MathOptimizer/blob/main/src/Parser.cpp) is a top-down LL(1) parser, also implemented from scratch for learning purposes. The parser takes a token stream as input, and outputs a AST.

I also implemented a bottom-up LR(1) parser in a [separate branch](https://github.com/caojoshua/MathOptimizer/tree/bottomup/src) as a learning experience. I am keeping it out of the main branch because it is. In practice, I would use a parsing library to help build a finite state automata implementation.

## Optimizer

The [optimizer](https://github.com/caojoshua/MathOptimizer/blob/bottomup/src/Optimizer.cpp) is the most interesting, non-trivial part of the project, and contributes to over half of the lines of c++ code. This pass takes in the AST as input, and outputs a logically equivalent, simpliifed AST. Starting from the leaves, the optimizer runs the following passes on each node:

### MergeWithChildren
MergeWithChildren merges OperatorNode parameters with child node parameters if they have the same precedence. For example, `(x+y)+z` -> `x+y+z`. This is required for folding passes.

### SortNodes
SortNodes 'sorts' OperatorNode child parameters. The sort is arbitray and is only used to create consistent ordering of parameters. The ordering helps with the FoldTerms pass.

### FoldConstants
FoldCostants folds OperatorNode child constant nodes together into one constant node. For example, `1+x+2+y+3` -> `6+x+y`.

### FoldTerms
FoldTerms folds OperatorNode terms. For example, `2*x*y+3*y*x` -> `5*x*y`. To compare terms for the same identifiers, we take advantage of the SortNodes pass and linearly scan each OperatorNode child. Without sort nodes, we would need to check each term against each other, resulting in a O(n^2) operation. SortNodes is only a O(n log n) operation

### FoldIdenties
Finally, we fold identity rules:
`x(+|-)0` -> `x`
`x(*|/)1` -> `x`
`x*0` -> `0`

## CodeGen

CodeGen is implemented as [Node](https://github.com/caojoshua/MathOptimizer/blob/bottomup/src/Optimizer.cpp) member functions. Number nodes output their number, and Identifier nodes output their identifier. OperatorNodes CodeGen all their child nodes, and adds parenthesis around Sum nodes if they are deeper than a Product node.
