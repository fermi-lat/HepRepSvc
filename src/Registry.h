// $Header:
//
// Description: This class holds a list of HepRep fillers indexed by
// the typetree name. For GLAST we are using just two different
// typetree: 
//  - Event: this one hold all the event data types (MonteCarlo, Recon, Digi
//           etc etc)
//  - Geometry3D: this one hold all the geometry data types
//
// Author:R.Giannitrapni

#ifndef REGISTRY_H
#define REGISTRY_H

#include <string>
#include <vector>
#include <map>

#include "HepRepSvc/IRegistry.h"

class IFiller;
class IBuilder;

class Registry: public IRegistry
{
 public:
  
  /// This method register a new filler with a name
  virtual void registerFiller(IFiller*, std::string);
  
  /// This method retrive the collection of Fillers registered
  /// for a given typetree name
  virtual const fillerCol& getFillersByType(std::string type);
  
  /// This method retrive the list of typetree names registered
  virtual const std::vector<std::string>& getTypeTrees();
  
  /// This method return the size of the registry (number of fillers
  /// registered
  virtual unsigned int size(){return m_fillers.size();};

  /// This method add an instancetree
  virtual void addInstanceTree(std::string type, std::string instance)
    {m_instances[instance] = type;}

  /// This method clear the instancetree map
  virtual void clearInstanceTrees(){m_instances.clear();}

  /// Get the instance trees map
  virtual const std::map<std::string, std::string>& getInstanceTrees(){
    return m_instances;};  

  /// This method force all the fillers to use a particular builder
  virtual void useBuilder(IBuilder*);

  /// Get the type given the instance name
  virtual std::string getTypeByInstance(std::string);

 private:
  /// This map holds the fillers registered in indexed by the typetree name
  std::map<std::string, fillerCol> m_fillers;

  /// This map holds the actual instance trees for the event indexed
  /// by the relative typetree
  std::map<std::string, std::string> m_instances;
};

#endif //REGISTRY_H
