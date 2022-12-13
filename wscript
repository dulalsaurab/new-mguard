# -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

from waflib import Context, Logs, Utils
import os, subprocess

VERSION = "0.0.1"
APPNAME = "mguard"

def options(opt):
    opt.load(['compiler_c', 'compiler_cxx', 'gnu_dirs'])
    opt.load(['default-compiler-flags', 'coverage', 'sanitizers',
              'boost'],
             tooldir=['.waf-tools'])

    optgrp = opt.add_option_group('mguard Options')
    optgrp.add_option('--with-examples', action='store_true', default=False,
                      help='Build examples')

    optgrp.add_option('--with-tests', action='store_true', default=False,
                      help='Build unit tests')
    
def configure(conf):
    conf.load(['compiler_c', 'compiler_cxx', 'gnu_dirs',
               'default-compiler-flags', 'boost'])

    conf.env.WITH_EXAMPLES = conf.options.with_examples
    conf.env.WITH_TESTS = conf.options.with_tests    

    pkg_config_path = os.environ.get('PKG_CONFIG_PATH', '%s/pkgconfig' % conf.env.LIBDIR)
    conf.check_cfg(package='libndn-cxx', args=['--cflags', '--libs'], uselib_store='NDN_CXX',
                   pkg_config_path=pkg_config_path)

    # conf.check_cfg(package='gtkmm-3.0', args=['--cflags', '--libs'], uselib_store='gtkmm', pkg_config_path=pkg_config_path)


    boost_libs = ['system', 'iostreams', 'filesystem', 'regex']
    
    if conf.env.WITH_TESTS:
        boost_libs.append('unit_test_framework')

    conf.check_boost(lib=boost_libs, mt=True)

    conf.check_cfg(package='libnac-abe', args=['--cflags', '--libs'], uselib_store='NAC-ABE',
                   pkg_config_path=pkg_config_path)

    conf.check_cfg(package='PSync', args=['--cflags', '--libs'], uselib_store='PSYNC',
                   pkg_config_path=pkg_config_path)

    conf.check_compiler_flags()

    # Loading "late" to prevent tests from being compiled with profiling flags
    conf.load('coverage')
    conf.load('sanitizers')

    conf.env.prepend_value('STLIBPATH', ['.'])
    conf.define_cond('WITH_TESTS', conf.env.WITH_TESTS)
    # The config header will contain all defines that were added using conf.define()
    # or conf.define_cond().  Everything that was added directly to conf.env.DEFINES
    # will not appear in the config header, but will instead be passed directly to the
    # compiler on the command line.
    conf.write_config_header('src/config.hpp')


def build(bld):
    bld.shlib(features="c cshlib",
              target='mguard',
              vnum=VERSION,
              cnum=VERSION,
              source=bld.path.ant_glob('src/**/*.cpp'),
              use='NDN_CXX BOOST PSYNC NAC-ABE gtkmm',
              includes='./src',
              export_includes='./src')

    if bld.env.WITH_TESTS:
        bld.recurse('tests')

    if bld.env.WITH_EXAMPLES:
        bld.recurse('examples')

    # bld.recurse('controller')

    headers = bld.path.ant_glob('src/**/*.hpp')
    bld.install_files(bld.env.INCLUDEDIR, headers, relative_trick=True)

    bld.install_files('${INCLUDEDIR}/',
                      bld.path.find_resource('src/config.hpp'))

    bld(features='subst',
        source='mguard.pc.in',
        target='mguard.pc',
        install_path='${LIBDIR}/pkgconfig',
        VERSION=VERSION)

def version(ctx):
    # don't execute more than once
    if getattr(Context.g_module, 'VERSION_BASE', None):
        return

    Context.g_module.VERSION_BASE = Context.g_module.VERSION
    Context.g_module.VERSION_SPLIT = VERSION_BASE.split('.')

    # first, try to get a version string from git
    gotVersionFromGit = False
    try:
        cmd = ['git', 'describe', '--always', '--match', '%s*' % GIT_TAG_PREFIX]
        out = subprocess.check_output(cmd, universal_newlines=True).strip()
        if out:
            gotVersionFromGit = True
            if out.startswith(GIT_TAG_PREFIX):
                Context.g_module.VERSION = out.lstrip(GIT_TAG_PREFIX)
            else:
                # no tags matched
                Context.g_module.VERSION = '%s-commit-%s' % (VERSION_BASE, out)
    except (OSError, subprocess.CalledProcessError):
        pass

    versionFile = ctx.path.find_node('VERSION')
    if not gotVersionFromGit and versionFile is not None:
        try:
            Context.g_module.VERSION = versionFile.read()
            return
        except EnvironmentError:
            pass

    # version was obtained from git, update VERSION file if necessary
    if versionFile is not None:
        try:
            if versionFile.read() == Context.g_module.VERSION:
                # already up-to-date
                return
        except EnvironmentError as e:
            Logs.warn('%s exists but is not readable (%s)' % (versionFile, e.strerror))
    else:
        versionFile = ctx.path.make_node('VERSION')

    try:
        versionFile.write(Context.g_module.VERSION)
    except EnvironmentError as e:
        Logs.warn('%s is not writable (%s)' % (versionFile, e.strerror))

def dist(ctx):
    version(ctx)

def distcheck(ctx):
    version(ctx)
