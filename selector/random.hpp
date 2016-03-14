#ifndef MAP_ELITES_SELECTOR_RANDOM_HPP
#define MAP_ELITES_SELECTOR_RANDOM_HPP



namespace sferes
{
namespace selector{
  struct Random{
    
    template <typename indiv_t>
    indiv_t operator()(const std::vector<indiv_t>& pop)const
    {
      int x1 = misc::rand< int > (0, pop.size());
      return pop[x1];
    }
  };
}
}
#endif
