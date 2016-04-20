#! /usr/bin/env python
# encoding: utf-8
# Konstantinos Chatzilygeroudis - 2015

"""
Quick n dirty DART detection
"""

import os
from waflib.Configure import conf


def options(opt):
	opt.add_option('--dart', type='string', help='path to DART physics engine/sim', dest='dart')

@conf
def check_dart(conf):
	if conf.options.dart:
		includes_check = [conf.options.dart + '/include']
		libs_check = [conf.options.dart + '/lib']
	else:
		includes_check = ['/usr/local/include', '/usr/include']
		libs_check = ['/usr/local/lib', '/usr/lib']

		if 'RESIBOTS_DIR' in os.environ:
			includes_check = [os.environ['RESIBOTS_DIR'] + '/include'] + includes_check
			libs_check = [os.environ['RESIBOTS_DIR'] + '/lib'] + libs_check

	# DART requires some of bullets includes (if installed with bullet enabled)
	bullet_check = ['/usr/local/include/bullet', '/usr/include/bullet']
	bullet_found = False
	try:
		bullet_found = conf.find_file('btBulletCollisionCommon.h', bullet_check)
	except:
		bullet_found = False

	# DART requires assimp library
	assimp_check = ['/usr/local/include', '/usr/include']
	assimp_libs = ['/usr/local/lib', '/usr/lib']
	assimp_found = False
	try:
		assimp_found = conf.find_file('assimp/scene.h', assimp_check)
		assimp_found = assimp_found and conf.find_file('libassimp.dylib', assimp_libs)
	except:
		assimp_found = False

	# DART requires OSG library for their graphic version
	osg_check = ['/usr/local/include', '/usr/include']
	osg_libs = ['/usr/local/lib', '/usr/lib', '/usr/lib/x86_64-linux-gnu']
	osg_found = False
	osg_comp = ['osg', 'osgViewer', 'osgManipulator', 'osgGA', 'osgDB','OpenThreads']
	try:
		osg_found = True
		for f in osg_comp:
			osg_found = osg_found and conf.find_file(f + '/Version', osg_check)
			osg_found = osg_found and conf.find_file('lib' + f + '.dylib', osg_libs)
	except:
		osg_found = False

	try:
		conf.start_msg('Checking for DART includes')
		res = conf.find_file('dart/dart.h', includes_check)
		#res = res and conf.find_file('dart/dart-core.h', includes_check)
		conf.end_msg('ok')
		conf.start_msg('DART: Checking for optional Bullet includes')
		more_includes = []
		if bullet_found:
			more_includes += bullet_check
			conf.end_msg('ok')
		else:
			conf.end_msg('Not found - be sure that your DART installation is without Bullet enabled', 'RED')
		if assimp_found:
			more_includes += assimp_check
		conf.start_msg('Checking for DART libs')
		res = res and conf.find_file('libdart.so', libs_check)
		#res = res and conf.find_file('libdart-core.so', libs_check)
		conf.end_msg('ok')
		conf.env.INCLUDES_DART = includes_check + more_includes
		conf.env.LIBPATH_DART = libs_check
		#conf.env.LIB_DART = ['dart', 'dart-core','dart-utils','dart-utils-urdf']
		conf.env.LIB_DART = ['dart','dart-utils','dart-utils-urdf']
		conf.start_msg('DART: Checking for Assimp')
		if assimp_found:
			conf.end_msg('ok')
			conf.env.LIBPATH_DART = conf.env.LIBPATH_DART + assimp_libs
			conf.env.LIB_DART.append('assimp')
		else:
			conf.end_msg('Not found - Your programs may not compile', 'RED')
		if bullet_found:
			conf.env.LIB_DART.append('BulletCollision')
			conf.env.LIB_DART.append('LinearMath')
		conf.start_msg('Checking for DART OSG includes (optional)')
		res = res and conf.find_file('osgDart/osgDart.h', includes_check)
		conf.end_msg('ok')
		conf.start_msg('Checking for DART OSG libs (optional)')
		res = res and conf.find_file('libosgDart.dylib', libs_check)
		conf.end_msg('ok')
		conf.env.INCLUDES_DART_GRAPHIC = conf.env.INCLUDES_DART
		conf.env.LIBPATH_DART_GRAPHIC = conf.env.LIBPATH_DART
		conf.env.LIB_DART_GRAPHIC = conf.env.LIB_DART + ['osgDart']
		conf.start_msg('DART: Checking for OSG (optional)')
		if osg_found:
			conf.env.INCLUDES_DART_GRAPHIC += osg_check
			conf.env.LIBPATH_DART_GRAPHIC += osg_libs
			conf.env.LIB_DART_GRAPHIC += osg_comp
			conf.end_msg('ok')
		else:
			conf.end_msg('Not found - Your graphical programs may not compile', 'RED')
		conf.get_env()['BUILD_GRAPHIC'] = True
	except:
		conf.end_msg('Not found', 'RED')
		return
	return 1
