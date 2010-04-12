# -*- python -*-
# $Header
# Authors: Riccardo Giannitrapani <riccardo@fisica.uniud.it> 
# Version: HepRepSvc-00-32-05
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

progEnv.Tool('registerTargets', package = 'HepRepSvc',
             libraryCxts = [[HepRepSvc, libEnv]],
             testAppCxts = [[test_HepRepSvc, progEnv]], 
             includes = listFiles(['HepRepSvc/*.h'], recursive=1))
