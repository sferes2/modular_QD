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


//#define NO_PARALLEL

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
#include <sferes/gen/sampled.hpp>
#include <sferes/phen/parameters.hpp>
#include <sferes/modif/dummy.hpp>
#include <sferes/run.hpp>
#include <sferes/stat/best_fit.hpp>

#include "map_elite.hpp"
#include "fit_map.hpp"

#include <hexapod_robdyn/hexapod_robdyn_simu.hpp>
//#include <hexapod_dart/hexapod_dart_simu.hpp>

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
      SFERES_ARRAY(size_t, behav_shape, 100, 100);
      
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
      SFERES_CONST size_t nb_gen = 10001;
      SFERES_CONST size_t dump_period = 500;
    };
    struct parameters
    {
        SFERES_CONST float min = 0;
        SFERES_CONST float max = 1;
    };

  struct sampled
  {
    SFERES_ARRAY(float, values,0,0.25,0.5,0.75,1)
    SFERES_CONST float mutation_rate=0.05f;
    SFERES_CONST float cross_rate = 0.00f;
    SFERES_CONST bool ordered = true;
  };

};


namespace global {
  boost::shared_ptr<ode::Environment_hexa> env;
  boost::shared_ptr<hexapod_robdyn::Hexapod> robot;
  //std::shared_ptr<hexapod_dart::Hexapod> robot;


}

FIT_MAP(HexaTurnFit)
{
    public:
    template<typename Indiv>
      void eval(Indiv& ind, bool print=false)
    {

      std::vector<double> ctrl(ind.size());
      for (size_t i = 0; i < ind.size(); ++i)
	{
	  //std::cout<<i<<":"<<ind.data(i)<<"   ";
	  ctrl[i] = ind.data(i);
	}
      //std::cout<<std::endl;

      //auto local_robot=global::robot->clone();
      //std::shared_ptr<hexapod_dart::Hexapod> local_robot = std::make_shared<hexapod_dart::Hexapod>(global::filename, global::brk);
      
      //using desc_t = boost::fusion::vector<>;
      //hexapod_dart::HexapodDARTSimu<hexapod_dart::desc<desc_t>> simu(ctrl, local_robot);

      hexapod_robdyn::HexapodRobdynSimu simu(ctrl, global::robot);
      simu.controller().set_parameters(ctrl);

      simu.run(3);

      if(simu.covered_distance()<-1000)
	{
	  _dead=true;
	  if(print)
            {
	      std::cout<<"DEAD"<<std::endl;
	    }
	  return;
	}
      else
	{
	  //you have to compute the desc before call quality_orientation
	  _pos=simu.final_pos();
       	  float L=1;//1 meter -> area radius 
	  std::vector<float> data = {(float) (_pos[0]+L)/(2*L), (float) (_pos[1]+L)/(2*L)};
	  this->set_desc(data);

	  if(print)
	    {
	      std::cout<<"pos: "<<_pos.transpose()<<std::endl;
	      std::cout<<"data: "<<data[0]<<" "<<data[1]<<std::endl;
	      

	    }
	  
	  this->_value = _quality_orient(simu, print);

	}
      
    //std::cout << simu.covered_distance() << " " << simu.arrival_angle() << std::endl;
    //std::cout << simu.energy() << std::endl;
    //std::vector<double> v;
    //simu.get_descriptor<hexapod_dart::descriptors::DutyCycle>(v);
    //for (size_t i = 0; i < v.size(); i++) {
    //    std::cout << v[i] << " ";
    //}
    //std::cout << std::endl;
    //std::vector<double> vv;
    //simu.get_descriptor<hexapod_dart::descriptors::RotationTraj>(vv);
    //for (size_t i = 0; i < vv.size(); i++) {
    //    std::cout << vv[i] << " ";
    //}
    //std::cout << std::endl;

    }

 private:
    Eigen::Vector3d _pos;
    template<typename Simu_t>
      float _quality_orient( Simu_t& simu, bool print )
    {
      float direction;
      float B= sqrt((this->_pos[0]/2)*(this->_pos[0]/2)+(this->_pos[1]/2)*(this->_pos[1]/2));
      float alpha=atan2(this->_pos[1],this->_pos[0]);
      float A= B/cos(alpha);

      float beta=atan2(this->_pos[1],this->_pos[0]-A);

      if(this->_pos[0]>=0)
	direction=beta-M_PI;
      else
	direction=beta;
      while(direction<-M_PI)
	direction+=2*M_PI;
      while(direction>M_PI)
	direction-=2*M_PI;

      direction=std::round(direction*100)/100.0*180/M_PI;

      assert(direction>-180);
      assert(direction<180);

      float arrival=simu.arrival_angle();//*180/M_PI;
      assert(arrival>-180);
      assert(arrival<180);
      if(print)
	{
	  std::cout<<"direction: "<<direction<<std::endl;
	  std::cout<<"arrival: "<<arrival<<std::endl;
	  std::cout<<"error: "<<- std::abs(direction-arrival)<<std::endl;
	  
	}


      return - std::abs(direction-arrival);
      

    }



};

    using namespace sferes;
    typedef HexaTurnFit<Params> fit_t;
    typedef gen::Sampled<36, Params> gen_t;
    typedef phen::Parameters<gen_t, fit_t, Params> phen_t;

