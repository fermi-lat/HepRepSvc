#ifndef ACDDIGIFILLER_H
#define ACDDIGIFILLER_H

#include <vector>
#include <string>

#include "Filler.h"

class IAcdGeometrySvc;
class IDataProviderSvc;
class HepRepInitSvc;

namespace Event {
  class AcdDigi;
}

class AcdTileDim;
class AcdRibbonDim;


/** 
 *  @class AcdDigiFiller
 *
 *  @brief This is the HepRep fillers for all the ACD relevant
 *  objects 
 *
 *  This filler outlines Acd tiles and ribbons with signals in them.
 *
 *  @author R.Giannitrapani
 **/

class AcdDigiFiller: public Filler{
  
 public:
  AcdDigiFiller(HepRepInitSvc* hrisvc,
		IAcdGeometrySvc* gsvc,
		IDataProviderSvc* dpsvc);
  
  /// This method init the type tree
  virtual void buildTypes ();
  /// This method fill the instance tree, using the string vector to decide
  /// which subinstances to fill
  virtual void fillInstances (std::vector<std::string>&);

  /// Check to see if a type is in the list of known types
  bool hasType(std::vector<std::string>& list, std::string type); 

 protected:

  /// Fill the HepReps for the Digis
  void fillAcdDigiCol( );

  /// Fill the HepRep for a single AcdDigi
  void addAcdDigi( const Event::AcdDigi& aDigi );

private:

  HepRepInitSvc* m_hrisvc;
  IAcdGeometrySvc* m_acdsvc;
  IDataProviderSvc* m_dpsvc;

};

#endif // ACDRECONFILLER_H
