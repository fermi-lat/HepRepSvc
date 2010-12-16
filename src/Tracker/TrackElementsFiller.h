#ifndef TrackElementsFiller_H
#define TrackElementsFiller_H

#include "src/Filler.h"
//#include "geometry/Vector.h"

class IGlastDetSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

namespace Event
{
    class TkrFilterParams;
    class TkrBoundBox;
    class TkrBoundBoxPoints;
    class TkrVecNode;
    class TkrVecPointsLink;
    template <class T1, class T2> class RelTable;
    typedef RelTable<TkrFilterParams, TkrBoundBox>       TkrFilterParamsToBoxTab;
    typedef RelTable<TkrFilterParams, TkrBoundBoxPoints> TkrFilterParamsToPointsTab;
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
  void drawFilterParamsCol();
  void drawFilterParams(Event::TkrFilterParams* filterParams);
  void drawTkrBoundBoxes(Event::TkrFilterParams* filterParams, Event::TkrFilterParamsToBoxTab& paramsToBoxTab);
  void drawTkrBoundBoxPoints(Event::TkrFilterParams* filterParams, Event::TkrFilterParamsToPointsTab& paramsToPointsTab);
  void drawVectorLinks();
  void drawNodeTrees();
  void drawNode(const Event::TkrVecNode* vecNode, std::string& lineColor, int lineWidth = 2);
  void drawSingleVectorLink(const Event::TkrVecPointsLink* vecLink, std::string& lineColor);

  IDataProviderSvc*     m_dpsvc;
  IParticlePropertySvc* m_ppsvc;
};

#endif //TrackElementsFiller_H
