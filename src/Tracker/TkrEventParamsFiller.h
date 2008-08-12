#ifndef TkrEventParamsFiller_H
#define TkrEventParamsFiller_H
#include <vector>
#include <string>

#include "src/Filler.h"
#include "geometry/Vector.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

// Foward declarations...
namespace idents {
  class TkrId;
};
namespace Event {
  class TkrTrackParams;
}
class Point;

/** 
 *  @class TkrEventParamsFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class TkrEventParamsFiller: public Filler{
  
 public:
  TkrEventParamsFiller(HepRepInitSvc* hrisvc,
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
  IGlastDetSvc*         m_gdsvc;
  IDataProviderSvc*     m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //TkrEventParamsFiller_H
