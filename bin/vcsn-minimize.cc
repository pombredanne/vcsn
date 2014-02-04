#include <iostream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include <vcsn/dyn/algos.hh>

#include "parse-args.hh"

struct minimize: vcsn_function
{
  int work_aut(const options& opts) const
  {
    using namespace vcsn::dyn;
    // Input.
    auto aut = read_automaton(opts);
    std::string algo = 1 <= opts.argv.size() ? opts.argv[0] : "signature";

    // Process.
    auto res = vcsn::dyn::minimize(aut, algo);

    // Output.
    *opts.out << sort(res) << std::endl;
    return 0;
  }
};

int main(int argc, char* const argv[])
{
  return vcsn_main(argc, argv, minimize{});
}
