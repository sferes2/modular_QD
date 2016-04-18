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
#include <sferes/gen/evo_float.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/run.hpp>
#include <sferes/stat/best_fit.hpp>

#include "map_elite.hpp"
#include "fit_map.hpp"
#include "stat_map.hpp"
#include "stat_archive.hpp"
#include "stat_selection.hpp"


#include "arm_hori.hpp"

/*#define NO_MPI
#ifdef GRAPHIC
#define NO_PARALLEL
#include "renderer/osg_visitor.hh"
#endif

#ifndef NO_PARALLEL
#include <sferes/eval/parallel.hpp>
#ifndef NO_MPI
#include <sferes/eval/mpi.hpp>
#endif
#else
#include <sferes/eval/eval.hpp>
#endif*/



using namespace sferes::gen::evo_float;


struct Params
{

  struct nov{
    SFERES_CONST size_t deep=5;
    SFERES_CONST size_t k=15;
    SFERES_CONST double l=0.01;
    SFERES_CONST double eps=0.1;
    
  };
    struct ea
    {
        /*SFERES_CONST size_t res_x = 256;
      SFERES_CONST size_t res_y = 256;*/

        SFERES_CONST size_t behav_dim = 2;
        SFERES_ARRAY(size_t, behav_shape, 128, 128);

    };
    struct pop
    {
        // number of initial random points
        //SFERES_CONST size_t init_size = 100;
        // size of a batch
        SFERES_CONST size_t size = 200;
        SFERES_CONST size_t nb_gen = 50001;
        SFERES_CONST size_t dump_period = -1;
        SFERES_CONST size_t dump_period_archive = 1000;
    };
    struct parameters
    {
        SFERES_CONST float min = -1;
        SFERES_CONST float max = 1;
    };
    struct evo_float
    {
        SFERES_CONST float cross_rate = 0.25f;
        SFERES_CONST float mutation_rate = 0.1f;
        SFERES_CONST float eta_m = 10.0f;
        SFERES_CONST float eta_c = 10.0f;
        SFERES_CONST mutation_t mutation_type = polynomial;
        SFERES_CONST cross_over_t cross_over_type = sbx;
    };

};



FIT_MAP(ArmFit)
{
    public:
    template<typename Indiv>
    void eval(Indiv& ind)
    {
    
      Eigen::VectorXd angle(ind.size());
      for (size_t i = 0; i < ind.size(); ++i)
	angle[i] = ind.data(i)*M_PI/2;
      this->_value = - sqrt((angle.array()-angle.mean()).square().mean());
      Eigen::Vector3d pos=robot::Arm::forward_model(angle);
      float L=robot::Arm::max_length();
      std::vector<float> data = {(float) (pos[0]+L)/(2*L), (float) (pos[1]+L)/(2*L)};
      //this->set_desc(ind.gen().data(0), ind.gen().data(1));
      this->set_desc(data);
    }

    bool dead() {return false;}
};

int main()
{
    srand (time(NULL));
  

    using namespace sferes;

    typedef ArmFit<Params> fit_t;
    typedef gen::EvoFloat<8, Params> gen_t;
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
    typedef aggregator::Map<phen_t, Params> aggreg_t;
    typedef boost::fusion::vector<stat::Map<phen_t, Params> > stat_t;

#else // ARCHIVE
    typedef aggregator::Archive<phen_t, Params> aggreg_t;
    typedef boost::fusion::vector<stat::Archive<phen_t, Params> > stat_t;
    //typedef boost::fusion::vector<stat::Archive<phen_t, Params>, stat::Selection<phen_t,Params> > stat_t;
#endif



#if defined(RANDOM)
    typedef selector::Random<phen_t> select_t;
#elif defined(FITNESS)
    typedef selector::ScoreProportionate<phen_t,selector::getFitness> select_t;
#elif defined(NOVELTY)
    typedef selector::ScoreProportionate<phen_t,selector::getNovelty> select_t;
#elif defined(CURIOSITY)
    typedef selector::ScoreProportionate<phen_t,selector::getCuriosity> select_t;
#elif defined(POPFITNESS)
    typedef selector::PopulationBased<phen_t,selector::getFitness> select_t;
#elif defined(POPNOVELTY)
    typedef selector::PopulationBased<phen_t,selector::getNovelty> select_t;
#elif defined(POPCURIOSITY)
    typedef selector::PopulationBased<phen_t,selector::getCuriosity> select_t;
#else // NOSELECTION
    typedef selector::NoSelection<phen_t> select_t;

#endif

    typedef ea::MapElite<phen_t, eval_t, stat_t, modifier_t, select_t, aggreg_t, Params> ea_t;

    ea_t ea;
    ea.run();
}
