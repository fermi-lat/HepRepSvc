# -*- python -*-
# $Header
# Authors: Riccardo Giannitrapani <riccardo@fisica.uniud.it> 
# Version: HepRepSvc-00-27-01
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

progEnv.Tool('registerObjects', package = 'HepRepSvc', libraries = [HepRepSvc], testApps = [test_HepRepSvc], 
	includes = listFiles(['HepRepSvc/*.h']))

