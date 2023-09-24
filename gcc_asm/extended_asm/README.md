# Extended Asm - Assembler Instructions with C Expression Operands

With extended asm you can read and write C variables from assembler and perform jumps from assembler code to C labels. Extended asm syntax uses colons (‘:’) to delimit the operand parameters after the assembler template:

```text
asm asm-qualifiers ( AssemblerTemplate 
                     : OutputOperands 
                   [ : InputOperands
                   [ : Clobbers ] ] )

asm asm-qualifiers ( AssemblerTemplate 
                     : OutputOperands
                     : InputOperands
                     : Clobbers
                     : GotoLabels )
```

where in the last form, __asm-qualifiers__ contains `goto` (and in the first form, not).

The asm keyword is a GNU extension. When writing code that can be compiled with -ansi and the various -std options, use `__asm__` instead of asm.

## Qualifiers

### volatile

* The typical use of extended asm statements is to manipulate input values to produce output values.
* However, your asm statements may also produce side effects.
* If so, you may need to use the volatile qualifier to disable certain optimizations.

### inline

* If you use the inline qualifier, then for inlining purposes the size of the asm statement is taken as the smallest size possible.

### goto

* This qualifier informs the compiler that the asm statement may perform a jump to one of the labels listed in the __GotoLabels__.

## Parameters

### AssemblerTemplate

* This is a literal string that is the template for the assembler code. It is a combination of fixed text and tokens that refer to the input, output, and goto parameters.

### OutputOperands

* A comma-separated list of the C variables modified by the instructions in the __AssemblerTemplate__. An empty list is permitted.

### InputOperands

* A comma-separated list of C expressions read by the instructions in the __AssemblerTemplate__. An empty list is permitted.

### Clobbers

* A comma-separated list of registers or other values changed by the __AssemblerTemplate__, beyond those listed as outputs. An empty list is permitted.

### GotoLabels

* When you are using the `goto` form of asm, this section contains the list of all C labels to which the code in the __AssemblerTemplate__ may jump.
* asm statements may not perform jumps into other asm statements, only to the listed __GotoLabels__. GCC’s optimizers do not know about other jumps; therefore they cannot take account of them when deciding how to optimize.

The total number of input + output + goto operands is limited to 30.

## Remarks

Note that extended asm statements must be inside a function. Only basic asm may be outside functions. Functions declared with the `naked` attribute also require basic asm.

## Volatile

* GCC’s optimizers sometimes discard asm statements if they determine there is no need for the output variables.
* Also, the optimizers may move code out of loops if they believe that the code will always return the same result (i.e. none of its input values change between calls).
* Using the `volatile` qualifier disables these optimizations.
* asm statements that have no output operands and asm `goto` statements, are implicitly volatile.

### examples

* This i386 code demonstrates a case that does not use (or require) the `volatile` qualifier. If it is performing assertion checking, this code uses asm to perform the validation. Otherwise, `dwRes` is unreferenced by any code. As a result, the optimizers can discard the asm statement, which in turn removes the need for the entire `DoCheck` routine. By omitting the `volatile` qualifier when it isn’t needed you allow the optimizers to produce the most efficient code possible.

```c
void DoCheck(uint32_t dwSomeValue)
{
   uint32_t dwRes;

   // Assumes dwSomeValue is not zero.
   asm ("bsfl %1,%0"
     : "=r" (dwRes)
     : "r" (dwSomeValue)
     : "cc");

   assert(dwRes > 3);
}
```

* The next example shows a case where the optimizers can recognize that the input (dwSomeValue) never changes during the execution of the function and can therefore move the asm outside the loop to produce more efficient code. Again, using the `volatile` qualifier disables this type of optimization.

```c
void do_print(uint32_t dwSomeValue)
{
   uint32_t dwRes;

   for (uint32_t x=0; x < 5; x++)
   {
      // Assumes dwSomeValue is not zero.
      asm ("bsfl %1,%0"
        : "=r" (dwRes)
        : "r" (dwSomeValue)
        : "cc");

      printf("%u: %u %u\n", x, dwSomeValue, dwRes);
   }
}
```

