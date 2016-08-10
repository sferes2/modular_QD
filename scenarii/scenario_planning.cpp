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

#include <iostream>
#include <cmath>

#include <boost/foreach.hpp>
#include <boost/multi_array.hpp>
#include <boost/array.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <Eigen/Core>

#include <sferes/eval/parallel.hpp>
#include <sferes/gen/sampled.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/run.hpp>
#include <sferes/stat/best_fit.hpp>

#include "quality_diversity.hpp"
#include "fit_qd.hpp"


#define NO_PARALLEL



struct Params
{

  struct nov{
    SFERES_CONST size_t deep=3; //7x7
    SFERES_CONST size_t k=15;
    SFERES_CONST double l=0.01;
    SFERES_CONST double eps=0.1;
    
  };
    struct ea
    {
        /*SFERES_CONST size_t res_x = 256;
      SFERES_CONST size_t res_y = 256;*/

      //SFERES_CONST size_t behav_dim = 10;
      //SFERES_ARRAY(size_t, behav_shape, 21, 21, 21, 21, 21, 21);
      SFERES_CONST size_t behav_dim = 6;
      SFERES_ARRAY(size_t, behav_shape, 7, 7, 7, 7, 7, 7);
      
    };
    struct pareto
    {
      SFERES_CONST bool genoDiv =true;
    };
    struct pop
    {
        // number of initial random points
        //SFERES_CONST size_t init_size = 100;
        // size of a batch
        SFERES_CONST size_t size = 200;
        SFERES_CONST size_t nb_gen = 2001;
        SFERES_CONST size_t dump_period = 100;
    };
    struct parameters
    {
        SFERES_CONST float min = 0;
        SFERES_CONST float max = 1;
    };

  struct sampled
  {
    SFERES_ARRAY(float, values,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20)
    SFERES_CONST float mutation_rate=0.1f;
    SFERES_CONST float cross_rate = 0.00f;
    SFERES_CONST bool ordered = false;
  };

};

namespace global{
  Eigen::Matrix<int,21,3> activities;
}

FIT_QD(PlanningFit)
{
    public:
    template<typename Indiv>
    void eval(Indiv& ind)
    {
      float duration=0;
      size_t n=0;
      size_t c=0;
      size_t total=0;
      std::vector<size_t> checked(6,0);
      //std::vector<float> desc(10,0);
      std::vector<float> desc(6,0);
      
      for (size_t i = 0; i < ind.size(); ++i)
	{
	  size_t act=ind.data(i);
	  /*if(act!=0){
	    desc[c]=(float)(act)/20;
	    c++;
	  }*/
	  
	  duration+=global::activities(act,2);

	  size_t g1=global::activities(act,0);

	  if(g1!=0)
	  {
	    
	    if(!is_validated(g1,checked,n) && check_condition(g1,checked,n))
	      {
		checked[n]=g1;
		desc[n]=(float)g1/6;
		n++;
		total+=g1;
	      }

	    size_t g2=global::activities(act,1);
	    if(g2!=0)
	      {
		if(!is_validated(g2,checked,n) && check_condition(g2,checked,n))
		  {
		    checked[n]=g2;
		    desc[n]=(float)g2/6;
		    n++;
		    total+=g2;
		  }
		  
		
	      }
	  }
	  
	  
	}
      
      if(total != 21)
	this->_dead=true;
      
      
      this->_value= - duration;
      
      this->set_desc(desc);
    }
    bool is_validated(size_t g, std::vector<size_t> checked, size_t n)
    {
      for (size_t j = 0; j < n; ++j)
	      if(checked[j]== g )
		return true;
      return false;
    }
    bool check_condition(size_t g, std::vector<size_t> checked, size_t n)
    {
      return true;
      /*switch(g){
      case 3:
	return is_validated(1, checked,n) && is_validated(2, checked,n);
	break;
      case 5:
      case 6:
	return is_validated(3, checked,n) && is_validated(4, checked,n);
	break;
	
      default:
	return true;
	break;
	}*/
    }
};

