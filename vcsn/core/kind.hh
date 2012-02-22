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

  template <class Kind, class Alphabet>
  struct label_trait
  {
  };

  template <class Alphabet>
  struct label_trait<labels_are_letters, Alphabet>
  {
    typedef typename Alphabet::letter_t label_t;
  };

  template <class Alphabet>
  struct label_trait<labels_are_words, Alphabet>
  {
    typedef typename Alphabet::word_t label_t;
  };
}

#endif // VCSN_CORE_KIND_HH
