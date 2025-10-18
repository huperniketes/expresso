
> how does C handle Infinity? Is there an Infinitesimal?

✦ Those are great questions about the nuances of floating-point arithmetic in C.

  1. How does C handle Infinity?

  In C, Infinity is a special floating-point value defined by the IEEE 754 standard, which is used by almost all modern hardware. It represents a value that is larger than any other representable 
  number.

   * Creation: It typically results from two kinds of operations:
       1. Division by zero: 1.0 / 0.0 results in Infinity.
       2. Overflow: A calculation that exceeds the maximum value of a floating-point type (e.g., DBL_MAX * 2.0) results in Infinity.
   * Usage: C provides tools in the <math.h> library to work with it. The macro INFINITY provides the value itself, and the isinf() function can check if a variable holds an infinite value.
   * Behavior: Arithmetic with Infinity is well-defined (e.g., Infinity + 100 is still Infinity, and 100 / Infinity is 0.0).

  However, it's important to note that the Expresso specification explicitly forbids this C behavior. The spec requires that operations like division-by-zero must be detected before they happen and 
  reported as a runtime error, rather than resulting in an Infinity value.

  2. Is there an Infinitesimal?

  C does not have a special named value for "infinitesimal" in the way it has INFINITY.

  However, the concept is represented by subnormal numbers (also called denormalized numbers).

   * The Concept: There is a gap between zero and the smallest possible number that can be represented normally in floating-point format. To handle calculations that "underflow" into this gap, hardware 
     can create subnormal numbers.
   * What they are: Subnormal numbers are extremely tiny floating-point values that have lost some of their precision to represent a magnitude even closer to zero. They provide "gradual underflow," 
     smoothly transitioning to zero rather than abruptly dropping to zero once a calculation is too small.

  So, while there is no INFINITESIMAL constant, subnormal numbers are the C language's practical equivalent to the mathematical concept of an infinitesimal. The Expresso specification does not 
  currently define any special handling for them, so they would be treated like any other very small floating-point value.

> Can we realistically define and support an Infinitesimal value in Expresso?

✦ That's an interesting language design question.

  Technically, yes, it is realistic to define and support an Infinitesimal value. The most robust way to do this would be to add it as a new, distinct type to the Expresso Value system, separate from 
  float or int.

  However, doing so would introduce significant complexity, and you would need to make several difficult design decisions first:

   1. Defining the Semantics: You would need to specify the rules for every possible operation. For example, what is the result of:
       * 5 + Infinitesimal? (Presumably 5)
       * 5 / Infinitesimal? (This would logically be Infinity, which the spec currently forbids).
       * Infinitesimal / Infinitesimal? (This would logically be NaN, which is also forbidden).
       * Infinitesimal == 0? (Is it equal to zero, or just very close?)

   2. Defining Creation: You would need to decide how an Infinitesimal value is created. Does it result from a calculation that underflows (which is complex to detect reliably), or is it created via a 
      special keyword?

   3. Implementation Cost: This would be a major change. Every part of the evaluator that handles arithmetic and comparisons would need to be updated to correctly handle the new Infinitesimal type, 
      significantly increasing the implementation and testing effort.

  Given that this feature is not in the current specification and the complexity it would add, it might be best to defer it unless there is a strong use case.

  For now, I would recommend sticking to the standard floating-point model (which includes subnormal numbers as a stand-in for infinitesimals) and focusing on the existing requirements.