int main()
{

  srand(time(NULL));
    tbb::task_scheduler_init init(20);  

    global::activities<<
      0,     0,     0,
      1,     0,     1,
      3,     4,     1,
      4,     2,     3,
      4,     0,     1,
      5,     0,     3,
      1,     6,     1,
      2,     1,     3,
      1,     3,     1,
      6,     4,     2,
      5,     3,     3,
      5,     0,     3,
      1,     4,     1,
      3,     0,     2,
      1,     5,     3,
      3,     0,     3,
      5,     4,     1,
      4,     0,     1,
      5,     2,     3,
      4,     0,     1,
      5,     1,     1;
     
    
    using namespace sferes;

    typedef PlanningFit<Params> fit_t;
    typedef gen::Sampled<10, Params> gen_t;
    typedef phen::Parameters<gen_t, fit_t, Params> phen_t;
    typedef eval::Parallel<Params> eval_t;
    /*#ifndef NO_PARALLEL
    typedef eval::Parallel<Params> eval_t;
#else
    typedef eval::Eval<Params> eval_t;
#endif*/

    //    typedef boost::fusion::vector<stat::Map<phen_t, Params>, stat::BestFit<phen_t, Params>,stat::Selection<phen_t,Params> > stat_t;
    //

    typedef modif::Dummy<> modifier_t;

#if defined(GRID)
    typedef container::Grid<phen_t, Params> container_t;
    //typedef boost::fusion::vector<stat::Map<phen_t, Params>,stat::Progress<phen_t, Params> > stat_t;

#else // ARCHIVE
    typedef container::Archive<phen_t, Params> container_t;
    //typedef boost::fusion::vector<stat::Archive<phen_t, Params>,stat::Progress<phen_t, Params> > stat_t;
    //typedef boost::fusion::vector<stat::Archive<phen_t, Params>, stat::Selection<phen_t,Params> > stat_t;
#endif

    typedef boost::fusion::vector<stat::Container<phen_t, Params>,stat::Progress<phen_t, Params> > stat_t;

#if defined(RANDOM)
    typedef selector::Random<phen_t> select_t;
#elif defined(FITNESS)
    typedef selector::ScoreProportionate<phen_t,selector::getFitness> select_t;
#elif defined(NOVELTY)
    typedef selector::ScoreProportionate<phen_t,selector::getNovelty> select_t;
#elif defined(CURIOSITY)
    typedef selector::ScoreProportionate<phen_t,selector::getCuriosity> select_t;
#elif defined(POPFITNESS)
    typedef selector::PopulationBased<phen_t, selector::ScoreProportionate<phen_t, selector::getFitness> > select_t;
#elif defined(POPNOVELTY)
    typedef selector::PopulationBased<phen_t, selector::ScoreProportionate<phen_t, selector::getNovelty> > select_t;
#elif defined(POPCURIOSITY)
    typedef selector::PopulationBased<phen_t, selector::ScoreProportionate<phen_t, selector::getCuriosity> > select_t;
#elif defined(TOURFITNESS)
    typedef selector::TournamentBased<phen_t,selector::getFitness> select_t;
#elif defined(TOURNOVELTY)
    typedef selector::TournamentBased<phen_t,selector::getNovelty> select_t;
#elif defined(TOURCURIOSITY)
    typedef selector::TournamentBased<phen_t,selector::getCuriosity> select_t;
#elif defined(PARETO) //NSLC
    typedef selector::ParetoBased<phen_t,boost::fusion::vector<selector::getNovelty,selector::getLocalQuality>, Params > select_t;
#else // NOSELECTION
    typedef selector::NoSelection<phen_t> select_t;

#endif

    typedef ea::QualityDiversity<phen_t, eval_t, stat_t, modifier_t, select_t, container_t, Params> ea_t;

    ea_t ea;
    ea.run();
}
