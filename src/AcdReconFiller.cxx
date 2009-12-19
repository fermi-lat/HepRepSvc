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
#include "Event/Recon/AcdRecon/AcdReconV2.h"
#include "Event/Recon/AcdRecon/AcdHit.h"
#include "Event/Recon/AcdRecon/AcdTkrHitPoca.h"
#include "Event/Recon/AcdRecon/AcdTkrGapPoca.h"
#include "Event/Recon/AcdRecon/AcdEventTopology.h"
#include "Event/Recon/AcdRecon/AcdTkrPoint.h"
#include "Event/Digi/AcdDigi.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "idents/VolumeIdentifier.h"

#include <list>

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
  defineAcdHitType("AcdHitCol");
  
  // AcdTkrAssocs
  m_builder->addType("AcdRecon","AcdTkrAssocCol","Track Assocication Collection","");  
  defineAcdTkrAssocType("AcdTkrAssocCol","yellow");

  // AcdEventTopology
  m_builder->addType("AcdRecon","AcdEventTopology","Event level ACD data","");  
  defineAcdEventTopologyType("AcdEventTopology");  

}


// This method fill the instance tree Event/MC with the actual TDS content
void AcdReconFiller::fillInstances (std::vector<std::string>& typesList) {

  if ( hasType(typesList,"Recon/AcdRecon") ) {
    fillAcdRecon(typesList);
  }

  return;
}

/// Define the structure for AcdHit
void AcdReconFiller::defineAcdHitType(const char* parent) {
  m_builder->addType(parent, "AcdHit","","");
  m_builder->addAttDef("Mips_A","Calibrated Pulse Height of signal in PMT A","float","(mips)");
  m_builder->addAttDef("Mips_B","Calibrated Pulse Height of signal in PMT B","float","(mips)");
  m_builder->addAttDef("PHA_A","Raw Pulse Height of signal in PMT A","int","(PHA)");
  m_builder->addAttDef("PHA_B","Raw Pulse Height of signal in PMT B","int","(PHA)");  
  m_builder->addAttDef("AcceptMapBits","Above Zero-Suppress thresholds","string","(> pedestal + 25)");
  m_builder->addAttDef("HitMapBits","Above VETO thresholds in PMT","string","(> 0.45 mips)");  
  m_builder->addAttDef("Range","Readout Ranges for PMT","string","Low Range: 0-5 mips, High Range: 5-1000 Mips");
  m_builder->addAttValue("DrawAs","Line","");
}

void AcdReconFiller::defineAcdTkrAssocType(const char* parent, const char* color) {
  m_builder->addType(parent, "AcdTkrAssoc","","");
  m_builder->addAttDef("Index","Track Index","int","");
  m_builder->addAttDef("Energy","Track Energy","float","(MeV)");
  m_builder->addAttDef("Start","Track Start","string","(mm)");
  m_builder->addAttDef("Dir","Track Dir","string","(mm)");
  m_builder->addAttDef("ArcLength","Dist to Track End Point","float","(mm)");  
  m_builder->addAttDef("VetoSigmaHit","Veto Esimator based on ACD hits","float","(sigma)");
  m_builder->addAttDef("VetoSigmaGap","Veto Esimator based on ACD gaps","float","(sigma)");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color",color,"");  
  
  defineAcdTkrHitPocaType("AcdTkrAssoc");
  defineAcdTkrGapPocaType("AcdTkrAssoc");  
  defineAcdTkrPointType("AcdTkrAssoc");  
}

void AcdReconFiller::defineAcdEventTopologyType(const char* /* parent */) {
  /// Define the structure for AcdTkrAssoc
  m_builder->addAttDef("TileCount","Number of hit tiles","int","");
  m_builder->addAttDef("RibbonCount","Number of hit ribbons","int","");
  m_builder->addAttDef("TileVeto","Number of tiles with veto","int","");
  m_builder->addAttDef("TileEnergy","Total Energy in tiles","float","(MeV)");
  m_builder->addAttDef("RibbonEnergy","Total Energy in ribbons","float","(MeV)");
  m_builder->addAttDef("NTilesByFace","Number of tiles by face","string","");
  m_builder->addAttDef("NTilesByRow","Number of tiles by row","string","");
  m_builder->addAttDef("EnergyByFace","Tile energy by face","string","(MeV)");
  m_builder->addAttDef("EnergyByRow","Tile energy by row","string","(MeV)");
  m_builder->addAttDef("NSidesHit","Number of sides with hits","int","");
  m_builder->addAttDef("NSidesVeto","Number of sides with veto","int",""); 
}


