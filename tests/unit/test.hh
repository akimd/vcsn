#pragma once

#include <iostream>

#include <vcsn/misc/echo.hh>

#define ASSERT_EQ(Lhs, Rhs)                             \
  do {                                                  \
    auto lhs_ = Lhs;                                    \
    auto rhs_ = Rhs;                                    \
    if (lhs_ != rhs_)                                   \
      {                                                 \
        SHOWH("assertion failed: " #Lhs " == " #Rhs);   \
        SHOWH("    " << lhs_ << " != " << rhs_);        \
        ++nerrs;                                        \
      }                                                 \
    else if (getenv("VERBOSE"))                         \
      {                                                 \
        SHOWH("PASS: " #Lhs " == " #Rhs);               \
      }                                                 \
  } while (false)
