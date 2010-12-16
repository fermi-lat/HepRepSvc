#include "TrackElementsFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrTrackElements.h"
#include "Event/Recon/TkrRecon/TkrTree.h"
#include "Event/Recon/TkrRecon/TkrFilterParams.h"
#include "Event/Recon/TkrRecon/TkrBoundBox.h"
#include "Event/Recon/TkrRecon/TkrBoundBoxPoints.h"

#include <sstream>
#include <iostream>
#include <algorithm>

namespace {
    const float _dH = 0.228; // size of TkrVecPoint hit
}

// Constructor
TrackElementsFiller::TrackElementsFiller(HepRepInitSvc*        hrisvc,
                                         IGlastDetSvc*         gsvc,
                                         IDataProviderSvc*     dpsvc,
                                         IParticlePropertySvc* ppsvc) : m_dpsvc(dpsvc),m_ppsvc(ppsvc) 
{
    return;
}

// This method build the types for the HepRep
void TrackElementsFiller::buildTypes()
{
    // Define the collection of TkrVecPoints objects
    m_builder->addType("TkrRecon","TkrVecPoints", "Vector Points","");
    m_builder->addAttDef("NumVecPoints", "Number of Vector Points", "Physics", "");

    // Define the TkrVecPoints objects themselves
    m_builder->addType("TkrVecPoints","TkrVecPoint","Vector Point collection","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","red","");
    m_builder->addAttValue("FillColor", "red", "");
    m_builder->addAttValue("FillType", "NonZero", "");
    m_builder->addAttValue("Fill", true, "");
    m_builder->addAttDef("BiLayer", "Bi-layer of point", "Physics", "");
    m_builder->addAttDef("Position", "Position", "Physics", "");

    // Define the collection of TkrBoundBoxes to go with the above points
    m_builder->addType("TkrRecon", "TkrFilterParamsCol", "TkrFilterParamsCol", "");
    m_builder->addAttDef("Number Filter Params", "Number of TkrFilterParamsCol", "Physics", "");

    m_builder->addType("TkrFilterParamsCol","TkrFilterParams", "Tracker Filter Parameters","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","0.98,0.50,0.45",""); // "Salmon"
    m_builder->addAttDef("Status Bits", "Status Bits","Physics","");
    m_builder->addAttDef("Position","Track position","Physics","");
    m_builder->addAttDef("Direction","Track direction","Physics","");
    m_builder->addAttDef("Energy","Event Energy","Physics","MeV");
    m_builder->addAttDef("NumBiLayerHits","Number Bilayers Hit","Physics","#");
    m_builder->addAttDef("NumIterations","Number Iterations","Physics","#");
    m_builder->addAttDef("NumHitsTotal","Number Hits Total","Physics","#");
    m_builder->addAttDef("NumDropped","Number Dropped Hits","Physics","#");
    m_builder->addAttDef("ChiSquare","Moments ChiSquare","Physics","#");
    m_builder->addAttDef("TransRms","Scaled Transvers Moment","Physics","#");
    m_builder->addAttDef("LongRmsAve","Average Longitudinal Moment","Physics","#");

    // Define the collection of TkrBoundBoxes to go with the above points
    m_builder->addType("TkrFilterParams", "TkrBoundBoxes", "TkrBoundBox collection", "");
    m_builder->addAttDef("Number Boxes", "Number of TkrBoundBoxes", "Physics", "");

    // Define the individual TkrBoundBoxes
    m_builder->addType("TkrBoundBoxes", "TkrBoundBox", "TkrBoundBox", "");
    m_builder->addAttValue("DrawAs", "Prism", "");
    m_builder->addAttValue("Color", "yellow", "");
    m_builder->addAttValue("FillColor", "purple", "");
    m_builder->addAttValue("FillType", "NonZero", "");
    m_builder->addAttValue("Fill", true, "");
    // Output the quantities stored in the box
    m_builder->addAttDef("NumVecPoints",  "Number of TkrVecPoints in Box",      "Physics", "");
    m_builder->addAttDef("BiLayer",       "BiLayer for this box",               "Physics", "");
    m_builder->addAttDef("BoxCenter",     "Box Center Position",                "Physics", "");
    m_builder->addAttDef("AvePosition",   "Average Position of TkrVecPoints",   "Physics", "");
    m_builder->addAttDef("Hit Density",   "Density of TkrVecPoints in Box",     "Physics", "");
    m_builder->addAttDef("Mean Distance", "Mean Distance Between TkrVecPoints", "Physics", "");
    m_builder->addAttDef("RMS Distance",  "RMS Distance Between TkrVecPoints",  "Physics", "");

    // Define the collection of TkrBoundBoxPoints to go with the above boxes
    m_builder->addType("TkrFilterParams", "TkrBoundBoxPointsCol", "TkrBoundBoxPoints Collection", "");
    m_builder->addAttDef("Number Points", "Number of TkrBoundBoxPoints", "Physics", "");

    // Define the individual TkrBoundBoxes
    m_builder->addType("TkrBoundBoxPointsCol", "TkrBoundBoxPoints", "TkrBoundBoxPoints", "");
    m_builder->addAttValue("DrawAs", "Line", "");
    m_builder->addAttValue("Color", "green", "");
    // Output the quantities stored in the box
    m_builder->addAttDef("Distance", "Distance between associated TkrVecPoints", "Physics", "");

    // Now the TkrVecPointsLinks collection
    m_builder->addType("TkrRecon","TkrVecPointsLinks", "Vector links","");
    m_builder->addAttDef("NumVecLinks", "Number of Vector Links", "Physics", "");

    // And, in particular, a single TkrVecPointsLink
    m_builder->addType("TkrVecPointsLinks","TkrVecPointsLink","Vector link collection","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","white","");
//    m_builder->addAttValue("LineStyle", "Dashed","");
    m_builder->addAttDef("Status Low", "Vector Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","Vector High Status Bits","Physics","");
    m_builder->addAttDef("MaxScatAngle","Maximum expected scattering angle","Physics","");
    m_builder->addAttDef("Angle2NextLink","Angle to next link","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");
    // Here we define the prisms at each end of the links
    m_builder->addType("TkrVecPointsLink","TkrVecPointsLinkPos","TkrVecPointsLink Position","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("BiLayer", "Bi-layer of point", "Physics", "");
    m_builder->addAttDef("Position", "Position", "Physics", "");

    // Now define the TkrTree collection
    m_builder->addType("TkrRecon", "Candidate Trees", "Track Candidate Trees", "");
    m_builder->addAttDef("NumTrees", "Number of Trees", "Physics", "");

    // And the TkrTree objects which are the "parents" of the actual nodes 
    m_builder->addType("Candidate Trees", "Candidate Tree", "Track Candidate Tree", "");
    m_builder->addAttDef("Depth", "Depth of Candidate Tree", "Physics", "");
    m_builder->addAttDef("Leaves", "Number of Leaves in Tree", "Physics", "");
    m_builder->addAttDef("Links", "Number of links in Tree", "Physics", "");
    m_builder->addAttDef("Number Daughters", "Number daughters", "Physics", "");
    m_builder->addAttDef("Best RMS angle", "RMS deviations of best branch", "Physics", "");
    m_builder->addAttDef("Best Bi Layers", "Number bi layers of best branch", "physics", "");
    m_builder->addAttDef("Number Tracks", "Number of Tracks associated to this Tree", "physics", "");

    m_builder->addType("Candidate Tree", "TkrVecNodes", "TkrVecNodes in Tree", "");
    m_builder->addAttDef("Number Nodes", "Number of Nodes in Tree", "Physics", "");

    // Start with info from the node
    m_builder->addType("TkrVecNodes", "TkrVecNode", "TkrVecNode", "");
    m_builder->addAttDef("Nodes below", "Depth of node tree", "Physics", "");
    m_builder->addAttDef("Leaves below", "Number leaves below this node", "Physics", "");
    m_builder->addAttDef("Links below", "Number links below this node", "Physics", "");
    m_builder->addAttDef("RMS angle", "RMS deviations to this node", "Physics", "");
    m_builder->addAttDef("Number Daughters", "Number daughters", "Physics", "");
    m_builder->addAttDef("Best RMS angle", "RMS deviations of best branch", "Physics", "");
    m_builder->addAttDef("Best Bi Layers", "Number bi layers of best branch", "physics", "");
    m_builder->addAttDef("Dist to Main", "Distance to Main Branch", "physics", "");
    m_builder->addAttDef("Num in Best RMS", "Number in Best RMS Angle", "physics", "");

    // Now continue with information specific to the link
    m_builder->addType("TkrVecNode", "TkrVecPointsLink", "TkrVecPointsLink", "");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("Status Low", "Vector Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","Vector High Status Bits","Physics","");
    m_builder->addAttDef("MaxScatAngle","Maximum expected scattering angle","Physics","");
    m_builder->addAttDef("Angle2NextLink","Angle to next link","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");
    // Here we define the prisms at each end of the links
    m_builder->addType("TkrVecPointsLink","TkrVecPointsLinkPos","TkrVecPointsLink Position","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("BiLayer", "Bi-layer of point", "Physics", "");
    m_builder->addAttDef("Position", "Position", "Physics", "");
}


// This method fill the instance tree Event with the actual TDS content
void TrackElementsFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (hasType(typesList,"Recon/TkrRecon/TkrVecPoints"))    ///TkrVecPointsLink"))
    {
        drawVectorPoints();
    }

    if (hasType(typesList,"Recon/TkrRecon/TkrFilterParamsCol"))    ///TkrVecPointsLink"))
    {
        drawFilterParamsCol();
    }

    if (hasType(typesList,"Recon/TkrRecon/TkrVecPointsLinks"))    ///TkrVecPointsLink"))
    {
        drawVectorLinks();
    }

    if (hasType(typesList,"Recon/TkrRecon/Candidate Trees"))
    {
        drawNodeTrees();
    }

    return;
}

void TrackElementsFiller::drawVectorPoints()
{
    // Retrieve pointer to the vector links collection in the TDS
    Event::TkrVecPointCol* vecPointsCol = 
        SmartDataPtr<Event::TkrVecPointCol>(m_dpsvc, EventModel::TkrRecon::TkrVecPointCol);

    //Now see if we can do the drawing
    if (!vecPointsCol) return;

    // Add an instance of TkrVecPoints
    m_builder->addInstance("TkrRecon","TkrVecPoints");

    // Figure out how many we have and tell the display what to expect
    int numVecPoints = vecPointsCol->size();
        
    m_builder->addAttValue("NumVecPoints", (float)(numVecPoints), "");

    m_builder->setSubinstancesNumber("TkrVecPoints", numVecPoints);

    double lineWidth = 2;

    // Loop through all the links in the collection
    for(Event::TkrVecPointCol::iterator vecPointItr = vecPointsCol->begin();
        vecPointItr != vecPointsCol->end();
        vecPointItr++)
    {
        Event::TkrVecPoint* vecPoint = *vecPointItr;

        // Get the two attributes: position and layer
        Point position = vecPoint->getPosition();
        int   layer    = vecPoint->getLayer();
        float xClusWid = 0.5*vecPoint->getXCluster()->size()*_dH;
        float yClusWid = 0.5*vecPoint->getYCluster()->size()*_dH;

        // Add an instance of TkrVecPoint
        m_builder->addInstance("TkrVecPoints","TkrVecPoint");
        
        m_builder->addAttValue("LineWidth", (float)lineWidth, "");

        // Display the attributes
        m_builder->addAttValue("BiLayer",   (float)(layer),              "");
        m_builder->addAttValue("Position",  getPositionString(position), "");

        // this is the prism that we pick
        drawPrism(position.x(), position.y(), position.z(), xClusWid, yClusWid, 0.9*_dH);
    }

    return;
}
  
void TrackElementsFiller::drawFilterParamsCol()
{
    // Retrieve pointer to the TkrFilterParams collection in the TDS
    Event::TkrFilterParamsCol* tkrFilterParamsCol = 
        SmartDataPtr<Event::TkrFilterParamsCol>(m_dpsvc, EventModel::TkrRecon::TkrFilterParamsCol);

    // No Filter Params, no work
    if (!tkrFilterParamsCol) return;
    
    // Now retrieve the relations between the TkrFilterParams and their associated TkrBoundBoxes
    Event::TkrFilterParamsToBoxTabList* tkrFilterParamsToBoxList = 
        SmartDataPtr<Event::TkrFilterParamsToBoxTabList>(m_dpsvc, EventModel::TkrRecon::TkrFilterParamsToBoxTab);

    // Dummy list in case nothing in TDS
    Event::TkrFilterParamsToBoxTabList dummyList;
    dummyList.clear();

    if (!tkrFilterParamsToBoxList) tkrFilterParamsToBoxList = &dummyList;

    // Use the above to now make a relational table between TkrFilterParams and TkrBoundBoxes
    Event::TkrFilterParamsToBoxTab tkrFilterParamsToBoxTab(tkrFilterParamsToBoxList);

    // And now retrieve the relations between the TkrFilterParams and their associated TkrBoundBoxPoints
    Event::TkrFilterParamsToPointsTabList* tkrFilterParamsToPointsList = 
        SmartDataPtr<Event::TkrFilterParamsToPointsTabList>(m_dpsvc, EventModel::TkrRecon::TkrFilterParamsToPointsTab);

    // Dummy list in case nothing in TDS
    Event::TkrFilterParamsToPointsTabList dummyPointsList;
    dummyPointsList.clear();

    if (!tkrFilterParamsToPointsList) tkrFilterParamsToPointsList = &dummyPointsList;

    // And now make a relational table between TkrFilterParams and TkrBoundBoxPoints
    Event::TkrFilterParamsToPointsTab tkrFilterParamsToPointsTab(tkrFilterParamsToPointsList);

    // Ready for work, initiate the drawing sequence
    m_builder->addInstance("TkrRecon","TkrFilterParamsCol");

    int numFilterParams = tkrFilterParamsCol->size();
        
    m_builder->addAttValue("Number Filter Params", (float)(numFilterParams), "");

    m_builder->setSubinstancesNumber("TkrFilterParamsCol", numFilterParams);

    // Loop through the collection
    for(Event::TkrFilterParamsCol::iterator paramsItr  = tkrFilterParamsCol->begin();
                                            paramsItr != tkrFilterParamsCol->end();
                                            paramsItr++)
    {
        // Recover a pointer to to the TkrFilterParams
        Event::TkrFilterParams* tkrFilterParams = *paramsItr;

        // Output the information contained in the TkrFilterParams
        drawFilterParams(tkrFilterParams);

        // Draw the boxes associated with these params
        drawTkrBoundBoxes(tkrFilterParams, tkrFilterParamsToBoxTab);

        // Draw the connections between TkrVecPoints for the associated hits
        drawTkrBoundBoxPoints(tkrFilterParams, tkrFilterParamsToPointsTab);
    }

    return;
}

void TrackElementsFiller::drawFilterParams(Event::TkrFilterParams* tkrFilterParams)
{
    // Add an instance of the drawing object to the collection
    m_builder->addInstance("TkrFilterParamsCol","TkrFilterParams");

    // And start filling it
    m_builder->addAttValue("Position",       getPositionString(tkrFilterParams->getEventPosition()), "");
    m_builder->addAttValue("Direction",      getDirectionString(tkrFilterParams->getEventAxis()), "");
    m_builder->addAttValue("Energy",         (float)tkrFilterParams->getEventEnergy(), "");
    m_builder->addAttValue("NumBiLayerHits", tkrFilterParams->getNumBiLayers(), "");
    m_builder->addAttValue("NumIterations",  tkrFilterParams->getNumIterations(), "");
    m_builder->addAttValue("NumHitsTotal",   tkrFilterParams->getNumHitsTotal(), "");
    m_builder->addAttValue("NumDropped",     tkrFilterParams->getNumDropped(), "");
    m_builder->addAttValue("ChiSquare",      (float)tkrFilterParams->getChiSquare(), "");
    m_builder->addAttValue("TransRms",       (float)tkrFilterParams->getTransRms(), "");
    m_builder->addAttValue("LongRmsAve",     (float)tkrFilterParams->getLongRmsAve(), "");

    //Build strings for status bits
    unsigned int statBits = tkrFilterParams->getStatusBits();
    m_builder->addAttValue("Status Bits",getBits(statBits, 15, 0),"");

    // Set up to draw the track
    Point  position = tkrFilterParams->getEventPosition();
    Vector axis     = tkrFilterParams->getEventAxis();

    Point  start    = position - 500. * axis;
    Point  stop     = position + 500. * axis;

    m_builder->addPoint(start.x(), start.y(), start.z());
    m_builder->addPoint(stop.x(),  stop.y(),  stop.z());

    return;
}

void TrackElementsFiller::drawTkrBoundBoxes(Event::TkrFilterParams* tkrFilterParams, Event::TkrFilterParamsToBoxTab& paramsToBoxTab)
{
    // Use this to get a vector containing all related TkrBoundBoxes
    std::vector<Event::TkrFilterParamsToBoxRel*> paramsToBoxVec = paramsToBoxTab.getRelByFirst(tkrFilterParams);

    // Add an instance of TkrVecPoints
    m_builder->addInstance("TkrFilterParams","TkrBoundBoxes");

    // Figure out how many we have and tell the display what to expect
    int numVecPointBoxes = paramsToBoxVec.size();
        
    m_builder->addAttValue("Number Boxes", (float)(numVecPointBoxes), "");

    m_builder->setSubinstancesNumber("TkrBoundBoxes", numVecPointBoxes);

    // Now loop through this vector to draw the bounding boxes
    std::vector<Event::TkrFilterParamsToBoxRel*>::iterator paramsToBoxItr = paramsToBoxVec.begin();

    while(paramsToBoxItr != paramsToBoxVec.end())
    {
        Event::TkrBoundBox* vecPointBox = (*paramsToBoxItr++)->getSecond();

        // Add an instance of TkrVecPoint
        m_builder->addInstance("TkrBoundBoxes","TkrBoundBox");

        // Retrieve and fill stuff we want to see
        int           nPoints     = vecPointBox->size();
        int           biLayer     = vecPointBox->getBiLayer();
        const  Point& boxCenter   = vecPointBox->getBoxCenterPos();
        const  Point& avePosition = vecPointBox->getAveragePosition();
        double        hitDensity  = vecPointBox->getHitDensity();
        double        meanDist    = vecPointBox->getMeanDist();
        double        rmsDist     = vecPointBox->getRmsDist();

        m_builder->addAttValue("NumVecPoints",  float(nPoints),                 "");
        m_builder->addAttValue("BiLayer",       float(biLayer),                 "");
        m_builder->addAttValue("BoxCenter",     getPositionString(boxCenter),   "");
        m_builder->addAttValue("AvePosition",   getPositionString(avePosition), "");
        m_builder->addAttValue("Hit Density",   float(hitDensity),              "");
        m_builder->addAttValue("Mean Distance", float(meanDist),                "");
        m_builder->addAttValue("RMS Distance",  float(rmsDist),                 "");

        // Set up to draw the shape
        const  Point& lowEdge   = vecPointBox->getLowCorner();
        const  Point& highEdge  = vecPointBox->getHighCorner();
        double boxSigmaX        = 0.5 * (highEdge.x() - lowEdge.x());
        double boxSigmaY        = 0.5 * (highEdge.y() - lowEdge.y());

        drawPrism(boxCenter.x(), boxCenter.y(), boxCenter.z(), boxSigmaX, boxSigmaY, _dH);
    }

    return;
}

void TrackElementsFiller::drawTkrBoundBoxPoints(Event::TkrFilterParams* tkrFilterParams, 
                                                Event::TkrFilterParamsToPointsTab& paramsToPointsTab)
{
    // First up let's draw the links between TkrVecPoints from the minimum spanning tree (if they are there)
    std::vector<Event::TkrFilterParamsToPointsRel*> paramsToPointsVec = paramsToPointsTab.getRelByFirst(tkrFilterParams);

    // Add an instance of TkrVecPoints
    m_builder->addInstance("TkrFilterParams","TkrBoundBoxPointsCol");

    // Figure out how many we have and tell the display what to expect
    int numTkrBoundBoxPoints = paramsToPointsVec.size();
        
    m_builder->addAttValue("Number Points", (float)(numTkrBoundBoxPoints), "");

    m_builder->setSubinstancesNumber("TkrBoundBoxPoints", numTkrBoundBoxPoints);

    // Now loop through this vector to draw the links between TkrVecPoints
    std::vector<Event::TkrFilterParamsToPointsRel*>::iterator paramsToPointsItr = paramsToPointsVec.begin();

    while(paramsToPointsItr != paramsToPointsVec.end())
    {
        Event::TkrBoundBoxPoints* boxPoints = (*paramsToPointsItr++)->getSecond();

        // Add an instance of TkrBoundBoxPoints
        m_builder->addInstance("TkrBoundBoxPointsCol","TkrBoundBoxPoints");

        m_builder->addAttValue("Distance",  float(boxPoints->getSeparation()), "");

        Point firstPoint = boxPoints->getFirstTkrVecPoint()->getPosition();
        m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());

        Point scndPoint  = boxPoints->getSecondTkrVecPoint()->getPosition();
        m_builder->addPoint(scndPoint.x(), scndPoint.y(), scndPoint.z());
    }

    return;
}


