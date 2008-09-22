#include "AcdReconFiller.h"
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
#include "Event/Recon/AcdRecon/AcdRecon.h"
#include "Event/Recon/AcdRecon/AcdHit.h"
#include "Event/Recon/AcdRecon/AcdTkrHitPoca.h"
#include "Event/Recon/AcdRecon/AcdTkrGapPoca.h"
#include "Event/Recon/AcdRecon/AcdTkrIntersection.h"
#include "Event/Digi/AcdDigi.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "idents/VolumeIdentifier.h"


#include <algorithm>

// Constructor
AcdReconFiller::AcdReconFiller(HepRepInitSvc* hrisvc,
                               IAcdGeometrySvc* acdsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
m_hrisvc(hrisvc),m_acdsvc(acdsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc){    
}


// This method build the types for the HepRep
void AcdReconFiller::buildTypes() {

  // Recon Objects
  m_builder->addType("Recon","AcdRecon","AcdRecon Tree","");
  // AcdHits
  m_builder->addType("AcdRecon", "AcdHitCol", "Collection of AcdHits (Calibrated signals)", "");  
  m_builder->addType("AcdHitCol", "AcdHit","","");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttDef("Mips_A","Calibrated Pulse Height of signal in PMT A","float","(mips)");
  m_builder->addAttDef("Mips_B","Calibrated Pulse Height of signal in PMT B","float","(mips)");
  m_builder->addAttDef("PHA_A","Raw Pulse Height of signal in PMT A","int","(PHA)");
  m_builder->addAttDef("PHA_B","Raw Pulse Height of signal in PMT B","int","(PHA)");  
  m_builder->addAttDef("AcceptMapBits","Above Zero-Suppress thresholds","string","(> pedestal + 25)");
  m_builder->addAttDef("HitMapBits","Above VETO thresholds in PMT","string","(> 0.45 mips)");  
  m_builder->addAttDef("Range","Readout Ranges for PMT","string","Low Range: 0-5 mips, High Range: 5-1000 Mips");
 

  // AcdTkrHitPocas
  m_builder->addType("AcdRecon","AcdTkrHitPocaCol","Poca Collection","");  
  m_builder->addType("AcdTkrHitPocaCol","AcdTkrHitPoca","Data about Track-Hit POCA","");  
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color","red",""); 
  m_builder->addAttDef("Id","ID of the ACD Detector element","","");
  m_builder->addAttDef("TrackIndex","Index of associated track","","");
  m_builder->addAttDef("Doca","Distance of closest approach","float","(mm)");  
  m_builder->addAttDef("DocaErr","Error on Distance of closest approach project along the POCA vector","float","in (mm)");  
  m_builder->addAttDef("ArcLength","Arclength along the track at which the POCA occurs","float","(mm)");  
  m_builder->addAttDef("Poca_X","X value of POCA","float","(mm)");  
  m_builder->addAttDef("Poca_Y","Y value of POCA","float","(mm)");  
  m_builder->addAttDef("Poca_Z","Z value of POCA","float","(mm)");  
  m_builder->addAttDef("PocaVector_X","X value of vector from POCA to element edge","float","(mm)");  
  m_builder->addAttDef("PocaVector_Y","Y value of vector from POCA to element edge","float","(mm)");  
  m_builder->addAttDef("PocaVector_Z","Z value of vector from POCA to element edge","float","(mm)");  
  // Local coords
  m_builder->addType("AcdTkrHitPoca","AcdTkrLocalCoords","Point where Track crosses plane of the tile","");  
  m_builder->addAttDef("Region","Code that tells which part of the element was hit","int","");  
  m_builder->addAttDef("Active_X","Active Distance in Local X","float","(mm)");
  m_builder->addAttDef("Active_Y","Active Distance in Local Y","float","(mm)");
  m_builder->addAttDef("Global_X","X value of point where track crosses element plane","float","(mm)");
  m_builder->addAttDef("Global_Y","Y value of point where track crosses element plane","float","(mm)");
  m_builder->addAttDef("Global_Z","Z value of point where track crosses element plane","float","(mm)");
  m_builder->addAttDef("Local_XXCov","local X Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_YYCov","local Y Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_XYCov","correlation term of local X-Y Error","float","(mm^2)");

  // AcdGapHitPocas
  m_builder->addType("AcdRecon","AcdTkrGapPocaCol","GAP Poca Collection","");  
  m_builder->addType("AcdTkrGapPocaCol","AcdTkrGapPoca","Data about Track-GAP POCA","");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color","yellow",""); 
  m_builder->addAttDef("GapId","ID of the Gap in the ACD Detector","","");
  m_builder->addAttDef("TrackIndex","Index of associated track","","");
  m_builder->addAttDef("Doca","Distance of closest approach","float","(mm)");  
  m_builder->addAttDef("DocaErr","Error on Distance of closest approach project along the POCA vector","float","in (mm)");  
  m_builder->addAttDef("ArcLength","Arclength along the track at which the POCA occurs","float","(mm)");  
  m_builder->addAttDef("Region","Code that tells which part of the element was hit","int","");  
  m_builder->addAttDef("Active_X","Active Distance in Local X","float","(mm)");
  m_builder->addAttDef("Active_Y","Active Distance in Local Y","float","(mm)");
  m_builder->addAttDef("Poca_X","X value of POCA","float","(mm)");  
  m_builder->addAttDef("Poca_Y","Y value of POCA","float","(mm)");  
  m_builder->addAttDef("Poca_Z","Z value of POCA","float","(mm)");  
  m_builder->addAttDef("PocaVector_X","X value of vector from POCA to element edge","float","(mm)");  
  m_builder->addAttDef("PocaVector_Y","Y value of vector from POCA to element edge","float","(mm)");  
  m_builder->addAttDef("PocaVector_Z","Z value of vector from POCA to element edge","float","(mm)");  
  // Local coords
  m_builder->addType("AcdTkrGapPoca","AcdTkrLocalCoords","Point where Track crosses plane of the tile","");  
  m_builder->addAttDef("Global_X","X value of point where track crosses element plane","float","(mm)");
  m_builder->addAttDef("Global_Y","Y value of point where track crosses element plane","float","(mm)");
  m_builder->addAttDef("Global_Z","Z value of point where track crosses element plane","float","(mm)");
  m_builder->addAttDef("Local_XXCov","local X Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_YYCov","local Y Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_XYCov","correlation term of local X-Y Error","float","(mm^2)");
  

  // AcdTkrIntersections
  m_builder->addType("AcdRecon","AcdTkrIntersectionCol","Collection of intersections between track an GEANT model","");  
  m_builder->addType("AcdTkrIntersectionCol","AcdTkrIntersection","Intersection between track and GEANT model","");
  m_builder->addAttValue("DrawAs","Point","");
  m_builder->addAttValue("MarkerName","Circle","");  
  m_builder->addAttValue("MarkerSize", 1, "");
  m_builder->addAttDef("Id","ID of the ACD Detector element","","");
  m_builder->addAttDef("TrackIndex","Index of associated track","","");
  m_builder->addAttDef("Local_X","X Position of Intersection in Local frame","float","(mm)");  
  m_builder->addAttDef("Local_Y","Y Position of Intersection in Local frame","float","(mm)");    
  m_builder->addAttDef("ArcLength","Arclength along the track at which the POCA occurs","float","(mm)");  
  m_builder->addAttDef("Pathlength","Pathlength of track through element","float","(mm)");  
  m_builder->addAttDef("CosTheta","Angle of track w.r.t. detector plane","float","(mm)");  
  m_builder->addAttDef("Global_X","X value of Point","float","(mm)");  
  m_builder->addAttDef("Global_Y","Y value of Point","float","(mm)");  
  m_builder->addAttDef("Global_Z","Z value of Point","float","(mm)");    
  m_builder->addAttDef("Local_XXCov","local X Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_YYCov","local Y Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_XYCov","correlation term of local X-Y Error","float","(mm^2)");
}


// This method fill the instance tree Event/MC with the actual TDS content
void AcdReconFiller::fillInstances (std::vector<std::string>& typesList) {

  if ( hasType(typesList,"Recon/AcdRecon") ) {
    fillAcdRecon(typesList);
  }

  return;
}



void AcdReconFiller::fillAcdRecon(std::vector<std::string>& typesList ) {
  
  SmartDataPtr<Event::AcdRecon> acdRec(m_dpsvc, EventModel::AcdRecon::Event);
  if ( !acdRec ) return;
  m_builder->addInstance("Recon","AcdRecon");
  
  if ( hasType(typesList, "Recon/AcdRecon/AcdHitCol/AcdHit" ) ){
    fillAcdHitCol(*acdRec);
  }
       
  if ( hasType(typesList, "Recon/AcdRecon/AcdTkrHitPocaCol/AcdTkrHitPoca" ) ) {
    fillAcdTkrHitPocaCol(*acdRec);
  }
  
  if ( hasType(typesList, "Recon/AcdRecon/AcdTkrGapPocaCol/AcdTkrGapPoca" ) ) {
    fillAcdTkrGapPocaCol(*acdRec);
  }

  if ( hasType(typesList, "Recon/AcdRecon/AcdTkrIntersectionCol/AcdTkrIntersection" ) ) {
    fillAcdTkrIntersectionCol(*acdRec);
  }

}

  /// Fill the HepReps for the AcdHitCol
void AcdReconFiller::fillAcdHitCol( const Event::AcdRecon& recon ) {
  const Event::AcdHitCol& col = recon.getAcdHitCol();
  if ( col.size() == 0 ) return;
  m_builder->addInstance("AcdRecon","AcdHitCol");
  m_builder->setSubinstancesNumber("AcdHitCol", col.size());
  for ( Event::AcdHitCol::const_iterator itr = col.begin(); itr != col.end(); itr++ ) {
    const Event::AcdHit* obj = *itr;
    addAcdHit(*obj);
  }
}

/// Fill the HepReps for the AcdTkrHitPocaCol
void AcdReconFiller::fillAcdTkrHitPocaCol( const Event::AcdRecon& recon ) {
  const Event::AcdTkrHitPocaCol& col = recon.getAcdTkrHitPocaCol();
  if ( col.size() == 0 ) return;
  m_builder->addInstance("AcdRecon","AcdTkrHitPocaCol");
  m_builder->setSubinstancesNumber("AcdTkrHitPocaCol", col.size());
  for ( Event::AcdTkrHitPocaCol::const_iterator itr = col.begin(); itr != col.end(); itr++ ) {
    const Event::AcdTkrHitPoca* obj = *itr;
    addAcdTkrHitPoca(*obj);
  }
}

/// Fill the HepReps for the AcdTkrGapPocaCol
void AcdReconFiller::fillAcdTkrGapPocaCol( const Event::AcdRecon& recon ) {
  const Event::AcdTkrGapPocaCol& col = recon.getAcdTkrGapPocaCol();
  if ( col.size() == 0 ) return;
  m_builder->addInstance("AcdRecon","AcdTkrGapPocaCol");
  m_builder->setSubinstancesNumber("AcdTkrGapPocaCol", col.size());
  for ( Event::AcdTkrGapPocaCol::const_iterator itr = col.begin(); itr != col.end(); itr++ ) {
    const Event::AcdTkrGapPoca* obj = *itr;
    addAcdTkrGapPoca(*obj);
  }
}

/// Fill the HepReps for the AcdTkrIntersectionCol
void AcdReconFiller::fillAcdTkrIntersectionCol( const Event::AcdRecon& recon ) {
  const Event::AcdTkrIntersectionCol& col = recon.getAcdTkrIntersectionCol();
  if ( col.size() == 0 ) return;
  m_builder->addInstance("AcdRecon","AcdTkrIntersectionCol");
  m_builder->setSubinstancesNumber("AcdTkrIntersectionCol", col.size());
  for ( Event::AcdTkrIntersectionCol::const_iterator itr = col.begin(); itr != col.end(); itr++ ) {
    const Event::AcdTkrIntersection* obj = *itr;
    addAcdTkrIntersection(*obj);
  }
}

/// Fill the HepRep for a single AcdHit
void AcdReconFiller::addAcdHit( const Event::AcdHit& aHit ) {
 
  m_builder->addInstance("AcdHitCol","AcdHit");  
  idents::AcdId id = aHit.getAcdId();

  m_builder->addAttValue("Id", (float)id.id(), "");

  if (aHit.getHitMapBit(Event::AcdHit::A) || 
      aHit.getHitMapBit(Event::AcdHit::B))
    m_builder->addAttValue("Color","red","");
  else if (aHit.getAcceptMapBit(Event::AcdHit::A) || 
	   aHit.getAcceptMapBit(Event::AcdHit::B))
    m_builder->addAttValue("Color","160,32,240",""); // this is *really* purple!

  m_builder->addAttValue("Mips_A",aHit.mips(Event::AcdHit::A),"");
  m_builder->addAttValue("Mips_B",aHit.mips(Event::AcdHit::B),"");
  m_builder->addAttValue("PHA_A",aHit.getPha(Event::AcdHit::A),"");
  m_builder->addAttValue("PHA_B",aHit.getPha(Event::AcdHit::B),"");

  std::string rangeString;
  std::string acceptString;
  std::string hitmapString;
 
  rangeString += (aHit.getFlags(Event::AcdHit::B) & Event::AcdHit::PMT_RANGE_BIT) == 0 ? "Low," : "High,";
  rangeString += (aHit.getFlags(Event::AcdHit::B) & Event::AcdHit::PMT_RANGE_BIT) == 0 ? "Low," : "High";

  acceptString += aHit.getAcceptMapBit(Event::AcdHit::A) ? "t " : "f ";
  acceptString += aHit.getAcceptMapBit(Event::AcdHit::B) ? "t" : "f";
  
  hitmapString += aHit.getHitMapBit(Event::AcdHit::A) ? "t " : "f ";
  hitmapString += aHit.getHitMapBit(Event::AcdHit::B) ? "t " : "f ";

  m_builder->addAttValue("AcceptMapBits",acceptString,"");
  m_builder->addAttValue("HitMapBits",hitmapString,"");
  m_builder->addAttValue("Range",rangeString,"");  

  if ( id.tile() ) {
    const AcdTileDim* tile = m_acdsvc->geomMap().getTile(id,*m_acdsvc);
    AcdHepRepPointFiller::addPointsAcdTile(*m_builder,*tile);
  } else if ( id.ribbon() ) {
    const AcdRibbonDim* ribbon = m_acdsvc->geomMap().getRibbon(id,*m_acdsvc);
    AcdHepRepPointFiller::addPointsAcdRibbon(*m_builder,*ribbon);
  }
}

/// Fill the HepRep for a AcdTkrHitPoca
void AcdReconFiller::addAcdTkrHitPoca( const Event::AcdTkrHitPoca& aPoca ) {

  m_builder->addInstance("AcdTkrHitPocaCol","AcdTkrHitPoca");  
  idents::AcdId id = aPoca.getId();
  m_builder->addAttValue("Id", (float)id.id(), "");
  m_builder->addAttValue("TrackIndex", aPoca.trackIndex(), "");
  addAcdPocaData(aPoca);  
  m_builder->addInstance("AcdTkrHitPoca","AcdTkrLocalCoords");  
  addAcdTkrLocalCoords(aPoca);
}

/// Fill the HepRep for a AcdTkrGapPoca
void AcdReconFiller::addAcdTkrGapPoca( const Event::AcdTkrGapPoca& aPoca ) {
  m_builder->addInstance("AcdTkrGapPocaCol","AcdTkrGapPoca");  
  idents::AcdGapId id = aPoca.getId();
  m_builder->addAttValue("GapId", (int)id.asShort(), "");
  m_builder->addAttValue("TrackIndex", aPoca.trackIndex(), "");
  addAcdPocaData(aPoca);  
  m_builder->addInstance("AcdTkrGapPoca","AcdTkrLocalCoords");  
  addAcdTkrLocalCoords(aPoca);
}

/// Fill the HepRep for AcdTkrLocalCoords
void AcdReconFiller::addAcdTkrLocalCoords( const Event::AcdTkrLocalCoords& local ) {  
  m_builder->addAttValue("Region", local.getRegion(), "");
  m_builder->addAttValue("Active_X", local.getActiveX(), "");
  m_builder->addAttValue("Active_Y", local.getActiveY(), "");
  m_builder->addAttValue("Local_XXCov", local.getLocalXXCov(), "");
  m_builder->addAttValue("Local_YYCov", local.getLocalYYCov(), "");
  m_builder->addAttValue("Local_XYCov", local.getLocalXYCov(), "");
}

/// Fill the HepRep for AcdPocaData
void AcdReconFiller::addAcdPocaData( const Event::AcdPocaData& pocaData ) {
  if ( pocaData.getArcLength() < 0. ) {
    m_builder->addAttValue("LineStyle", "Dashed", "");
    m_builder->addAttValue("LineWidth", 1, "");
  }
  m_builder->addPoint(pocaData.getPoca().x(),pocaData.getPoca().y(),pocaData.getPoca().z());
  m_builder->addPoint(pocaData.getPoca().x()-pocaData.getPocaVector().x(),
		      pocaData.getPoca().y()-pocaData.getPocaVector().y(),
		      pocaData.getPoca().z()-pocaData.getPocaVector().z());
  m_builder->addAttValue("Doca",pocaData.getDoca(),"");
  m_builder->addAttValue("DocaErr",pocaData.getDocaErr(),"");
  m_builder->addAttValue("ArcLength",pocaData.getArcLength(),"");
  m_builder->addAttValue("Poca_X",(float)pocaData.getPoca().x(),"");
  m_builder->addAttValue("Poca_Y",(float)pocaData.getPoca().y(),"");
  m_builder->addAttValue("Poca_Z",(float)pocaData.getPoca().z(),"");
  m_builder->addAttValue("PocaVector_X",(float)pocaData.getPocaVector().x(),"");
  m_builder->addAttValue("PocaVector_Y",(float)pocaData.getPocaVector().y(),"");
  m_builder->addAttValue("PocaVector_Z",(float)pocaData.getPocaVector().z(),"");
}

/// Fill the HepRep for AcdTkrIntersection
void AcdReconFiller::addAcdTkrIntersection( const Event::AcdTkrIntersection& inter ) {
  m_builder->addInstance("AcdTkrIntersectionCol","AcdTkrIntersection");  
  m_builder->addAttValue("Id", (int)inter.getTileId().id(), "");
  m_builder->addAttValue("TrackIndex", inter.getTrackIndex(), "");
  m_builder->addAttValue("Local_X", (float)inter.getLocalX(), "");
  m_builder->addAttValue("Local_Y", (float)inter.getLocalY(), "");
  m_builder->addAttValue("ArcLength", (float)inter.getArcLengthToIntersection(),"");
  m_builder->addAttValue("Pathlength", (float)inter.getPathLengthInTile(), "");
  m_builder->addAttValue("CosTheta", (float)inter.getCosTheta(), "");
  m_builder->addAttValue("Global_X", (float)inter.getGlobalPosition().x(), "");
  m_builder->addAttValue("Global_Y", (float)inter.getGlobalPosition().y(), "");
  m_builder->addAttValue("Global_Z", (float)inter.getGlobalPosition().z(), "");
  m_builder->addAttValue("Local_XXCov", (float)inter.getLocalXXCov(), "");
  m_builder->addAttValue("Local_YYCov", (float)inter.getLocalYYCov(), "");
  m_builder->addAttValue("Local_XYCov", (float)inter.getLocalXYCov(), "");  
  m_builder->addPoint(inter.getGlobalPosition().x(),inter.getGlobalPosition().y(),inter.getGlobalPosition().z());
  if ( inter.getArcLengthToIntersection() < 0. ) {
    m_builder->addAttValue("Color","green","");
  } else {
    m_builder->addAttValue("Color","blue","");
  }
}


/// Check to see if a type is in the list of known types
bool AcdReconFiller::hasType(std::vector<std::string>& list, std::string type) {
  for (std::vector<std::string>::const_iterator itr = list.begin(); itr != list.end(); itr++ ) {
    if ( *itr == type ) return true;
  }
  return false;
}


