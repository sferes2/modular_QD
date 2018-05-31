#ifndef QD_UPDATER_GRID_QD_HPP
#define QD_UPDATER_GRID_QD_HPP

#include <vector>
#include <boost/shared_ptr.hpp>
#include <container/grid.hpp>

namespace sferes
{
namespace updater
{
template <typename Phen, typename Params>
class GridQD
{
  public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;
    typedef boost::multi_array<indiv_t, Params::ea::behav_dim> array_t;
    typedef boost::array<typename array_t::index, Params::ea::behav_dim> behav_index_t;
    typedef typename array_t::template const_array_view<Params::ea::behav_dim>::type view_t;
    typedef typename container::Grid<Phen, Params> grid_t;

    void operator()(grid_t &grid, pop_t &offspring, pop_t &parents, const std::vector<bool> &added) const
    {
        assert(offspring.size() == parents.size());

        _update_novelty(grid);

        for (size_t i = 0; i < offspring.size(); i++)
            _update_indiv(offspring[i], grid);

        for (size_t i = 0; i < parents.size(); i++)
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
    void _update_novelty(grid_t &grid) const
    {
        tbb::parallel_for(tbb::blocked_range<indiv_t *>(grid.archive().data(), grid.archive().data() + grid.archive().num_elements()), Par_novelty(grid, *this));
    }

    struct Par_novelty
    {
        Par_novelty(const grid_t &grid, const GridQD &updater) : _grid(grid), _updater(updater) {}
        const grid_t &_grid;
        const GridQD &_updater;
        void operator()(const tbb::blocked_range<indiv_t *> &r) const
        {
            for (indiv_t *indiv = r.begin(); indiv != r.end(); ++indiv)
                if (*indiv)
                    _updater._update_indiv(*indiv, _grid);
        }
    };

    void _update_indiv(indiv_t &indiv, const grid_t &grid) const
    {
        if (indiv->fit().dead())
        {
            indiv->fit().set_novelty(-std::numeric_limits<double>::infinity());
            indiv->fit().set_local_quality(-std::numeric_limits<double>::infinity());
            return;
        }

        int count = 0;
        std::vector<indiv_t> neigh = grid.get_neighbors(indiv);

        indiv->fit().set_novelty(-(double)neigh.size());
        for (auto &n : neigh)
            if (n->fit().value() < indiv->fit().value())
                count++;
        indiv->fit().set_local_quality(count);
    }
};
} // namespace updater
} // namespace sferes

#endif
