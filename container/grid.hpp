#ifndef QD_CONTAINER_GRID_HPP
#define QD_CONTAINER_GRID_HPP

namespace sferes
{
namespace container
{
template <typename Phen, typename Params>
class Grid
{
public:
  typedef boost::shared_ptr<Phen> indiv_t;
  typedef typename std::vector<indiv_t> pop_t;
  typedef typename pop_t::iterator it_t;

  static const size_t behav_dim = Params::ea::behav_dim;
  typedef boost::multi_array<indiv_t, behav_dim> array_t;
  typedef boost::array<typename array_t::index, behav_dim> behav_index_t;
  typedef boost::array<float, behav_dim> point_t;
  typedef typename array_t::multi_array_base::index_range index_range_t;
  typedef boost::detail::multi_array::index_gen<behav_dim, behav_dim> index_gen_t;
  typedef typename array_t::template const_array_view<behav_dim>::type view_t;

  behav_index_t behav_shape;

  Grid()
  {
    assert(behav_dim == Params::ea::behav_shape_size());

    for (size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
      behav_shape[i] = Params::ea::behav_shape(i);

    //allocate space for _array and _array_parents
    _array.resize(behav_shape);
    _array_parents.resize(behav_shape);
  }

  std::vector<indiv_t> get_neighbors(const indiv_t &indiv) const
  {
    view_t neighborhood = _get_neighborhood(indiv);
    std::vector<indiv_t> neigh;
    iterate(neighborhood, neigh);
    return neigh;
  }

  void get_full_content(std::vector<indiv_t> &content) const
  {
    for (const indiv_t *i = _array.data(); i < (_array.data() + _array.num_elements()); ++i)
      if (*i)
        content.push_back(*i);
  }

  std::vector<bool> add(const pop_t &offspring, const pop_t &parents)
  {
    std::vector<bool> added(offspring.size());

    for (size_t i = 0; i < offspring.size(); ++i)
      added[i] = _add(offspring[i], parents[i]);

    return added;
  }

  // const array_t &archive() const { return _array; }
  // const array_t &parents() const { return _array_parents; }
  array_t &archive() { return _array; }
  array_t &parents() { return _array_parents; }

protected:
  array_t _array;
  array_t _array_parents;

  // Converts the descriptor into a Point_t
  template <typename I>
  point_t get_point(const I &indiv) const
  {
    point_t p;
    for (size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
      p[i] = std::min(1.0f, indiv->fit().desc()[i]);

    return p;
  }

  template <typename I>
  float _dist_center(const I &indiv)
  {
    /* Returns distance to center of behavior descriptor cell */
    float dist = 0.0;
    point_t p = get_point(indiv);
    for (size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
      dist += pow(p[i] - (float)round(p[i] * (float)(behav_shape[i] - 1)) / (float)(behav_shape[i] - 1), 2);

    dist = sqrt(dist);
    return dist;
  }

  bool _add(indiv_t i1, indiv_t parent)
  {
    if (i1->fit().dead())
      return false;

    behav_index_t behav_pos = _get_index(i1);

    float epsilon = 0.00;
    if (!_array(behav_pos) || (i1->fit().value() - _array(behav_pos)->fit().value()) > epsilon || (fabs(i1->fit().value() - _array(behav_pos)->fit().value()) <= epsilon && _dist_center(i1) < _dist_center(_array(behav_pos))))
    {
      _array(behav_pos) = i1;
      _array_parents(behav_pos) = parent;
      return true;
    }
    return false;
  }

  behav_index_t _get_index(const indiv_t &indiv) const
  {
    point_t p = get_point(indiv);
    behav_index_t behav_pos;
    for (size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
    {
      behav_pos[i] = round(p[i] * (behav_shape[i] - 1));
      //behav_pos[i] = std::min(behav_pos[i], behav_shape[i] - 1);
      assert(behav_pos[i] < behav_shape[i]);
    }
    return behav_pos;
  }

  view_t _get_neighborhood(const indiv_t &indiv) const
  {
    behav_index_t ind = _get_index(indiv);
    index_gen_t indix;
    int i = 0;
    for (auto it = indix.ranges_.begin(); it != indix.ranges_.end(); it++)
    {
      *it = index_range_t(std::max((int)ind[i] - (int)Params::nov::deep, 0), std::min(ind[i] + Params::nov::deep + 1, (size_t)behav_shape[i])); //bound! so stop at id[i]+2-1

      i++;
    }

    view_t ngbh = _array[indix];
    return ngbh;
  }

  // Functor to iterate over a Boost MultiArray concept instance.
    template <typename T, typename V, size_t Dimensions = T::dimensionality>
    struct IterateHelper
    {
        void operator()(T &array, V &vect) const
        {
            for (auto element : array)
                IterateHelper<decltype(element), V>()(element, vect);
        }
    };

    // Functor specialization for the final dimension.
    template <typename T, typename V>
    struct IterateHelper<T, V, 1>
    {
        void operator()(T &array, V &vect) const
        {
            for (auto &element : array)
                if (element)
                    vect.push_back(element);
        }
    };

    // Utility function to apply a function to each element of a Boost
    // MultiArray concept instance (which includes views).
    template <typename T, typename V>
    static void iterate(T &array, V &vect)
    {
        IterateHelper<T, V>()(array, vect);
    }
};
} // namespace container
} // namespace sferes

#endif
