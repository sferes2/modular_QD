#ifndef QD_UPDATER_DUMMY_HPP
#define QD_UPDATER_DUMMY_HPP

#include <vector>
#include <boost/shared_ptr.hpp>

namespace sferes
{
namespace updater
{
template <typename Container, typename Phen, typename Params>
struct DummyUpdater
{
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;

    // may update the container
    void operator()(Container &container, const pop_t &offspring, const pop_t &parents, const std::vector<bool> &added) const
    {
        // Do nothing
    }
};
} // namespace updater
} // namespace sferes

#endif
