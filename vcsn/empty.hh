#ifndef VCSN_EMPTY_HH
# define VCSN_EMPTY_HH

# include <functional> // std::hash

namespace vcsn
{
  /// Empty labels, for LAU.
  struct empty_t
  {};

  // So that we can make a map with empty_t as key, for
  // mutable_automaton's entries.
  inline
  bool operator<(empty_t, empty_t)
  {
    return false;
  }

  // For unordered containers.
  inline
  bool operator==(empty_t, empty_t)
  {
    return true;
  }
}

namespace std
{
  // For unordered containers.
  template <>
  struct hash<vcsn::empty_t>
  {
    size_t operator()(vcsn::empty_t) const
    {
      return 0;
    }
  };
}

#endif // !VCSN_EMPTY_HH
