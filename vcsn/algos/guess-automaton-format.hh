#pragma once

namespace vcsn
{
  /*--------------------------.
  | guess_automaton_format.   |
  `--------------------------*/

  /// Guess the format of an automaton file.
  ///
  /// \param is      the input stream
  /// \requires is to be seekable
  /// \returns the name of the format, suitable for read_automaton.
  std::string
  guess_automaton_format(std::istream& is);
} // namespace vcsn
