# cumtber

`cumtber` is a small, experimental language front end written in C++17. It
tokenizes source code, parses it into an abstract syntax tree (AST), and prints
a textual representation of the parsed program.

The project is intended for learning and experimenting with the fundamentals
of compiler construction: lexical analysis, expression parsing, AST design,
operator precedence, functions, and control flow.

> [!NOTE]
> This project is under active development. It currently produces diagnostic
> AST-like text; it does not generate machine code or execute the input program.

## Features

- Numeric literals and identifiers
- Binary expressions with `+`, `-`, `*`, `<`, and `=`
- Parenthesized expressions
- Function definitions and calls, including nested calls
- `if`/`else` expressions
- `while` expressions
- Line comments beginning with `#`
- Line-numbered syntax errors

## Requirements

- A C++17-compatible compiler
- CMake 3.10 or later
- A build tool supported by CMake, such as Make or Ninja

## Build

Clone the repository and build the executable from the project root:

```bash
git clone https://github.com/fall545/cumtber.git
cd cumtber
cmake -S . -B build
cmake --build build
```

The resulting executable is `build/cumtber`.

## Usage

The parser reads source code from standard input. Run it interactively:

```bash
./build/cumtber
```

Or provide one of the sample programs through standard input:

```bash
./build/cumtber < test/fib.cumtber
./build/cumtber < test/if.cumtber
./build/cumtber < test/while.cumtber
```

Due to the current EOF-handling limitation described below, the process may
need to be stopped manually after it parses the available input.

## Language Examples

Define a function:

```text
def minus a, b {
    a - b;
}
```

Call a function:

```text
minus(10, 3);
```

Use conditional expressions:

```text
if x {
    y;
} else {
    z;
}
```

Use a loop:

```text
while x {
    y;
}
```

Comments extend from `#` to the end of the line:

```text
# Compute the difference
def minus a, b {
    a - b;
}
```

See the [`test`](test) directory for more complete input examples.

## Project Structure

```text
.
|-- Cmakelists.txt       # CMake build configuration
|-- main.cpp             # Parser entry point and top-level loop
|-- include/
|   |-- ast.h            # AST node definitions
|   |-- parser.h         # Parser interface
|   `-- tokenizer.h      # Tokenizer and token definitions
|-- src/
|   |-- expressionast.cpp
|   |-- functionast.cpp
|   `-- if-while.cpp
`-- test/                # Example cumtber programs
```

## How It Works

1. The tokenizer reads characters from standard input and emits tokens.
2. The parser consumes those tokens and constructs AST nodes.
3. Each AST node's `codegen()` method returns a textual description.
4. The top-level loop prints the result and continues until end-of-file.

## Current Limitations

- There is no runtime, interpreter, LLVM backend, or native code generation.
- The tokenizer does not yet handle a standard stream EOF reliably, so the
  parser may not terminate automatically after redirected input is exhausted.
- The grammar and error recovery are still experimental.
- Function bodies and control-flow blocks currently support only a limited
  subset of statements.
- Automated tests have not yet been added; files in `test/` are manual parser
  inputs.

## Contributing

Contributions are welcome. Keep public declarations in `include/`, place
implementations in `src/`, and add or update a sample input in `test/` when
changing the grammar. Please build the project and run the relevant samples
before opening a pull request.

## License

This project is licensed under the [GNU General Public License v3.0](LICENSE).
