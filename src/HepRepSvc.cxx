// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepSvc.cxx,v 1.10 2004/07/21 15:31:29 riccardo Exp $
// 
//  Original author: R.Giannitrapani
//

#include <sstream>

#include "Registry.h"
#include "HepRepSvc/IServer.h"
#include "HepRepSvc/IStreamer.h"
#include "HepRepSvc/IRegister.h"

#include "SvcAdapter.h"
#include "HepRepSvc.h"
#include "HepRepGeometry.h"

#include "MonteCarloFiller.h"
#include "ReconFiller.h"
#include "GeometryFiller.h"

#include "FluxSvc/IFluxSvc.h"
#include "FluxSvc/IFlux.h"

#include "RootIo/IRootIoSvc.h"

#include "Event/TopLevel/Event.h"
#include "Event/TopLevel/EventModel.h"

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/IObjManager.h"
#include "GaudiKernel/IToolFactory.h"
#include "GaudiKernel/IAppMgrUI.h"
#include "GaudiKernel/Property.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"

// declare the service factories for the FluxSvc
static SvcFactory<HepRepSvc> a_factory;
const ISvcFactory& HepRepSvcFactory = a_factory;

// Standard Constructor
HepRepSvc::HepRepSvc(const std::string& name,ISvcLocator* svc)
: Service(name,svc)
{
  /// This property turn on/off the auto streaming of the heprep at
  /// the end of each event
  declareProperty("autoStream" , m_autoStream=""); 
  declareProperty("streamPath"   , m_streamPath="");
  declareProperty("geometryDepth", m_geomDepth=4);

  m_server=0;
}

// Standard Destructor
HepRepSvc::~HepRepSvc()  
{
}

// This method implement the IRunnable run method by delegating it to
// the registered IServer, if it has been registered.
StatusCode HepRepSvc::run()
{
  if (m_server!=0)
    m_server->run();
  return StatusCode::SUCCESS;
};

// initialize
StatusCode HepRepSvc::initialize () 
{
    StatusCode  status =  Service::initialize ();
    
    // bind all of the properties for this service
    setProperties ();

    // get the application manager UI
    serviceLocator()->queryInterface(IID_IAppMgrUI, (void**)&m_appMgrUI);

    // Build the registry
    m_registry = new Registry();

    // Build the service adapter
    m_adapter = new SvcAdapter(this);
    
    // open the message log
    MsgStream log( msgSvc(), name() );

    // get the Particle Property Service
    IParticlePropertySvc* pps = 0;
    status = service("ParticlePropertySvc", pps);
    if( status.isFailure()) {
      log << MSG::ERROR << "Could not find ParticlePropertySvc" << endreq;
      return status;
    }

    // get the Glast Detector Service    
    IGlastDetSvc* gsvc = 0;
    status = service("GlastDetSvc", gsvc);
    if( status.isFailure()) {
      log << MSG::ERROR << "Could not find GlastDetSvc" << endreq;
      return status;
    }

    // get the Event Data Service
    IDataProviderSvc* esvc = 0;
    status = service("EventDataSvc", esvc);
    if( status.isFailure()) {
      log << MSG::ERROR << "Could not find EventDataSvc" << endreq;
      return status;
    }

    m_idpsvc = esvc;
    // get the Flux Service    
    IService* theSvc;
    StatusCode sc = serviceLocator()->getService( "FluxSvc", theSvc, true );
    if ( sc.isSuccess() ) {
      sc = theSvc->queryInterface(IFluxSvc::interfaceID(), (void**)&m_fluxSvc);
    }     
    else m_fluxSvc = 0;

    sc = serviceLocator()->getService( "RootIoSvc", theSvc, true );
    if ( sc.isSuccess() ) {
      sc = theSvc->queryInterface(IRootIoSvc::interfaceID(), (void**)&m_rootIoSvc);
    }
    else m_rootIoSvc = 0;
     
    
    
    // use the incident service to register begin, end events
    IIncidentSvc* incsvc = 0;
    status = service ("IncidentSvc", incsvc, true);
    if( status.isFailure() ) return status;
    incsvc->addListener(this, "BeginEvent", 100);
    incsvc->addListener(this, "EndEvent", 0);
    
    // Register the geometry filler
    m_registry->registerFiller(new GeometryFiller(m_geomDepth,gsvc), 
			       "Geometry3D");
    // Register the Recon filler 
    m_registry->registerFiller(new ReconFiller(gsvc,esvc,pps), "Event");
    // Register the mc filler
    m_registry->registerFiller(new MonteCarloFiller(gsvc,esvc,pps), "Event");



    //----------------------------------------------------------------
    // most of  the following taken from FluxSvc
    
    // look for a factory of an AlgTool that implements the IRegister interface:
    // if found, make one and call the special method 
    
    // Manager of the AlgTool Objects
    IObjManager* objManager=0;             
    
    // locate Object Manager to locate later the tools 
    status = serviceLocator()->service("ApplicationMgr", objManager );
    if( status.isFailure()) {
      log << MSG::ERROR << "Unable to locate ObjectManager Service" << endreq;
      return status;
    }
    
    IToolFactory* toolfactory = 0;
    
    // search throught all objects (factories?)
    for(IObjManager::ObjIterator it = objManager->objBegin(); 
	it !=objManager->objEnd(); ++ it){
      
      std::string tooltype= (*it)->ident();
      // is it a tool factory?
      const IFactory* factory = objManager->objFactory( tooltype );
      IFactory* fact = const_cast<IFactory*>(factory);
      status = fact->queryInterface( IID_IToolFactory, (void**)&toolfactory );
      if( status.isSuccess() ) {
	
	IAlgTool* itool = toolfactory->instantiate(name()+"."+tooltype,  this );       
	IRegister* ireg;
	status =itool->queryInterface( IRegister::interfaceID(), (void**)&ireg);
	if( status.isSuccess() ){
	  log << MSG::INFO << "Registering HepRep server/streamer " << endreq;
	  ireg->registerMe(this);
	}
	log << MSG::DEBUG << "Releasing the tool " << tooltype << endreq;
	itool->release();
      }
      
    }
    
    return StatusCode::SUCCESS;
}

