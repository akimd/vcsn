#pragma once

#include <string>
#include <sstream>

#include <vcsn/dyn/fwd.hh>

namespace vcsn
{
  namespace dyn
  {
    // efsm.cc.
    automaton read_efsm(std::istream& is);
    // fado.cc.
    automaton read_fado(std::istream& is);
  }
}
