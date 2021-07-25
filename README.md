# MathOptimizer
MathOptimizer is a program simplifies math expressions. Math expressions are designed in the grammar described in [grammar.txt](https://github.com/caojoshua/MathOptimizer/blob/main/grammar.txt). I chose to build this project to practice c++, parsing, and simple numerical optimizations on a AST in preparation for interviews. I don't plan on introducing new features or maintaining this project.

Some example simplifications:

* constant folding: `1+2+3` -> `6`
* precedence: `1+2*3` -> `7`
* precendence with parenthesis: `(1+2)*3` -> `9`
* simplifications with variables: `1+x+3` -> `x + 4
* adding terms: `3*x*y + 2*y*x` -> `5 * x * y`

See [#testing](#testing) for a more detailed list of features.

## Building

### Requirements

* c++ compiler that supports c++11
* cmake >= 3.10
* clang-format
* python3 to run tests
* compiles on Fedora 33 for sure, and probably on any modern Linux, BSD, Mac. Not sure about windows.

### Steps

First `cd` into the project directory. Then run the following:

```
mkdir <build-dir>
cd <build-dir>
cmake .. # add -DCMAKE_BUILD_TYPE=Debug right after cmake to include debug flags
cmake --build .
```

To autoformat from the project directory:

```
cmake --build <build-dir> -- format
```
## Running

The binary is built as <build-dir>/src/mathopt. The binary takes a math expression as a single argument. For example:

```
<build-dir>/src/mathopt "1+2+x+y"
```

## <a id="testing"></a> Testing

To run tests, run `test.py` from the project directory. It takes the MathOptimizer executable as its first argument. If the argument is not provided, the script assumes that the executable exists as `mathopt` on the PATH.

The tests are not fully comprehensive, but is enough to catch most regressions. The tests can also be read to better understand the capabilities of MathOptimizer.

### Potential Future Work

Although I don't plan on further developing this project, here are some ideas that I thought about

1. Support for exponents, and all the optimizations that come with it, such as
  * `x * x` -> `x^2`
  * `x / x` -> `1` (technically possible with the current grammar, but I did not implement as it's logic would be similar to exponent logic)
  * `x^y^z` -> `x^(y*z)`
  * `x^a*x^b` -> `x^(a+b)
  * `x^(-a)` -> `1/x^a`
2. On unexpected tokens, actually output what the unexpected token is, rather than just `fatal: unexpected token`. Could be solved with some `tokenToStr()` function.
3. Fix memory leaks. Most happy cases seem to pass valgrind tests.
  * Testcases with errors that `exit(1)` don't clean up the heap. This in release is fine since the process is exiting, but doesn't play well when testing with valgrind
4. Tests for improper grammar. Currently we only test against testcases that follow the grammar, but we could check for fatal messages.
5. Error for dividing by 0. We currently see infs, which is maybe fine as is.
6. Support for increased floating point precision. Currently, anything beyond 5 decimal points often does unexpected rounding, and I'm not sure why.

### Related Work
* [Wolfram language](https://en.wikipedia.org/wiki/Wolfram_Language) is the most comprehensive math language. [Mathematica](https://en.wikipedia.org/wiki/Wolfram_Mathematica) is the main implementation of the language with many optimizations, but the software is proprietary
* [pythonica](https://github.com/mikexstudios/pythonica) is a python implementation of a simple math language. It has not been maintained since around 2010. Looked at the source code for some ideas.
* [this stack exchange post](https://mathematica.stackexchange.com/questions/4454/is-there-an-open-source-implementation-of-mathematica-the-language) links to other related projects
