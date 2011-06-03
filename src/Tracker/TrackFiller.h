#ifndef TrackFiller_H
#define TrackFiller_H
#include <vector>
#include <string>

#include "src/Filler.h"
#include "src/Tracker/ClusterUtil.h"

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

class TrackFiller: public ClusterUtil {
  
 public:
  TrackFiller(HepRepInitSvc* hrisvc,
              IGlastDetSvc* gsvc,
              IDataProviderSvc* dpsvc,
              IParticlePropertySvc* ppsvc);
  
  virtual ~TrackFiller() {;}

  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

private:
  std::string getTkrIdString(const idents::TkrId& tkrId);
  std::string getSlopeString(const Event::TkrTrackParams& params);
  void buildTrack( IBuilder* builder);


  //HepRepInitSvc* m_hrisvc;
  //IGlastDetSvc* m_gdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

    /*
    double m_siStripPitch; 
    double m_stripLength;
    double m_siThickness;
    double m_siWaferSide;
    double m_siWaferActiveSide;
    double m_ssdGap;
    int    m_nWaferAcross;
    int    m_numXTowers;
    int    m_numYTowers;
    double m_towerPitch;
    double m_activeWidth;
    */

};

#endif //TrackFiller_H