* The following example demonstrates a case where you need to use the `volatile` qualifier. It uses the x86 `rdtsc` instruction, which reads the computer’s time-stamp counter. Without the `volatile` qualifier, the optimizers might assume that the asm block will always return the same value and therefore optimize away the second call.

```c
uint64_t msr;

asm volatile ( "rdtsc\n\t"    // Returns the time in EDX:EAX.
        "shl $32, %%rdx\n\t"  // Shift the upper bits left.
        "or %%rdx, %0"        // 'Or' in the lower bits.
        : "=a" (msr)
        : 
        : "rdx");

printf("msr: %llx\n", msr);

// Do other work...

// Reprint the timestamp
asm volatile ( "rdtsc\n\t"    // Returns the time in EDX:EAX.
        "shl $32, %%rdx\n\t"  // Shift the upper bits left.
        "or %%rdx, %0"        // 'Or' in the lower bits.
        : "=a" (msr)
        : 
        : "rdx");

printf("msr: %llx\n", msr);
```

* GCC’s optimizers do not treat this code like the non-volatile code in the earlier examples. They do not move it out of loops or omit it on the assumption that the result from a previous call is still valid.

* Note that the compiler can move even `volatile` asm instructions relative to other code, including across jump instructions. For example, on many targets there is a system register that controls the rounding mode of floating-point operations. Setting it with a `volatile` asm statement, as in the following PowerPC example, does not work reliably.

```c
asm volatile("mtfsf 255, %0" : : "f" (fpenv));
sum = x + y;
```

* The compiler may move the addition back before the `volatile` asm statement. To make it work as expected, add an artificial dependency to the asm by referencing a variable in the subsequent code, for example:

```c
asm volatile ("mtfsf 255,%1" : "=X" (sum) : "f" (fpenv));
sum = x + y;
```

Under certain circumstances, GCC may duplicate (or remove duplicates of) your assembly code when optimizing. This can lead to unexpected duplicate symbol errors during compilation if your asm code defines symbols or labels. Using ‘%=’ may help resolve this problem.

## Assembler Template

An assembler template is a literal string containing assembler instructions. __The compiler replaces tokens in the template that refer to inputs, outputs, and goto labels, and then outputs the resulting string to the assembler.__ The string can contain any instructions recognized by the assembler, including directives. GCC does not parse the assembler instructions themselves and does not know what they mean or even whether they are valid assembler input. __However, it does count the statements.__

You may place multiple assembler instructions together in a single asm string, separated by the characters normally used in assembly code for the system. __A combination that works in most places is a newline to break the line, plus a tab character to move to the instruction field (written as ‘\n\t’).__ Some assemblers allow semicolons as a line separator. However, note that some assembler dialects use semicolons to start a comment.

Do not expect a sequence of asm statements to remain perfectly consecutive after compilation, even when you are using the volatile qualifier. __If certain instructions need to remain consecutive in the output, put them in a single multi-instruction asm statement.__

__Accessing data from C programs without using input/output operands (such as by using global symbols directly from the assembler template) may not work as expected.__ Similarly, calling functions directly from an assembler template requires a detailed understanding of the target assembler and ABI.

Since GCC does not parse the assembler template, it has no visibility of any symbols it references. This may result in GCC discarding those symbols as unreferenced unless they are also listed as input, output, or goto operands.

### Special format strings

In addition to the tokens described by the input, output, and goto operands, these tokens have special meanings in the assembler template:

‘%%’

* Outputs a single ‘%’ into the assembler code.

‘%=’

* __Outputs a number that is unique to each instance of the asm statement in the entire compilation.__ This option is useful when creating local labels and referring to them multiple times in a single template that generates multiple assembler instructions.

‘%{’
‘%|’
‘%}’

* __Outputs ‘{’, ‘|’, and ‘}’ characters (respectively) into the assembler code.__ When unescaped, these characters have special meaning to indicate multiple assembler dialects, as described below.

### Multiple assembler dialects in asm templates

__On targets such as x86, GCC supports multiple assembler dialects. The -masm option controls which dialect GCC uses as its default for inline assembler.__ The target-specific documentation for the -masm option contains the list of supported dialects, as well as the default dialect if the option is not specified. This information may be important to understand, since assembler code that works correctly when compiled using one dialect will likely fail if compiled using another.

