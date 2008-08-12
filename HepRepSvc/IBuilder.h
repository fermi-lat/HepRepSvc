#ifndef IBUILDER_H
#define IBUILDER_H

#include <string>

/** 
 *  @class IBuilder
 *
 *  @brief An abstract class to decouple compilation time to runtime
 *  implementation of the HepRep.
 * 
 *  This builder can be implemented in order to concretely build the
 *  HepRep. It is used by the fillers at runtime, so by changing the
 *  builder it is possible to build different representations of the
 *  HepRep (XML, Corba, etc) using the same fillers.
 * 
 *  @author R.Giannitrapani
 */

class IBuilder{
 public:
  /// Add an AttDef to the hierarchy
  virtual void addAttDef(std::string name,
                           std::string desc,
                           std::string type,
                           std::string extra) = 0;
  /// Add a string AttValue to the hierarchy
  virtual void addAttValue(std::string name,
                           std::string value,
                           std::string showlabel) = 0;
  /// Add an int AttValue to the Hierarchy
  virtual void addAttValue(std::string name,
                           int         value,
                           std::string showlabel) = 0;
  /// Add a float AttValue to the Hierarchy
  virtual void addAttValue(std::string name,
                           float         value,
                           std::string showlabel) = 0;
  /// Add a Point to the Hierarchy
  virtual void addPoint(float x, float y, float z) = 0;
  /// Add an Instance to the Hierarchy
  virtual void addInstance(std::string father, 
                           std::string type) = 0;

  /// Add a type to the Hierarchy
  virtual void addType(std::string father, 
                       std::string name, 
                       std::string desc, 
                       std::string info) = 0;
  
  /// Set the number of subinstances of a given instance if known;
  /// this can help a lot for the Corba implementation
  virtual void setSubinstancesNumber(std::string name, unsigned int n) = 0;

};

#endif //IBUILDER_H
