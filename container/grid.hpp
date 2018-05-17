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
  // typedef typename std::vector<std::vector<indiv_t>> front_t;

  static const size_t behav_dim = Params::ea::behav_dim;
  typedef boost::multi_array<indiv_t, behav_dim> array_t;
  typedef boost::array<typename array_t::index, behav_dim> behav_index_t;
  typedef boost::array<float, behav_dim> point_t;

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

  template <typename I>
  behav_index_t get_index(const I &indiv) const
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

  const array_t &archive() const { return _array; }
  const array_t &parents() const { return _array_parents; }

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

    behav_index_t behav_pos = get_index(i1);

    float epsilon = 0.00;
    if (!_array(behav_pos) || (i1->fit().value() - _array(behav_pos)->fit().value()) > epsilon || (fabs(i1->fit().value() - _array(behav_pos)->fit().value()) <= epsilon && _dist_center(i1) < _dist_center(_array(behav_pos))))
    {
      _array(behav_pos) = i1;
      _array_parents(behav_pos) = parent;
      return true;
    }
    return false;
  }
};
} // namespace container
} // namespace sferes

#endif
