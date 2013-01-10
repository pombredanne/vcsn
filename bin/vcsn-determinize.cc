#include <iostream>
#include <vcsn/algos/dyn.hh>

int
main (int argc, char* const argv[])
{
  assert(2 == argc);
  std::string file = argv[1];

  // Input.
  using namespace vcsn::dyn;
  vcsn::dyn::automaton aut = read_automaton_file(file);

  // Process.
  aut = determinize(aut);

  // Output.
  print(aut, std::cout, FileType::dot) << std::endl;
}