void TrackElementsFiller::drawVectorLinks()
{
    // Retrieve pointer to the vector links collection in the TDS
    Event::TkrVecPointsLinkCol* vecPointsLinkCol = 
        SmartDataPtr<Event::TkrVecPointsLinkCol>(m_dpsvc, EventModel::TkrRecon::TkrVecPointsLinkCol);

    //Now see if we can do the drawing
    if (!vecPointsLinkCol) return;

    Event::TkrTrackElemToLinksTabList* elemsToLinksList = 
        SmartDataPtr<Event::TkrTrackElemToLinksTabList>(m_dpsvc, EventModel::TkrRecon::TkrTrackElemsToLinksTab);

    // Dummy list in case nothing in TDS
    Event::TkrTrackElemToLinksTabList dummyList;
    dummyList.clear();

    if (!elemsToLinksList) elemsToLinksList = &dummyList;

    Event::TkrTrackElemToLinksTab elemsToLinksTab(elemsToLinksList);

    m_builder->addInstance("TkrRecon","TkrVecPointsLinks");

    int numVecLinks = vecPointsLinkCol->size();
        
    m_builder->addAttValue("NumVecLinks", (float)(numVecLinks), "");

    if (numVecLinks == 0 || numVecLinks > 25000) return;

    m_builder->setSubinstancesNumber("TkrVecPointsLinks", numVecLinks);

    // Let's count how many we actually add... just to be sure
    int countEm = 0;

    // Loop through all the links in the collection
    for(Event::TkrVecPointsLinkCol::iterator vecLinkItr = vecPointsLinkCol->begin();
        vecLinkItr != vecPointsLinkCol->end();
        vecLinkItr++)
    {
        Event::TkrVecPointsLink* vecLink = *vecLinkItr;

        m_builder->addInstance("TkrVecPointsLinks","TkrVecPointsLink");
        m_builder->setSubinstancesNumber("TkrVecPointsLink", 2);

        countEm++;

        std::vector<Event::TkrTrackElemToLinksRel*> elemToLinksVec = elemsToLinksTab.getRelBySecond(vecLink);
        std::string linkColor = "white";

        if      (vecLink->skip1Layer()) linkColor  = "204,204,204";
        else if (vecLink->skip2Layer()) linkColor  = "153,153,153";
        else if (vecLink->skipsLayers()) linkColor = "102,102,102";

        if (!elemToLinksVec.empty()) 
        {
            std::vector<Event::TkrTrackElemToLinksRel*>::iterator trkElemItr;
            for(trkElemItr = elemToLinksVec.begin(); trkElemItr != elemToLinksVec.end(); trkElemItr++)
            {
                Event::TkrTrackElements* trackElem = (*trkElemItr)->getFirst();

                if (trackElem->getStatusBits() & Event::TkrTrackElements::NOTBEST) continue;

                linkColor = "green";

                break;
            }
        }
        else if (vecLink->associated()) continue;

        drawSingleVectorLink(vecLink, linkColor);
    }

    return;
}

