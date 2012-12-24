
#include <vcsn/algos/dyn.hh>
#include <lib/vcsn/rat/driver.hh>
#include <vcsn/algos/xml.hh>

int
main(const int argc, char const* argv[])
{
  using namespace vcsn::dyn;

  std::string cname = argv[1];
  context* ctx = vcsn::dyn::make_context(cname, "abcd");
  vcsn::rat::driver d(*ctx);
  for (int i = 1; i < argc; ++i)
    {
      auto exp = read_ratexp_file(argv[i], *ctx);
      print(exp, std::cout, FileType::xml);
    }
}
