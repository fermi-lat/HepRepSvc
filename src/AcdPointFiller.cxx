#include "AcdPointFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "AcdUtil/AcdTileDim.h"
#include "AcdUtil/AcdRibbonDim.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"


/// Create the outline of a tile
void AcdHepRepPointFiller::addPointsAcdTile( IBuilder& builder, const AcdTileDim& tile ) {
  if ( tile.nVol() == 2 ) {
    addPointsAcdBentTile(builder,tile);
    return;
  }
  if ( tile.dim(0).size() > 3 ) {
    addPointsAcdTrapezoidTile(builder,tile);
    return;
  }
  addPointsAcdNormalTile(builder,tile);
}


/// Create the outline of a bent tile (top edge tiles)
void AcdHepRepPointFiller::addPointsAcdBentTile( IBuilder& builder, const AcdTileDim& tile ) {
  // top part
  for ( unsigned i0(0); i0 < 4; i0++ ) {    
    unsigned idx = i0 + tile.sharedEdge(0) + 1;
    const HepPoint3D& corner = tile.corner(0)[idx%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  } 
  // side part
  for ( unsigned i1(0); i1 < 4; i1++ ) {    
    unsigned idx = i1 + tile.sharedEdge(1) + 1;
    const HepPoint3D& corner = tile.corner(1)[idx%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  } 
  // last point
  unsigned idx = tile.sharedEdge(0) + 1;
  const HepPoint3D& corner = tile.corner(0)[idx%4];
  builder.addPoint(corner.x(),corner.y(),corner.z());
}

/// Create the outline of a normal tile (most of the tiles)
void AcdHepRepPointFiller::addPointsAcdNormalTile(IBuilder& builder,  const AcdTileDim& tile ) {
  for ( unsigned i(0); i < 5; i++) {
    const HepPoint3D& corner = tile.corner(0)[i%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  }
}

/// Create the outline of a Trapezoidal tile (side corner tiles)
void AcdHepRepPointFiller::addPointsAcdTrapezoidTile ( IBuilder& builder, const AcdTileDim& tile ) {
  for ( unsigned i(0); i < 5; i++) {
    const HepPoint3D& corner = tile.corner(0)[i%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  }
}

/// Create the outline of a Ribbon
void AcdHepRepPointFiller::addPointsAcdRibbon( IBuilder& /*builder*/, const AcdRibbonDim& /* ribbon */ ) {

}

