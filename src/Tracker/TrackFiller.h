#ifndef TrackFiller_H
#define TrackFiller_H
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


//class Point;

/** 
 *  @class TrackFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class TrackFiller: public Filler {
  
 public:
  TrackFiller(HepRepInitSvc* hrisvc,
              IGlastDetSvc* gsvc,
              IDataProviderSvc* dpsvc,
              IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

private:
  std::string getTkrIdString(const idents::TkrId& tkrId);
  std::string getSlopeString(const Event::TkrTrackParams& params);

  HepRepInitSvc* m_hrisvc;
  IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //TrackFiller_H
