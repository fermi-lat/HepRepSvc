#ifndef VertexFiller_H
#define VertexFiller_H
#include <vector>
#include <string>

#include "src/Filler.h"

#include "geometry/Vector.h"
#include "geometry/Point.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

/** 
 *  @class VertexFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class VertexFiller: public Filler{
  
 public:
  VertexFiller(HepRepInitSvc* hrisvc,
               IGlastDetSvc* gsvc,
               IDataProviderSvc* dpsvc,
               IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

 private:
  
  HepRepInitSvc* m_hrisvc;
  IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //VertexFILLER_H
