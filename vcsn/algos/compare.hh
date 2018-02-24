#pragma once

namespace vcsn::dyn::detail
{
  template <typename ValueSet1, typename ValueSet2,
            typename DynValue>
  int
  compare_bridge(const DynValue& lhs, const DynValue& rhs)
  {
    auto joined = join<ValueSet1, ValueSet2>(lhs, rhs);
    return joined.valueset.compare(joined.lhs,
                                   joined.rhs);
  }

  /// Bridge (compare).
  template <typename ExpSet1, typename ExpSet2>
  int
  compare_expression(const expression& lhs, const expression& rhs)
  {
    return compare_bridge<ExpSet1, ExpSet2>(lhs, rhs);
  }

  /// Bridge (compare).
  template <typename LabelSet1, typename LabelSet2>
  int
  compare_label(const label& lhs, const label& rhs)
  {
    return compare_bridge<LabelSet1, LabelSet2>(lhs, rhs);
  }

  /// Bridge (compare).
  template <typename PolynomialSet1, typename PolynomialSet2>
  int
  compare_polynomial(const polynomial& lhs, const polynomial& rhs)
  {
    return compare_bridge<PolynomialSet1, PolynomialSet2>(lhs, rhs);
  }

  /// Bridge (compare).
  template <typename WeightSet1, typename WeightSet2>
  int
  compare_weight(const weight& lhs, const weight& rhs)
  {
    return compare_bridge<WeightSet1, WeightSet2>(lhs, rhs);
  }
}
