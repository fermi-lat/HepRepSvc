// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/HepRepSvc/IHepRepSvc.h,v 1.1.1.1 2002/09/20 08:50:19 riccardo Exp $
// 
//!  \author: R.Giannitrapani
//

#ifndef IHEPREPSVC_H
#define IHEPREPSVC_H
#include <string>
#include "GaudiKernel/IService.h"

class IRegistry;
class IServer;
class IStreamer;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IHepRepSvc(950, 1 , 0); 


/** 
 * @class IHepRepSvc
 *
 * @brief The abstract interface to the HepRepSvc
 *
 * This service is the main one to be used for HepRep production
 *
 */
class  IHepRepSvc : virtual public IService {
 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IHepRepSvc; } 
  
  /// Return the pointer to the Registry
  virtual const IRegistry* getRegistry() = 0;

  /// Use a streamer to save an HepRep file
  virtual void saveHepRep(std::string strName, std::string fileName) = 0;

  /// Set a server 
  virtual void setServer(IServer*) = 0;

  /// Add a streamer with a name
  virtual void addStreamer(std::string, IStreamer*) = 0;
};

#endif  // IHEPREPSVC_H
