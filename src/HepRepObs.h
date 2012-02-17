/** @file HepRepObs.h
@brief definition of the class HepRepObs

$Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepObs.h,v 1.4 2012/02/15 20:20:28 heather Exp $

*/
#ifndef _HepRepObs_H
#define _HepRepObs_H 1

#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/Property.h"

//#include "GuiSvc/IGuiTool.h"

#include "HepRepSvc.h"

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
* $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepObs.h,v 1.4 2012/02/15 20:20:28 heather Exp $
*/
class HepRepObs : public IToolSvc::Observer
{
public:

    HepRepObs();

    virtual ~HepRepObs() { };

    virtual void onCreate(const IAlgTool* tool);
   
    virtual void onRetrieve(const IAlgTool* tool) { };

    void setHepRepSvc(const HepRepSvc* svc){ m_hepRepSvc = svc; };

private:  
    const HepRepSvc* m_hepRepSvc;

};

#endif // _HepRepObs_H

