//| This file is a part of the sferes2 framework.
//| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
//| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
//|
//| This software is a computer program whose purpose is to facilitate
//| experiments in evolutionary computation and evolutionary robotics.
//|
//| This software is governed by the CeCILL license under French law
//| and abiding by the rules of distribution of free software.  You
//| can use, modify and/ or redistribute the software under the terms
//| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
//| following URL "http://www.cecill.info".
//|
//| As a counterpart to the access to the source code and rights to
//| copy, modify and redistribute granted by the license, users are
//| provided only with a limited warranty and the software's author,
//| the holder of the economic rights, and the successive licensors
//| have only limited liability.
//|
//| In this respect, the user's attention is drawn to the risks
//| associated with loading, using, modifying and/or developing or
//| reproducing the software by the user in light of its specific
//| status of free software, that may mean that it is complicated to
//| manipulate, and that also therefore means that it is reserved for
//| developers and experienced professionals having in-depth computer
//| knowledge. Users are therefore encouraged to load and test the
//| software's suitability as regards their requirements in conditions
//| enabling the security of their systems and/or data to be ensured
//| and, more generally, to use and operate it in the same conditions
//| as regards security.
//|
//| The fact that you are presently reading this means that you have
//| had knowledge of the CeCILL license and that you accept its terms.


#ifndef MAP_ELITE_HPP_
#define MAP_ELITE_HPP_

#include <algorithm>
#include <limits>

#include <boost/foreach.hpp>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/timer/timer.hpp>


#include <sferes/stc.hpp>
#include <sferes/ea/ea.hpp>
#include <sferes/fit/fitness.hpp>

#include "selector.hpp"
#include "aggregator.hpp"

namespace sferes
{
namespace ea
{

// Main class
// extended version of SFERES_EA with Select and Aggreg
  template<typename Phen, typename Eval, typename Stat, typename FitModifier, typename Select, typename Aggreg , typename Params, typename Exact = stc::Itself>
  class MapElite : public ea::Ea < Phen, Eval, Stat, FitModifier, Params,typename stc::FindExact<MapElite<Phen, Eval, Stat, FitModifier,Select, Aggreg , Params, Exact>, Exact>::ret >
  {
    public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;
    typedef typename pop_t::iterator it_t;
    typedef typename std::vector<std::vector<indiv_t> > front_t;

    //typedef boost::array<float, 2> point_t;


    typedef boost::shared_ptr<Phen> phen_t;
    //typedef boost::multi_array<phen_t, 2> array_t;


    /*static const size_t res_x = Params::ea::res_x;
      static const size_t res_y = Params::ea::res_y;*/

    //static const size_t behav_dim = Params::ea::behav_shape_size();
    static const size_t behav_dim = Params::ea::behav_dim;
    typedef boost::array<float, behav_dim> point_t;


    MapElite(){}
    /*    {
        // - my_type my_name(size_t i)
        // - size_t my_name_size()

        assert(behav_dim == Params::ea::behav_shape_size());
        //boost::array<long int, behav_dim> tmp_shape;
        for(size_t i = 0; i < Params::ea::behav_shape_size(); ++i)
            behav_shape[i] = Params::ea::behav_shape(i);


        //boost::array<typename array_t::index, behav_dim> shape = behav_shape;
        _array.resize(behav_shape);
        _array_parents.resize(behav_shape);

        //boost::array<typename array_t::index, behav_dim> shape = {{ 2, 2 }}; //behav_shape
        //allocate space for _array and _array_parents
	}*/

    /*MapElite() :
    _array(boost::extents[res_x][res_y]),
    _array_parents(boost::extents[res_x][res_y])
      {
      }*/

    void random_pop()
    {
      parallel::init();
      _parents.resize(Params::pop::size);
      BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_parents)
        {
	  indiv = boost::shared_ptr<Phen>(new Phen());
	  indiv->random();
        }
      this->_eval_pop(this->_parents, 0, this->_parents.size());
      BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_parents)
	_aggreg.add_to_archive(indiv, indiv);
      
      _offspring.resize(Params::pop::size);
      BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_offspring)
        {
	  indiv = boost::shared_ptr<Phen>(new Phen());
	  indiv->random();
        }

      this->_eval_pop(this->_offspring, 0, this->_offspring.size());
      BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_offspring)
	_aggreg.add_to_archive(indiv, indiv);

      _aggreg.update();

    }

    void epoch()
    {
      std::cout<<"gen "<<this->gen()<<std::endl;
      this->_pop.clear();
      _aggreg.get_full_content(this->_pop);
      
      _parents.resize(Params::pop::size);
      _select(_parents,*this);
      
      //CLEAN OFFSPRING AFTER SELECT as it can be used by Select
      _offspring.clear();
      _added.clear();

      std::cout<<"pop creation "<<std::endl;

      std::vector<size_t> a;
      misc::rand_ind(a, _parents.size());
      for (size_t i = 0; i < Params::pop::size; i+=2)                                                                                                                                        
	{	    
	  boost::shared_ptr<Phen> i1, i2;
	  _parents[i]->cross(_parents[i+1], i1, i2);
	  i1->mutate();
	  i2->mutate();
	  i1->develop();
	  i2->develop();
	  _offspring.push_back(i1);
	  _offspring.push_back(i2);
	}
      std::cout<<"pop eval "<<std::endl;
      this->_eval_pop(_offspring, 0, _offspring.size());
      

      std::cout<<"storing "<<std::endl;
      assert(_offspring.size() == _parents.size());
      _added.resize(_offspring.size());
      for (size_t i = 0; i < _offspring.size(); ++i)
	_added[i]=_aggreg.add_to_archive(_offspring[i], _parents[i]);
      std::cout<<"update "<<std::endl;
      _aggreg.update();
      std::cout<<"end gen "<<std::endl;
    }



    template<typename I>
    point_t get_point(const I& indiv) const
    {
      return _aggreg.get_point(indiv);
    }
    
    const Aggreg& aggreg()const {return _aggreg;}

    const pop_t& pop()const {return this->_pop;}
    const pop_t& offspring()const {return _offspring;}
    const pop_t& parents()const {return _parents;}
    const std::vector<bool>& added()const {return _added;}
    protected:
    
    Select _select;
    Aggreg _aggreg;
    
    pop_t _offspring, _parents;
    std::vector<bool> _added;
};
}
}
#endif
