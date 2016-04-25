#! /usr/bin/env python
# encoding: utf-8
# Konstantinos Chatzilygeroudis - 2015

"""
Quick n dirty robdyn detection
"""

import os
from waflib.Configure import conf


def options(opt):
	opt.add_option('--robdyn', type='string', help='path to robdyn', dest='robdyn')

@conf
def check_robdyn(conf):
	includes_check = ['/usr/local/include/robdyn', '/usr/include/robdyn']
	libs_check = ['/usr/local/lib', '/usr/lib']
	if 'RESIBOTS_DIR' in os.environ:
		includes_check = [os.environ['RESIBOTS_DIR'] + '/include/robdyn'] + includes_check
		libs_check = [os.environ['RESIBOTS_DIR'] + '/lib'] + libs_check

	if conf.options.robdyn:
		includes_check = [conf.options.robdyn + '/include/robdyn']
		libs_check = [conf.options.robdyn + '/lib']

	try:
		conf.start_msg('Checking for robdyn includes')
		res = conf.find_file('ode/environment.hh', includes_check)
		res = res and conf.find_file('ode/environment_hexa.hh', includes_check)
		conf.end_msg('ok')
		conf.start_msg('Checking for robdyn libs')
		res = res and conf.find_file('librobdyn.a', libs_check)
		conf.end_msg('ok')
		conf.env.INCLUDES_ROBDYN = includes_check
		conf.env.LIBPATH_ROBDYN = libs_check
		conf.env.DEFINES_ROBDYN = ['USE_ROBDYN']
		conf.env.LIB_ROBDYN = ['robdyn']
		conf.start_msg('Checking for robdyn osg visitor libs')
		res = res and conf.find_file('librobdyn_osgvisitor.a', libs_check)
		conf.end_msg('ok')
		conf.get_env()['BUILD_GRAPHIC'] = True
		conf.env.LIB_ROBDYN.append('robdyn_osgvisitor');
	except:
		conf.end_msg('Not found', 'RED')
		return
	return 1