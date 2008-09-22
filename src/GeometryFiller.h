#ifndef GEOMETRYFILLER_H
#define GEOMETRYFILLER_H

#include <vector>
#include <string>

#include "Filler.h"

class IBuilder;
class IGlastDetSvc;
class HepRepInitSvc;
class HepRepGeometry;

/** 
 *  @class GeometryFiller
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */

class GeometryFiller : public Filler {

  
 public:
  GeometryFiller(unsigned int depth,
		 HepRepInitSvc* hrisvc,
		 IGlastDetSvc* gsvc,
		 int geomType );
  
  // needed to satisfy the abstract class 
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  
 private:
  HepRepInitSvc* m_hrisvc;
  HepRepGeometry* m_geometry;
  IGlastDetSvc* m_gsvc;

};

#endif //GEOMETRYFILLER_H