void TrackElementsFiller::drawSingleVectorLink(const Event::TkrVecPointsLink* vecLink, std::string& linkColor)
{
    m_builder->addAttValue("Color",           linkColor,                                 "");
    m_builder->addAttValue("MaxScatAngle",    (float)(vecLink->getMaxScatAngle()),       "");
    m_builder->addAttValue("Angle2NextLink",  (float)(vecLink->getAngleToNextLink()),    "");
    m_builder->addAttValue("Start Position",  getPositionString(vecLink->getPosition()), "");
    m_builder->addAttValue("Start Direction", getDirectionString(vecLink->getVector()),  "");

    //Build strings for status bits
    unsigned int statusBits = vecLink->getStatusBits();

    m_builder->addAttValue("Status Low",  getBits(statusBits, 15, 0),  "");
    m_builder->addAttValue("Status High", getBits(statusBits, 31, 16), "");

    Point firstPoint = vecLink->getPosition();
    m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());

    Point scndPoint  = vecLink->getBotPosition();
    m_builder->addPoint(scndPoint.x(), scndPoint.y(), scndPoint.z());

    // Add an instance of TkrVecPointsLinkPos
    m_builder->addInstance("TkrVecPointsLink","TkrVecPointsLinkPos");
    m_builder->addAttValue("Color", "yellow", "");

    // Display the attributes
    m_builder->addAttValue("Position", getPositionString(firstPoint), "");

    // Get size of prism to draw
    float xClusWid = 0.5*vecLink->getFirstVecPoint()->getXCluster()->size()*_dH;
    float yClusWid = 0.5*vecLink->getFirstVecPoint()->getYCluster()->size()*_dH;

    // this is the prism that we pick
    drawPrism(firstPoint.x(), firstPoint.y(), firstPoint.z(), xClusWid, yClusWid, _dH);

    // Add an instance of TkrVecPointsLinkPos
    m_builder->addInstance("TkrVecPointsLink","TkrVecPointsLinkPos");

    // Display the attributes
    m_builder->addAttValue("Position", getPositionString(scndPoint), "");

    // Get size of prism to draw
    xClusWid = 0.5*vecLink->getSecondVecPoint()->getXCluster()->size()*_dH;
    yClusWid = 0.5*vecLink->getSecondVecPoint()->getYCluster()->size()*_dH;

    // this is the prism that we pick
    drawPrism(scndPoint.x(), scndPoint.y(), scndPoint.z(), xClusWid, yClusWid, _dH);

    return;
}

