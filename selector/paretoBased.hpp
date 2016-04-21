#ifndef MAP_ELITES_SELECTOR_PARETOBASED_HPP
#define MAP_ELITES_SELECTOR_PARETOBASED_HPP

#include <sferes/ea/dom_sort.hpp>
#include <sferes/ea/crowd.hpp>

namespace sferes
{
  namespace selector
  {
    template <typename Phen, typename ObjSelector, typename Params>
    struct ParetoBased{
      
      typedef boost::shared_ptr<ea::crowd::Indiv<Phen> > indiv_t;
      typedef typename std::vector<indiv_t> pop_t;
      typedef typename pop_t::iterator it_t;
      typedef typename std::vector<std::vector<indiv_t> > front_t;

      
      template<typename EA>
      void operator()(std::vector<boost::shared_ptr<Phen> >& new_pop,const EA& ea)const{
	assert(ea.parents().size());
	assert(ea.offspring().size());
	//void _fill_nondominated_sort(pop_t& mixed_pop, pop_t& new_pop) {
	pop_t parents, offspring, mixed_pop;
	_pop2crow(ea.parents(), parents);
	_pop2crow(ea.offspring(), offspring);
	_merge(parents, offspring, mixed_pop);
	_update_obj(mixed_pop);
	
	assert(mixed_pop.size());
	front_t fronts;
	
	_rank_crowd(mixed_pop, fronts);
	new_pop.clear();
	
	// fill the i first layers
	size_t i;
	for (i = 0; i < fronts.size(); ++i)
	  if (fronts[i].size() + new_pop.size() < Params::pop::size)
	    new_pop.insert(new_pop.end(), fronts[i].begin(), fronts[i].end());
	  else
	    break;
	
	size_t size = Params::pop::size - new_pop.size();
	// sort the last layer
	if (new_pop.size() < Params::pop::size) 
	  {
	    std::sort(fronts[i].begin(), fronts[i].end(), ea::crowd::compare_crowd());
	  for (size_t k = 0; k < size ; ++k)
	    {
	      assert(i < fronts.size());
	      new_pop.push_back(fronts[i][k]);
	    }
	  }
	assert(new_pop.size() == Params::pop::size);

      }


      void _rank_crowd(pop_t& pop, front_t& fronts) const {
        std::vector<size_t> ranks;

	ea::dom_sort(pop, fronts, ranks);
        //_update_pareto_front(fronts);
        parallel::p_for(parallel::range_t(0, fronts.size()),
                        ea::crowd::assign_crowd<indiv_t >(fronts));

        for (size_t i = 0; i < ranks.size(); ++i)
          pop[i]->set_rank(ranks[i]);
        parallel::sort(pop.begin(), pop.end(), ea::crowd::compare_ranks());;
      }

      void _pop2crow(const std::vector<boost::shared_ptr<Phen> >& pop, pop_t& pop2 )const  {
        assert(!pop.empty());
        pop2.resize(pop.size());
        for (size_t i = 0; i < pop.size(); ++i)
          pop2[i] = boost::shared_ptr<ea::crowd::Indiv<Phen> >(new ea::crowd::Indiv<Phen>(*pop[i]));
      }
      
      void _crow2pop(const pop_t& pop1,
                        std::vector<boost::shared_ptr<Phen> > & pop2) const{
        pop2.resize(pop1.size());
        for (size_t i = 0; i < pop1.size(); ++i)
          pop2[i] = pop1[i];
      }

      //
      void _merge(const pop_t& pop1, const pop_t& pop2, pop_t& pop3) const {
        assert(pop1.size());
        assert(pop2.size());
        pop3.clear();
        pop3.insert(pop3.end(), pop1.begin(), pop1.end());
        pop3.insert(pop3.end(), pop2.begin(), pop2.end());
        assert(pop3.size() == pop1.size() + pop2.size());
      }

      struct ObjUpdator{
	ObjUpdator(indiv_t& indiv):_indiv(indiv){}
	indiv_t& _indiv; 
	template<typename T>
	void operator()(T& x) const{
	  _indiv->fit().add_obj();
	  _indiv->fit().set_obj(_indiv->fit().objs().size() - 1 , x.getValue(_indiv));
	}
      };
      void _update_obj(pop_t& pop) const
      {
	for (auto& indiv : pop)
	  {
	    if(indiv->fit().objs().size() != boost::fusion::size(_obj_selector))
	       boost::fusion::for_each(_obj_selector, ObjUpdator(indiv));
	    //for(auto& obj: indiv->fit().objs())
	    //  std::cout<<obj<<"  ";
	    //std::cout<<std::endl;
	  }
      }

      ObjSelector _obj_selector;
    };
  }
}
#endif
