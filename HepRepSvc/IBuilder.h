#ifndef IBUILDER_H
#define IBUILDER_H

#include <string>

/** 
 *  @class IBuilder
 *
 *  @brief 
 *
 *  @author 
 */

class IBuilder{
 public:
  virtual void addAttDef(std::string name,
                           std::string desc,
                           std::string type,
                           std::string extra) = 0;
  
  virtual void addAttValue(std::string name,
                           std::string value,
                           std::string showlabel) = 0;

  virtual void addAttValue(std::string name,
                           int         value,
                           std::string showlabel) = 0;

  virtual void addAttValue(std::string name,
                           float         value,
                           std::string showlabel) = 0;
  
  virtual void addPoint(float x, float y, float z) = 0;

  virtual void addInstance(std::string father, 
                           std::string type) = 0;
  
  virtual void addType(std::string father, 
                       std::string name, 
                       std::string desc, 
                       std::string info) = 0;
  

  virtual void setSubinstancesNumber(std::string name, unsigned int n) = 0;

};

#endif //IBUILDER_H