void TrackElementsFiller::drawNodeTrees()
{
    // Retrieve pointer to the vector of head nodes in the TDS
    Event::TkrTreeCol* treeCol = SmartDataPtr<Event::TkrTreeCol>(m_dpsvc, "/Event/TkrRecon/TkrTreeCol");

    //Now see if we can do the drawing
    if (!treeCol) return;

    m_builder->addInstance("TkrRecon","Candidate Trees");

    // Set the number of nodes so display will know
    int numTrees = treeCol->size();

    if (numTrees == 0) return;
        
    m_builder->addAttValue("NumTrees", (float)(numTrees), "");

    m_builder->setSubinstancesNumber("Candidate Trees", numTrees);

    // Set the color and width for the first tree
    std::string lineColor = "red";
    int         lineWidth = 2;

    // Loop over the collection of head nodes
    for(Event::TkrTreeCol::iterator treeItr = treeCol->begin(); treeItr != treeCol->end(); treeItr++)
    {
        const Event::TkrVecNode* vecNode = (*treeItr)->getHeadNode();

        // Recover the details for this tree
        int   treeDepth     = vecNode->getDepth();
        int   numLeaves     = vecNode->getNumLeaves();
        int   numVecLinks   = vecNode->getNumNodesInTree();
        int   bestNumLayers = vecNode->getBestNumBiLayers();
        float bestRmsAngle  = vecNode->getBestRmsAngle();
        int   numDaughters  = vecNode->size();
        int   numTracks     = (*treeItr)->size();

        // Add the Candidate Tree instance
        m_builder->addInstance("Candidate Trees", "Candidate Tree");
        m_builder->addAttValue("Depth",            (float)treeDepth,     "");
        m_builder->addAttValue("Leaves",           (float)numLeaves,     "");
        m_builder->addAttValue("Links",            (float)numVecLinks,   "");
        m_builder->addAttValue("Number Daughters", (float)numDaughters,  "");
        m_builder->addAttValue("Best RMS angle",   bestRmsAngle,         "");
        m_builder->addAttValue("Best Bi Layers",   (float)bestNumLayers, "");
        m_builder->addAttValue("Number Tracks",    (float)numTracks,     "");

        m_builder->setSubinstancesNumber("Candidate Tree", numVecLinks);

        m_builder->addInstance("Candidate Tree", "TkrVecNodes");
        m_builder->addAttValue("Number Nodes", (float)numVecLinks,   "");


        // Now draw the node (which will draw the daughter nodes)
        drawNode(vecNode, lineColor, lineWidth);

        lineColor = "255,64,64";
        lineWidth = 1;
    }

    return;
}

