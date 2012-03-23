#include <vcsn/weights/z.hh>
#include <vcsn/io/parse-rat-exp.hh>

int
main()
{
  typedef vcsn::z weightset_t;
  typedef typename weightset_t::value_t weight_t;
  if (vcsn::rat::exp* e = vcsn::rat::parse<weightset_t>())
    {
      const auto* down = down_cast<const vcsn::rat::node<weight_t>*>(e);
      vcsn::rat::printer<weightset_t>
        print(std::cout, weightset_t(), true, true);
      down->accept(print);
      std::cout << std::endl;
      return 0;
    }
  return 1;
}
