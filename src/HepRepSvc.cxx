// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepSvc.cxx,v 1.2 2002/09/20 14:21:07 riccardo Exp $
// 
//  Original author: R.Giannitrapani
//

#ifdef DEFECT_NO_STRINGSTREAM
#include <strstream>
#else
#include <sstream>
#endif

#include "zlib.h"

#include "HepRepSvc/IFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "XmlBuilder.h"

#include "HepRepSvc.h"
#include "HepRepGeometry.h"

#include "MonteCarloFiller.h"
#include "ReconFiller.h"
#include "GeometryFiller.h"

#include "Event/TopLevel/Event.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
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
  declareProperty("saveXml" , m_saveXml=0); 
  declareProperty("xmlPath"   , m_xmlPath="");
  declareProperty("geometryDepth", m_geomDepth=4);
}

// Standard Destructor
HepRepSvc::~HepRepSvc()  
{
}

// initialize
StatusCode HepRepSvc::initialize () 
{
    StatusCode  status =  Service::initialize ();
    
    // bind all of the properties for this service
    setProperties ();
    
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

   // use the incident service to register begin, end events
    IIncidentSvc* incsvc = 0;
    status = service ("IncidentSvc", incsvc, true);
    if( status.isFailure() ) return status;

    incsvc->addListener(this, "BeginEvent", 100);
    incsvc->addListener(this, "EndEvent", 0);

    registerFiller(new GeometryFiller(m_geomDepth,gsvc), "Geometry3D");
    registerFiller(new ReconFiller(gsvc,esvc,pps), "Event");
    registerFiller(new MonteCarloFiller(gsvc,esvc,pps), "Event");
    
    return status;
}

// This method set a builder to be used by the registered fillers
void HepRepSvc::useBuilder(IBuilder* b)
{
  std::map<std::string, fillerCol>::iterator i;

  for(i=m_fillers.begin();i!=m_fillers.end();i++)
    {
      fillerCol::iterator j;
      
      for(j=(i->second).begin();j!=(i->second).end();j++)
        {
          (*j)->setBuilder(b);
        }
    }
}

// This method return the fillers by the type
IHepRepSvc::fillerCol& HepRepSvc::getFillersByType(std::string type)
{
  return m_fillers[type];
}

std::vector<std::string>& HepRepSvc::getTypeTrees()
{
  std::vector<std::string>* types = new std::vector<std::string>; 

  std::map<std::string, fillerCol>::iterator i;
  
  for(i=m_fillers.begin();i!=m_fillers.end();i++)
    {
      types->push_back(i->first);
    }
  
  return *types;
}

// Register a filler with the service
void HepRepSvc::registerFiller(IFiller* f, std::string tree)
{
  m_fillers[tree].push_back(f);
}

// finalize
StatusCode HepRepSvc::finalize ()
{
    StatusCode  status = StatusCode::SUCCESS;

    return status;
}

/// Query interface
StatusCode HepRepSvc::queryInterface(const IID& riid, void** ppvInterface)  {
    if ( IID_IHepRepSvc.versionMatch(riid) )  {
        *ppvInterface = (IHepRepSvc*)this;
    }
    else  {
        return Service::queryInterface(riid, ppvInterface);
    }
    addRef();
    return SUCCESS;
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

#ifdef DEFECT_NO_STRINGSTREAM
  std::strstream sName;
  std::strstream sFileName;
#else
  std::stringstream sName;
  std::stringstream sFileName;
#endif

  IDataProviderSvc* esvc = 0;
  service("EventDataSvc", esvc);

  SmartDataPtr<Event::EventHeader>
    event(esvc, "/Event");

  static temp = 0;
  // sName << "Event-" << event->event();
  sName << "Event-" << temp;
  temp++;
  
  clearInstanceTrees();
  addInstanceTree("Geometry3D","GLAST-LAT");
#ifdef DEFECT_NO_STRINGSTREAM
  sName << std::ends;
#endif
  addInstanceTree("Event",sName.str());
  if (m_saveXml)
    {
      sFileName << m_xmlPath << sName.str() << ".xml.gz";
#ifdef DEFECT_NO_STRINGSTREAM
      sFileName << std::ends;
#endif
      saveXML(sFileName.str());
      log << MSG::DEBUG << "Saved XML file for HepRep" << endreq;
    }
}

void HepRepSvc::saveXML(std::string nameFile)
{
  
  gzFile file = gzopen(nameFile.c_str(), "wb9");  
  
  XMLBuilder builder(file);
  useBuilder(&builder);
  
  typedef std::vector<IFiller*> fillerCol;
  std::vector<std::string>::const_iterator i; 

  gzprintf(file,"<heprep>\n");

  std::vector<std::string>& types = getTypeTrees();

  for(i=types.begin();i!=types.end();i++)
    {
      fillerCol::const_iterator j;
      gzprintf(file,
               "<typetree name=\"%s\" version=\"1.0\">\n",  
               (*i).c_str());
      
      fillerCol temp = getFillersByType((*i));
      
      for(j=temp.begin();j!=temp.end();j++)
        {
          (*j)->buildTypes();
          builder.endTypes();
        }

      gzprintf(file,"</typetree>\n");
      builder.reset();
    }

  std::map<std::string, std::string>::const_iterator it;

  std::vector<std::string> typesList;

  for(it=m_instances.begin();it!=m_instances.end();it++)
    {
      std::cout << it->second.c_str() << std::endl;
      fillerCol::const_iterator jt;
      fillerCol temp = getFillersByType(it->first);
      gzprintf(file,
              "<instancetree name=\"%s\" version=\"1.0\" reqtypetree=\"\" ",  
              it->second.c_str()); 
      gzprintf(file,"typeName=\"%s\" typeVersion=\"1.0\">\n" ,it->first.c_str());

      for(jt=temp.begin();jt!=temp.end();jt++)
        {
          (*jt)->fillInstances(typesList);          
          builder.endInstances();
        }
      gzprintf(file,"</instancetree>\n");
      builder.reset();
    }      

  gzprintf(file,"</heprep>\n");
  
  gzclose(file);


}

void WARNING (const char * text ){  std::cerr << "WARNING: " << text << '\n';}
void FATAL(const char* s){std::cerr << "\nERROR: "<< s;}
