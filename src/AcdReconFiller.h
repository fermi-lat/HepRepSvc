#ifndef ACDRECONFILLER_H
#define ACDRECONFILLER_H

#include <vector>
#include <string>

#include "Filler.h"
#include "CLHEP/Geometry/Point3D.h"

class IAcdGeometrySvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

namespace Event {
  class AcdReconV2;
  class AcdHit;
  class AcdAssoc;
  class AcdTkrGapPoca;
  class AcdTkrHitPoca;
  class AcdTkrPoint;
  class AcdEventTopology;
  class AcdTkrLocalCoords;
  class AcdPocaData;
}

namespace idents {
  class AcdId;
}

class AcdTileDim;
class AcdRibbonDim;


/** 
 *  @class AcdReconFiller
 *
 *  @brief This is the HepRep fillers for all the ACD relevant
 *  objects (recond and digi).
 *
 *  This filler outlines Acd tiles and ribbons with signals in them.
 *
 *  @author R.Giannitrapani
 **/

class AcdReconFiller: public Filler{
  
 public:
  AcdReconFiller(HepRepInitSvc* hrisvc,
                 IAcdGeometrySvc* gsvc,
                 IDataProviderSvc* dpsvc,
                 IParticlePropertySvc* ppsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  /// Check to see if a type is in the list of known types
  bool hasType(std::vector<std::string>& list, std::string type); 

 protected:

  /// Define the structure for AcdHit
  void defineAcdHitType(const char* parent);

  /// Define the structure for AcdTkrAssoc
  void defineAcdTkrAssocType(const char* parent, const char* color);

  /// Define the structure for AcdTkrAssoc
  void defineAcdEventTopologyType(const char* parent);

  /// Define the structure for AcdTkrHitPoca
  void defineAcdTkrHitPocaType(const char* parent);
    
  /// Define the structure for AcdTkrGapPoca
  void defineAcdTkrGapPocaType(const char* parent);

  /// Define the structure for AcdTkrGapPoca
  void defineAcdTkrPointType(const char* parent);
  
  /// Define the structure for AcdPocaData
  void defineAcdPocaDataType(const char* parent, const char* color, const char* errColor);

  /// Define the structure for AcdPocaData
  void defineAcdPocaErrorType(const char* parent, const char* color);

  /// Define the structure for AcdTkrLocalCoords
  void defineAcdTkrLocalCoordsType(const char* parent, const char* color, const char* errColor);
  
  /// Define the structure for AcdLocalErrorAxis
  void defineAcdLocalErrorAxisType(const char* parent, const char* color);  

  /// Fill the HepReps for the Recons stuff
  void fillAcdRecon(std::vector<std::string>& typesList);
  
  /// Fill the HepReps for the AcdHitCol
  void fillAcdHitCol( const Event::AcdReconV2& recon );

  /// Fill the HepReps for the AcdTkrAssocCol
  void fillAcdTkrAssocCol( const Event::AcdReconV2& recon );

  /// Fill the HepReps for the AcdEventTopology
  void fillAcdEventTopology( const Event::AcdReconV2& recon );  

  /// Fill the HepRep for a single AcdHit
  void addAcdHit( const char* parent, const Event::AcdHit& aHit );

  /// Fill the HepRep for a single AcdTkrAssoc
  void addAcdTkrAssoc( const char* parent, const Event::AcdAssoc& anAssoc );

  /// Fill the HepRep for a AcdTkrHitPoca
  void addAcdTkrHitPoca( const char* parent, const Event::AcdTkrHitPoca& aPoca );  

  /// Fill the HepRep for a AcdTkrGapPoca
  void addAcdTkrGapPoca( const char* parent, const Event::AcdTkrGapPoca& aPoca );  

  /// Fill the HepRep for a AcdTkrPoint
  void addAcdTkrPoint( const char* parent, const Event::AcdTkrPoint& aPoint );

  /// Fill the HepRep for AcdTkrLocalCoords
  void addAcdTkrLocalCoords( const char* parent, 
			     const idents::AcdId& acdId,
			     const Event::AcdTkrLocalCoords& local );

  /// Fill the HepRep for AcdPocaData
  void addAcdPocaData( const char* parent, const Event::AcdPocaData& pocaData );


private:

  HepRepInitSvc* m_hrisvc;
  IAcdGeometrySvc* m_acdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif // ACDRECONFILLER_H
