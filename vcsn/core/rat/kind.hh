#ifndef VCSN_CORE_RAT_KIND_HH
# define VCSN_CORE_RAT_KIND_HH

# include <vcsn/core/kind.hh>

namespace vcsn
{
  // Whether RatExp are labelled by letters.
  struct atoms_are_letters
  {
  };

  // Whether RatExp are labelled by words.
  struct atoms_are_words
  {
  };


  /*-----------------------------------------.
  | Convert from atom kinds to label kinds.  |
  `-----------------------------------------*/

  template <class Kind, class GenSet>
  struct atom_trait
  {
  };

  template <class GenSet>
  struct atom_trait<atoms_are_letters, GenSet>
  {
    using type = typename GenSet::letter_t;
  };

  template <class GenSet>
  struct atom_trait<atoms_are_words, GenSet>
  {
    using type = typename GenSet::word_t;
  };


  /*-----------------------------------------.
  | Convert from atom kinds to label kinds.  |
  `-----------------------------------------*/

  template <typename AtomKind>
  struct label_kind
  {};

  template <>
  struct label_kind<atoms_are_words>
  {
    using type = labels_are_words;
  };

  template <>
  struct label_kind<atoms_are_letters>
  {
    using type = labels_are_letters;
  };

  /*-----------------------------------------.
  | Convert from label kinds to atom kinds.  |
  `-----------------------------------------*/

  template <typename LabelKind>
  struct atom_kind
  {};

  template <>
  struct atom_kind<labels_are_words>
  {
    using type = atoms_are_words;
  };

  template <>
  struct atom_kind<labels_are_letters>
  {
    using type = atoms_are_letters;
  };

}

#endif // !VCSN_CORE_RAT_KIND_HH
