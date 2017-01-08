#pragma once

#include <iostream>

#include <vcsn/misc/escape.hh>
#include <vcsn/misc/show.hh>
#include <vcsn/misc/to-string.hh>

// A macro, because we capture __LINE__ and __FILE__ and #Lhs etc.
#define ASSERT_EQ(Lhs, Rhs)                                             \
  do {                                                                  \
    auto lhs_ = Lhs;                                                    \
    auto rhs_ = Rhs;                                                    \
    if (lhs_ != rhs_)                                                   \
      {                                                                 \
        SHOWH("FAIL: assertion failed: " #Lhs " == " #Rhs);             \
        SHOW("expected:\n\t\"" << lhs_ << '"');                         \
        SHOW("effective:\n\t\"" << rhs_ << '"');                        \
        ++nerrs;                                                        \
      }                                                                 \
    else if (getenv("VERBOSE"))                                         \
      SHOWH("PASS: " #Lhs " == " #Rhs);                                 \
  } while (false)

// Using a valueset.
#define ASSERT_VS_EQ(Vs, Lhs, Rhs)                                      \
  do {                                                                  \
    const auto& vs = Vs;                                                \
    auto lhs_ = Lhs;                                                    \
    auto rhs_ = Rhs;                                                    \
    if (!vs.equal(lhs_, rhs_))                                          \
      {                                                                 \
        SHOWH("FAIL: assertion failed: " << vcsn::to_string(vs)         \
              << ".equal(" #Lhs ", " #Rhs ")");                         \
        SHOW("expected:\n\t\"" << vcsn::to_string(vs, lhs_) << '"');    \
        SHOW("effective:\n\t\"" << vcsn::to_string(vs, rhs_) << '"');   \
        ++nerrs;                                                        \
      }                                                                 \
    else if (getenv("VERBOSE"))                                         \
      SHOWH("PASS: " << vcsn::to_string(vs)                             \
            << ".equal(" #Lhs ", " #Rhs ")");   \
  } while (false)
