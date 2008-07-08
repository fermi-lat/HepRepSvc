
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
 * $Header: /nfs/slac/g/glast/ground/cvs/TkrRecon/TkrRecon/Services/HepRepInitSvc.h,v 1.17 2006/11/04 16:31:47 lsrea Exp $
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

 
private:

    /// Variables which can be changed in TkrControl
    bool m_vtx_dashes;
};

#endif // __HEPREPINITSVC_H