// finalize
StatusCode HepRepSvc::finalize ()
{
  // @todo Need to destroy all the streamers and the server
  StatusCode  status = StatusCode::SUCCESS;

  if (m_server!=0)
    m_server->shutDown();

	std::cout << "Closing " << std::endl;

  return status;
}

// handle "incidents"
void HepRepSvc::handle(const Incident &inc)
{
  if( inc.type()=="BeginEvent")beginEvent();
  else if(inc.type()=="EndEvent")endEvent();
}

// This method is called for each event at the beginning
void HepRepSvc::beginEvent()
{
}

// This method is called for each event at the end
void HepRepSvc::endEvent()
{
  // open the message log
  MsgStream log( msgSvc(), name() );

  std::stringstream sName;
  
  // set the name of the instance tree representing the event this is
  // a temporary hack that set the name as Event-xxx, with xxx an
  // increasing integer
  static int temp = 0;
  sName << "Event-" << temp;
  temp++;


  // This is to retrive event and run number from the event, but seems to be
  // broken .. so I comment it out for now
  // SmartDataPtr<Event::EventHeader>
  //  evt(m_idpsvc, EventModel::EventHeader);
  // if (evt)
  // {
  // unsigned int evtRun = evt->run();
  // unsigned int evtEvent = evt->event();
  // }
  
  // Set the registry with the instance trees names of this event
  // after clearing the names list; we also add the dependency of
  // the event instancetree to the geometry instancetree
  m_registry->clearInstanceTrees();
  m_registry->addInstanceTree("Geometry3D","GLAST-LAT");
  m_registry->addInstanceTree("Event",sName.str());
  m_registry->addDependency(sName.str(),"GLAST-LAT");
  
  // If autoStream has been set to a name of a streamer in the
  // jobOptions file, than we save automatically the HepRep to a file
  if (m_autoStream!="")
    {
      std::stringstream sFileName;
      sFileName << m_streamPath << sName.str();
      saveHepRep(m_autoStream, sFileName.str());
      log << MSG::DEBUG << "Streamed the HepRep to file using the " << 
    	m_autoStream << " streamer" << endreq;
    }
}

// This method save the HepRep to an external file using one streamer
// identified by its name
void HepRepSvc::saveHepRep(std::string strName, std::string fileName)
{
  IStreamer* st = m_streamers[strName];

  if (st != 0)
    st->saveHepRep(fileName);
}

// This method set the server used for an interactive session
void HepRepSvc::setServer(IServer* s)
{
  s->setRegistry(m_registry);
  s->setSvcAdapter(m_adapter);
  m_server = s;
}

// This method add a new streamer to the list of registered one, using
// a (unique) name for identification
void HepRepSvc::addStreamer(std::string name, IStreamer* s)
{
  s->setRegistry(m_registry);
  m_streamers[name] = s;
}


// Return valid commands accepted by this server
std::string HepRepSvc::getCommands()
{
  std::stringstream sNames;
  sNames << "next,commands";  
  
  if (m_fluxSvc)
    sNames << ",fluxes,source";
  
  if (m_rootIoSvc)
    sNames << ",eventId,eventIdx";
  
  return sNames.str();     
}

// This method return the list of sources
std::string HepRepSvc::getSources(){
  std::stringstream sNames;

  std::vector<std::pair< std::string ,std::list<std::string> > > sources = m_fluxSvc->sourceOriginList();
  std::vector<std::pair< std::string ,std::list<std::string> > >::const_iterator sit;

      for (sit=sources.begin(); sit!=sources.end(); ++sit) {
        std::string libname = (*sit).first;
        std::list<std::string> slist = (*sit).second;
        std::list<std::string>::const_iterator fit;
        sNames << libname << ": \n";
        for(fit=slist.begin(); fit!=slist.end(); ++fit){
            sNames << "  - " << (*fit) << "\n";            
        }
      }

      sNames << '\0';

  return sNames.str();
}

// This method set the Event ID to a pair Run/Event
bool HepRepSvc::setEventId(int run, int event)
{
  // Make sure Index is set to -1
  m_rootIoSvc->setIndex(-1);
  return m_rootIoSvc->setRunEventPair(std::pair<int, int>(run, event));
}

// This method set the Event index
bool HepRepSvc::setEventIndex(int index)
{
  return m_rootIoSvc->setIndex(index);
}

// This method set the actual source
void HepRepSvc::setSource(std::string source){
  IFlux *   iflux;
  m_fluxSvc->source(source, iflux);
}


/// Query interface
StatusCode HepRepSvc::queryInterface(const IID& riid, void** ppvInterface)  {
    if ( IID_IHepRepSvc.versionMatch(riid) )  {
        *ppvInterface = (IHepRepSvc*)this;
    }
    else if (IID_IRunable.versionMatch(riid) ) {
      *ppvInterface = (IRunable*)this;
    }
    else  {
      return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return SUCCESS;
}


void WARNING (const char * text ){  std::cerr << "WARNING: " << text << '\n';}
void FATAL(const char* s){std::cerr << "\nERROR: "<< s;}