void run_behavior(int narg, char ** varg)
{
  std::cout<<"run behavior"<<std::endl;
  phen_t indiv;
  assert(narg>=indiv.gen().size()+2);
  std::cout<<"narg "<<narg <<" indiv.gen "<<  indiv.gen().size()<<std::endl;
  for (size_t i = 0; i < indiv.gen().size(); ++i)
    {
      std::cout<<i<<":"<<varg[i+2]<<"   ";
      indiv.gen().set_data(i,std::atof(varg[i+2])*4);
      std::cout<<indiv.gen().data(i)<<"   "<<std::endl;
    }
  
  indiv.develop();
  indiv.fit().eval(indiv,true);
}


int main(int narg, char ** varg)
{
    srand (time(NULL));
  
    tbb::task_scheduler_init init(20);
    dInitODE();

    global::env = boost::shared_ptr<ode::Environment_hexa>(new ode::Environment_hexa(0));
    std::vector<int> brk = {};

    global::robot = boost::shared_ptr<hexapod_robdyn::Hexapod>(new hexapod_robdyn::Hexapod(*global::env, Eigen::Vector3d(0, 0, 0.5), brk));

    double step = 0.001;

    // low gravity to slow things down (eq. smaller timestep?)                                                                                                    
    global::env->set_gravity(0, 0, -9.81);
    bool stabilized = false;
    int stab = 0;
    for (size_t s = 0; s < 1000 && !stabilized; ++s) {

      Eigen::Vector3d prev_pos = global::robot->pos();
      global::robot->next_step(step);
      global::env->next_step(step);

      if ((global::robot->pos() - prev_pos).norm() < 1e-4)
	stab++;
      else
	stab = 0;
      if (stab > 100)
	stabilized = true;
    }





    /*if(narg<2)
    {
      std::cout<< "Please provide path to urdf file"<<std::endl;
      return -1;
      }*/

      
    //std::vector<int> brk={};
    // global::robot = std::make_shared<hexapod_dart::Hexapod>(varg[1], brk);
     
if(narg>2){
      run_behavior(narg, varg);
      return 0;
     }

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
    typedef aggregator::Grid<phen_t, Params> aggreg_t;
    //typedef boost::fusion::vector<stat::Map<phen_t, Params>,stat::Progress<phen_t, Params> > stat_t;

#else // ARCHIVE
    typedef aggregator::Archive<phen_t, Params> aggreg_t;
    //typedef boost::fusion::vector<stat::Archive<phen_t, Params>,stat::Progress<phen_t, Params> > stat_t;
    //typedef boost::fusion::vector<stat::Archive<phen_t, Params>, stat::Selection<phen_t,Params> > stat_t;
#endif

    typedef boost::fusion::vector<stat::Aggregator<phen_t, Params>,stat::Progress<phen_t, Params> > stat_t;



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

    typedef ea::MapElite<phen_t, eval_t, stat_t, modifier_t, select_t, aggreg_t, Params> ea_t;

    ea_t ea;
    std::cout<<"start"<<std::endl;
    ea.run();
    std::cout<<"end"<<std::endl;

    global::robot.reset();
    global::env.reset();
  
    std::cout<<"end"<<std::endl;
}
