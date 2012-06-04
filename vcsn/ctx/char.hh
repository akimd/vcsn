#ifndef VCSN_CTX_CHAR_HH
# define VCSN_CTX_CHAR_HH

# include <vcsn/alphabets/char.hh>
# include <vcsn/alphabets/setalpha.hh>
# include <vcsn/weights/b.hh>
# include <vcsn/weights/z.hh>
# include <vcsn/weights/z_min.hh>

namespace vcsn
{
  namespace ctx
  {
    template <typename WeightSet>
    struct char_
    {
      using genset_t = vcsn::set_alphabet<vcsn::char_letters>;
      genset_t gs_;
      using weightset_t = WeightSet;
      weightset_t ws_;
    };

    using char_b = char_<vcsn::b>;
    using char_z = char_<vcsn::z>;
    using char_z_min = char_<vcsn::z_min>;
  }
};

#endif // !VCSN_CTX_CHAR_HH
