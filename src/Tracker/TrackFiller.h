#ifndef TrackFiller_H
#define TrackFiller_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;

// Foward declarations...
namespace idents {
  class TkrId;
};
namespace Event {
  class TkrTrackParams;
}
class Point;

/** 
 *  @class TrackFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class TrackFiller: public IFiller{
  
 public:
  TrackFiller(IGlastDetSvc* gsvc,
              IDataProviderSvc* dpsvc,
              IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
  std::string getTkrIdString(const idents::TkrId& tkrId);
  std::string getPositionString(const Point& position);
  std::string getSlopeString(const Event::TkrTrackParams& params);

  IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif //TrackFiller_H
