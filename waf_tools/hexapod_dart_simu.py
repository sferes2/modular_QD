#! /usr/bin/env python
# encoding: utf-8
# Konstantinos Chatzilygeroudis - 2015

"""
Quick n dirty hexapod_dart_simu detection
"""

import os
from waflib.Configure import conf


def options(opt):
    opt.add_option('--simu', type='string', help='path to hexapod_dart_simu', dest='simu')

@conf
def check_hexapod_dart_simu(conf):
    includes_check = ['/usr/local/include', '/usr/include']
    libs_check = ['/usr/local/lib', '/usr/lib']

    if 'RESIBOTS_DIR' in os.environ:
        includes_check = [os.environ['RESIBOTS_DIR'] + '/include'] + includes_check
        libs_check = [os.environ['RESIBOTS_DIR'] + '/lib'] + libs_check

    if conf.options.simu:
        includes_check = [conf.options.simu + '/include']
        libs_check = [conf.options.simu + '/lib']

    try:
        conf.start_msg('Checking for hexapod_dart_simu includes')
        res = conf.find_file('hexapod_dart/hexapod_dart_simu.hpp', includes_check)
        conf.end_msg('ok')
        conf.env.INCLUDES_HEXAPOD_DART_SIMU = includes_check
        
    except:
        conf.end_msg('Not found', 'RED')
        return
    return 1
