/** @file HepRepObs.h
@brief definition of the class HepRepObs

$Header: /nfs/slac/g/glast/ground/cvs/GlastSvc/src/GuiSvc/GuiSvc.h,v 1.13 2006/03/21 01:26:09 usher Exp $

*/
#ifndef _HepRepObs_H
#define _HepRepObs_H 1

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Property.h"

#include "GuiSvc/IGuiTool.h"

/** @class HepRepObs
*
* @brief Gaudi Service for setting the random engines and seeds
* for shared libraries that use random numbers (via CLHEP) 
* 
* This service, in its initialize() method, collects the adresses 
* of all tools that implement the  IRandomAccess interface (one in each
* Dll that uses random numbers). The RandomAccess tool lives in 
* GlastRandomSvc. The initialize() method also sets the random engine
* for each of its Dll's either via the job options parameter
* RandomEngine or the default which is currently TripleRand. The
* handle() methods listens for BeginEvent events via the
* IncidentListener service and increments the run and particle 
* sequence numbers, sets those in the MCEvent header, then sets the
* seed for each of the Dll's that use randoms, based on the run and
* particle sequence numbers.
* 
*
* @authors Toby Burnett, Karl Young
*
* $Header: /nfs/slac/g/glast/ground/cvs/GlastSvc/src/GlastRandomSvc/GlastRandomSvc.h,v 1.13 2006/03/21 01:26:09 usher Exp $
*/
class HepRepObs : public IToolSvc::Observer
{
public:

    HepRepObs();

    virtual ~HepRepObs() { };

    void onCreate(IAlgTool& tool);
   
    void onRetrieve(IAlgTool& tool) { };

    void setHepRepMgr(gui::GuiMgr* guiMgr) { m_guiMgr = guiMgr; };

private:  
    gui::GuiMgr* m_guiMgr;

};

#endif // _HepRepObs_H

