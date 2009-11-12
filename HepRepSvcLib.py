# $Header: /nfs/slac/g/glast/ground/cvs/GlastRelease-scons/HepRepSvc/HepRepSvcLib.py,v 1.2 2008/11/07 16:34:13 ecephas Exp $
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = ['HepRepSvc'])
    env.Tool('CalUtilLib')
    env.Tool('CalibDataLib')
    env.Tool('GlastSvcLib')
    env.Tool('AcdUtilLib')
    env.Tool('addLibrary', library = env['clhepLibs'])
def exists(env):
    return 1;
