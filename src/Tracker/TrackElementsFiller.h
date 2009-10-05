#ifndef TrackElementsFiller_H
#define TrackElementsFiller_H

#include "src/Filler.h"
#include "geometry/Vector.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

#include <string>

//class Point;

/** 
 *  @class TrackElementsFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class TrackElementsFiller : public Filler
{  
public:
    TrackElementsFiller(HepRepInitSvc* hrisvc,
                        IGlastDetSvc* gsvc,
                        IDataProviderSvc* dpsvc,
                        IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

private:

  IDataProviderSvc*     m_dpsvc;
  IParticlePropertySvc* m_ppsvc;
};

#endif //TrackElementsFiller_H
