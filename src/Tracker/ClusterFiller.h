#ifndef CLUSTERFILLER_H
#define CLUSTERFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IGlastDetSvc;
class ITkrGeometrySvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

/** 
 *  @class ClusterFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class ClusterFiller: public IFiller{
  
 public:
  ClusterFiller(HepRepInitSvc* hrisvc,
                IGlastDetSvc* gsvc,
                ITkrGeometrySvc* tgsvc,
                IDataProviderSvc* dpsvc,
                IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
    void drawPrism(double x, double y, double z, double dx, double dy, double dz);
    HepRepInitSvc* m_hrisvc;
    IGlastDetSvc* m_gdsvc;
    ITkrGeometrySvc* m_tgsvc;
    IDataProviderSvc* m_dpsvc;
    IParticlePropertySvc* m_ppsvc;

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
};

#endif //ClusterFILLER_H
