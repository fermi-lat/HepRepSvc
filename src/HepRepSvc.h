// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepSvc.h,v 1.4 2004/05/20 08:13:47 riccardo Exp $
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

//forward declarations
template <class TYPE> class SvcFactory;
class IGlastDetSvc;
class IRegistry;
class IStreamer;
class IServer;
class IAppMgrUI;
class SvcAdapter;
class IFluxSvc;
class IRootIoSvc;

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
  virtual StatusCode queryInterface( const IID& riid, void** ppvUnknown );

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

  /// This method set the Event ID to a pair Run/Event
  bool setEventId(int run, int event);

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

    /// A property to be used to set the saving of XML HepRep files to
    /// true or false by setting the name of a streamer; by default it
    /// is a null string, so autostream is disabled
    std::string m_autoStream;

    /// A property to set the local path to save the XML HepRep files
    std::string m_streamPath;

    /// A property to set the depth of the geometry 
    int m_geomDepth;

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
};


#endif // HEPREPSVC_H
