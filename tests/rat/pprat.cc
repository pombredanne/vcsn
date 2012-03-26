#include <cstdlib>

#include <vcsn/weights/b.hh>
#include <vcsn/weights/z.hh>
#include <vcsn/core/rat/factory_.hh>
#include <vcsn/core/rat/printer.hh>
#include <vcsn/io/parse-rat-exp.hh>

static
void
usage(const char* prog, int status)
{
  std::cerr <<
    "usage: " << prog << " WEIGHT-SET\n"
    "\n"
    "WEIGHT-SET:\n"
    "  z  for Z\n"
    "  b  for Boolean\n";
  exit(status);
}

template <typename WeightSet>
int
pp(const WeightSet& ws)
{
  typedef WeightSet weightset_t;
  typedef typename weightset_t::value_t weight_t;
  vcsn::factory_<weightset_t> factory(ws);
  if (vcsn::rat::exp* e = vcsn::rat::parse_file("-", factory))
    {
      const auto* down = down_cast<const vcsn::rat::node<weight_t>*>(e);
      vcsn::rat::printer<weightset_t> print(std::cout, ws, true, true);
      down->accept(print);
      std::cout << std::endl;
      return 0;
    }
  return 1;
}

int
main(int argc, const char* argv[])
{
  if (argc != 2)
    usage(argv[0], EXIT_FAILURE);
  std::string w(argv[1]);

  if (w == "z")
    return pp(vcsn::z());
  else if (w == "b")
    return pp(vcsn::b());
  else
    usage(argv[0], EXIT_FAILURE);
}
