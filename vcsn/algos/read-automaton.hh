#pragma once

#include <vcsn/concepts/automaton.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/automaton.hh>

namespace vcsn
{
  /*------------------.
  | read_automaton.   |
  `------------------*/

  /// Static version of the read_automaton function.
  ///
  /// Read an automaton with a specified context (from the Aut
  /// template parameter).
  ///
  /// \param is      the input stream
  /// \param f       the automaton format
  template <Automaton Aut>
  Aut
  read_automaton(std::istream& is, const std::string& f = "default")
  {
    dyn::automaton res = dyn::read_automaton(is, f);
    // Automaton typename.
    auto vname = res->vname();
    VCSN_REQUIRE(vname == Aut::element_type::sname(),
                 f, ": invalid context: ", vname,
                 ", expected: ", Aut::element_type::sname());
    return std::move(res->as<Aut>());
  }
} // namespace vcsn
