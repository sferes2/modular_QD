#ifndef MAP_ELITES_AGGREGATOR_ARCHIVE_HPP
#define MAP_ELITES_AGGREGATOR_ARCHIVE_HPP


#include <sferes/stc.hpp>
#include <ssrc/spatial/kd_tree.h>
#include <boost/timer.hpp>

namespace sferes
{
  namespace aggregator{

    template<typename Phen, typename Params>
    class Archive{
    public:
    
      Archive(){}

      std::vector<indiv_t> get_full_content()
      {
	std::vector<indiv_t> content;
	std::for_each(auto it=_archive.begin(); it!=_archive.end();it++)
	  content.push_back(*it);
            //std::cout<<"full " <<(*i)->fit().novelty()<<std::endl;                                                                                                       
          
	return content;
      }

      bool add_to_archive(indiv_t i1, indiv_t parent)
      {




      template<typename Ea>
      void apply(Ea& ea)
      {
	// add current pop to the tree
	parallel::init();
	Tree apop = _archive;
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    point_t p;
	    novelty::_behavior_to_point(ea.pop()[i]->fit().behavior(), &p);
	    typename Tree::iterator it = apop.find(p);
	    if (it != apop.end())
	      it->second ++;
	    else
	      apop.insert(p, 1);
	  }
	apop.optimize();

	novelty::_p_novelty<typename Ea::phen_t, Tree, Params> pnov(ea.pop(), apop);
	parallel::p_for(parallel::range_t(0, ea.pop().size()), pnov);


	//pnov(parallel::range_t(0, ea.pop().size()));

#ifndef KONLY
	// update archive


	int l =  ea.pop()[0]->fit().objs().size() - 1;
	size_t added = 0;
	for (size_t i = 0; i < ea.pop().size(); ++i)
	  {
	    float n = ea.pop()[i]->fit().obj(l);
	    if (n > _rho_min)
	      {
		point_t p;
		novelty::_behavior_to_point(ea.pop()[i]->fit().behavior(), &p);
		typename Tree::iterator it = _archive.find(p);
		if (it == _archive.end())
		  {
		    _archive.insert(p, 1);
		    _not_added = 0;
		    added++;
		    std::cout<<"added a indivi., n="<<n<<" rho="<<_rho_min<<std::endl;
		    // for (size_t j = 0; j < ea.pop()[i]->fit().behavior().size(); ++j)
		    //   std::cout<<ea.pop()[i]->fit().behavior()[j]<<" ";
		    // std::cout<<std::endl;
		  }
	      }
	    else
	      ++_not_added;
	  }
	_archive.optimize();



#endif
	++_gen;

      }
      template<typename Behavior,typename Point>
      void _behavior_to_point(const Behavior& b, Point* p)
      {
	assert(p->size() == b.size());
	for (size_t i = 0; i < p->size(); ++i)
	  (*p)[i] = b[i];
      }
    
    
      template<typename Phen, typename Tree, typename Params>
       struct _p_novelty
      {
	typedef typename boost::shared_ptr<Phen> phen_ptr_t;
	typedef typename std::vector<phen_ptr_t> pop_t;
	pop_t _pop;
	const Tree& _apop;
	
	_p_novelty(const pop_t& pop, const Tree& apop) : 
	  _pop(pop), _apop(apop) {}
	_p_novelty(const _p_novelty& ev) : 
	  _pop(ev._pop), _apop(ev._apop) {}
	
	// use the Euclidean distance !
	void operator() (const parallel::range_t& r) const
	{
	  std::vector<typename Tree::key_type> nearest;
	  for (size_t i = r.begin(); i != r.end(); ++i)
	    {
	      typename Tree::key_type q;
	      _behavior_to_point(_pop[i]->fit().behavior(), &q);
	      // k nearest points
	      typedef typename Tree::knn_iterator knn_iterator_t;
	      std::pair<knn_iterator_t, knn_iterator_t> 
		range = _apop.find_nearest_neighbors(q, Params::novelty::k, false);
	
	      nearest.clear();
	      for(knn_iterator_t it = range.first, end = range.second;
		  it != end && nearest.size() < Params::novelty::k; ++it)
		for (size_t z = 0; 
		     z < it->second && nearest.size() < Params::novelty::k; ++z)
		  nearest.push_back(it->first);
		  
	      assert(nearest.size() == k);

	      // compute the mean distance
	      float nov = 0.0f;
	      BOOST_FOREACH(typename Tree::key_type& x, nearest)	
		nov += _dist(x, q);
	      nov /= Params::novelty::k;

	      // last objective -> novelty
	      int l =  _pop[i]->fit().objs().size() - 1;
	      assert(l >= 0);
	      _pop[i]->fit().set_obj(l, nov);
	  
	    }
	}
      };
    }


    protected:
      typedef std::array<float, Params::novelty::nb_dims> point_t;
      typedef ssrc::spatial::kd_tree<point_t, int> Tree;
      Tree _archive;

    };
  }
}

#endif
      


    };

  }
}

#endif
