// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepSvc.h,v 1.15.94.1 2011/06/13 21:48:37 lsrea Exp $
// 
//  Original author: R.Giannitrapani

#ifndef HEPREPSVC_H
#define HEPREPSVC_H

// includes
#include <string>
#include <map>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IRunable.h"
#include "HepRepSvc/IHepRepSvc.h"
#include "HepRepObs.h"

//forward declarations
template <class TYPE> class SvcFactory;
class IGlastDetSvc;
class ITkrGeometrySvc;
class IRegistry;
class IStreamer;
class IServer;
class IAppMgrUI;
class SvcAdapter;
class IFluxSvc;
class IRootIoSvc;
class IDataProviderSvc;
class IAlgManager;

class HepRepSvc : virtual public Service,  
                  virtual public IIncidentListener, 
                  virtual public IHepRepSvc, 
                  virtual public IRunable 
{  
 public:

  virtual const IRegistry* getRegistry(){return m_registry;};
  
  //------------------------------------------------------------------
  //  stuff required by a Service
    
  /// perform initializations for this service. 
  virtual StatusCode initialize ();
    
  /// perform the finalization, as required for a service.
  virtual StatusCode finalize ();
  
  /// Query interface
  virtual StatusCode queryInterface( const InterfaceID& riid, void** ppvUnknown );

  /// for the IRunnable interfce
  virtual StatusCode run();
    
  /// Handles incidents, implementing IIncidentListener interface
  virtual void handle(const Incident& inc);    

  /// Set a server
  virtual void setServer(IServer* s);

  /// Add a streamer
  virtual void addStreamer(std::string name, IStreamer* s);

  /// Use a streamer to save an HepRep file
  virtual void saveHepRep(std::string strName, std::string fileName);

  /// Get the pointer to the application manager
  IAppMgrUI*  getAppMgrUI(){return m_appMgrUI;};  

  /// Get the list of sources
  std::string getSources();

  /// Set the flux
  void setSource(std::string);

  /// sent the valid setEvent command accepted by this server
  std::string getCommands();  


  /// The method tries to go back to a previous event
  bool previousEvent(int);
  
  /// This method set the Event ID to a pair Run/Event
  bool setEventId(int run, int event);

  /// This method sets the ROOT files to use as input
  bool openFile(const char* mc, const char *digi, const char *rec, 
                const char* relation, const char *gcr);

  /// This method get the Event ID as a pair Run/Event
  std::string getEventId();

  /// This method set the Event index 
  bool setEventIndex(int index);

  /// This method replay a given algorithm
  bool replayAlgorithm(std::string algName);

  /// This method change the property of a given algorithm
  bool setAlgProperty(std::string algName, std::string propName, std::string propValue);
 
  /// This method return a dir with a FRED installation, if set so in the
  /// jobOptions file; otherwise it returns (by default) an empty string
  std::string getStartFred(){return m_startFred;}; 
  std::string getStartWired() {
      //std::cout << "HepRepSvc: m_startWired = " << m_startWired << std::endl;
      return m_startWired;
  };
  
protected: 
    
    /// Standard Constructor
    HepRepSvc ( const std::string& name, ISvcLocator* al );
    
    /// destructor
    virtual ~HepRepSvc ();
    
private:
    friend class SvcFactory<HepRepSvc>;

    /// Method invoked at the start of every event
    void beginEvent();

    /// Method invoked at the end of every event
    void endEvent();

    /// A string containing a FRED dir installation; if empty, the optional
    /// server will not try to start a FRED instance to connect to the server.
    /// If not empty, the server can use this string to launch FRED and attach
    /// it to the server itself.
    std::string m_startFred;
    /// same for WIRED
    std::string m_startWired;
    
    /// A property to be used to set the saving of XML HepRep files to
    /// true or false by setting the name of a streamer; by default it
    /// is a null string, so autostream is disabled
    std::string m_autoStream;

    /// A property to set the local path to save the XML HepRep files
    std::string m_streamPath;

    /// A property to set the depth of the geometry 
    int m_geomDepth;

    /// A property to specify how to draw the geometry
    std::string m_geomType;

    /// The fillers registry
    IRegistry* m_registry;

    /// A server to be used in the run method
    IServer* m_server;

    /// A map of streamers indexed by name
    std::map<std::string, IStreamer*> m_streamers;

    /// Reference to application manager UI
    IAppMgrUI*    m_appMgrUI;

    /// The service adapter
    SvcAdapter* m_adapter;

    /// The FluxSvc, used to set fluxes from inside clients
    IFluxSvc* m_fluxSvc;

    /// The RootIoSvc used to set event Run/Event for random access from Root
    ///files
    IRootIoSvc* m_rootIoSvc;

    /// The Data Provider Service Interface.
    IDataProviderSvc* m_idpsvc;

    /// The interface to the Algorithms manager
    IAlgManager* m_AlgMgr;

    IToolSvc *m_toolSvc; // to handle observer
    HepRepObs *m_heprepObs;
};


#endif // HEPREPSVC_H
