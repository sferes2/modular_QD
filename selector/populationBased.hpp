#ifndef MAP_ELITES_SELECTOR_POPULATIONBASED_HPP
#define MAP_ELITES_SELECTOR_POPULATIONBASED_HPP


#include "scoreProportionate.hpp"

namespace sferes
{
namespace selector
{
  template <typename Phen, typename Selector= ScoreProportionate<getFitness> >
  struct PopulationBased{
    
    
    typedef boost::shared_ptr<Phen> indiv_t;
    
    /*struct Comparator{
      template<typename T1, typename T2>
      bool operator()(const T1& t1, const T2& t2)const
      {
	return ValueSelector::getValue(t1) > ValueSelector::getValue(t2); //descending order
      }
      };*/

    
    template<typename EA>
    void operator()(std::vector<indiv_t>& pop,const EA& ea)const{
      std::vector<indiv_t> temp(ea.parents().size()+ea.offspring().size());
      assert(temp.size()>=pop.size());
      
      size_t ind=0;
      for (auto& indiv : ea.parents()){
	temp[ind]=indiv;
	ind++;
      }
      for (auto& indiv : ea.offspring()){
	temp[ind]=indiv;
	ind++;
      }
      
      //std::sort(temp.begin(),temp.end(),Comparator());
      
      /*ind=0;
      for (auto& indiv : pop)
	{
	  indiv = temp[ind];
	  ind++;
	  }*/


      _selector(pop,temp);
      
    }
    Selector _selector;
  };
}
}
#endif

  
