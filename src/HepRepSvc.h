// $Header: /nfs/slac/g/glast/ground/cvs/FluxSvc/src/FluxSvc.h,v 1.1 2002/02/02 01:33:25 srobinsn Exp $
// 
//  Original author: R.Giannitrapani

#ifndef HEPREPSVC_H
#define HEPREPSVC_H

// includes
#include <string>
#include <map>
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IIncidentListener.h"
#include "HepRepSvc/IHepRepSvc.h"

//forward declarations
template <class TYPE> class SvcFactory;
class IGlastDetSvc;
class IFiller;


class HepRepSvc : virtual public Service,  virtual public IIncidentListener,
                  virtual public IHepRepSvc
{  
 public:
  
  //  virtual StatusCode buildGeometry();
  virtual void registerFiller(IFiller*, std::string);

  virtual void useBuilder(IBuilder* b);

  virtual fillerCol& getFillersByType(std::string type);

  virtual std::vector<std::string>& getTypeTrees();

  virtual unsigned int size(){return m_fillers.size();};

  //------------------------------------------------------------------
  //  stuff required by a Service
    
  /// perform initializations for this service. 
  virtual StatusCode initialize ();
    
  /// perform the finalization, as required for a service.
  virtual StatusCode finalize ();
  
  /// Query interface
  virtual StatusCode queryInterface( const IID& riid, void** ppvUnknown );
    
  /// Handles incidents, implementing IIncidentListener interface
  virtual void handle(const Incident& inc);    

  /// This method produce an XML file with the actual HepRep representation. 
  void saveXML(std::string fileName);

  /// This method add an instancetree 
  void addInstanceTree(std::string type, std::string instance)
    {m_instances[type] = instance;}

  /// This method clear the instancetree map
  void clearInstanceTrees(){m_instances.clear();}

  /// Get the instance trees map
  const std::map<std::string, std::string>& getInstanceTrees(){return m_instances;};  

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

    /// A property to be used to set the saving of XML HepRep files to true
    /// or false
    bool m_saveXml;

    /// A property to set the local path to save the XML HepRep files
    std::string m_xmlPath;

    /// A property to set the depth of the geometry 
    int m_geomDepth;

    /// This map holds the fillers registered in indexed by the typetree name
    std::map<std::string, fillerCol> m_fillers;
    
    /// This map holds the actual instance trees for the event together indexed
    /// by the relative typetree
    std::map<std::string, std::string> m_instances;
};


#endif // HEPREPSVC_H
