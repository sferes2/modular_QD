#ifndef QD_VARIATOR_STANDARD_HPP
#define QD_VARIATOR_STANDARD_HPP

#include <vector>
#include <boost/shared_ptr.hpp>

namespace sferes
{
namespace variator
{
template <typename Phen, typename Params>
struct Standard
{
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;

    void operator()(pop_t& offspring, const pop_t& parents) const
    {
        offspring.clear();

		// Generation of the offspring
		std::vector<size_t> a;
		misc::rand_ind(a, parents.size());
		for (size_t i = 0; i < Params::pop::size; i += 2)
		{
			indiv_t i1, i2;
			parents[a[i]]->cross(parents[a[i + 1]], i1, i2);
			i1->mutate();
			i2->mutate();
			i1->develop();
			i2->develop();
			offspring.push_back(i1);
			offspring.push_back(i2);
		}
    }
};
} // namespace variator
} // namespace sferes

#endif
