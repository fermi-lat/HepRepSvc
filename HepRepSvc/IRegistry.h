// $Header:
//

#ifndef IREGISTRY_H
#define IREGISTRY_H

#include <string>
#include <vector>
#include <map>
#include "HepRepSvc/HepRepId.h"

class IFiller;
class IBuilder;

/** 
 * @class IRegistry
 *
 * @brief The interface to the fillers Registry class
 *
 * This class holds a list of HepRep fillers indexed by the typetree
 * name. For GLAST we are using just two different typetree: 
 *
 *  - Event: this one hold all the event data types (MonteCarlo,
 *    Recon, Digi etc etc) 
 *  - Geometry3D: this one hold all the geometry data types
 *
 * Author:R.Giannitrapni
 *
 */
class IRegistry
{
 public:
  typedef std::vector<IFiller*> fillerCol;
  
  /// This method register a new filler with a name
  virtual void registerFiller(IFiller*, std::string) = 0;
  
  /// This method retrive the collection of Fillers registered
  /// for a given typetree name
  virtual const fillerCol& getFillersByType(std::string) = 0;
  
  /// This method retrive the list of typetree names registered
  virtual const std::vector<std::string>& getTypeTrees() = 0;
  
  /// This method return the size of the registry (number of fillers
  /// registered
  virtual unsigned int size() = 0;

  /// This method add an instancetree
  virtual void addInstanceTree(std::string type, std::string instance) = 0;

  /// This method clear the instancetree map
  virtual void clearInstanceTrees() = 0;

  /// Get the instance trees map
  virtual const std::map<std::string, std::string>& getInstanceTrees() = 0;

  /// Get possible dependent instance trees name given an instance tree name
  virtual const std::vector<std::string>& getDependencies(std::string) = 0;

  /// This method add an instancetrees dependency
  virtual void addDependency(std::string inst, std::string dep) = 0;
  
  /// Get the principal instance tree name
  virtual const std::string getPrincipalTree() = 0;  

  /// Set the principal instance tree name
  virtual setPrincipalTree(std::string) = 0;  
  
  /// Get the type given the instance name
  virtual std::string getTypeByInstance(std::string) = 0;

  /// This method force all the fillers to use a particular builder
  virtual void useBuilder(IBuilder*) = 0;

};

#endif //IREGISTRY_H