void TrackElementsFiller::drawNode(const Event::TkrVecNode* vecNode, std::string& lineColor, int lineWidth)
{
    // Recover the TkrVecPointsLink associated to this node
    const Event::TkrVecPointsLink* vecLink = vecNode->getAssociatedLink();

    // Note that the top node will not have an associated link as it is the anchor node
    if (vecLink)
    {
        m_builder->addInstance("TkrVecNodes","TkrVecNode");
        
        // Recover the details for this tree
        int   treeDepth     = vecNode->getDepth();
        int   numLeaves     = vecNode->getNumLeaves();
        int   numVecLinks   = vecNode->getNumNodesInTree();
        int   numDaughters  = vecNode->size();
        float rmsAngle      = vecNode->getRmsAngle();
        int   bestNumLayers = vecNode->getBestNumBiLayers();
        float bestRmsAngle  = vecNode->getBestRmsAngle();
        int   numAngInRms   = vecNode->getNumAnglesInSum();
        int   distToMain    = vecNode->getBiLyrs2MainBrch();

        m_builder->addAttValue("Nodes below",      (float)treeDepth,                          "");
        m_builder->addAttValue("Leaves below",     (float)numLeaves,                          "");
        m_builder->addAttValue("Links below",      (float)numVecLinks,                        "");
        m_builder->addAttValue("Number Daughters", (float)numDaughters,                       "");
        m_builder->addAttValue("RMS angle",        rmsAngle,                                  "");
        m_builder->addAttValue("Best RMS angle",   bestRmsAngle,                              "");
        m_builder->addAttValue("Best Bi Layers",   float(bestNumLayers),                      "");
        m_builder->addAttValue("Dist to Main",     float(numAngInRms),                        "");
        m_builder->addAttValue("Num in Best RMS",  float(distToMain),                         "");

        // Now draw the actual link
        m_builder->addInstance("TkrVecNode", "TkrVecPointsLink");
        m_builder->addAttValue("LineWidth",        (float)lineWidth,                          "");
        m_builder->addAttValue("Color",            lineColor,                                 "");

        drawSingleVectorLink(vecLink, lineColor);
    }

    // Now draw the links associated to the daughters of this node
    if (vecNode->empty()) return;

    for(Event::TkrVecNodeSet::const_iterator nodeItr = vecNode->begin(); nodeItr != vecNode->end(); nodeItr++)
    {
        const Event::TkrVecNode* daughterNode = *nodeItr;

        drawNode(daughterNode, lineColor, lineWidth);
    }

    return;
}
