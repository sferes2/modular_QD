#ifndef QD_UPDATER_GRID_QD_HPP
#define QD_UPDATER_GRID_QD_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <container/grid.hpp>

namespace sferes
{
namespace updater
{
template <typename Container, typename Phen, typename Params> // think about what other typenames are needed
class GridQD
{
  public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;
    typedef boost::multi_array<indiv_t, Params::ea::behav_dim> array_t;
    typedef boost::array<typename array_t::index, Params::ea::behav_dim> behav_index_t;
    typedef typename array_t::multi_array_base::index_range index_range_t;
    typedef boost::detail::multi_array::index_gen<Params::ea::behav_dim, Params::ea::behav_dim> index_gen_t;
    typedef typename array_t::template const_array_view<Params::ea::behav_dim>::type view_t;
    typedef typename container::Grid<Phen, Params> grid_t;

    behav_index_t behav_shape;

    // void operator()(Container &container, const pop_t &offspring, const pop_t &parents, const std::vector<bool> &added) const
    void operator()(grid_t &grid, const pop_t &offspring, const pop_t &parents, const std::vector<bool> &added)
    {
        assert(offspring.size() > 0 && offspring.size() == parents.size());

        _update_novelty(grid);

        for (size_t i = 0; i < offspring.size(); i++)
            // _update_indiv(offspring[i], *this);
            _update_indiv(offspring[i], grid);

        for (size_t i = 0; i < parents.size(); i++)
            // _update_indiv(parents[i], *this);
            _update_indiv(parents[i], grid);

        // Update the curiosity score
        if (added.size() > 0)
        {
            for (size_t i = 0; i < parents.size(); i++)
            {
                if (added[i])
                    parents[i]->fit().set_curiosity(parents[i]->fit().curiosity() + 1);
                else
                    parents[i]->fit().set_curiosity(parents[i]->fit().curiosity() - 0.5);
            }
        }
    }

  protected:
    void _update_novelty(grid_t &grid)
    {
        // tbb::parallel_for(tbb::blocked_range<indiv_t *>(_array.data(), _array.data() + _array.num_elements()),
        //                   Par_novelty<Grid<Phen, Params>>(*this));
        tbb::parallel_for(tbb::blocked_range<indiv_t *>(grid.archive().data(), grid.archive().data() + grid.archive().num_elements()),
                          Par_novelty<grid_t>(grid));
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

    template <typename Grid_t>
    struct Par_novelty
    {
        Par_novelty(const Grid_t &grid) : _grid(grid) {} //,_par_array(grid._array){}
        const Grid_t &_grid;
        //array_t& _par_array;
        void operator()(const tbb::blocked_range<indiv_t *> &r) const
        {
            for (indiv_t *indiv = r.begin(); indiv != r.end(); ++indiv)
                if (*indiv)
                {
                    /*int count =0;
                    view_t neighborhood = _grid.get_neighborhood(*indiv);
                    std::vector<indiv_t> neigh;
                    iterate(neighborhood,neigh);

                    (*indiv)->fit().set_novelty(-(double)neigh.size());
                    for(auto& n : neigh)
                    if(n->fit().value() < (*indiv)->fit().value())
                        count++;
                        (*indiv)->fit().set_local_quality(count);*/
                    _update_indiv(*indiv, _grid);
                }
        }
    };

    //WARNING, individuals in population can be dead...
    // template <typename Grid_t>
    // static void _update_indiv(indiv_t &indiv, const Grid_t &grid)
    void _update_indiv(indiv_t &indiv, const grid_t &grid)
    {
        if (indiv->fit().dead())
        {
            indiv->fit().set_novelty(-std::numeric_limits<double>::infinity());
            indiv->fit().set_local_quality(-std::numeric_limits<double>::infinity());
            return;
        }

        int count = 0;
        // view_t neighborhood = grid.get_neighborhood(indiv);
        view_t neighborhood = get_neighborhood(grid, indiv);
        std::vector<indiv_t> neigh;
        iterate(neighborhood, neigh);

        indiv->fit().set_novelty(-(double)neigh.size());
        for (auto &n : neigh)
            if (n->fit().value() < indiv->fit().value())
                count++;
        indiv->fit().set_local_quality(count);
    }

    // template <typename Grid_t>
    // view_t get_neighborhood(const Grid_t &grid, indiv_t indiv) const
    view_t get_neighborhood(const grid_t &grid, indiv_t indiv) const
    {
        behav_index_t ind = get_index(indiv);
        index_gen_t indix;
        int i = 0;
        for (auto it = indix.ranges_.begin(); it != indix.ranges_.end(); it++)
        {
            *it = index_range_t(std::max((int)ind[i] - (int)Params::nov::deep, 0), std::min(ind[i] + Params::nov::deep + 1, (size_t)behav_shape[i])); //bound! so stop at id[i]+2-1

            i++;
        }

        // view_t ngbh = _array[indix];
        view_t ngbh = grid.archive()[indix];
        return ngbh;
    }
};
} // namespace updater
} // namespace sferes

#endif
