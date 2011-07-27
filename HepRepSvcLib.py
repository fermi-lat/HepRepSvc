# $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/HepRepSvcLib.py,v 1.3 2009/11/12 02:00:30 jrb Exp $
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
