#ifndef ACDRECONFILLER_H
#define ACDRECONFILLER_H

#include <vector>
#include <string>

#include "Filler.h"

class IAcdGeometrySvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class HepRepInitSvc;

namespace Event {
  class AcdRecon;
  class AcdHit;
  class AcdTkrGapPoca;
  class AcdTkrHitPoca;
  class AcdTkrIntersection;
  class AcdTkrLocalCoords;
  class AcdPocaData;
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

  /// Fill the HepReps for the Recons stuff
  void fillAcdRecon(std::vector<std::string>& typesList);

  /// Fill the HepReps for the AcdHitCol
  void fillAcdHitCol( const Event::AcdRecon& recon );

  /// Fill the HepReps for the AcdTkrHitPocaCol
  void fillAcdTkrHitPocaCol( const Event::AcdRecon& recon );

  /// Fill the HepReps for the AcdTkrGapPocaCol
  void fillAcdTkrGapPocaCol( const Event::AcdRecon& recon );
  
  /// Fill the HepReps for the AcdTkrIntersectionCol
  void fillAcdTkrIntersectionCol( const Event::AcdRecon& recon );

  /// Fill the HepRep for a single AcdHit
  void addAcdHit( const Event::AcdHit& aHit );

  /// Fill the HepRep for a AcdTkrHitPoca
  void addAcdTkrHitPoca( const Event::AcdTkrHitPoca& aPoca );  

  /// Fill the HepRep for a AcdTkrGapPoca
  void addAcdTkrGapPoca( const Event::AcdTkrGapPoca& aPoca );  

  /// Fill the HepRep for AcdTkrLocalCoords
  void addAcdTkrLocalCoords( const Event::AcdTkrLocalCoords& local );

  /// Fill the HepRep for AcdPocaData
  void addAcdPocaData( const Event::AcdPocaData& pocaData );

  /// Fill the HepRep for AcdTkrIntersection
  void addAcdTkrIntersection( const Event::AcdTkrIntersection& inter );


private:

  HepRepInitSvc* m_hrisvc;
  IAcdGeometrySvc* m_acdsvc;
  IDataProviderSvc* m_dpsvc;
  IParticlePropertySvc* m_ppsvc;

};

#endif // ACDRECONFILLER_H
