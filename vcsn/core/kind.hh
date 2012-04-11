#ifndef VCSN_CORE_KIND_HH
#define VCSN_CORE_KIND_HH

namespace vcsn
{
  struct labels_are_letters
  {
  };

  struct labels_are_words
  {
  };

  template <class Kind, class GenSet>
  struct label_trait
  {
  };

  template <class GenSet>
  struct label_trait<labels_are_letters, GenSet>
  {
    typedef typename GenSet::letter_t label_t;
  };

  template <class GenSet>
  struct label_trait<labels_are_words, GenSet>
  {
    typedef typename GenSet::word_t label_t;
  };
}

#endif // VCSN_CORE_KIND_HH