If your code needs to support multiple assembler dialects (for example, if you are writing public headers that need to support a variety of compilation options), use constructs of this form:

```text
{ dialect0 | dialect1 | dialect2... }
```

This construct outputs `dialect0` when using dialect #0 to compile the code, dialect1 for dialect #1, etc. If there are fewer alternatives within the braces than the number of dialects the compiler supports, the construct outputs nothing.

For example, if an x86 compiler supports two dialects (‘att’, ‘intel’), an assembler template such as this:

```text
"bt{l %[Offset],%[Base] | %[Base],%[Offset]}; jc %l2"
```

is equivalent to one of

```text
"btl %[Offset],%[Base] ; jc %l2"   /* att dialect */
"bt %[Base],%[Offset]; jc %l2"     /* intel dialect */
```

Using that same compiler, this code:

```text
"xchg{l}\t{%%}ebx, %1"
```

corresponds to either

```text
"xchgl\t%%ebx, %1"                 /* att dialect */
"xchg\tebx, %1"                    /* intel dialect */
```

There is no support for nesting dialect alternatives.

## Output Operands

An asm statement has zero or more output operands indicating the names of C variables modified by the assembler code.

In this i386 example, old (referred to in the template string as %0) and *Base (as %1) are outputs and Offset (%2) is an input:

```c
bool old;

__asm__ ("btsl %2,%1\n\t" // Turn on zero-based bit #Offset in Base.
         "sbb %0,%0"      // Use the CF to calculate old.
   : "=r" (old), "+rm" (*Base)
   : "Ir" (Offset)
   : "cc");

return old;
```

Operands are separated by commas. Each operand has this format:

```text
[ [asmSymbolicName] ] constraint (cvariablename)
```

### asmSymbolicName

* __Specifies a symbolic name for the operand. Reference the name in the assembler template by enclosing it in square brackets (i.e. ‘%[Value]’).__ The scope of the name is the asm statement that contains the definition. Any valid C variable name is acceptable, including names already defined in the surrounding code. No two operands within the same asm statement can use the same symbolic name.

* __When not using an asmSymbolicName, use the (zero-based) position of the operand in the list of operands in the assembler template.__ For example if there are three output operands, use ‘%0’ in the template to refer to the first, ‘%1’ for the second, and ‘%2’ for the third.

### constraint

* A string constant specifying constraints on the placement of the operand.

* __Output constraints must begin with either ‘=’ (a variable overwriting an existing value) or ‘+’ (when reading and writing). When using ‘=’, do not assume the location contains the existing value on entry to the asm, except when the operand is tied to an input.__

* __After the prefix, there must be one or more additional constraints that describe where the value resides. Common constraints include ‘r’ for register and ‘m’ for memory. When you list more than one possible location (for example, "=rm"), the compiler chooses the most efficient one based on the current context.__ If you list as many alternates as the asm statement allows, you permit the optimizers to produce the best possible code. If you must use a specific register, but your Machine Constraints do not provide sufficient control to select the specific register you want, __local register variables__ may provide a solution.

### cvariablename

* Specifies a C lvalue expression to hold the output, typically a variable name. The enclosing parentheses are a required part of the syntax.

When the compiler selects the registers to use to represent the output operands, it does not use any of the clobbered registers.

__Output operand expressions must be lvalues.__ The compiler cannot check whether the operands have data types that are reasonable for the instruction being executed. For output expressions that are not directly addressable (for example a bit-field), the constraint must allow a register. In that case, GCC uses the register as the output of the asm, and then stores that register into the output.

__Operands using the ‘+’ constraint modifier count as two operands (that is, both as input and output) towards the total maximum of 30 operands per asm statement.__

__Use the ‘&’ constraint modifier on all output operands that must not overlap an input. Otherwise, GCC may allocate the output operand in the same register as an unrelated input operand, on the assumption that the assembler code consumes its inputs before producing outputs.__ This assumption may be false if the assembler code actually consists of more than one instruction.

The same problem can occur if one output parameter (a) allows a register constraint and another output parameter (b) allows a memory constraint. The code generated by GCC to access the memory address in b can contain registers which might be shared by a, and GCC considers those registers to be inputs to the asm. As above, GCC assumes that such input registers are consumed before any outputs are written. This assumption may result in incorrect behavior if the asm statement writes to a before using b. Combining the ‘&’ modifier with the register constraint on a ensures that modifying a does not affect the address referenced by b. Otherwise, the location of b is undefined if a is modified before using b.

