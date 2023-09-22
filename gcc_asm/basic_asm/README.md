# Basic Asm — Assembler Instructions Without Operands

```text
asm asm-qualifiers ( AssemblerInstructions )
```

For the C language

* `asm` keyword is a GNU extension
* `__asm__` is an [alternate keyword](https://gcc.gnu.org/onlinedocs/gcc/Alternate-Keywords.html)

For the C++ language

* `asm` is a standard keyword
* `__asm__` used for code compiled with `-fno-asm`

## Qualifiers

volatile

* The optional `volatile` qualifier has no effect. All basic `asm` blocks are implicitly volatile.

inline

* If you use the `inline` qualifier, then for inlining purposes the size of the `asm` statement is taken as the smallest size possible.

## Parameters

AssemblerInstructions

* This is a literal string that specifies the assembler code.
* The string can contain any instructions recognized by the assembler, including directives.
* GCC does not parse the assembler instructions themselves and does not know what they mean or even whether they are valid assembler input.
* You may place multiple assembler instructions together in a single `asm` string, separated by the characters normally used in assembly code for the system.
* A combination that works in most places is a newline to break the line, plus a tab character (written as ‘\n\t’).
* Some assemblers allow semicolons as a line separator.
* However, note that some assembler dialects use semicolons to start a comment.

## Remarks

Using extended `asm` typically produces smaller, safer, and more efficient code, and in most cases it is a better solution than basic `asm`. However, there are two situations where only basic `asm` can be used:

* Extended `asm` statements have to be inside a C function, so to write inline assembly language at file scope (“top-level”), outside of C functions, you must use basic `asm`. You can use this technique to emit assembler directives, define assembly language macros that can be invoked elsewhere in the file, or write entire functions in assembly language. Basic `asm` statements outside of functions may not use any qualifiers.
* Functions declared with the `naked` attribute also require basic `asm`.

Do not expect a sequence of `asm` statements to remain perfectly consecutive after compilation. If certain instructions need to remain consecutive in the output, put them in a single multi-instruction `asm` statement. Note that GCC’s optimizers can move `asm` statements relative to other code, including across jumps.

`asm` statements may not perform jumps into other `asm` statements. GCC does not know about these jumps, and therefore cannot take account of them when deciding how to optimize. Jumps from `asm` to C labels are only supported in extended `asm`.

Under certain circumstances, GCC may duplicate (or remove duplicates of) your assembly code when optimizing. This can lead to unexpected duplicate symbol errors during compilation if your assembly code defines symbols or labels.

Warning: The C standards do not specify semantics for `asm`, making it a potential source of incompatibilities between compilers. These incompatibilities may not produce compiler warnings/errors.

GCC does not parse basic `asm`’s _AssemblerInstructions_, which means there is no way to communicate to the compiler what is happening inside them. GCC has no visibility of symbols in the `asm` and may discard them as unreferenced. It also does not know about side effects of the assembler code, such as modifications to memory or registers. Unlike some compilers, GCC assumes that no changes to general purpose registers occur. This assumption may change in a future release.

To avoid complications from future changes to the semantics and the compatibility issues between compilers, consider replacing basic `asm` with extended `asm`.

The compiler copies the assembler instructions in a basic `asm` verbatim to the assembly language output file, without processing dialects or any of the ‘%’ operators that are available with extended `asm`. This results in minor differences between basic `asm` strings and extended `asm` templates. For example, to refer to registers you might use ‘%eax’ in basic `asm` and ‘%%eax’ in extended `asm`.

On targets such as x86 that support multiple assembler dialects, all basic `asm` blocks use the assembler dialect specified by the `-masm` command-line option. Basic `asm` provides no mechanism to provide different assembler strings for different dialects.

For basic `asm` with non-empty assembler string GCC assumes the assembler block does not change any general purpose registers, but it may read or write any globally accessible variable.

Here is an example of basic `asm` for i386:

```c
/* Note that this code will not compile with -masm=intel */
#define DebugBreak() asm("int $3")
```
