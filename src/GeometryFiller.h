#ifndef GEOMETRYFILLER_H
#define GEOMETRYFILLER_H
#include <vector>
#include <string>

#include "HepRepSvc/IFiller.h"

class IBuilder;
class IGlastDetSvc;
class HepRepGeometry;

/** 
 *  @class GeometryFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class GeometryFiller: public IFiller{
  
 public:
  GeometryFiller(unsigned int depth,IGlastDetSvc* gsvc);
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  
 private:
  HepRepGeometry* m_geometry;
  IGlastDetSvc* m_gsvc;
};

#endif //GEOMETRYFILLER_H