__asm supports operand modifiers on operands (for example ‘%k2’ instead of simply ‘%2’).__ Generic Operand modifiers lists the modifiers that are available on all targets. Other modifiers are hardware dependent.

If the C code that follows the asm makes no use of any of the output operands, use volatile for the asm statement to prevent the optimizers from discarding the asm statement as unneeded.

### examples

This code makes no use of the optional __asmSymbolicName__. Therefore it references the first output operand as %0 (were there a second, it would be %1, etc). The number of the first input operand is one greater than that of the last output operand. In this i386 example, that makes Mask referenced as %1:

```c
uint32_t Mask = 1234;
uint32_t Index;

  asm ("bsfl %1, %0"
     : "=r" (Index)
     : "r" (Mask)
     : "cc");
```

That code overwrites the variable Index (‘=’), placing the value in a register (‘r’). Using the generic ‘r’ constraint instead of a constraint for a specific register allows the compiler to pick the register to use, which can result in more efficient code. This may not be possible if an assembler instruction requires a specific register.

The following i386 example uses the asmSymbolicName syntax. It produces the same result as the code above, but some may consider it more readable or more maintainable since reordering index numbers is not necessary when adding or removing operands. The names aIndex and aMask are only used in this example to emphasize which names get used where. It is acceptable to reuse the names Index and Mask.

```c
uint32_t Mask = 1234;
uint32_t Index;

  asm ("bsfl %[aMask], %[aIndex]"
     : [aIndex] "=r" (Index)
     : [aMask] "r" (Mask)
     : "cc");
```

Here are some more examples of output operands.

```c
uint32_t c = 1;
uint32_t d;
uint32_t *e = &c;

asm ("mov %[e], %[d]"
   : [d] "=rm" (d)
   : [e] "rm" (*e));
```

Here, d may either be in a register or in memory. Since the compiler might already have the current value of the uint32_t location pointed to by e in a register, you can enable it to choose the best location for d by specifying both constraints.

## Flag Output Operands

Some targets have a special register that holds the “flags” for the result of an operation or comparison. Normally, the contents of that register are either unmodifed by the asm, or the asm statement is considered to clobber the contents.

On some targets, a special form of output operand exists by which conditions in the flags register may be outputs of the asm. The set of conditions supported are target specific, but the general rule is that the output variable must be a scalar integer, and the value is boolean. When supported, the target defines the preprocessor symbol `__GCC_ASM_FLAG_OUTPUTS__`.

Because of the special nature of the flag output operands, the constraint may not include alternatives.

Most often, the target has only one flags register, and thus is an implied operand of many instructions. In this case, the operand should not be referenced within the assembler template via %0 etc, as there’s no corresponding text in the assembly language.

## Input Operand

Input operands make values from C variables and expressions available to the assembly code.

Operands are separated by commas. Each operand has this format:

```text
[ [asmSymbolicName] ] constraint (cexpression)
```

### asmSymbolicName

* __Specifies a symbolic name for the operand. Reference the name in the assembler template by enclosing it in square brackets (i.e. ‘%[Value]’).__ The scope of the name is the asm statement that contains the definition. Any valid C variable name is acceptable, including names already defined in the surrounding code. No two operands within the same asm statement can use the same symbolic name.

* __When not using an asmSymbolicName, use the (zero-based) position of the operand in the list of operands in the assembler template.__ For example if there are two output operands and three inputs, use ‘%2’ in the template to refer to the first input operand, ‘%3’ for the second, and ‘%4’ for the third.

### constraint

* A string constant specifying constraints on the placement of the operand.

* __Input constraint strings may not begin with either ‘=’ or ‘+’.__ When you list more than one possible location (for example, ‘"irm"’), the compiler chooses the most efficient one based on the current context. __If you must use a specific register, but your Machine Constraints do not provide sufficient control to select the specific register you want, local register variables may provide a solution.__

* __Input constraints can also be digits (for example, "0"). This indicates that the specified input must be in the same place as the output constraint at the (zero-based) index in the output constraint list. When using asmSymbolicName syntax for the output operands, you may use these names (enclosed in brackets ‘[]’) instead of digits.__

