#ifndef _UPDATER_VALUEUPDATER_HPP
#define _UPDATER_VALUEUPDATER_HPP

namespace sferes
{
namespace updater
{
struct dummy
{
    template <typename Phen>
    static inline void set_value(Phen p, const double &v)
    {
        //do nothing
    }
};

struct novelty
{
    template <typename Phen>
    static inline void set_value(Phen p, const double &novelty)
    {
        p->fit().set_novelty(novelty);
    }
};

struct curiosity
{
    template <typename Phen>
    static inline void set_value(Phen p, const double &curiosity)
    {
        p->fit().set_curiosity(curiosity);
    }
};
struct local_quality
{
    template <typename Phen>
    static inline void set_value(Phen p, const double &lq)
    {
        p->fit().set_local_quality(lq);
    }
};
} // namespace updater
} // namespace sferes
#endif