/// Define the structure for AcdTkrHitPoca
void AcdReconFiller::defineAcdTkrHitPocaType(const char* parent) {
  m_builder->addType(parent,"AcdTkrHitPoca","Data about Track-Hit POCA","");  
  m_builder->addAttDef("Id","ID of the ACD Detector element","int","");
  m_builder->addAttDef("TrackIndex","Index of associated track","int","");
  m_builder->addAttDef("Active_X","ActiveDistance in X","float","(mm)");
  m_builder->addAttDef("Active_Y","ActiveDistance in Y","float","(mm)");  
  m_builder->addAttDef("Mips_A","Calibrated Pulse Height of signal in PMT A","float","(mips)");
  m_builder->addAttDef("Mips_B","Calibrated Pulse Height of signal in PMT B","float","(mips)");  
  m_builder->addAttDef("VetoSigma","Veto Esimator","float","(sigma>0)");  
  defineAcdPocaDataType("AcdTkrHitPoca","red","red");
  defineAcdTkrLocalCoordsType("AcdTkrHitPoca","red","red");
}
/// Define the structure for AcdTkrGapPoca
void AcdReconFiller::defineAcdTkrGapPocaType(const char* parent) {
  m_builder->addType(parent,"AcdTkrGapPoca","Data about Track-GAP POCA","");
  m_builder->addAttDef("GapId","ID of the Gap in the ACD Detector","int","");
  m_builder->addAttDef("TrackIndex","Index of associated track","int","");
  m_builder->addAttDef("VetoSigma","Veto Esimator","float","(sigma>0)");  
  defineAcdPocaDataType("AcdTkrGapPoca","0,100,0","0,50,0");
  defineAcdTkrLocalCoordsType("AcdTkrGapPoca","0,100,0","0,50,0");
}

/// Define the structure for AcdTkrGapPoca
void AcdReconFiller::defineAcdTkrPointType(const char* parent) {
  m_builder->addType(parent,"AcdTkrPoint","Data about POCA","");
  m_builder->addAttDef("TrackIndex","which track","int","");
  defineAcdTkrLocalCoordsType("AcdTkrPoint","0,100,0","0,50,0");
}


/// Define the structure for AcdPocaData
void AcdReconFiller::defineAcdPocaDataType(const char* parent, const char* color, const char* errColor) {
  m_builder->addType(parent,"AcdPocaData","Data about POCA","");
  m_builder->addAttDef("Doca","Distance of closest approach","float","(mm)");  
  m_builder->addAttDef("DocaErrProj","Error on DOCA from track projection","float","in (mm)");  
  m_builder->addAttDef("DocaErrProp","Error on DOCA from Kalman Propagation","float","in (mm)");  
  m_builder->addAttDef("ArcLength","Arclength along the track at which the POCA occurs","float","(mm)"); 
  m_builder->addAttDef("Volume","Which volume used to get POCA","int","");  
  m_builder->addAttDef("Region","Which region used to get POCA","int","");    
  m_builder->addAttDef("Poca","Point of Closest Approach","string","(mm)");  
  m_builder->addAttDef("Voca","Vector of Closest Approach","string","(mm)");  
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color",color,"");  
  m_builder->addAttValue("LineStyle","Dashed","");
  defineAcdPocaErrorType("AcdPocaData",errColor);
}

/// Define the structure for AcdLocalErrorAxis
void AcdReconFiller::defineAcdPocaErrorType(const char* parent, const char* color) {
  m_builder->addType(parent,"AcdPocaError","","");
  m_builder->addAttDef("Error","Size of error","float","(mm)");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("LineStyle","Solid","");
  m_builder->addAttValue("Color",color,"");
}


