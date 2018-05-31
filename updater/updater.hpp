#ifndef UPDATER_HPP
#define UPDATER_HPP

namespace sferes
{
namespace updater
{
template <typename Phen, typename ValueUpdater, typename Params>
class Updater
{
  public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;

    void operator(pop_t &offspring, pop_t &parents, const std::vector<bool> &added) const
    {
        ValueUpdater::set_value
        boost::fusion::accumulate(_value_updater, 0, ObjUpdator(indiv));
    }

protected:
    struct ValUpdater
    {
        ValUpdater(indiv_t &indiv) : _indiv(indiv) {}
        indiv_t &_indiv;
        template <typename T>
        size_t operator()(size_t index, T &x) const
        {
            _indiv->fit().set_obj(index, x.getValue(_indiv));
            x.set_value()
            return ++index;
        }
    };
    void _update_obj(pop_t &pop) const
    {
        for (auto &indiv : pop)
        {
            while (indiv->fit().objs().size() != boost::fusion::size(_obj_selector))
                indiv->fit().add_obj();
            boost::fusion::accumulate(_obj_selector, 0, ObjUpdator(indiv));
        }
    }

    ValueUpdater _value_updater;
};
} // namespace updater
} // namespace sferes

#endif
