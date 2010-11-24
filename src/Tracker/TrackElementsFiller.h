#ifndef TrackElementsFiller_H
#define TrackElementsFiller_H

#include "src/Filler.h"
#include "geometry/Vector.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

namespace Event
{
    class TkrVecNode;
    class TkrVecPointsLink;
}

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

  void drawVectorPoints();
  void drawVectorLinks();
  void drawNodeTrees();
  void drawNode(const Event::TkrVecNode* vecNode, std::string& lineColor, int lineWidth = 2);
  void drawSingleVectorLink(const Event::TkrVecPointsLink* vecLink, std::string& lineColor);

  IDataProviderSvc*     m_dpsvc;
  IParticlePropertySvc* m_ppsvc;
};

#endif //TrackElementsFiller_H
