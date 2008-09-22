
#ifndef HepRepGeometry_H
#define HepRepGeometry_H

#include <vector>
#include "GlastSvc/GlastDetSvc/IGeometry.h"

namespace HepGeom {class Transform3D;}

class IBuilder;
class Transform3D;
class Filler;
class MsgStream;

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
  
  typedef enum GeomType { Illegal = -1,
                          FullGeom = 0,
			  LatOnly = 1,
			  ActiveOnly = 2,
			  Minimal = 3 };

  static GeomType getGeomType( const std::string& value, MsgStream& log );

public:
    HepRepGeometry(unsigned int depth, Filler* filler, std::string mode, int geomType );
    ~HepRepGeometry();

    void setDepthLevel(unsigned int l){m_depth = l;};

    virtual IGeometry::VisitorRet
        pushShape(ShapeType s,
        const UintVector& id,
        std::string name,
        std::string material,
        const DoubleVector& params,
        VolumeType type,
        SenseType sense);

    virtual void popShape();

    void setBuilder(IBuilder* b){m_builder = b;};
    void setHepRepMode(std::string m){m_hrMode = m;};

    void reset();

    /// Set the visible typesList
    void setTypesList(std::vector<std::string> l){m_typesList = l;};

    void drawTransformedPrism(double dx, double dy, double dz,
        HepGeom::Transform3D& global);
    void drawTransformedTrap(double dx1, double dx2, 
        double dxDiff, double dy, double dz,
        HepGeom::Transform3D& trans); 
    bool hasType(std::vector<std::string>& list, std::string type); 

private:
    std::vector<std::string> m_typesList;

    std::vector<std::string> m_actualType;

    std::vector<std::string> m_actualInstance;

    std::vector<std::string> m_types;

    std::vector<HepGeom::Transform3D> m_actualTransform;

    unsigned int m_depth;
    unsigned int m_actualDepth;
    bool m_stopped;
    int m_geomType;

    IBuilder* m_builder;
    Filler*   m_filler;

    std::string m_hrMode;
    std::string m_mode;
};

#endif //HepRepGeometry_H
