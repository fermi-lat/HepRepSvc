#include "AcdDigiFiller.h"
#include "AcdPointFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "AcdUtil/AcdTileDim.h"
#include "AcdUtil/AcdRibbonDim.h"
#include "AcdUtil/AcdGeomMap.h"
#include "AcdUtil/IAcdGeometrySvc.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Digi/AcdDigi.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "idents/VolumeIdentifier.h"


#include <algorithm>

// Constructor
AcdDigiFiller::AcdDigiFiller(HepRepInitSvc* hrisvc,
			     IAcdGeometrySvc* acdsvc,
			     IDataProviderSvc* dpsvc)
  :m_hrisvc(hrisvc),m_acdsvc(acdsvc),m_dpsvc(dpsvc){
}


// This method build the types for the HepRep
void AcdDigiFiller::buildTypes() {

  // Digi Objects
  m_builder->addType("Digi","AcdDigiCol","AcdDigiTree","");  
  // AcdDigis
  m_builder->addType("AcdDigiCol","AcdDigi","Collection of AcdDigis (Uncalibrated signals)","");
  m_builder->addAttDef("Id","ID of the ACD Detector","","");
  m_builder->addAttValue("DrawAs","Line","");  
  m_builder->addAttDef("PHA_A","Raw Pulse Height of signal in PMT A","int","(PHA)");
  m_builder->addAttDef("PHA_B","Raw Pulse Height of signal in PMT B","int","(PHA)");  
  m_builder->addAttDef("AcceptMapBits","Above Zero-Suppress thresholds","string","(> pedestal + 25)");
  m_builder->addAttDef("HitMapBits","Above VETO thresholds in PMT","string","(> 0.45 mips)");  
  m_builder->addAttDef("Range","Readout Ranges for PMT","string","Low Range: 0-5 mips, High Range: 5-1000 Mips");
}


// This method fill the instance tree Event/MC with the actual TDS content
void AcdDigiFiller::fillInstances (std::vector<std::string>& typesList) {

  if ( hasType(typesList,"Digi/AcdDigiCol") ) {
    fillAcdDigiCol();
  }

  return;
}

void AcdDigiFiller::fillAcdDigiCol( ) {  
  SmartDataPtr<Event::AcdDigiCol> acdDigiCol(m_dpsvc, EventModel::Digi::AcdDigiCol);
  if ( !acdDigiCol ) return;
  if ( acdDigiCol->size()>0) return;
  m_builder->addInstance("Digi","AcdDigiCol");
  m_builder->setSubinstancesNumber("AcdDigiCol", acdDigiCol->size());
  for ( Event::AcdDigiCol::const_iterator acdIt = acdDigiCol->begin(); acdIt != acdDigiCol->end(); acdIt++ ){
    addAcdDigi(*(*acdIt));
  }  
}

/// Fill the HepRep for a single AcdDigi
void AcdDigiFiller::addAcdDigi( const Event::AcdDigi& aDigi ) {
   
  m_builder->addInstance("AcdDigiCol","AcdDigi");  
  idents::AcdId id = aDigi.getId();

  m_builder->addAttValue("Id", (float)id.id(), "");

  if (aDigi.getHitMapBit(Event::AcdDigi::A) || 
      aDigi.getHitMapBit(Event::AcdDigi::B))
    m_builder->addAttValue("Color","red","");
  else if (aDigi.getAcceptMapBit(Event::AcdDigi::A) || 
	   aDigi.getAcceptMapBit(Event::AcdDigi::B))
    m_builder->addAttValue("Color","160,32,240",""); // this is *really* purple!

  if ( id.tile() ) {
    const AcdTileDim* tile = m_acdsvc->geomMap().getTile(id,*m_acdsvc);
    AcdHepRepPointFiller::addPointsAcdTile(*m_builder,*tile);
  } else if ( id.ribbon() ) {
    const AcdRibbonDim* ribbon = m_acdsvc->geomMap().getRibbon(id,*m_acdsvc);
    AcdHepRepPointFiller::addPointsAcdRibbon(*m_builder,*ribbon);
  }

  // Fill in attributes of the two PMTs
  m_builder->addAttValue("PHA_A",aDigi.getPulseHeight(Event::AcdDigi::A),"");
  m_builder->addAttValue("PHA_B",aDigi.getPulseHeight(Event::AcdDigi::B),"");

  std::string rangeString;
  std::string acceptString;
  std::string hitmapString;
  
  rangeString += aDigi.getRange(Event::AcdDigi::A) == 0 ? "Low," : "High,";
  rangeString += aDigi.getRange(Event::AcdDigi::B) == 0 ? "Low" : "High";

  acceptString += aDigi.getAcceptMapBit(Event::AcdDigi::A) ? "t " : "f ";
  acceptString += aDigi.getAcceptMapBit(Event::AcdDigi::B) ? "t" : "f";
  
  hitmapString += aDigi.getHitMapBit(Event::AcdDigi::A) ? "t " : "f ";
  hitmapString += aDigi.getHitMapBit(Event::AcdDigi::B) ? "t " : "f ";

  m_builder->addAttValue("AcceptMapBits",acceptString,"");
  m_builder->addAttValue("HitMapBits",hitmapString,"");
  m_builder->addAttValue("Range",rangeString,"");  
}

/// Check to see if a type is in the list of known types
bool AcdDigiFiller::hasType(std::vector<std::string>& list, std::string type) {
  for (std::vector<std::string>::const_iterator itr = list.begin(); itr != list.end(); itr++ ) {
    if ( *itr == type ) return true;
  }
  return false;
}


