#ifndef XMLBUILDER_H
#define XMLBUILDER_H
#include <stdio.h>
#include "zlib.h"
#include <string>
#include <vector>
#include "HepRepSvc/IBuilder.h"

/** 
 *  @class XMLBuilder
 *
 *  @brief 
 *
 *  @author 
 */

class XMLBuilder : public IBuilder{
 public:
  XMLBuilder(gzFile file){m_file = file;};

  virtual void addAttDef(std::string name,
                           std::string desc,
                           std::string type,
                           std::string extra);
  
  virtual void addAttValue(std::string name,
                             std::string value,
                             std::string showlabel);

  virtual void addAttValue(std::string name,
                             int         value,
                             std::string showlabel);

  virtual void addAttValue(std::string name,
                             float         value,
                             std::string showlabel);
  
  virtual void addPoint(float x, float y, float z);
  
  virtual void addInstance(std::string father, 
                             std::string type);

  virtual void addType(std::string father, 
                         std::string name, 
                         std::string desc, 
                         std::string info);

  virtual void setSubinstancesNumber(std::string, unsigned int){};

  void endTypes();
  void endInstances();
  void reset();
 
  std::string addIdent(int i);
 private:
  std::vector<std::string> m_actualType;
  std::vector<std::string> m_actualInstance;
  gzFile m_file;
};

#endif //XMLBUILDER_H