/// Define the structure for AcdTkrLocalCoords
void AcdReconFiller::defineAcdTkrLocalCoordsType(const char* parent, const char* color, const char* errColor) {
  m_builder->addType(parent,"AcdTkrLocalCoords","Point where Track crosses plane of the tile","");  
  m_builder->addAttDef("Region","Code that tells which part of the element was hit","int","");  
  m_builder->addAttDef("ArcLength","Arclength along the track at which the POCA occurs","float","(mm)");  
  m_builder->addAttDef("CosTheta","Angle of track w.r.t. detector plane","float","(mm)");  
  m_builder->addAttDef("Global","Global Position of Point","string","(mm)");  
  m_builder->addAttDef("Local_X","Distance in Local X","float","(mm)");
  m_builder->addAttDef("Local_Y","Distance in Local Y","float","(mm)");
  m_builder->addAttDef("Local_XXCov_Proj","local X Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_YYCov_Proj","local Y Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_XYCov_Proj","correlation term of local X-Y Error","float","(mm^2)");
  m_builder->addAttDef("Local_XXCov_Prop","local X Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_YYCov_Prop","local Y Error squared","float","(mm^2)");
  m_builder->addAttDef("Local_XYCov_Prop","correlation term of local X-Y Error","float","(mm^2)");
  m_builder->addAttValue("DrawAs","Point","");
  m_builder->addAttValue("MarkerName","Circle","");  
  m_builder->addAttValue("MarkerSize", 1, "");
  m_builder->addAttValue("Color",color,"");   
  defineAcdLocalErrorAxisType("AcdTkrLocalCoords",errColor);
}


/// Define the structure for AcdLocalErrorAxis
void AcdReconFiller::defineAcdLocalErrorAxisType(const char* parent, const char* color) {
  m_builder->addType(parent,"AcdLocalErrorAxis","","");
  m_builder->addAttDef("ErrorAxis","Error Axis","string","(mm)");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color",color,"");
}


void AcdReconFiller::fillAcdRecon(std::vector<std::string>& typesList ) {
  
  SmartDataPtr<Event::AcdReconV2> acdRec(m_dpsvc, EventModel::AcdReconV2::Event);
  if ( acdRec == 0 ) {    
    std::cout << "No AcdRecon " << EventModel::AcdRecon::Event << ' ' << m_dpsvc << std::endl;
    return; 
  }
  
  m_builder->addInstance("Recon","AcdRecon");
  
  if ( hasType(typesList, "Recon/AcdRecon/AcdHitCol/AcdHit" ) ){
    fillAcdHitCol(*acdRec);
  }

  if ( hasType(typesList, "Recon/AcdRecon/AcdTkrAssocCol/AcdTkrAssoc" ) ){
    fillAcdTkrAssocCol(*acdRec);
  }
       
  if ( hasType(typesList, "Recon/AcdRecon/AcdEventTopology" ) ) {
    fillAcdEventTopology(*acdRec);
  } 

}

  /// Fill the HepReps for the AcdHitCol
void AcdReconFiller::fillAcdHitCol( const Event::AcdReconV2& recon ) {
  const Event::AcdHitCol& col = recon.getHitCol();
  if ( col.size() == 0 ) return;
  m_builder->addInstance("AcdRecon","AcdHitCol");
  m_builder->setSubinstancesNumber("AcdHitCol", col.size());
  for ( Event::AcdHitCol::const_iterator itr = col.begin(); itr != col.end(); itr++ ) {
    const Event::AcdHit* obj = *itr;
    addAcdHit("AcdHitCol",*obj);
  }
}

/// Fill the HepReps for the AcdTkrAssocCol
void AcdReconFiller::fillAcdTkrAssocCol( const Event::AcdReconV2& recon ) {
  const Event::AcdTkrAssocCol& col = recon.getTkrAssocCol();
  if ( col.size() == 0 ) return;
    
  std::vector<const Event::AcdTkrAssoc*> toDraw;
  for ( Event::AcdTkrAssocCol::const_iterator itr = col.begin(); itr != col.end(); itr++ ) {
    const Event::AcdTkrAssoc* obj = *itr;
    if ( obj->getTrackIndex() < 0 ) continue;
    toDraw.push_back(obj);
  }

  m_builder->addInstance("AcdRecon","AcdTkrAssocCol");
  m_builder->setSubinstancesNumber("AcdTkrAssocCol", toDraw.size() );

  for ( std::vector<const Event::AcdTkrAssoc*>::const_iterator itrD = toDraw.begin(); itrD != toDraw.end(); itrD++ ) {
    const Event::AcdTkrAssoc* obj = *itrD;
    addAcdTkrAssoc("AcdTkrAssocCol",*obj);
  }
}

