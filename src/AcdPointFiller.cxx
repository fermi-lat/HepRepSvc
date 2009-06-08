#include "AcdPointFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "AcdUtil/AcdTileDim.h"
#include "AcdUtil/AcdRibbonDim.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Point3D.h"


/// Create the outline of a tile
void AcdHepRepPointFiller::addPointsAcdTile( IBuilder& builder, const AcdTileDim& tile ) {
  if ( tile.nVol() == 2 ) {
    addPointsAcdBentTile(builder,tile);
    return;
  }
  if ( tile.getSection(0)->m_dim.size() > 3 ) {
    addPointsAcdTrapezoidTile(builder,tile);
    return;
  }
  addPointsAcdNormalTile(builder,tile);
}


/// Create the outline of a bent tile (top edge tiles)
void AcdHepRepPointFiller::addPointsAcdBentTile( IBuilder& builder, const AcdTileDim& tile ) {
  // top part
  for ( unsigned i0(0); i0 < 4; i0++ ) {    
    unsigned idx = i0 + tile.getSection(0)->m_shared + 1;
    const HepPoint3D& corner = tile.getSection(0)->m_corners[idx%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  } 
  // side part
  for ( unsigned i1(0); i1 < 4; i1++ ) {    
    unsigned idx = i1 + tile.getSection(0)->m_shared + 1;
    const HepPoint3D& corner = tile.getSection(0)->m_corners[idx%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  } 
  // last point
  unsigned idx = tile.getSection(0)->m_shared + 1;
  const HepPoint3D& corner = tile.getSection(0)->m_corners[idx%4];
  builder.addPoint(corner.x(),corner.y(),corner.z());
}

/// Create the outline of a normal tile (most of the tiles)
void AcdHepRepPointFiller::addPointsAcdNormalTile(IBuilder& builder,  const AcdTileDim& tile ) {
  for ( unsigned i(0); i < 5; i++) {
    const HepPoint3D& corner = tile.getSection(0)->m_corners[i%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  }
}

/// Create the outline of a Trapezoidal tile (side corner tiles)
void AcdHepRepPointFiller::addPointsAcdTrapezoidTile ( IBuilder& builder, const AcdTileDim& tile ) {
  for ( unsigned i(0); i < 5; i++) {
    const HepPoint3D& corner = tile.getSection(0)->m_corners[i%4];
    builder.addPoint(corner.x(),corner.y(),corner.z());
  }
}

/// Create the outline of a Ribbon
void AcdHepRepPointFiller::addPointsAcdRibbon( IBuilder& builder, const AcdRibbonDim& ribbon ) {
  
  for ( unsigned i(0); i < ribbon.nSegments(); i++ ) {
    const AcdRibbonSegment* seg = ribbon.getSegment(i);
    const HepPoint3D& p1 = seg->m_start;
    const HepPoint3D& p2 = seg->m_end;
    builder.addPoint(p1.x(),p1.y(),p1.z());
    builder.addPoint(p2.x(),p2.y(),p2.z());  
  }
}

void AcdHepRepPointFiller::addErrors( IBuilder& builder, 
				      const HepPoint3D& center,
				      const HepVector3D& vect ) {
  HepPoint3D l1 = center + vect;
  HepPoint3D l2 = center - vect;
  builder.addAttValue("X",(float)vect.x(),"");
  builder.addAttValue("Y",(float)vect.y(),"");
  builder.addAttValue("Z",(float)vect.z(),"");
  builder.addPoint(l1.x(),l1.y(),l1.z());
  builder.addPoint(l2.x(),l2.y(),l2.z());  
}
  

