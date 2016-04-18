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

       void get_full_content(std::vector<indiv_t>& content)
      {
	for(auto it=_archive.begin(); it!=_archive.end();it++)
	  content.push_back((*it).second);
            //std::cout<<"full " <<(*i)->fit().novelty()<<std::endl;                                                                                                       
      }
      

      bool add_to_archive(indiv_t i1, indiv_t parent){
	if( _add_to_archive(i1,parent))
	  {
	    parent->fit().set_curiosity(parent->fit().curiosity()+1);
	    return true;
	  }
	else
	  {
	    parent->fit().set_curiosity(parent->fit().curiosity()-0.5);
	    return false;
	}
      }
      
      

      void update(){
	_archive.optimize();
	_update_novelty();
      }
    

      static double get_sum_dist_kppn(const indiv_t& indiv, const Tree&  apop, const int K,const bool omit=true)
      {
	 pop_t nearest;
	 get_knn(indiv,apop,K,nearest,omit); //here we omit because indivs are in the archive
	 // compute the mean distance                                                                                                                                         
	 double sum = 0.0f;                                                                                                                                                    
	 BOOST_FOREACH(indiv_t& x, nearest)                                                                                                                   
	   sum += _dist(x->fit().desc(), indiv->fit().desc());
	 return sum;
       }
      
      static indiv_t get_nearest(const indiv_t& indiv, const Tree&  apop, const bool omit_query_point) {
	typename Tree::key_type q;
	Archive::_behavior_to_point(indiv->fit().desc(), &q);
	typename Tree::iterator it=apop.find_nearest_neighbor(q,omit_query_point);
	return it->second;
      }

      static void get_knn(const indiv_t& indiv, const Tree&  apop, int k, pop_t& nearest, const bool omit_query_point){
	typename Tree::key_type q;
	Archive::_behavior_to_point(indiv->fit().desc(), &q);
	// k nearest points
	typedef typename Tree::knn_iterator knn_iterator_t;
	std::pair<knn_iterator_t, knn_iterator_t> 
	  range = apop.find_nearest_neighbors(q, k, omit_query_point);
	
	nearest.clear();
	for(knn_iterator_t it = range.first, end = range.second; it != end && nearest.size() < k; ++it)
	  //for (size_t z = 0; z < it->second && nearest.size() < Params::nov::k; ++z)
	  nearest.push_back(it->second);
	
	assert(nearest.size() == k);
      }

      static double get_novelty(const indiv_t& indiv, const Tree&  apop) {
	return get_sum_dist_kppn(indiv,apop,Params::nov::k,true)/Params::nov::k;
      }
      static double get_novelty(const indiv_t& indiv, typename pop_t::iterator begin, typename pop_t::iterator end) { 
	if(std::distance(begin,end)>Params::nov::k){
	  //NEED to sort
	  //  std::sort(begin,end,_compare_dist_f(indiv));
	  assert(false);
	}
	double sum =0;
	typename pop_t::iterator it= begin;
	for(int i =0;i<Params::nov::k; i++){
	  sum += _dist((*it)->fit().desc(), indiv->fit().desc());
	  it++;
	}
	return sum/Params::nov::k;
      }

      const Tree& archive() const{return _archive;}
  
    protected:
bool _add_to_archive(indiv_t i1, indiv_t parent)
      {
	
	//TODO
	// update archive
	if(i1->fit().dead())
	  return false;
	if (_archive.size()<Params::nov::k || _dist(get_nearest(i1,_archive,false)->fit().desc(), i1->fit().desc()) > Params::nov::l) //ADD because new
	  {
	    _add(i1);
	    return true;
	  }
	else 
	  {
	    pop_t neigh_current;
	    get_knn(i1, _archive, 2, neigh_current, false); //Be careful, the first one referes to nn
	    if(_dist(i1->fit().desc(), neigh_current[1]->fit().desc())  < (1-Params::nov::eps)*Params::nov::l)//too close the second NN -- this works better
	    //if(_dist(i1->fit().desc(), neigh_current[1]->fit().desc())  < Params::nov::l)//too close the second NN
	      {
		return false;    
	      }
	    indiv_t nn=neigh_current[0];

	    std::vector<double> score_cur(2,0), score_nn(2,0);
	    score_cur[0]=i1->fit().value();
	    score_nn[0]=nn->fit().value();
	    
	    //Compute the Novelty
	    neigh_current.clear();
            get_knn(i1, _archive, Params::nov::k+1, neigh_current, false); //Be careful, the first one referes to nn 
	    score_cur[1] = get_novelty( nn , neigh_current.begin()++, neigh_current.end());
	    score_nn[1] = get_novelty( nn , _archive);
	    
	    //TEST
	    int score=0;     
            
	    if((score_cur[0] >= (1-sign(score_nn[0])*Params::nov::eps)*score_nn[0] && score_cur[1] >= (1-sign(score_nn[1])*Params::nov::eps)*score_nn[1] ) &&
	       ((score_cur[0]-score_nn[0])*std::abs(score_nn[1])>-(score_cur[1]-score_nn[1])*std::abs(score_nn[0]))) //add if significatively better on one objective
	      {
		_replace(nn,i1);
		return true;
	      }
	    //-------
	    //THIS WORKS as well but less
	    /*int score=0;     
            
	    if(score_cur[0] >= score_nn[0] && score_cur[1] >= (1-sign(score_nn[1])*Params::nov::eps)*score_nn[1] ) //not replacing something less efficient
	      {
		_replace(nn,i1);
		return true;
		}*/
	    //-------
	    // THIS WORKS--------  TO STAY IN THE ARCHIVE YOU NEED TO epsilon dominate! (kind of)
	    /*int score=0;
	    for(int i =0;i<score_cur.size(); i++){
	      if(score_cur[i] < (1-sign(score_nn[i])*Params::nov::eps)*score_nn[i] )
	      return false;//nothing below the epsilon in one obj
	      if(score_cur[i] >=score_nn[i] )
		score++;
	    }
	    if(score>=1)//if better on at least 1 objective
	      {
		//std::cout<<"replace"<<std::endl;
		_replace(nn,i1);
		return true;
	      }
	    */
	    //---------------
	    else{
	      return false;  
	    }
	  }
	      
      }




      void _add(const indiv_t& tobeinserted)
      {
	point_t p;
	this->_behavior_to_point(tobeinserted->fit().desc(), &p);
	typename Tree::iterator it = _archive.find(p);
	if (it == _archive.end())
	  {
	    _archive.insert(p, tobeinserted);
	  }
      }

      void _replace(const indiv_t& toberemoved,const indiv_t& tobeinserted){
	point_t remove;
	this->_behavior_to_point(toberemoved->fit().desc(), &remove);
	_archive.remove(remove);
	    //std::cout<<"problem remove"<<std::endl;
	_add(tobeinserted);
      }


      
      void _update_novelty(){
	tbb::parallel_for_each(_archive.begin(),_archive.end(),_p_novelty(_archive));
      }


      template<typename Behavior,typename Point>
      static void _behavior_to_point(const Behavior& b, Point* p)
      {
	assert(p->size() == b.size());
	for (size_t i = 0; i < p->size(); ++i)
	  (*p)[i] = b[i];
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

      Tree _archive;

    };
  }
}

#endif
    
