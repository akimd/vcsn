#ifndef VCSN_LABELSET_FWD_HH
# define VCSN_LABELSET_FWD_HH

namespace vcsn
{

  // tupleset.hh.
  template <typename... LabelSets>
  class tupleset;

  namespace ctx
  {
    // letterset.hh.
    template <typename GenSet>
    class letterset;

    // nullableset.hh.
    template <typename GenSet>
    class nullableset;

    // oneset.hh.
    class oneset;

    // wordset.hh.
    template <typename GenSet>
    class wordset;
  }

} // namespace vcsn

#endif // !VCSN_LABELSET_FWD_HH
