#ifndef VCSN_ALGOS_REDUCE_HH
# define VCSN_ALGOS_REDUCE_HH

# include <map>
# include <unordered_map>
# include <vector>

# include <vcsn/algos/copy.hh>
# include <vcsn/algos/transpose.hh>
# include <vcsn/core/mutable-automaton.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/weightset/q.hh>
# include <vcsn/weightset/r.hh>
# include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace detail
  {
    /*
      The core algorithm computes a scaled basis from a list of
      vectors.  There is a permutation on entries in such a way that
      the first non-zero entry of the i-th vector of the basis has
      index i.  (in the field case, this entry is equal to 1)

      If v is a new vector, for every vector b_i of the bases:

      - either v[i] is zero and there is nothing to do

      - or v[i] is different from 0 in this case, v is modified as v:=
        v - v[i].b which makes v[i]=0 (in the Z case, b may also be
        modified) This is performed by the reduce_vector method

      After the iteration, if v is null, it means that the vector was in the
      vector space (or the Z-module) generated by the basis.
      Otherwise, a non zero entry of v is selected (find_pivot),
      v is normalized (normalization_vector) and the permutation is
      updated in such a way that the pivot becomes the first  non zero
      entry of v, which is inserted in the basis.

      Once the basis is stabilized, a bottom-up reduction is applied
      to the scaled basis in order to make it more "diagonal".
      Finally, the new automaton is built w.r.t. the computed basis.
      The construction of the automaton can not be done on-the-fly due
      to both the Z case where the basis is modified and the bottom-up
      reduction.
    */


    /*

     The implementation of elementary methods slightly differ
     w.r.t. the weightset.

     Generically,

       find_pivot returns the first non zero entry

       reduce_vector computes current := current - current[i].b_i

       normalisation_vector applied a ratio in such a way that
       the pivot is 1

       bottom_up_reduction applies, for each vector of the
       basis, reduce_vector to every preceding vector in the
       basis.


       In Q : find_pivot searchs for the entry where
       |numerator|+|denominator| is minimal

       In R : find_pivot searchs for the entry x where |x|+1/|x|
       is minimal

       In Z : find_pivot searchs for the (non zero) entry x where
       |x| is minimal
       reduce_vector both reduce the current and the basis vector
         (w.r.t. the gcd of current[i] and b_i[i])
       normalisation_vector does not apply
       bottom_up_reduction does not apply

    */

    template<typename Weightset>
    struct select
    {
      template<typename Reduc, typename Vector>
      static unsigned
      find_pivot(Reduc* that, const Vector& v,
                 unsigned begin, unsigned* permutation)
      {
        return that->find_pivot(v, begin, permutation);
      }

      template<typename Reduc, typename Vector>
      static void
      reduce_vector(Reduc* that, Vector& vbasis,
                    Vector& current, unsigned b, unsigned* permutation)
      {
        that->reduce_vector(vbasis, current, b, permutation);
      }

      template<typename Reduc, typename Vector>
      static void
      normalisation_vector(Reduc* that, Vector& v,
                           unsigned pivot, unsigned* permutation)
      {
        that->normalisation_vector(v, pivot, permutation);
      }

      template<typename Reduc, typename Basis>
      static void
      bottom_up_reduction(Reduc* that, Basis& basis, unsigned* permutation)
      {
        that->bottom_up_reduction(basis, permutation);
      }

      template<typename Reduc, typename Basis, typename Vector>
      static void
      vector_in_new_basis(Reduc* that, Basis& basis,
                          Vector& current, Vector& new_vector,
                          unsigned* permutation)
      {
        that->vector_in_new_basis(basis, current, new_vector, permutation);
      }
    };

    template <>
    struct select<q> : select<void>
    {
      template<typename Reduc, typename Vector>
      static unsigned
      find_pivot(Reduc* that, const Vector& v,
                 unsigned begin, unsigned* permutation)
      {
        return that->find_pivot_by_norm(v, begin, permutation);
      }
    };

    template <>
    struct select<r> : select<void>
    {
      template<typename Reduc, typename Vector>
      static unsigned
      find_pivot(Reduc* that, const Vector& v,
                 unsigned begin, unsigned* permutation)
      {
        return that->find_pivot_by_norm(v, begin, permutation);
      }
    };

    template <>
    struct select<z> : select<void>
    {
      template<typename Reduc, typename Vector>
      static unsigned
      find_pivot(Reduc* that, const Vector& v,
                 unsigned begin, unsigned* permutation)
      {
        return that->find_pivot_by_norm(v, begin, permutation);
      }

      template <typename Reduc, typename Vector>
      static void
      reduce_vector(Reduc* that, Vector& vbasis,
                    Vector& current, unsigned b, unsigned* permutation)
      {
        that->z_reduce_vector(vbasis, current, b, permutation);
      }

      template <typename Reduc, typename Vector>
      static void normalisation_vector(Reduc*, Vector&, unsigned, unsigned*)
      {}

      template<typename Reduc, typename Basis>
      static void bottom_up_reduction(Reduc*, Basis&, unsigned*)
      {}

      template<typename Reduc, typename Basis, typename Vector>
      static void
      vector_in_new_basis(Reduc* that, Basis& basis,
                          Vector& current, Vector& new_vector,
                          unsigned* permutation)
      {
        that->z_vector_in_new_basis(basis, current, new_vector, permutation);
      }
    };

    template <typename Aut>
    class left_reductioner
    {
      static_assert(labelset_t_of<Aut>::is_free(),
                    "reduce: requires free labelset");

      using automaton_t = Aut;
      using context_t = context_t_of<automaton_t>;
      using weightset_t = typename context_t::weightset_t;

    public:
      using output_automaton_t = typename automaton_t::element_type::automaton_nocv_t;

    private:
      using label_t = label_t_of<automaton_t>;
      using state_t = state_t_of<automaton_t>;
      using output_state_t = state_t_of<output_automaton_t>;
      using weight_t = typename context_t::weight_t;
      using vector_t = std::vector<weight_t>;
      using matrix_t = std::vector<std::map<std::size_t, weight_t> > ;
      using matrix_set_t = std::map<label_t, matrix_t>;

    public:
      left_reductioner(const automaton_t& input)
        : input_(input)
        , res_(make_shared_ptr<output_automaton_t>(input_->context()))
      {}

      /// Create the linear representation of the input
      void linear_representation()
      {
        std::unordered_map<state_t, unsigned> state_to_index;
        unsigned i = 0;
        for (auto s: input_->states())
          state_to_index[s] = i++;
        dimension = i;
        if (dimension == 0)
          return;
        init.resize(i);
        final.resize(i);
        // Computation of the initial vector.
        for (auto t : input_->initial_transitions())
          init[state_to_index[input_->dst_of(t)]] = input_->weight_of(t);
        // Computation of the final vector.
        for (auto t : input_->final_transitions())
          final[state_to_index[input_->src_of(t)]] = input_->weight_of(t);
        // For each letter, we define an adjency matrix.
        for (auto t : input_->transitions())
          {
            auto it = letter_matrix_set.find(input_->label_of(t));
            if (it == letter_matrix_set.end())
              it = letter_matrix_set.emplace(input_->label_of(t),
                                             matrix_t(dimension)).first;
            it->second[state_to_index[input_->src_of(t)]]
              [state_to_index[input_->dst_of(t)]] = input_->weight_of(t);
          }
      }

      //utility methods

      /// Computes the product of a row vector with a matrix
      void product_vector_matrix(const vector_t& v,
                                 const matrix_t& m,
                                 vector_t& res)
      {
        for (unsigned i = 0; i < dimension; i++)
          for (auto it : m[i])
            {
              unsigned j = it.first;
              res[j] = ws_.add(res[j], ws_.mul(v[i], it.second));
            }
      }

      /// Computes the scalar product of two vectors.
      weight_t scalar_product(const vector_t& v,
                              const vector_t& w)
      {
        weight_t res = ws_.zero();
        for (unsigned i = 0; i < dimension; ++i)
          res = ws_.add(res, ws_.mul(v[i], w[i]));
        return res;
      }

      ///  Specializations for Q and R.
      using z_weight_t = vcsn::detail::z_impl::value_t; // int or long
      using q_weight_t = vcsn::detail::q_impl::value_t;
      using r_weight_t = vcsn::detail::r_impl::value_t; // = double

      /*
        The pivot is the entry x of the vector such that norm(x) is minimal.
        This "norm" highly depends on the semiring.

        For rational numbers, it is the sum of the numerator and the
        denominator (absolute value)

        For "real" numbers, it is |x|+|1/x| .
      */

      static weight_t norm(const q_weight_t& w)
      {
        return w.den+abs(w.num);
      }

      /// Norm for real numbers; a "stable" pivot should minimize this norm.
      static weight_t norm(const r_weight_t& w)
      {
        return fabs(w)+fabs(1/w);
      }

      /// Norm for integers.
      static weight_t norm(const z_weight_t& w)
      {
        return abs(w);
      }

      // Works for both Q and R.
      unsigned
      find_pivot_by_norm(const vector_t& v, unsigned begin,
                         unsigned* permutation)
      {
        weight_t min;
        unsigned pivot = dimension;
        for (unsigned i = begin; i < dimension; ++i)
          if (!ws_.is_zero(v[permutation[i]])
              && (pivot == dimension
                  || ws_.less_than(norm(v[permutation[i]]), min)))
            {
              pivot = i;
              min = norm(v[permutation[i]]);
            }
        return pivot;
      }

      // End of Q/R specializations.


      // Specialization for Z.

      // Gcd function that also computes the Bezout coefficients.
      // Used in the z reduction.
      static z_weight_t
      gcd(z_weight_t x, z_weight_t y, z_weight_t& a, z_weight_t& b)
      {
        z_weight_t res = 0;
        //gcd = ax + by
        if (y == 0)
          {
            a = 1;
            b = 0;
            res = x;
          }
        else if (x < 0)
          {
            res = gcd(-x, y, a, b);
            a = -a;
          }
        else if (y < 0)
          {
            res = gcd(x, -y, a, b);
            b = -b;
          }
        else if (x < y)
          res = gcd(y, x, b, a);
        else
          {
            z_weight_t z = x % y;  // z= x- (x/y)*y;
            res = gcd(y, z, b, a);
            //res=by+az = (b-a(x/y)) y + ax
            b -= a * (x / y);
          }
        assert(res == a * x + b * y);
        return res;
      }

      /*
        gcd = a.vbasis[pivot] + b.current[pivot]
        current[pivot] is made zero with a unimodular linear transformation
        This also modifies the basis vector
        [ vbasis  ] := [        a                      b       ] . [ vbasis  ]
        [ current ]    [-current[pivot]/gcd   vbasis[pivot]/gcd]   [ current ]
      */
      void z_reduce_vector(vector_t& vbasis, vector_t& current,
                           unsigned nb, unsigned* permutation)
      {
        unsigned pivot = permutation[nb]; //pivot of vector vbasis
        if (ws_.is_zero(current[pivot]))
          return;
        weight_t bp = vbasis[pivot];
        weight_t cp = current[pivot];
        weight_t a,b;
        weight_t g = gcd(bp, cp, a, b);
        bp /= g;
        cp /= g;
        for (unsigned i = nb; i < dimension; ++i)
          {
            weight_t tmp = current[permutation[i]];
            current[permutation[i]] = bp*tmp - cp*vbasis[permutation[i]];
            vbasis[permutation[i]] = a*vbasis[permutation[i]] + b*tmp;
          }
      }

      void z_vector_in_new_basis(std::vector<vector_t>& basis,
                                 vector_t& current, vector_t& new_vector,
                                 unsigned* permutation)
      {
        for (unsigned b = 0; b < basis.size(); ++b)
          {
            vector_t& vbasis = basis[b];
            unsigned pivot = permutation[b]; //pivot of vector vbasis
            new_vector[b] = current[pivot] / vbasis[pivot];
            if (!ws_.is_zero(new_vector[b]))
              {
                current[pivot] = ws_.zero();
                for (unsigned i = b+1; i < dimension; ++i)
                  current[permutation[i]]
                    -= new_vector[b] * vbasis[permutation[i]];
              }
          }
      }
      // End of Z specializations.

     /* Generic subroutines.
         These methods are written for any (skew) field.
         Some are specialized for Q and R for stability issues.
         Some are specialized for Z which is an Euclidean domain.
      */

      /// Return the first (w.r.t the column permutation) non zero
      /// element as pivot.
      unsigned find_pivot(const vector_t& v, unsigned begin,
                          unsigned* permutation)
      {
        for (unsigned i = begin; i < dimension; ++i)
          if (!ws_.is_zero(v[permutation[i]]))
            return i;
        return dimension;
      }

      /** Reduce a vector w.r.t. a vector of the basis.

        When this method is called, in vbasis and current,
        all the entries that come before (w.r.t the permutation) the pivot
        are zero.
        Moreover, vbasis[pivot]=1
        This method computes current := current - current[pivot].vbasis
      */
      weight_t reduce_vector(vector_t& vbasis,
                             vector_t& current, unsigned b,
                             unsigned* permutation)
      {
        unsigned pivot = permutation[b]; //pivot of vector vbasis
        weight_t ratio = current[pivot];//  vbasis[pivot] is one
        if (ws_.is_zero(ratio))
          return ratio;
        // This is safer than current[p] = current[p]-ratio*vbasis[p];
        current[pivot] = ws_.zero();
        for (unsigned i = b+1; i < dimension; ++i)
          current[permutation[i]]
            = ws_.sub(current[permutation[i]],
                      ws_.mul(ratio, vbasis[permutation[i]]));
        return ratio;
      }

      /// Normalize the basis vector such that its pivot is equal to 1.
      void normalisation_vector(vector_t& v, unsigned pivot,
                                unsigned* permutation)
      {
        weight_t k = v[permutation[pivot]];
        if (!ws_.is_one(k))
          {
            v[permutation[pivot]] = ws_.one();
            for (unsigned r = pivot + 1; r < dimension; ++r)
              v[permutation[r]] = ws_.rdiv(v[permutation[r]], k);
          }
      }

      /// Apply reduction to vectors of the basis to maximize the
      /// number of zeros.
      void bottom_up_reduction(std::vector<vector_t>& basis,
                               unsigned* permutation)
      {
        for (unsigned b = basis.size()-1; 0 < b; --b)
          for (unsigned c = 0; c < b; ++c)
            reduce_vector(basis[b], basis[c], b, permutation);
      }

      /// Compute the coordinate of a vector in the new basis.
      void vector_in_new_basis(std::vector<vector_t>& basis,
                               vector_t& current, vector_t& new_vector,
                               unsigned* permutation)
      {
        for (unsigned b = 0; b < basis.size(); ++b)
          new_vector[b] = reduce_vector(basis[b], current, b, permutation);
      }

      /** Core algorithm
          This algorithm computes a basis of I.mu(w).
          The basis is scaled.
          An automaton where states correspond to the vectors of this basis is built
       */
    public:
      output_automaton_t operator()()
      {
        // Used to select the proper overload depending on weightset_t.
        using type_t = select<weightset_t>;

        linear_representation();
        // The basis is a list of vectors, each vector is associated with
        // a state of the output
        std::vector<vector_t> basis;
        // The permutation array corresponds to a permutation of the indices
        // (i.e. the columns of the matrices)
        // such that permtuation[k] is the pivot of the k-th vector of the basis.
        unsigned permutation[dimension];
        for (unsigned i = 0; i < dimension; ++i)
          permutation[i] = i;
        // If the initial vector is null, the function immediatly returns

        // A non zero entry is chosen as pivot
        unsigned pivot = type_t::find_pivot(this, init, 0, permutation);
        if (pivot == dimension) //all components of init are 0
          return res_;
        // The pivot of the first basis vector is permutation[0];
        permutation[0] = pivot;
        permutation[pivot] = 0;
        // The initial vector is the first element of the new basis
        // (up to the normalisation w.r.t the pivot)
        vector_t first(init);
        type_t::normalisation_vector(this, first, 0, permutation);
        basis.push_back(first);
        // To each vector of the basis, all the successor vectors are
        // computed, reduced to respect to the basis, and finally, if
        // linearly independant, pushed at the end of the basis
        // itself.
        for (unsigned nb = 0; nb < basis.size(); ++nb)
          // All the vectors basis[nb].mu(a) are processed
          for (auto mu : letter_matrix_set) //mu is a pair (letter,matrix)
            {
              vector_t current(dimension);
              product_vector_matrix(basis[nb], mu.second, current);
              //reduction of current w.r.t each basis vector;
              for (unsigned b = 0; b < basis.size(); ++b)
                type_t::reduce_vector(this, basis[b],
                                      current, b, permutation);
              // After reduction, we put current in the basis if it is
              // not null and we search for the pivot of current.
              pivot = type_t::find_pivot(this, current, basis.size(),
                                         permutation);
              if (pivot != dimension) //otherwise, current is null
                {
                  if (pivot != basis.size())
                    std::swap(permutation[pivot], permutation[basis.size()]);
                  type_t::normalisation_vector(this, current,
                                               basis.size(), permutation);
                  basis.push_back(current);
                }
            }

        // now, we use each vector to reduce the preceding vectors in
        // the basis.  If weightset=Z we do not do it.
        type_t::bottom_up_reduction(this, basis, permutation);

        // Construction of the output automaton
        // 1. States
        std::vector<output_state_t> states(basis.size());
        for (unsigned b = 0; b < basis.size(); ++b)
          states[b] = res_->new_state();
        // 2. Initial vector
        vector_t vect_new_basis(basis.size());
        type_t::vector_in_new_basis(this, basis, init,
                                    vect_new_basis, permutation);
        for (unsigned b = 0; b < basis.size(); ++b)
          res_->set_initial(states[b], vect_new_basis[b]);
        // 3. Each vector of the basis is a state; computation of the
        // final function and the successor function
        for (unsigned v = 0; v < basis.size(); ++v)
          {
            weight_t k = scalar_product(basis[v],final);
            if(!ws_.is_zero(k))
              res_->set_final(states[v],k);
            for (auto mu : letter_matrix_set)
              {
                // mu is a pair (letter,matrix).
                vector_t current(dimension);
                product_vector_matrix(basis[v], mu.second, current);
                type_t::vector_in_new_basis(this, basis, current,
                                            vect_new_basis, permutation);
                for (unsigned b = 0; b < basis.size(); ++b)
                  res_->new_transition(states[v], states[b],
                                       mu.first, vect_new_basis[b]);
              }
        }
        return res_;
      }

    private:
      automaton_t input_;
      const weightset_t_of<automaton_t> ws_ = *input_->weightset();

      output_automaton_t res_;

      // Linear representation of the input.
      unsigned dimension;
      vector_t init;
      vector_t final;
      matrix_set_t letter_matrix_set;
    };

  }

  template <typename Aut>
  auto
  left_reduce(const Aut& input)
    -> typename detail::left_reductioner<Aut>::output_automaton_t
  {
    detail::left_reductioner<Aut> left_reduce(input);
    return left_reduce();
  }

  template <typename Aut>
  auto
  right_reduce(const Aut& input)
    -> typename detail::left_reductioner<Aut>::output_automaton_t
  {
    return left_reduce(transpose(input));
  }


  template <typename Aut>
  auto
  reduce(const Aut& input)
    -> typename detail::left_reductioner<Aut>::output_automaton_t
  {
    return left_reduce(transpose(right_reduce(input)));
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut>
      automaton
      reduce(const automaton& aut)
      {
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::reduce(a));
      }

      REGISTER_DECLARE(reduce,
                       (const automaton& aut) -> automaton);
    }
  }

}

#endif
