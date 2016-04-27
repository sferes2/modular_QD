#ifndef _SELECTOR_TOURNAMENTBASED_HPP
#define _SELECTOR_TOURNAMENTBASED_HPP


namespace sferes{
  namespace selector{

    template <typename Phen, typename ValueSelector=getFitness>
    class TournamentBased{
    public:
      typedef boost::shared_ptr<Phen> indiv_t;      
      typedef typename std::vector<indiv_t >::iterator it_t;
      typedef typename std::vector<indiv_t >::const_iterator itc_t;


      template<typename EA>
      void operator()(std::vector<indiv_t>& pop,const EA& ea){
	this->operator()(pop,ea.pop());
      }
      
      void operator()(std::vector<indiv_t>& pop,const std::vector<indiv_t>& ea_pop){
	for (auto& indiv : pop)
	  {
	    int x1 = misc::rand< int > (0, ea_pop.size());
	    int x2 = misc::rand< int > (0, ea_pop.size());
	    if(ValueSelector::getValue(ea_pop[x1]) > ValueSelector::getValue(ea_pop[x2]))
	      indiv=ea_pop[x1];
	    else
	      indiv=ea_pop[x2];
          
	  }

      }

    };

  }}
#endif
