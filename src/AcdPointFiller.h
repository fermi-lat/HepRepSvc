#ifndef ACDPOINTFILLER_H
#define ACDPOINTFILLER_H

class AcdTileDim;
class AcdRibbonDim;
class IBuilder;

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

};

#endif // ACDRECONFILLER_H
