#pragma once

/*-------------------.
| likely, unlikely.  |
`-------------------*/

// Instrumentation of conditional values (hand made profiling).
//
// if (unlikely(condition))
// {
//   // Handle fallback, errors and this will never be executed in a
//   // normal running process.
// }

#define likely(Exp)   __builtin_expect(!!(Exp), 1)
#define unlikely(Exp) __builtin_expect(!!(Exp), 0)
