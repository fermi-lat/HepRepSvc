# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/HepRepSvc/HepRepSvcLib.py,v 1.1 2008/09/02 11:14:25 ecephas Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['HepRepSvc'])
    env.Tool('CalUtilLib')
    env.Tool('CalibDataLib')
    env.Tool('GlastSvcLib')
    env.Tool('AcdUtilLib')
def exists(env):
    return 1;
