#ifndef HEPREPGEOMETRY_H
#define HEPREPGEOMETRY_H

#include <vector>
#include "GlastSvc/GlastDetSvc/IGeometry.h"


class IBuilder;
class HepTransform3D;

/** 
 *  @class HepRepGeometry
 *
 *  @brief 
 *
 *  @author R.Giannitrapani
 */
class HepRepGeometry : public IGeometry 
{
 public:
  HepRepGeometry(unsigned int depth, std::string mode = "propagate");
  ~HepRepGeometry();

  void setDepthLevel(unsigned int l){m_depth = l;};

  virtual IGeometry::VisitorRet
    pushShape(ShapeType s,
              const UintVector& id,
              std::string name,
              std::string material,
              const DoubleVector& params,
              VolumeType type);
  
  virtual void popShape();

  void setBuilder(IBuilder* b){m_builder = b;};
  void setHepRepMode(std::string m){m_hrMode = m;};

  void reset();

  /// Set the visibile typesList
  void setTypesList(std::vector<std::string> l){m_typesList = l;};

  bool hasType(std::vector<std::string>& list, std::string type); 

 private:
  std::vector<std::string> m_typesList;

  std::vector<std::string> m_actualType;

  std::vector<std::string> m_actualInstance;

  std::vector<std::string> m_types;

  std::vector<HepTransform3D> m_actualTransform;

  unsigned int m_depth;
  unsigned int m_actualDepth;
  bool m_stopped;

  IBuilder* m_builder;

  std::string m_hrMode;
  std::string m_mode;
};

#endif //HEPREPGEOMETRY_H