/// Fill the HepReps for the AcdEventTopology
void AcdReconFiller::fillAcdEventTopology( const Event::AcdReconV2& recon ) {
  m_builder->addInstance("AcdRecon","AcdEventTopology");  
  const Event::AcdEventTopology& topo = recon.getEventTopology();
  m_builder->addAttValue("TileCount",(int)topo.getTileCount(),"");
  m_builder->addAttValue("RibbonCount",(int)topo.getRibbonCount(),"");
  m_builder->addAttValue("TileVeto",(int)topo.getTileVeto(),"");
  m_builder->addAttValue("TileEnergy",(float)topo.getTileEnergy(),"");
  m_builder->addAttValue("RibbonEnergy",(float)topo.getRibbonEnergy(),"");
  m_builder->addAttValue("NTilesByFace","xxx","");
  m_builder->addAttValue("NTilesByRow","xxx","");
  m_builder->addAttValue("EnergyByFace","xxx","");
  m_builder->addAttValue("EnergyByRow","xxx","");
  m_builder->addAttValue("NSidesHit",(int)topo.getNSidesHit(),"");
  m_builder->addAttValue("NSidesVeto",(int)topo.getNSidesVeto(),"");
}


/// Fill the HepRep for a single AcdHit
void AcdReconFiller::addAcdHit( const char* parent, const Event::AcdHit& aHit ) {
 
  m_builder->addInstance(parent,"AcdHit");  
  idents::AcdId id = aHit.getAcdId();

  m_builder->addAttValue("Id", (int)id.id(), "");

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

/// Fill the HepRep for a single AcdTkrAssoc
void AcdReconFiller::addAcdTkrAssoc( const char* parent, const Event::AcdTkrAssoc& anAssoc ) {
  m_builder->addInstance(parent,"AcdTkrAssoc"); 

  m_builder->addAttValue("Index",(int)anAssoc.getTrackIndex(),"");
  m_builder->addAttValue("Energy",(float)anAssoc.getEnergy(),"");
  m_builder->addAttValue("Start",getPositionString(anAssoc.getStart()),"");
  m_builder->addAttValue("Dir",getDirectionString(anAssoc.getDir()),"");
  m_builder->addAttValue("ArcLength",(float)anAssoc.getArcLength(),"");

  const Event::AcdTkrHitPoca* bestHit = anAssoc.getHitPoca();
  const Event::AcdTkrGapPoca* bestGap = anAssoc.getGapPoca();
  const Event::AcdTkrPoint* trackPoint = anAssoc.getPoint();

  float vetoHit = bestHit == 0 ? 50. : sqrt(bestHit->vetoSigma2());
  float vetoGap = bestGap == 0 ? 50. : sqrt(bestGap->vetoSigma2());

  m_builder->addAttValue("VetoSigmaHit",vetoHit,"");
  m_builder->addAttValue("VetoSigmaGap",vetoGap,"");

  HepPoint3D endPoint = anAssoc.getStart() + (anAssoc.getArcLength() * anAssoc.getDir());
  m_builder->addPoint(anAssoc.getStart().x(),anAssoc.getStart().y(),anAssoc.getStart().z());
  m_builder->addPoint(endPoint.x(),endPoint.y(),endPoint.z());
  
  if ( bestHit != 0 ) {
    addAcdTkrHitPoca("AcdTkrAssoc",*bestHit);
  }
  if ( bestGap != 0 ) {
    addAcdTkrGapPoca("AcdTkrAssoc",*bestGap);
  }
  if( trackPoint != 0 ) {
    addAcdTkrPoint("AcdTkrAssoc",*trackPoint);
  }

}


/// Fill the HepRep for a AcdTkrHitPoca
void AcdReconFiller::addAcdTkrHitPoca( const char* parent, const Event::AcdTkrHitPoca& aPoca ) {
  m_builder->addInstance(parent,"AcdTkrHitPoca");  
  idents::AcdId id = aPoca.getId();
  m_builder->addAttValue("Id", (int)id.id(), "");
  m_builder->addAttValue("TrackIndex", aPoca.trackIndex(), "");
  m_builder->addAttValue("Active_X",aPoca.getActiveX(),"");
  m_builder->addAttValue("Active_Y",aPoca.getActiveY(),"");
  m_builder->addAttValue("Mips_A",aPoca.mipsPmtA(),"");
  m_builder->addAttValue("Mips_B",aPoca.mipsPmtB(),"");
  m_builder->addAttValue("VetoSigma",(float)sqrt(aPoca.vetoSigma2()),"");

  addAcdPocaData("AcdTkrHitPoca",aPoca);  
  addAcdTkrLocalCoords("AcdTkrHitPoca",id,aPoca);
  
}

/// Fill the HepRep for a AcdTkrGapPoca
void AcdReconFiller::addAcdTkrGapPoca( const char* parent, const Event::AcdTkrGapPoca& aPoca ) {
  m_builder->addInstance(parent,"AcdTkrGapPoca");  
  idents::AcdGapId id = aPoca.getId();
  m_builder->addAttValue("GapId", (int)id.asShort(), "");
  m_builder->addAttValue("TrackIndex", aPoca.trackIndex(), "");
  m_builder->addAttValue("VetoSigma",(float)sqrt(aPoca.vetoSigma2()),"");

  addAcdPocaData("AcdTkrGapPoca",aPoca);  
  idents::AcdId tid(0,id.face(),id.row(),id.col()); 
  std::cout << "Gap " << id.asShort() << ' ' << tid.id() << std::endl;
  addAcdTkrLocalCoords("AcdTkrGapPoca",tid,aPoca);
}

void AcdReconFiller::addAcdTkrPoint( const char* parent, const Event::AcdTkrPoint& aPoint ) {
  static const idents::AcdId nullId;
  m_builder->addInstance(parent,"AcdTkrPoint");  
  m_builder->addAttValue("TrackIndex", aPoint.getTrackIndex(), "");
  //addAcdTkrLocalCoords("AcdTkrPoint",nullId,aPoint);
}


/// Fill the HepRep for AcdTkrLocalCoords
void AcdReconFiller::addAcdTkrLocalCoords( const char* parent, 
					   const idents::AcdId& acdId,
					   const Event::AcdTkrLocalCoords& local ) {    
  m_builder->addInstance(parent,"AcdTkrLocalCoords");  
  const HepPoint3D& global = local.getGlobalPosition();
  m_builder->addAttValue("Region", local.getLocalVolume(), "");
  m_builder->addAttValue("ArcLength",local.getArclengthToPlane(),"");
  m_builder->addAttValue("CosTheta",local.getCosTheta(),"");
  m_builder->addAttValue("Global", getPositionString(global), "");
  m_builder->addAttValue("Local_X", local.getLocalX(), "");
  m_builder->addAttValue("Local_Y", local.getLocalY(), "");
  m_builder->addAttValue("Local_XXCovProj", (float)local.getLocalCovProj()(1,1), "");
  m_builder->addAttValue("Local_YYCovProj", (float)local.getLocalCovProj()(2,2), "");
  m_builder->addAttValue("Local_XYCovProj", (float)local.getLocalCovProj()(1,2), "");
  m_builder->addAttValue("Local_XXCovProp", (float)local.getLocalCovProp()(1,1), "");
  m_builder->addAttValue("Local_YYCovProp", (float)local.getLocalCovProp()(2,2), "");
  m_builder->addAttValue("Local_XYCovProp", (float)local.getLocalCovProp()(1,2), "");
  HepVector3D v1_proj;
  HepVector3D v2_proj;
  HepVector3D v1_prop;
  HepVector3D v2_prop;
  if ( acdId.tile() ) {
    const AcdTileDim* tile = m_acdsvc->geomMap().getTile(acdId,*m_acdsvc);
    int whichVol = local.getLocalVolume();
    AcdFrameUtil::getErrorAxes(tile->getSection(whichVol)->m_invTrans,local.getLocalCovProj(),v1_proj,v2_proj);
    AcdFrameUtil::getErrorAxes(tile->getSection(whichVol)->m_invTrans,local.getLocalCovProp(),v1_prop,v2_prop);
  } else {
    //const AcdRibbonDim* ribbon = m_acdsvc->geomMap().getRibbon(acdId,*m_acdsvc);
    //int whichVol = local.getLocalVolume();
    //AcdFrameUtil::getErrorAxes(ribbon->inverseTransformation(whichVol),local.getLocalCovProj(),v1_proj,v2_proj);
    //AcdFrameUtil::getErrorAxes(ribbon->inverseTransformation(whichVol),local.getLocalCovProp(),v1_prop,v2_prop);    
  }
  m_builder->addPoint(global.x(),global.y(),global.z());  
  m_builder->setSubinstancesNumber("AcdTkrLocalCoords", 4);
  m_builder->addInstance("AcdTkrLocalCoords","AcdLocalErrorAxis");
  AcdHepRepPointFiller::addErrors(*m_builder,global,v1_proj);
  m_builder->addInstance("AcdTkrLocalCoords","AcdLocalErrorAxis");      
  AcdHepRepPointFiller::addErrors(*m_builder,global,v2_proj);    
  m_builder->addInstance("AcdTkrLocalCoords","AcdLocalErrorAxis");
  AcdHepRepPointFiller::addErrors(*m_builder,global,v1_prop);
  m_builder->addInstance("AcdTkrLocalCoords","AcdLocalErrorAxis");      
  AcdHepRepPointFiller::addErrors(*m_builder,global,v2_prop);    
}

/// Fill the HepRep for AcdPocaData
void AcdReconFiller::addAcdPocaData( const char* parent, const Event::AcdPocaData& pocaData ) {
  m_builder->addInstance(parent,"AcdPocaData");  

  const Point& poca = pocaData.getPoca();
  const Vector& voca = pocaData.getPocaVector();

  const HepPoint3D global(poca.x(),poca.y(),poca.z());

  Vector vErrProj = voca;
  if ( pocaData.getDocaErrProj() > 0 ) {
    vErrProj.setMag( pocaData.getDocaErrProj() );    
  } else {
    vErrProj.setMag( 0.01 );
  }
  Vector vErrProp = voca;
  if ( pocaData.getDocaErrProp() > 0 ) {
    vErrProp.setMag( pocaData.getDocaErrProp() );    
  } else {
    vErrProp.setMag( 0.01 );
  }

  m_builder->addPoint(pocaData.getPoca().x(),pocaData.getPoca().y(),pocaData.getPoca().z());
  m_builder->addPoint(pocaData.getPoca().x()-pocaData.getPocaVector().x(),
		      pocaData.getPoca().y()-pocaData.getPocaVector().y(),
		      pocaData.getPoca().z()-pocaData.getPocaVector().z());
  m_builder->addAttValue("Doca",pocaData.getDoca(),"");
  m_builder->addAttValue("DocaErrProj",pocaData.getDocaErrProj(),"");
  m_builder->addAttValue("DocaErrProp",pocaData.getDocaErrProp(),"");
  m_builder->addAttValue("ArcLength",pocaData.getArcLength(),"");
  m_builder->addAttValue("Poca",getPositionString(pocaData.getPoca()),"");
  m_builder->addAttValue("Voca",getDirectionString(pocaData.getPocaVector()),"");

  m_builder->setSubinstancesNumber("AcdPocaError", 2);

  m_builder->addInstance("AcdPocaData","AcdPocaError");
  m_builder->addAttValue("Error",pocaData.getDocaErrProj(),"");
  AcdHepRepPointFiller::addErrors(*m_builder,global,vErrProj);

  m_builder->addInstance("AcdPocaData","AcdPocaError");      
  m_builder->addAttValue("Error",pocaData.getDocaErrProp(),"");
  AcdHepRepPointFiller::addErrors(*m_builder,global,vErrProp);
}



/// Check to see if a type is in the list of known types
bool AcdReconFiller::hasType(std::vector<std::string>& list, std::string type) {
  for (std::vector<std::string>::const_iterator itr = list.begin(); itr != list.end(); itr++ ) {
    if ( *itr == type ) return true;
  }
  return false;
}


