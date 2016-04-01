#ifndef HAMMER_SELECTOR_NOVELTY_HPP
#define HAMMER_SELECTOR_NOVELTY_HPP

#include "tbb/parallel_reduce.h"
#include "tbb/parallel_sort.h"


namespace sferes{
  namespace selector{
    template <typename Phen>
    struct Novelty{
      typedef boost::shared_ptr<Phen> indiv_t;
      typedef typename std::vector<indiv_t >::iterator it_t;
      typedef typename std::vector<indiv_t >::const_iterator itc_t;



      void init(const std::vector<indiv_t>& pop)
      {
	_pop= std::vector<indiv_t>(pop);
	if(_pop.size()==0)
	  return;
	tbb::parallel_sort(_pop.begin(),_pop.end(),Comparator());
	_max=(*_pop.begin())->fit().novelty();
	_min=(*(--_pop.end()))->fit().novelty();
	

	if(_min!=_max){
	  //	  tools::rgen_double_t rgen(0.0, 1.0);
	  //double r = rgen.rand(); 
	  

	  _sum=0;
	  for(it_t it=_pop.begin(); it!=_pop.end();++it)
	    {
	      _sum+=((*it)->fit().novelty()-_min)/(_max-_min);
	      _map[_sum]=*it;
	    }
	  
	  /*typedef tbb::blocked_range<it_t> range_type;
	  _sum= tbb::parallel_reduce(
				    range_type( _pop.begin(), _pop.end() ), 0.0,
				    [](const range_type& r, double novelty)->double {
				      return std::accumulate(r.begin(),r.end(),novelty,[]( double const & current, indiv_t const& p)
							     { return current + p->fit().novelty(); });
				    },
				    std::plus<double>()
				    );*/
	  
	}
      }
      
      indiv_t operator()()const
      {
	if(_min!=_max){ 
	  double r= misc::rand((double) 1.0);
	  /*itc_t it=_pop.cbegin();
	  double p = ((*it)->fit().novelty()-_min)/(_sum-_pop.size()*min);
	  while (p<r)
	    {
	      it++;
	      p+=((*it)->fit().novelty()-_min)/(_sum-_pop.size()*_min);
	    }
	    return *it;*/

	  return _map.lower_bound(r*_sum)->second;
	}else{ //they all have the same score
	  //tools::rgen_int_t rgen(0, v.size()-1);
	  //return v[rgen.rand()];
	  int x1 = misc::rand< int > (0, _pop.size());
	  return _pop[x1];
	}
	return _pop[0];//should not be reached
	
      }
      struct Comparator{
	template<typename T1, typename T2>
	bool operator()(const T1& t1, const T2& t2)const
	{
	  return (t1)->fit().novelty() > (t2)->fit().novelty(); //descending order
	}


      };


      std::vector< indiv_t > _pop;
      std::map<double,indiv_t> _map;
    double _max;
    double _min;
    double _sum;
    };

  }
}

#endif
