#ifndef UPDATER_QD_HPP
#define UPDATER_QD_HPP

#include <vector>
#include <boost/shared_ptr.hpp>

namespace sferes
{
namespace updater
{
template <typename Phen, typename Container, typename NoveltyUpdater, typename LQUpdater, typename CuriosityUpdater, typename Params>
class UpdaterQD
{
  public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;

    void operator(Container &cont, pop_t &offspring, pop_t &parents, const std::vector<bool> &added) const
    {
        // Some containers might need a first optimization step
        cont.optimize();

        // Create a novelty struct

        // Update the individuals in the container

        // Update the offspring

        // Update the parents

        
    }

protected:
    
};
} // namespace updater
} // namespace sferes

#endif
