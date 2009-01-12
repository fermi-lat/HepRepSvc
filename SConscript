# -*- python -*-
# $Id: SConscript,v 1.15 2008/12/08 21:39:59 ecephas Exp $
# Authors: Riccardo Giannitrapani <riccardo@fisica.uniud.it> 
# Version: HepRepSvc-00-31-00

Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('HepRepSvcLib', depsOnly = 1)

HepRepSvc = libEnv.SharedLibrary('HepRepSvc', listFiles(['src/*.cxx',
				'src/Dll/*.cxx',
				'src/Tracker/*.cxx',
				'src/xml/*.cxx']))

progEnv.Tool('HepRepSvcLib')
test_HepRepSvc = progEnv.GaudiProgram('test_HepRepSvc', listFiles(['src/test/*.cxx']), test =1)

progEnv.Tool('registerObjects', package = 'HepRepSvc',
             libraries = [HepRepSvc],
             testApps = [test_HepRepSvc], 
             includes = listFiles(['HepRepSvc/*.h']))




