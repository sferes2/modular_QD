#ifndef MAP_ELITES_SELECTOR_RANDOM_HPP
#define MAP_ELITES_SELECTOR_RANDOM_HPP



namespace sferes
{
namespace selector{
  template <typename Phen>
  struct Random{

    typedef boost::shared_ptr<Phen> indiv_t;
    
    void init(const std::vector<indiv_t>& pop){
      _pop= &pop;
    }
    indiv_t operator()()const
    {
      int x1 = misc::rand< int > (0, _pop->size());
      return _pop->operator[](x1);
    }
    const std::vector<indiv_t>*  _pop;
  };
}
}
#endif
