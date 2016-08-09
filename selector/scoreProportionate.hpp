#ifndef _SELECTOR_SCOREPROPORTIONATE_HPP
#define _SELECTOR_SCOREPROPORTIONATE_HPP

#include "tbb/parallel_reduce.h"
#include "tbb/parallel_sort.h"


namespace sferes{
  namespace selector{

    template <typename Phen, typename ValueSelector=getFitness>
    class ScoreProportionate{
    public:
      typedef boost::shared_ptr<Phen> indiv_t;      
      typedef typename std::vector<indiv_t >::iterator it_t;
      typedef typename std::vector<indiv_t >::const_iterator itc_t;
      
      
      /*indiv_t operator()()const
      {
	if(min!=max){ 
	  double r= misc::rand((double) 1.0);
	  itc_t it=_pop.cbegin();
	  double p = (ValueSelector::getValue(*it)-min)/(sum-_pop.size()*min);
	  while (p<r)
	    {
	      it++;
	      p+=(ValueSelector::getValue(*it)-min)/(sum-_pop.size()*min);
	    }
	  return *it;
	}else{ //they all have the same score
	  //tools::rgen_int_t rgen(0, v.size()-1);
	  //return v[rgen.rand()];
	  int x1 = misc::rand< int > (0, _pop.size());
	  return _pop[x1];
	}
	return _pop[0];//should not be reached
	
	}*/
      template<typename EA>
      void operator()(std::vector<indiv_t>& pop,const EA& ea) const {
	this->operator()(pop,ea.pop());
      }

      void operator()(std::vector<indiv_t>& pop,const std::vector<indiv_t>& ea_pop) const {
	std::pair<double, double> res = getMinAndSum(ea_pop);
	double min=res.first;
	double sum=res.second-min*ea_pop.size();
	/*if(min!=max){ 
	  for (auto& indiv : pop)
	    {
	      double r= misc::rand((double) 1.0);
	      itc_t it=_pop.cbegin();
	      double p = (ValueSelector::getValue(*it)-min)/(sum-_pop.size()*min);
	      while (p<r)
		{
		  it++;
		  p+=(ValueSelector::getValue(*it)-min)/(sum-_pop.size()*min);
		}
	      indiv= *it;
	    }
	}else{ //they all have the same score
	  for (auto& indiv : pop)
	    {
	      int x1 = misc::rand< int > (0, _pop.size());
	      indiv=_pop[x1];
	    }	  
	    }*/
	for (auto& indiv : pop)
	  {
	    double r= misc::rand((double) sum);
	    
	    itc_t it=ea_pop.cbegin();
	    double p = (ValueSelector::getValue(*it)-min);
	    
	    while (p<r && it!=ea_pop.end())
	      {
		
		it++;
		p+=(ValueSelector::getValue(*it)-min);
	      }
	    indiv= *it;
	  }
	

	
      }

      /*struct Comparator{
	template<typename T1, typename T2>
	bool operator()(const T1& t1, const T2& t2)const
	{
	  return ValueSelector::getValue(t1) > ValueSelector::getValue(t2); //descending order
	  }
      };*/


    private:
      
      std::pair<double,double> getMinAndSum(const std::vector<indiv_t>& pop)const
      {
	/*_pop= std::vector<indiv_t>(pop);
	if(_pop.size()==0)
	  return;
	tbb::parallel_sort(_pop.begin(),_pop.end(),Comparator());
	max=ValueSelector::getValue(*_pop.begin());
	min=ValueSelector::getValue(*(--_pop.end()));
	  
	if(min!=max){
	  typedef tbb::blocked_range<it_t> range_type;
	  sum= tbb::parallel_reduce(
					   range_type( _pop.begin(), _pop.end() ), 0.0,
					   [](const range_type& r, double value)->double {
					     return std::accumulate(r.begin(),r.end(),value,[]( double const & current, indiv_t const& p)
								    { return current + ValueSelector::getValue(p); });
					   },
					   std::plus<double>()
					   );
	  
	  
					   }*/
	typedef tbb::blocked_range<itc_t> range_type;
	return tbb::parallel_reduce(
				    range_type( pop.begin(), pop.end() ), 
				    std::pair<double,double>(0,0),
				    [](const range_type& r, std::pair<double,double> value)->std::pair<double,double> {
				      //return std::accumulate(r.begin(),r.end(),value,[]( std::pair<double,double> const & current, indiv_t const& p)
				      //{ return std::pair<double,double>(std::min(current.first,ValueSelector::getValue(p)), current.second + ValueSelector::getValue(p)); }
				      
				      for( auto it=r.begin(); it!=r.end(); ++it )
					{
					  value.first=std::min(value.first,ValueSelector::getValue(*it));
					  value.second+=ValueSelector::getValue(*it);
					}
				      return value;
				      //   );
				    },
				    []( std::pair<double,double> x, std::pair<double,double> y )->std::pair<double,double> {
				    return {std::min(x.first,y.first), x.second+y.second};
				  }
				  
				  );
	

      }
      

    //std::vector< indiv_t > _pop;
      //double max;
      //double min;
      
    };

  }
}

#endif
