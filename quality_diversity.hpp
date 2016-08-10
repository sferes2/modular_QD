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


#ifndef QD_HPP_
#define QD_HPP_

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
#include "container.hpp"
#include "stat.hpp"

namespace sferes
{
namespace ea
{

// Main class
// extended version of SFERES_EA with Select and Aggreg
  template<typename Phen, typename Eval, typename Stat, typename FitModifier, typename Select, typename Container , typename Params, typename Exact = stc::Itself>
  class QualityDiversity : public ea::Ea < Phen, Eval, Stat, FitModifier, Params,typename stc::FindExact<QualityDiversity<Phen, Eval, Stat, FitModifier,Select, Container , Params, Exact>, Exact>::ret >
  {
    public:
    typedef boost::shared_ptr<Phen> indiv_t;
    typedef typename std::vector<indiv_t> pop_t;
    typedef typename pop_t::iterator it_t;
    typedef typename std::vector<std::vector<indiv_t> > front_t;

    typedef boost::shared_ptr<Phen> phen_t;

    static const size_t behav_dim = Params::ea::behav_dim;
    typedef boost::array<float, behav_dim> point_t;


    QualityDiversity(){}

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
      _add(_parents,_parents);


      _offspring.resize(Params::pop::size);
      BOOST_FOREACH(boost::shared_ptr<Phen>&indiv, this->_offspring)
        {
	  indiv = boost::shared_ptr<Phen>(new Phen());
	  indiv->random();
        }

      this->_eval_pop(this->_offspring, 0, this->_offspring.size());
      _add(_offspring,_offspring);

      this->_pop.clear();
      _container.get_full_content(this->_pop);
    }


    void epoch()
    {
      
      //_parents.resize(Params::pop::size);// THE SIZE OF THE PARENTS IS PROVIDED DURING THE INIT

      _select(_parents,*this);
      _offspring.clear();       //CLEAR _offspring and _parents ONLY after Select, as it can be used by Select

      std::vector<size_t> a;
      misc::rand_ind(a, _parents.size());
      for (size_t i = 0; i < Params::pop::size; i+=2)
	{	    
	  boost::shared_ptr<Phen> i1, i2;
	  _parents[a[i]]->cross(_parents[a[i+1]], i1, i2);
	  i1->mutate();
	  i2->mutate();
	  i1->develop();
	  i2->develop();
	  _offspring.push_back(i1);
	  _offspring.push_back(i2);
	}

      this->_eval_pop(_offspring, 0, _offspring.size());
      
      _add(_offspring,_parents);

      assert(_offspring.size() == _parents.size());
      this->_pop.clear();
      _container.get_full_content(this->_pop);
      
    }


    template<typename I>
    point_t get_point(const I& indiv) const
    {
      return _container.get_point(indiv);
    }
    
    const Container& container()const {return _container;}

    const pop_t& pop()const {return this->_pop;}
    const pop_t& offspring()const {return _offspring;}
    const pop_t& parents()const {return _parents;}
    const std::vector<bool>& added()const {return _added;}
    protected:


      bool _add_to_container(indiv_t i1, indiv_t parent){
	if( _container.add(i1,parent))
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

    void _add(pop_t& pop_off,  pop_t& pop_parents)
    {
      _added.resize(pop_off.size());
      for (size_t i = 0; i < pop_off.size(); ++i)
	_added[i]=_add_to_container(pop_off[i], pop_parents[i]);
      _container.update(pop_off, pop_parents);
      
    }


    
    Select _select;
    Container _container;
    
    pop_t _offspring, _parents;
    std::vector<bool> _added;
};
}
}
#endif