### cexpression

* This is the C variable or expression being passed to the asm statement as input. The enclosing parentheses are a required part of the syntax.

__When the compiler selects the registers to use to represent the input operands, it does not use any of the clobbered registers.__

If there are no output operands but there are input operands, place two consecutive colons where the output operands would go:

```c
__asm__ ("some instructions"
   : /* No outputs. */
   : "r" (Offset / 8));
```

Warning: Do not modify the contents of input-only operands (except for inputs tied to outputs). __The compiler assumes that on exit from the asm statement these operands contain the same values as they had before executing the statement.__ It is not possible to use clobbers to inform the compiler that the values in these inputs are changing. One common work-around is to tie the changing input variable to an output variable that never gets used. __Note, however, that if the code that follows the asm statement makes no use of any of the output operands, the GCC optimizers may discard the asm statement as unneeded.__

asm supports operand modifiers on operands (for example ‘%k2’ instead of simply ‘%2’). Generic Operand modifiers lists the modifiers that are available on all targets. Other modifiers are hardware dependent.

### examples

In this example using the fictitious combine instruction, the constraint "0" for input operand 1 says that it must occupy the same location as output operand 0. Only input operands may use numbers in constraints, and they must each refer to an output operand. Only a number (or the symbolic assembler name) in the constraint can guarantee that one operand is in the same place as another. The mere fact that foo is the value of both operands is not enough to guarantee that they are in the same place in the generated assembler code.

```c
asm ("combine %2, %0" 
   : "=r" (foo) 
   : "0" (foo), "g" (bar));
```

Here is an example using symbolic names.

```c
asm ("cmoveq %1, %2, %[result]" 
   : [result] "=r"(result) 
   : "r" (test), "r" (new), "[result]" (old));
```

## Clobbers and Scratch Registers

__While the compiler is aware of changes to entries listed in the output operands, the inline asm code may modify more than just the outputs.__ For example, calculations may require additional registers, or the processor may overwrite a register as a side effect of a particular assembler instruction. In order to inform the compiler of these changes, list them in the clobber list. __Clobber list items are either register names or the special clobbers (listed below). Each clobber list item is a string constant enclosed in double quotes and separated by commas.__

__Clobber descriptions may not in any way overlap with an input or output operand.__ For example, you may not have an operand describing a register class with one member when listing that register in the clobber list. Variables declared to live in specific registers and used as asm input or output operands must have no part mentioned in the clobber description. In particular, there is no way to specify that input operands get modified without also specifying them as output operands.

__When the compiler selects which registers to use to represent input and output operands, it does not use any of the clobbered registers. As a result, clobbered registers are available for any use in the assembler code.__

__Another restriction is that the clobber list should not contain the stack pointer register.__ This is because the compiler requires the value of the stack pointer to be the same after an asm statement as it was on entry to the statement. However, previous versions of GCC did not enforce this rule and allowed the stack pointer to appear in the list, with unclear semantics. This behavior is deprecated and listing the stack pointer may become an error in future versions of GCC.

Here is a realistic example for the VAX showing the use of clobbered registers:

```c
asm volatile ("movc3 %0, %1, %2"
                   : /* No outputs. */
                   : "g" (from), "g" (to), "g" (count)
                   : "r0", "r1", "r2", "r3", "r4", "r5", "memory");
```

Also, there are two special clobber arguments:

"cc"

* __The "cc" clobber indicates that the assembler code modifies the flags register.__ On some machines, GCC represents the condition codes as a specific hardware register; "cc" serves to name this register. On other machines, condition code handling is different, and specifying "cc" has no effect. But it is valid no matter what the target.

"memory"

* The "memory" clobber tells the compiler that the assembly code performs memory reads or writes to items other than those listed in the input and output operands (for example, accessing the memory pointed to by one of the input parameters). To ensure memory contains correct values, GCC may need to flush specific register values to memory before executing the asm. Further, the compiler does not assume that any values read from memory before an asm remain unchanged after that asm; it reloads them as needed. Using the "memory" clobber effectively forms a read/write memory barrier for the compiler.

