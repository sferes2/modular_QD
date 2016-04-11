#ifndef MAP_ELITES_SELECTOR_NOSELECTION_HPP
#define MAP_ELITES_SELECTOR_NOSELECTION_HPP



namespace sferes
{
namespace selector{
  template <typename Phen>
  struct NoSelection{

    typedef boost::shared_ptr<Phen> indiv_t;
    
    template<typename EA>
    void operator()(std::vector<indiv_t>& pop,const EA& ea)const{
      for (auto& indiv : pop)
	{
	  indiv = boost::shared_ptr<Phen>(new Phen());
	  indiv->random();
        }
    }
  };
}
}
#endif
