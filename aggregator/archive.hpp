#ifndef MAP_ELITES_AGGREGATOR_ARCHIVE_HPP
#define MAP_ELITES_AGGREGATOR_ARCHIVE_HPP

#include <ssrc/spatial/kd_tree.h>
#include "tool.hpp"
#include "tbb/parallel_for_each.h"


namespace sferes
{
  namespace aggregator{

    template<typename Phen, typename Params>
    class Archive{
    public:
      typedef boost::shared_ptr<Phen> indiv_t;
      typedef typename std::vector<indiv_t> pop_t;
      typedef typename pop_t::iterator it_t;
      typedef std::array<float, Params::ea::behav_dim> point_t;
      typedef ssrc::spatial::kd_tree<point_t, indiv_t> Tree;
  

      Archive(){}

      std::vector<indiv_t> get_full_content()
      {
	std::vector<indiv_t> content;
	for(auto it=_archive.begin(); it!=_archive.end();it++)
	  content.push_back((*it).second);
            //std::cout<<"full " <<(*i)->fit().novelty()<<std::endl;                                                                                                       
          
	return content;
      }
      
      template<typename Behavior,typename Point>
      static void _behavior_to_point(const Behavior& b, Point* p)
      {
	assert(p->size() == b.size());
	for (size_t i = 0; i < p->size(); ++i)
	  (*p)[i] = b[i];
      }


      bool add_to_archive(indiv_t i1, indiv_t parent)
      {
	
	//TODO
	// update archive
	if (_archive.size()<Params::nov::k || get_novelty(i1,_archive) > 0.01)
	  {
	    point_t p;
	    this->_behavior_to_point(i1->fit().desc(), &p);
	    typename Tree::iterator it = _archive.find(p);
	    if (it == _archive.end())
	      {
		_archive.insert(p, i1);
		_archive.optimize();
		return true;
	      }
	  }
	else
	  return false;
	    
      
      return false;
      
    }

      void update_novelty(){
	std::cout<<_archive.size()<<std::endl;
	// add current pop to the tree// ?? NO!
	/*parallel::init();
	Tree apop = _archive;
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    point_t p;
	    novelty::_behavior_to_point(ea.pop()[i]->fit().desc(), &p);
	    typename Tree::iterator it = apop.find(p);
	    if (it != apop.end())
	      it->second ++;
	    else
	      apop.insert(p, 1);
	  }
	apop.optimize();
	*/
	//tbb::parallel_for( tbb::blocked_range<indiv_t*>(_array.data(),_array.data() + _array.num_elements()),
	//                 Par_novelty<Map<Phen,Params> >(*this));

	/*tbb::parallel_for_each(_archive.begin(),_archive.end(), [=](typename Tree::value_type& val ) {
	    double nov= Archive::get_novelty(val.second->second,_apop);
	    val.second->fit().set_novelty(nov);
	    } );*/
	
	tbb::parallel_for_each(_archive.begin(),_archive.end(),_p_novelty(_archive));

	// _p_novelty<typename phen_t, Tree, Params> pnov(get_full_content(), apop);
	// parallel::p_for(parallel::range_t(0, ea.pop().size()), pnov);
      }
    
      
       struct _p_novelty
      {
	const Tree& _apop;
	_p_novelty(const Tree& apop) : 
	   _apop(apop) {}
	_p_novelty(const _p_novelty& ev) : 
	  _apop(ev._apop) {}
	
	// use the Euclidean distance !
	template<typename value_type>
	void operator() (value_type& v) const
	{
	  // for (auto it = r.begin(); it != r.end(); ++it)
	  //{
	     double nov= Archive::get_novelty(v.second,_apop);
	     v.second->fit().set_novelty(nov);
	     //}
	}
      };
      
    
      static double get_novelty(const indiv_t& indiv, const Tree&  apop) {
	pop_t nearest;
	typename Tree::key_type q;
	Archive::_behavior_to_point(indiv->fit().desc(), &q);
	// k nearest points
	typedef typename Tree::knn_iterator knn_iterator_t;
	std::pair<knn_iterator_t, knn_iterator_t> 
	  range = apop.find_nearest_neighbors(q, Params::nov::k, false);
	
	nearest.clear();
	for(knn_iterator_t it = range.first, end = range.second; it != end && nearest.size() < Params::nov::k; ++it)
	  //for (size_t z = 0; z < it->second && nearest.size() < Params::nov::k; ++z)
	  nearest.push_back(it->second);
	
	assert(nearest.size() == Params::nov::k);
	
	// compute the mean distance                                                                                                                                         
	double nov = 0.0f;                                                                                                                                                    
	BOOST_FOREACH(indiv_t& x, nearest)                                                                                                                   
	  nov += _dist(x->fit().desc(), indiv->fit().desc());                                                                                                                                                
	nov /= Params::nov::k;      
	
	return nov;
    }

      const Tree& archive() const{return _archive;}
  
    protected:
      Tree _archive;

    };
  }
}

#endif
    