* Note that this clobber does not prevent the processor from doing speculative reads past the asm statement. To prevent that, you need processor-specific fence instructions.

Flushing registers to memory has performance implications and may be an issue for time-sensitive code. You can provide better information to GCC to avoid this, as shown in the following examples. At a minimum, aliasing rules allow GCC to know what memory doesn’t need to be flushed.

### examples

Here is a fictitious sum of squares instruction, that takes two pointers to floating point values in memory and produces a floating point register output. Notice that x, and y both appear twice in the asm parameters, once to specify memory accessed, and once to specify a base register used by the asm. You won’t normally be wasting a register by doing this as GCC can use the same register for both purposes. However, it would be foolish to use both %1 and %3 for x in this asm and expect them to be the same. In fact, %3 may well not be a register. It might be a symbolic memory reference to the object pointed to by x.

```c
asm ("sumsq %0, %1, %2"
     : "+f" (result)
     : "r" (x), "r" (y), "m" (*x), "m" (*y));
```

Here is a fictitious \*z++ = \*x++ \* \*y++ instruction. Notice that the x, y and z pointer registers must be specified as input/output because the asm modifies them.

```c
asm ("vecmul %0, %1, %2"
     : "+r" (z), "+r" (x), "+r" (y), "=m" (*z)
     : "m" (*x), "m" (*y));
```

An x86 example where the string memory argument is of unknown length.

```c
asm("repne scasb"
    : "=c" (count), "+D" (p)
    : "m" (*(const char (*)[]) p), "0" (-1), "a" (0));
```

If you know the above will only be reading a ten byte array then you could instead use a memory input like: `"m" (*(const char (*)[10]) p)`.

Here is an example of a PowerPC vector scale implemented in assembly, complete with vector and condition code clobbers, and some initialized offset registers that are unchanged by the asm.

```c
void
dscal (size_t n, double *x, double alpha)
{
  asm ("/* lots of asm here */"
       : "+m" (*(double (*)[n]) x), "+&r" (n), "+b" (x)
       : "d" (alpha), "b" (32), "b" (48), "b" (64),
         "b" (80), "b" (96), "b" (112)
       : "cr0",
         "vs32","vs33","vs34","vs35","vs36","vs37","vs38","vs39",
         "vs40","vs41","vs42","vs43","vs44","vs45","vs46","vs47");
}
```

Rather than allocating fixed registers via clobbers to provide scratch registers for an asm statement, an alternative is to define a variable and make it an early-clobber output as with a2 and a3 in the example below. This gives the compiler register allocator more freedom. You can also define a variable and make it an output tied to an input as with a0 and a1, tied respectively to ap and lda. Of course, with tied outputs your asm can’t use the input value after modifying the output register since they are one and the same register. What’s more, if you omit the early-clobber on the output, it is possible that GCC might allocate the same register to another of the inputs if GCC could prove they had the same value on entry to the asm. This is why a1 has an early-clobber. Its tied input, lda might conceivably be known to have the value 16 and without an early-clobber share the same register as %11. On the other hand, ap can’t be the same as any of the other inputs, so an early-clobber on a0 is not needed. It is also not desirable in this case. An early-clobber on a0 would cause GCC to allocate a separate register for the "m" (*(const double (*)[]) ap) input. Note that tying an input to an output is the way to set up an initialized temporary register modified by an asm statement. An input not tied to an output is assumed by GCC to be unchanged, for example "b" (16) below sets up %11 to 16, and GCC might use that register in following code if the value 16 happened to be needed. You can even use a normal asm output for a scratch if all inputs that might share the same register are consumed before the scratch is used. The VSX registers clobbered by the asm statement could have used this technique except for GCC’s limit on the number of asm parameters.

