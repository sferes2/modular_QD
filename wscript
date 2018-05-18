#!/usr/bin/env python
#| This file is a part of the sferes2 framework.
#| Copyright 2009, ISIR / Universite Pierre et Marie Curie (UPMC)
#| Main contributor(s): Jean-Baptiste Mouret, mouret@isir.fr
#|
#| This software is a computer program whose purpose is to facilitate
#| experiments in evolutionary computation and evolutionary robotics.
#| 
#| This software is governed by the CeCILL license under French law
#| and abiding by the rules of distribution of free software.  You
#| can use, modify and/ or redistribute the software under the terms
#| of the CeCILL license as circulated by CEA, CNRS and INRIA at the
#| following URL "http://www.cecill.info".
#| 
#| As a counterpart to the access to the source code and rights to
#| copy, modify and redistribute granted by the license, users are
#| provided only with a limited warranty and the software's author,
#| the holder of the economic rights, and the successive licensors
#| have only limited liability.
#|
#| In this respect, the user's attention is drawn to the risks
#| associated with loading, using, modifying and/or developing or
#| reproducing the software by the user in light of its specific
#| status of free software, that may mean that it is complicated to
#| manipulate, and that also therefore means that it is reserved for
#| developers and experienced professionals having in-depth computer
#| knowledge. Users are therefore encouraged to load and test the
#| software's suitability as regards their requirements in conditions
#| enabling the security of their systems and/or data to be ensured
#| and, more generally, to use and operate it in the same conditions
#| as regards security.
#|
#| The fact that you are presently reading this means that you have
#| had knowledge of the CeCILL license and that you accept its terms.
import sys
import os
import sferes
sys.path.insert(0, sys.path[0]+'/waf_tools')
print sys.path[0]


from waflib.Configure import conf


import robdyn
import hexapod_controller



def options(opt) : 
    opt.load('ode')
    opt.load('robdyn')
    opt.load('hexapod_controller')
    opt.load('hexapod_robdyn_simu')


        

@conf
def configure(conf): 
    print 'conf exp:'
    conf.load('ode')
    conf.load('robdyn')
    conf.load('hexapod_controller')
    conf.load('hexapod_robdyn_simu')
    conf.check_ode()
    conf.check_robdyn()
    conf.check_hexapod_controller()
    conf.check_hexapod_robdyn_simu()


def build(bld):
     
     libs = 'HEXAPOD_ROBDYN_SIMU HEXAPOD_CONTROLLER ROBDYN ODE  EIGEN BOOST BOOST_UNIT_TEST_FRAMEWORK  BOOST_TIMER TBB '
     
     print ("Entering directory `" + os.getcwd() + "/modules/'")

     bld.program(features = 'cxx',
                 source = 'test_qd.cpp',
                 includes = '. ../../',
                 uselib = 'EIGEN BOOST BOOST_UNIT_TEST_FRAMEWORK TBB ',
                 use = 'sferes2',
                 target = 'test_qd')


    #  bld.program(features='cxx cxxstlib',
    #              source='scenarii/arm_hori.cpp',
    #              includes='. .. ../../',
    #              target='robot',
    #              uselib=libs)
                 
    #  varts=['GRID RANDOM','GRID NOSELECTION', 'GRID PARETO',
    #         'GRID FITNESS', 'GRID NOVELTY', 'GRID CURIOSITY',
    #         'GRID POPFITNESS', 'GRID POPNOVELTY', 'GRID POPCURIOSITY',
    #         'GRID TOURFITNESS', 'GRID TOURNOVELTY', 'GRID TOURCURIOSITY',
    #         'ARCHIVE RANDOM','ARCHIVE NOSELECTION', 'ARCHIVE PARETO',
    #         'ARCHIVE FITNESS','ARCHIVE NOVELTY','ARCHIVE CURIOSITY',
    #         'ARCHIVE POPFITNESS','ARCHIVE POPNOVELTY','ARCHIVE POPCURIOSITY',
    #         'ARCHIVE TOURFITNESS','ARCHIVE TOURNOVELTY','ARCHIVE TOURCURIOSITY']
    #  sferes.create_variants(bld,
    #                         source = 'scenarii/scenario_arm.cpp',
    #                         use = 'sferes2 robot',
    #                         includes='. .. ../../ ./scenarii/',
    #                         uselib = libs,
    #                         target = 'scenario_arm',
    #                         variants = varts)
 
    #  sferes.create_variants(bld,
    #                         source = 'scenarii/scenario_hexa_turn.cpp',
    #                         use = 'sferes2',
    #                         uselib = libs,
    #                         target = 'scenario_hexa_turn',
    #                         variants = varts)

    #  sferes.create_variants(bld,
    #                         source = 'scenarii/scenario_hexa_walk.cpp',
    #                         use = 'sferes2',
    #                         uselib = libs,
    #                         target = 'scenario_hexa_walk',
    #                         variants = varts)
 
    #  sferes.create_variants(bld,
    #                         source = 'scenarii/scenario_planning.cpp',
    #                         use = 'sferes2',
    #                         uselib = libs,
    #                         target = 'scenario_planning',
    #                         variants = varts)
 
 
 
 
