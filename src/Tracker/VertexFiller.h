#ifndef VertexFiller_H
#define VertexFiller_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;

/** 
 *  @class VertexFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class VertexFiller: public IFiller{
  
 public:
  VertexFiller(IGlastDetSvc* gsvc,
                 IDataProviderSvc* dpsvc,
                 IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
  IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //VertexFILLER_H
