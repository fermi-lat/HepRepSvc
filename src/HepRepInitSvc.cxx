
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "HepRepSvc/HepRepInitSvc.h"
//#include "src/HepRepControl.h"

//static const SvcFactory<HepRepInitSvc> s_factory;
//const ISvcFactory& HepRepInitSvcFactory = s_factory;
DECLARE_SERVICE_FACTORY(HepRepInitSvc);

HepRepInitSvc::HepRepInitSvc(const std::string& name, ISvcLocator* pSvcLocator) :
Service(name, pSvcLocator)
{
    // Get a pointer to the TkrControl object
    //HepRepControl* control = HepRepControl::getPtr();

    // Variables which can be modified in TkrControl
    declareProperty("VertexFiller_dashes",           m_vtx_dashes=true);
    declareProperty("ClusterFiler_showWide",         m_cls_showWide=true);
    declareProperty("MeritTupleFiller_doIt",         m_merit_doIt=true);
    declareProperty("MonteCarloFiller_useMcInfo",    m_useMcInfo=true);
    declareProperty("ClusterUtil_useTriggerInfo",    m_useTriggerInfo=true);
    declareProperty("ClusterUtil_useDiagnosticInfo", m_useDiagnosticInfo=true);
    declareProperty("ClusterUtil_useToTInfo",        m_useToTInfo=true);
    declareProperty("CalReconFiller_useColors",      m_calRecon_useColors=false);
    return; 
}

StatusCode HepRepInitSvc::initialize()
{
    StatusCode sc = StatusCode::SUCCESS;
    
    Service::initialize();
    setProperties();
    MsgStream log(msgSvc(), name());
    

    // Take care of resetting of control variables (if necessary)
    /*
    HepRepControl* control = HepRepControl::getPtr();
    if (m_vtxFllr_dashes != control->getVtxFllr_dashes() ) 
        control->setVtxFllr_dashes(m_vtxFllr_dashes);
    log << MSG::INFO << "m_vtxFllr_dashes is set to: " << control->getVtxFllr_dashes() << endreq;
    */

    return sc;
}

StatusCode HepRepInitSvc::finalize()
{
    return StatusCode::SUCCESS;
}

// queryInterface

StatusCode  HepRepInitSvc::queryInterface (const InterfaceID& riid, void **ppvIF)
{
    if (HepRepInitSvc::interfaceID() == riid) {
        *ppvIF = dynamic_cast<HepRepInitSvc*> (this);
        return StatusCode::SUCCESS;
    }
    else {
        return Service::queryInterface (riid, ppvIF);
    }
}

// access the type of this service

const InterfaceID&  HepRepInitSvc::type () const {
    return HepRepInitSvc::interfaceID();
}
