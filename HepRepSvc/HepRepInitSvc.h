
#ifndef __HEPREPINITSVC_H
#define __HEPREPINITSVC_H 1

#include "GaudiKernel/Service.h"

/** 
 * @class HepRepInitSvc
 *
 * @brief Initializes the tracker reconstruction
 *
 * 05-Feb-2002
 *
 * Sets up stages of the tracker reconstruction. Currently, patrec is the only
 * stage with more than one possible algorithm
 * 
 * @author Tracy Usher
 *
 * $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/HepRepSvc/HepRepInitSvc.h,v 1.3 2008/09/10 01:26:40 lsrea Exp $
 */


static const InterfaceID IID_HepRepInitSvc("HepRepInitSvc", 0 , 0); 

class HepRepInitSvc : public Service, public virtual IInterface
{
public:

    HepRepInitSvc(const std::string& name, ISvcLocator* pSvcLocator); 
   ~HepRepInitSvc() {}
    
    StatusCode       initialize();
    StatusCode       finalize();
        
    /// queryInterface - for implementing a Service this is necessary
    StatusCode       queryInterface(const InterfaceID& riid, void** ppvUnknown);

    static const InterfaceID& interfaceID() { return IID_HepRepInitSvc; }

    /// return the service type
    const InterfaceID& type() const;

    const bool getVertexFiller_dashes(){return m_vtx_dashes;}
    const bool getClusterFiller_showWide() { return m_cls_showWide;}
    const bool getMeritTupleFiller_doIt()  { return m_merit_doIt; } 

 
private:

    // Variables connected to jO parameters

    bool m_vtx_dashes;
    bool m_cls_showWide;
    bool m_merit_doIt;
};

#endif // __HEPREPINITSVC_H
