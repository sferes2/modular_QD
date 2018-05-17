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

// TODO:
// - value updater after adding to the container (it can also update the container by removing individuals)
// thus, it is dependent on the container
// - decoupling so that individuals can have multiple values (curiosity, novelty)
// - a generic value updater that checks whether a specific class is provided
// e.g. curiosity
// the updater is dependent on the fit provided

namespace sferes
{
namespace ea
{

// Main class
template <typename Phen, typename Eval, typename Stat, typename FitModifier, typename Selector, typename Variator, typename Container, typename Updater, typename Params, typename Exact = stc::Itself>
class QualityDiversity : public ea::Ea<Phen, Eval, Stat, FitModifier, Params, typename stc::FindExact<QualityDiversity<Phen, Eval, Stat, FitModifier, Selector, Variator, Container, Updater, Params, Exact>, Exact>::ret>
{
public:
	typedef boost::shared_ptr<Phen> indiv_t;
	typedef typename std::vector<indiv_t> pop_t;
	typedef typename pop_t::iterator it_t;
	typedef typename std::vector<std::vector<indiv_t>> front_t;

	typedef boost::shared_ptr<Phen> phen_t;

	static const size_t behav_dim = Params::ea::behav_dim;
	//typedef boost::array<float, behav_dim> point_t;

	QualityDiversity() {}

	// Random initialization of _parents and _offspring
	void random_pop()
	{
		parallel::init();
		_offspring.resize(Params::pop::size);
		BOOST_FOREACH (boost::shared_ptr<Phen> &indiv, this->_offspring)
		{
			indiv = boost::shared_ptr<Phen>(new Phen());
			indiv->random();
		}

		this->_eval_pop(this->_offspring, 0, this->_offspring.size());
		this->apply_modifier();

		// _add(_parents, _parents);
		_added = _container.add(_parents, _parents);
		_updater(_container, _parents, _parents, _added);

		this->_parents = this->_offspring;
		_offspring.resize(Params::pop::size);

		BOOST_FOREACH (boost::shared_ptr<Phen> &indiv, this->_offspring)
		{
			indiv = boost::shared_ptr<Phen>(new Phen());
			indiv->random();
		}

		this->_eval_pop(this->_offspring, 0, this->_offspring.size());
		this->apply_modifier();
		// _add(_offspring, _offspring);
		_added = _container.add(_offspring, _offspring);
		_updater(_container, _offspring, _offspring, _added);

		this->_pop.clear();
		_container.get_full_content(this->_pop);
	}

	// void epoch()
	// {
	// 	// Selection of the parents
	// 	_selector(_parents, *this);
	// 	_offspring.clear(); //CLEAR _offspring and _parents ONLY after Select, as it can be used by Select

	// 	// Generation of the offspring
	// 	std::vector<size_t> a;
	// 	misc::rand_ind(a, _parents.size());
	// 	for (size_t i = 0; i < Params::pop::size; i += 2)
	// 	{
	// 		boost::shared_ptr<Phen> i1, i2;
	// 		_parents[a[i]]->cross(_parents[a[i + 1]], i1, i2);
	// 		i1->mutate();
	// 		i2->mutate();
	// 		i1->develop();
	// 		i2->develop();
	// 		_offspring.push_back(i1);
	// 		_offspring.push_back(i2);
	// 	}

	// 	// Evaluation of the offspring
	// 	this->_eval_pop(_offspring, 0, _offspring.size());
	// 	this->apply_modifier();

	// 	// Addition of the offspring to the container
	// 	_add(_offspring, _parents);

	// 	assert(_offspring.size() == _parents.size());

	// 	this->_pop.clear();

	// 	// Copy of the containt of the container into the _pop object.
	// 	_container.get_full_content(this->_pop);
	// }

	// Main Iteration of the QD algorithm
	void epoch()
	{
		// Selection of parents
		_selector(_parents, *this);

		// Generation of offspring
		_variator(_offspring, _parents);

		// Evaluation of offspring
		this->_eval_pop(_offspring, 0, _offspring.size());
		this->apply_modifier();
		
		// Addition/Replacement in container
		_added = _container.add(_offspring, _parents);

		// Update certain properties of the individuals (based on whether they were added)
		// Here we might have a different update strategy for each property (novelty, curiosity) and container
		// we could use boost::fusion for handling different properties
		// container.update(_offspring, _parents, added);
		_updater(_container, _offspring, _parents, _added);

		// Copy of the content of the container into the _pop object
		this->_pop.clear();
		_container.get_full_content(this->_pop);
	}

	const Container &container() const { return _container; }
	const pop_t &pop() const { return this->_pop; }
	const pop_t &offspring() const { return _offspring; }
	const pop_t &parents() const { return _parents; }
	const std::vector<bool> &added() const { return _added; }

protected:
	// // Add the offspring into the container and update the score of the individuals from the container and both of the sub population (offspring and parents)
	// void _add(pop_t &pop_off, pop_t &pop_parents)
	// {
	// 	_added.resize(pop_off.size());
	// 	for (size_t i = 0; i < pop_off.size(); ++i)
	// 		_added[i] = _add_to_container(pop_off[i], pop_parents[i]);
	// 	_container.update(pop_off, pop_parents);
	// }

	// // add to the container procedure.
	// bool _add_to_container(indiv_t i1, indiv_t parent)
	// {
	// 	if (_container.add(i1, parent))
	// 	{
	// 		parent->fit().set_curiosity(parent->fit().curiosity() + 1);
	// 		return true;
	// 	}
	// 	else
	// 	{
	// 		parent->fit().set_curiosity(parent->fit().curiosity() - 0.5);
	// 		return false;
	// 	}
	// }

	Selector _selector;
	Variator _variator;
	Container _container;
	Updater _updater;

	pop_t _offspring, _parents;
	std::vector<bool> _added;
};
} // namespace ea
} // namespace sferes
#endif