```c
static void
dgemv_kernel_4x4 (long n, const double *ap, long lda,
                  const double *x, double *y, double alpha)
{
  double *a0;
  double *a1;
  double *a2;
  double *a3;

  __asm__
    (
     /* lots of asm here */
     "#n=%1 ap=%8=%12 lda=%13 x=%7=%10 y=%0=%2 alpha=%9 o16=%11\n"
     "#a0=%3 a1=%4 a2=%5 a3=%6"
     :
       "+m" (*(double (*)[n]) y),
       "+&r" (n),	// 1
       "+b" (y),	// 2
       "=b" (a0),	// 3
       "=&b" (a1),	// 4
       "=&b" (a2),	// 5
       "=&b" (a3)	// 6
     :
       "m" (*(const double (*)[n]) x),
       "m" (*(const double (*)[]) ap),
       "d" (alpha),	// 9
       "r" (x),		// 10
       "b" (16),	// 11
       "3" (ap),	// 12
       "4" (lda)	// 13
     :
       "cr0",
       "vs32","vs33","vs34","vs35","vs36","vs37",
       "vs40","vs41","vs42","vs43","vs44","vs45","vs46","vs47"
     );
}
```

## Goto Labels

__asm goto allows assembly code to jump to one or more C labels. The GotoLabels section in an asm goto statement contains a comma-separated list of all C labels to which the assembler code may jump.__ GCC assumes that asm execution falls through to the next statement (if this is not the case, consider using the __builtin_unreachable intrinsic after the asm statement). Optimization of asm goto may be improved by using the hot and cold label attributes.

__If the assembler code does modify anything, use the "memory" clobber to force the optimizers to flush all register values to memory and reload them if necessary after the asm statement.__

__Also note that an asm goto statement is always implicitly considered volatile.__

Be careful when you set output operands inside asm goto only on some possible control flow paths. If you don’t set up the output on given path and never use it on this path, it is okay. Otherwise, you should use ‘+’ constraint modifier meaning that the operand is input and output one. With this modifier you will have the correct values on all possible paths from the asm goto.

__To reference a label in the assembler template, prefix it with ‘%l’ (lowercase ‘L’) followed by its (zero-based) position in GotoLabels plus the number of input and output operands. Output operand with constraint modifier ‘+’ is counted as two operands because it is considered as one output and one input operand.__ For example, if the asm has three inputs, one output operand with constraint modifier ‘+’ and one output operand with constraint modifier ‘=’ and references two labels, refer to the first label as ‘%l6’ and the second as ‘%l7’.

__Alternately, you can reference labels using the actual C label name enclosed in brackets.__ For example, to reference a label named carry, you can use ‘%l[carry]’. The label must still be listed in the GotoLabels section when using this approach. It is better to use the named references for labels as in this case you can avoid counting input and output operands and special treatment of output operands with constraint modifier ‘+’.

Here is an example of asm goto for i386:

```c
asm goto (
    "btl %1, %0\n\t"
    "jc %l2"
    : /* No outputs. */
    : "r" (p1), "r" (p2) 
    : "cc" 
    : carry);

return 0;

carry:
return 1;
```

The following example shows an asm goto that uses a memory clobber.

```c
int frob(int x)
{
  int y;
  asm goto ("frob %%r5, %1; jc %l[error]; mov (%2), %%r5"
            : /* No outputs. */
            : "r"(x), "r"(&y)
            : "r5", "memory" 
            : error);
  return y;
error:
  return -1;
}
```

The following example shows an asm goto that uses an output.

```c
int foo(int count)
{
  asm goto ("dec %0; jb %l[stop]"
            : "+r" (count)
            :
            :
            : stop);
  return count;
stop:
  return 0;
}
```

The following artificial example shows an asm goto that sets up an output only on one path inside the asm goto. Usage of constraint modifier = instead of + would be wrong as factor is used on all paths from the asm goto.

```c
int foo(int inp)
{
  int factor = 0;
  asm goto ("cmp %1, 10; jb %l[lab]; mov 2, %0"
            : "+r" (factor)
            : "r" (inp)
            :
            : lab);
lab:
  return inp * factor; /* return 2 * inp or 0 if inp < 10 */
}
```

## Generic Operand Modifiers

The following table shows the modifiers supported by all targets and their effects:

| Modifier | Description | Example |
|----------|-------------|---------|
| c        | Require a constant operand and print the constant expression with no punctuation. | %c0 |
| n        | Like ‘%c’ except that the value of the constant is negated before printing. | %n0 |
| a        | Substitute a memory reference, with the actual operand treated as the address. This may be useful when outputting a “load address” instruction, because often the assembler syntax for such an instruction requires you to write the operand as if it were a memory reference. | %a0 |
| l        | Print the label name with no punctuation. | %l0 |
