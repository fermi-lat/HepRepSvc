#ifndef ACDPOINTFILLER_H
#define ACDPOINTFILLER_H

class AcdTileDim;
class AcdRibbonDim;
class IBuilder;


#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"


namespace AcdHepRepPointFiller {
  
  /// Create the outline of a tile
  void addPointsAcdTile( IBuilder& builder, const AcdTileDim& tile );

  /// Create the outline of a bent tile (top edge tiles)
  void addPointsAcdBentTile( IBuilder& builder, const AcdTileDim& tile );
  
  /// Create the outline of a normal tile (most of the tiles)
  void addPointsAcdNormalTile( IBuilder& builder, const AcdTileDim& tile );

  /// Create the outline of a Trapezoidal tile (side corner tiles)
  void addPointsAcdTrapezoidTile ( IBuilder& builder, const AcdTileDim& tile );

  /// Create the outline of a Ribbon
  void addPointsAcdRibbon( IBuilder& builder, const AcdRibbonDim& ribbon );

  typedef HepGeom::Point3D<double> HepPoint3D;
  typedef HepGeom::Vector3D<double> HepVector3D;
  
  /// Fill the error axes for a tile
  void addErrors( IBuilder& builder, const HepPoint3D& center, const HepVector3D& vect);

typedef HepGeom::Point3D<double> HepPoint3D;
typedef HepGeom::Vector3D<double> HepVector3D;


};

#endif // ACDRECONFILLER_H
