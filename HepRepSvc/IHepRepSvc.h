// $Header: /nfs/slac/g/glast/ground/cvs/FluxSvc/FluxSvc/IFluxSvc.h,v 1.12 2002/01/14 23:22:39 burnett Exp $
// 
//!  \author: R.Giannitrapani
//

#ifndef IHEPREPSVC_H
#define IHEPREPSVC_H
#include <string>
#include <vector>
#include "GaudiKernel/IService.h"

class IFiller;
class IBuilder;
class Registry;

// Declaration of the interface ID ( interface id, major version, minor version) 
static const InterfaceID IID_IHepRepSvc(950, 1 , 0); 


//! Abstract interface for the HepRep service, HepRepSvc.
class  IHepRepSvc : virtual public IService {
 public:

  typedef std::vector<IFiller*> fillerCol;

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IHepRepSvc; } 
  
  /// Method to register a new filler, given a TypeTree name
  virtual void registerFiller(IFiller* f, std::string tree) = 0;

  /// Method to set a concrete builder for the fillers
  virtual void useBuilder(IBuilder* b) = 0;

  /// Return a vector of fillers given the TypeTree name
  virtual fillerCol& getFillersByType(std::string type) = 0;

  /// Return a vector of TypeTree names
  virtual std::vector<std::string>& getTypeTrees() = 0;

  /// Return the number of TypeTrees
  virtual unsigned int size() = 0;
  
};

#endif  // IHEPREPSVC_H
