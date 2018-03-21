#pragma once

#include <string>
#include <sstream>

#include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    // daut.cc.
    automaton read_daut(std::istream& is, const location& loc);
    // efsm.cc.
    automaton read_efsm(std::istream& is, const location& loc);
    automaton read_efsm_bzip2(std::istream& is, const location& loc);
    automaton read_efsm_lzma(std::istream& is, const location& loc);
    // fado.cc.
    automaton read_fado(std::istream& is, const location& loc);
  }
}
