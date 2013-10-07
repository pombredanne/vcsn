#ifndef VCSN_MISC_MILITARY_ORDER_HH
# define VCSN_MISC_MILITARY_ORDER_HH

# include <tuple>
# include <type_traits>

namespace vcsn
{

  // Is it possible to write a C++ template to check for a function's
  // existence?  http://stackoverflow.com/questions/257288
  namespace detail
  {
    template<class>
    struct sfinae_true : std::true_type {};

    template <typename T>
    static auto test_size(int)
      -> sfinae_true<decltype(std::declval<T>().size())>;

    template <typename>
    static auto test_size(long) -> std::false_type;
  }

  /// Whether T features a size() function.
  template <typename T>
  struct has_size_member_function
    : decltype(detail::test_size<T>(0))
  {};

  /// Military strict order predicate.
  ///
  /// This predicate applies to any type which provides, or does not
  /// provide, a size method.  If two elements have the same size they
  /// are compared with operator<, otherwise, the shortest is the
  /// smallest.
  template <typename T>
  struct MilitaryOrder
  {
  public:
    bool operator()(const T& x, const T& y) const
    {
      return lt_<T>(x, y);
    }

  private:
    /// Case where T features a size() member function.
    template <typename T2>
    auto lt_(const T2& x, const T2& y) const
      -> typename std::enable_if<has_size_member_function<T2>::value, bool>::type
    {
      return
        std::forward_as_tuple(x.size(), x) < std::forward_as_tuple(y.size(), y);
    }

    /// Case where T does not feature a size() member function.
    template <typename T2>
    auto lt_(const T2& x, const T2& y) const
      -> typename std::enable_if<!has_size_member_function<T2>::value, bool>::type
    {
      return x < y;
    }
  };

}

#endif // !VCSN_MISC_MILITARY_ORDER_HH
