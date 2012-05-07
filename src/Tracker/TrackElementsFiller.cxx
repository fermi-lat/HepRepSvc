#include "TrackElementsFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "TkrUtil/ITkrGeometrySvc.h"
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
#include "Event/Recon/TkrRecon/TkrBoundBoxLinks.h"
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
                                         ITkrGeometrySvc*      tgsvc,
                                         IDataProviderSvc*     dpsvc,
                                         IParticlePropertySvc* ppsvc) : m_tgsvc(tgsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc) 
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
    m_builder->addAttDef("Average Distance", "Average Distance to axis", "Physics", "#");
    m_builder->addAttDef("TransRms","Scaled Transvers Moment","Physics","#");
    m_builder->addAttDef("LongRms","Scaled Longitudinal Moment","Physics","#");
    m_builder->addAttDef("LongRmsAsym","Longitudinal Moment Asymmetry","Physics","#");

    // Define the collection of TkrBoundBoxes to go with the above points
    m_builder->addType("TkrFilterParams", "TkrBoundBoxLinkCol", "TkrBoundLink collection", "");
    m_builder->addAttDef("Number Links", "Number of TkrBoundBoxLinks", "Physics", "");

    // Define the collection of TkrBoundBoxLinks which comprise the TkrFilterParams
    m_builder->addType("TkrBoundBoxLinkCol", "TkrBoundBoxLink", "TkrBoundBoxLinks", "");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color", "green", "");
    m_builder->addAttDef("Position",           "Position",             "Physics", "");
    m_builder->addAttDef("Distance to Parent", "Distance to Parent",   "Physics", "");

    // Define the individual TkrBoundBoxes which go with each link
    m_builder->addType("TkrBoundBoxLink", "TkrBoundBox", "TkrBoundBox", "");
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
    m_builder->addType("TkrBoundBoxLink", "TkrBoundBoxPointsCol", "TkrBoundBoxPoints Collection", "");
    m_builder->addAttDef("Number Points", "Number of TkrBoundBoxPoints", "Physics", "");

    // Define the individual TkrBoundBoxes
    m_builder->addType("TkrBoundBoxPointsCol", "TkrBoundBoxPoints", "TkrBoundBoxPoints", "");
    m_builder->addAttValue("DrawAs", "Line", "");
    m_builder->addAttValue("Color", "green", "");
    // Output the quantities stored in the box
    m_builder->addAttDef("Position", "Position", "Physics", "");
    m_builder->addAttDef("Distance Left",  "Distance to Left Daughter",   "Physics", "");
    m_builder->addAttDef("Distance Right", "Distance to Right Daughter",  "Physics", "");
    m_builder->addAttDef("AveSeparation",  "Average Separation to point", "Physics", "");

    // Now the TkrVecPointsLinks collection
    m_builder->addType("TkrRecon","TkrVecPointsLinks", "Vector links","");
    m_builder->addAttDef("NumVecLinks",    "Number of Vector Links",    "Physics", "");
    m_builder->addAttDef("HoughLinks",     "Number Hough links",        "Physics", "");
    m_builder->addAttDef("NNoSkipLinks",   "Number no skip links",      "Physics", "");
    m_builder->addAttDef("NSkip1LyrLinks", "Number skip 1 layer links", "Physics", "");
    m_builder->addAttDef("NSkip2LyrLinks", "Number skip 2 layer links", "Physics", "");
    m_builder->addAttDef("NSkip3LyrLinks", "Number skip 3 layer links", "Physics", "");
    m_builder->addAttDef("NSkip4LyrLinks", "Number skip 4 layer links", "Physics", "");

    // And, in particular, a single TkrVecPointsLink
    // **** Define collections depending on number of layers skipped
    m_builder->addType("TkrVecPointsLinks","TkrHoughLinks",   "Vector link collection","");
    m_builder->addType("TkrVecPointsLinks","TkrNoSkipLinks",  "Vector link collection","");
    m_builder->addType("TkrVecPointsLinks","TkrSkip1LyrLinks","Vector link collection","");
    m_builder->addType("TkrVecPointsLinks","TkrSkip2LyrLinks","Vector link collection","");
    m_builder->addType("TkrVecPointsLinks","TkrSkip3LyrLinks","Vector link collection","");
    m_builder->addType("TkrVecPointsLinks","TkrSkipNLyrLinks","Vector link collection","");

    // Define each of the above collections here
    m_builder->addType("TkrHoughLinks",    "TkrVecPointsLink","Vector link collection","");
    defineLinkAttributes();
    m_builder->addType("TkrNoSkipLinks",   "TkrVecPointsLink","Vector link collection","");
    defineLinkAttributes();
    m_builder->addType("TkrSkip1LyrLinks", "TkrVecPointsLink","Vector link collection","");
    defineLinkAttributes();
    m_builder->addType("TkrSkip2LyrLinks", "TkrVecPointsLink","Vector link collection","");
    defineLinkAttributes();
    m_builder->addType("TkrSkip3LyrLinks", "TkrVecPointsLink","Vector link collection","");
    defineLinkAttributes();
    m_builder->addType("TkrSkipNLyrLinks", "TkrVecPointsLink","Vector link collection","");
    defineLinkAttributes();

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

    // Include the tree axis parameters
    m_builder->addType("Candidate Tree", "Axis", "Tree Axis Parameters", "");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","red",""); 
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

void TrackElementsFiller::defineLinkAttributes()
{
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","75,200,255","");  // a light off blue, very pleasing and soothing to the eye
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
    m_builder->addAttValue("Color","255,100,200",""); // for top hits, contrasts with other color
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
    
    // Now retrieve the relations between the TkrFilterParams and their associated TkrBoundBoxLinks
    Event::TkrFilterParamsToLinksTabList* tkrFilterParamsToLinksList = 
        SmartDataPtr<Event::TkrFilterParamsToLinksTabList>(m_dpsvc, EventModel::TkrRecon::TkrFilterParamsToLinksTab);

    // Dummy list in case nothing in TDS
    Event::TkrFilterParamsToLinksTabList dummyList;
    dummyList.clear();

    if (!tkrFilterParamsToLinksList) tkrFilterParamsToLinksList = &dummyList;

    // Use the above to now make a relational table between TkrFilterParams and TkrBoundBoxes
    Event::TkrFilterParamsToLinksTab tkrFilterParamsToLinksTab(tkrFilterParamsToLinksList);
/*
    // And now retrieve the relations between the TkrFilterParams and their associated TkrBoundBoxPoints
    Event::TkrFilterParamsToPointsTabList* tkrFilterParamsToPointsList = 
        SmartDataPtr<Event::TkrFilterParamsToPointsTabList>(m_dpsvc, EventModel::TkrRecon::TkrFilterParamsToPointsTab);

    // Dummy list in case nothing in TDS
    Event::TkrFilterParamsToPointsTabList dummyPointsList;
    dummyPointsList.clear();

    if (!tkrFilterParamsToPointsList) tkrFilterParamsToPointsList = &dummyPointsList;

    // And now make a relational table between TkrFilterParams and TkrBoundBoxPoints
    Event::TkrFilterParamsToPointsTab tkrFilterParamsToPointsTab(tkrFilterParamsToPointsList);
*/
    // Ready for work, initiate the drawing sequence
    m_builder->addInstance("TkrRecon","TkrFilterParamsCol");

    int numFilterParams = tkrFilterParamsCol->size();
        
    m_builder->addAttValue("Number Filter Params", (float)(numFilterParams), "");

    m_builder->setSubinstancesNumber("TkrFilterParamsCol", numFilterParams);

    // Identify which collections we are setting up here 
    std::string parent   = "TkrFilterParamsCol";
    std::string instance = "TkrFilterParams";

    // Loop through the collection
    for(Event::TkrFilterParamsCol::iterator paramsItr  = tkrFilterParamsCol->begin();
                                            paramsItr != tkrFilterParamsCol->end();
                                            paramsItr++)
    {
        // Recover a pointer to to the TkrFilterParams
        Event::TkrFilterParams* tkrFilterParams = *paramsItr;

        // Output the information contained in the TkrFilterParams
        drawFilterParams(tkrFilterParams, parent, instance);

        // Now draw the associated stuff that goes with this TkrFilterParam
        drawTkrBoundBoxLinks(tkrFilterParams, tkrFilterParamsToLinksTab);

        // Draw the boxes associated with these params
//        drawTkrBoundBoxes(tkrFilterParams, tkrFilterParamsToBoxTab);

        // Draw the connections between TkrVecPoints for the associated hits
//        drawTkrBoundBoxPoints(tkrFilterParams, tkrFilterParamsToPointsTab);
    }

    return;
}

void TrackElementsFiller::drawFilterParams(const Event::TkrFilterParams* tkrFilterParams, std::string& parent, std::string& instance)
{
    // Add an instance of the drawing object to the collection
//    m_builder->addInstance("TkrFilterParamsCol","TkrFilterParams");
    m_builder->addInstance(parent, instance);

    // And start filling it
    m_builder->addAttValue("Position",         getPositionString(tkrFilterParams->getEventPosition()), "");
    m_builder->addAttValue("Direction",        getDirectionString(tkrFilterParams->getEventAxis()), "");
    m_builder->addAttValue("Energy",           (float)tkrFilterParams->getEventEnergy(), "");
    m_builder->addAttValue("NumBiLayerHits",   tkrFilterParams->getNumBiLayers(), "");
    m_builder->addAttValue("NumIterations",    tkrFilterParams->getNumIterations(), "");
    m_builder->addAttValue("NumHitsTotal",     tkrFilterParams->getNumHitsTotal(), "");
    m_builder->addAttValue("NumDropped",       tkrFilterParams->getNumDropped(), "");
    m_builder->addAttValue("ChiSquare",        (float)tkrFilterParams->getChiSquare(), "");
    m_builder->addAttValue("Average Distance", (float)tkrFilterParams->getAverageDistance(), "");
    m_builder->addAttValue("TransRms",         (float)tkrFilterParams->getTransRms(), "");
    m_builder->addAttValue("LongRms",          (float)tkrFilterParams->getLongRms(), "");
//    m_builder->addAttValue("LongRmsAsym",      (float)tkrFilterParams->getLongRmsAysm(), "");

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

void TrackElementsFiller::drawTkrBoundBoxLinks(Event::TkrFilterParams* tkrFilterParams, Event::TkrFilterParamsToLinksTab& paramsToLinksTab)
{
    // Use this to get a vector containing all related TkrBoundBoxes
    std::vector<Event::TkrFilterParamsToLinksRel*> paramsToLinksVec = paramsToLinksTab.getRelByFirst(tkrFilterParams);

    // Add an instance of TkrVecPoints
    m_builder->addInstance("TkrFilterParams","TkrBoundBoxLinkCol");

    // Figure out how many we have and tell the display what to expect
    int nBoundBoxLinks = paramsToLinksVec.size();
        
    m_builder->addAttValue("Number Links", (float)(nBoundBoxLinks), "");

    m_builder->setSubinstancesNumber("TkrBoundBoxLink", nBoundBoxLinks);

    // Now loop through this vector to draw the bounding boxes
    std::vector<Event::TkrFilterParamsToLinksRel*>::iterator paramsToLinkItr = paramsToLinksVec.begin();

    try {

    while(paramsToLinkItr != paramsToLinksVec.end())
    {
        // Recover the TkrBoundBoxLink from our list iterator
        Event::TkrBoundBoxLink* bbLink = (*paramsToLinkItr++)->getSecond();

        // Add an instance of TkrBoundBoxLink
        m_builder->addInstance("TkrBoundBoxLinkCol","TkrBoundBoxLink");

        // Recover and print position and distance to parent
        m_builder->addAttValue("Position",           getPositionString(bbLink->getPosition()),   "");
        m_builder->addAttValue("Distance to Parent", float(bbLink->getDistToParent()),           "");

        // If we have a parent then we need to draw line to it
        if (const Event::TkrBoundBoxLink* parent = bbLink->getParent())
        {
            Point firstPoint = bbLink->getPosition();
            m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());

            Point scndPoint  = parent->getPosition();
            m_builder->addPoint(scndPoint.x(), scndPoint.y(), scndPoint.z());
        }

        // Now recover the pointer to the associated TkrBoundBox
        const Event::TkrBoundBox* box = bbLink->getBoundBox();

        // Add an instance of TkrBoundBox
        m_builder->addInstance("TkrBoundBoxLink","TkrBoundBox");

        // Retrieve and fill stuff we want to see
        int           nPoints     = box->size();
        int           biLayer     = box->getBiLayer();
        const  Point& boxCenter   = const_cast<Event::TkrBoundBox*>(box)->getBoxCenterPos();
        const  Point& avePosition = box->getAveragePosition();
        double        hitDensity  = box->getHitDensity();
        double        meanDist    = box->getMeanDist();
        double        rmsDist     = box->getRmsDist();

        m_builder->addAttValue("NumVecPoints",  float(nPoints),                 "");
        m_builder->addAttValue("BiLayer",       float(biLayer),                 "");
        m_builder->addAttValue("BoxCenter",     getPositionString(boxCenter),   "");
        m_builder->addAttValue("AvePosition",   getPositionString(avePosition), "");
        m_builder->addAttValue("Hit Density",   float(hitDensity),              "");
        m_builder->addAttValue("Mean Distance", float(meanDist),                "");
        m_builder->addAttValue("RMS Distance",  float(rmsDist),                 "");

        // Set up to draw the shape
        const  Point& lowEdge   = box->getLowCorner();
        const  Point& highEdge  = box->getHighCorner();
        double boxSigmaX        = 0.5 * (highEdge.x() - lowEdge.x());
        double boxSigmaY        = 0.5 * (highEdge.y() - lowEdge.y());

        drawPrism(boxCenter.x(), boxCenter.y(), boxCenter.z(), boxSigmaX, boxSigmaY, _dH);

        // Now draw the points associated with this box
        m_builder->addInstance("TkrBoundBoxLink","TkrBoundBoxPointsCol");

        // Recover the top TkrBoundBoxPoint and start doing some drawing! 
        const Event::TkrBoundBoxPoint* topPoint = bbLink->getTopPoint();

        // Number of bound box points we'll end up drawing
        int numTkrBoundBoxPoints = countTkrBoundBoxPoints(topPoint);
        
        m_builder->addAttValue("Number Points", float(numTkrBoundBoxPoints), "");

        m_builder->setSubinstancesNumber("TkrBoundBoxPointsCol", numTkrBoundBoxPoints);

        drawTkrBoundBoxPoint(topPoint);
    }

    } catch(...) {
        int whathappened = 0;
    }

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

    // Recover the top TkrBoundBoxPoint and start doing some drawing! 
    const Event::TkrBoundBoxPoint* topPoint = paramsToPointsVec.front()->getSecond();

    // Number of bound box points we'll end up drawing
    numTkrBoundBoxPoints = 750;
        
    m_builder->addAttValue("Number Points", (float)(numTkrBoundBoxPoints), "");

    m_builder->setSubinstancesNumber("TkrBoundBoxPoints", numTkrBoundBoxPoints);

    drawTkrBoundBoxPoint(topPoint);

    return;
}

int TrackElementsFiller::countTkrBoundBoxPoints(const Event::TkrBoundBoxPoint* point)
{
    int numPoints = 0;

    // If no daughters then we are at the bottom
    if (point->getLeft() || point->getRight())
    {
        // The current point has either a left or right daughter point so it will 
        // create a box object to be drawn
        numPoints++;

        // Get the number of boxes down the left side
        numPoints += countTkrBoundBoxPoints(point->getLeft());

        // And now the number of boxes down the right side
        numPoints += countTkrBoundBoxPoints(point->getRight());
    }

    return numPoints;
}

void TrackElementsFiller::drawTkrBoundBoxPoint(const Event::TkrBoundBoxPoint* curPoint)
{
    // If no daughters then nothing to do 
    if (curPoint->getLeft() || curPoint->getRight())
    {
        // Add an instance of TkrBoundBoxPoints
        m_builder->addInstance("TkrBoundBoxPointsCol","TkrBoundBoxPoints");

        Point firstPoint = curPoint->getPosition();
        Point leftPoint  = firstPoint;
        Point rightPoint = firstPoint;

        if (curPoint->getLeft()) leftPoint = curPoint->getLeft()->getPosition();

        if (curPoint->getRight()) rightPoint = curPoint->getRight()->getPosition();

        // Define a midpoint
        Point midPoint = leftPoint;

        midPoint += rightPoint;
        midPoint *= 0.5;

        double distanceLeft  = (firstPoint - leftPoint).mag();
        double distanceRight = (firstPoint - rightPoint).mag();

        // Add output
        m_builder->addAttValue("Position",       getPositionString(firstPoint),       "");
        m_builder->addAttValue("Distance Left",  float(distanceLeft),                 "");
        m_builder->addAttValue("Distance Right", float(distanceRight),                "");
        m_builder->addAttValue("AveSeparation",  float(curPoint->getAveSeparation()), "");

        // Now draw the lines
        m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());
        m_builder->addPoint(midPoint.x(),   midPoint.y(),   midPoint.z()  );
        m_builder->addPoint(leftPoint.x(),  leftPoint.y(),  leftPoint.z()  );
        m_builder->addPoint(rightPoint.x(), rightPoint.y(), rightPoint.z()  );

        // Now draw points below
        if (curPoint->getLeft())  drawTkrBoundBoxPoint(curPoint->getLeft());
        if (curPoint->getRight()) drawTkrBoundBoxPoint(curPoint->getRight());
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

    // Creating mapping between layers skipped and the links
    std::map<int, std::vector<Event::TkrVecPointsLink*> > lyrsSkipToLinkMap;

    // Make a pass through all the links to count the various flavors
    for(Event::TkrVecPointsLinkCol::iterator vecLinkItr = vecPointsLinkCol->begin();
        vecLinkItr != vecPointsLinkCol->end();
        vecLinkItr++)
    {
        Event::TkrVecPointsLink* vecLink = *vecLinkItr;

        int layersSkipped = (vecLink->getStatusBits() & Event::TkrVecPointsLink::SKIPSLAYERS) >> 4;

        if (layersSkipped == 0 && vecLink->getStatusBits() & 0x03000000) layersSkipped = -1;

        lyrsSkipToLinkMap[layersSkipped].push_back(vecLink);
    }
        
    m_builder->addAttValue("NumVecLinks",    float(numVecLinks),    "");
    m_builder->addAttValue("HoughLinks",     float(lyrsSkipToLinkMap[-1].size()), "");
    m_builder->addAttValue("NNoSkipLinks",   float(lyrsSkipToLinkMap[0].size()),  "");
    m_builder->addAttValue("NSkip1LyrLinks", float(lyrsSkipToLinkMap[1].size()),  "");
    m_builder->addAttValue("NSkip2LyrLinks", float(lyrsSkipToLinkMap[2].size()),  "");
    m_builder->addAttValue("NSkip3LyrLinks", float(lyrsSkipToLinkMap[3].size()),  "");
    m_builder->addAttValue("NSkip4LyrLinks", float(lyrsSkipToLinkMap[4].size()),  "");

    // If nothing to draw may as well simply return
    if (numVecLinks == 0) return; // || numVecLinks > 25000) return;

//    m_builder->setSubinstancesNumber("TkrVecPointsLinks", numVecLinks);
    m_builder->setSubinstancesNumber("TkrVecPointsLinks", int(lyrsSkipToLinkMap.size()));

    // Now go through the map to draw the links by number of layers they skip
    std::map<int, std::vector<Event::TkrVecPointsLink*> >::iterator lyrToLinksItr;

    // Need a map to deciper the names... (should be done at init!)
    std::map<int, std::string> lyrToNameMap;

    lyrToNameMap[-1] = "TkrHoughLinks";
    lyrToNameMap[0]  = "TkrNoSkipLinks";
    lyrToNameMap[1]  = "TkrSkip1LyrLinks";
    lyrToNameMap[2]  = "TkrSkip2LyrLinks";
    lyrToNameMap[3]  = "TkrSkip3LyrLinks";
    lyrToNameMap[4]  = "TkrSkipNLyrLinks";

    for(lyrToLinksItr = lyrsSkipToLinkMap.begin(); lyrToLinksItr != lyrsSkipToLinkMap.end(); lyrToLinksItr++)
    {
        int layer = lyrToLinksItr->first;

        if (layer < -1 || layer > 4) continue;

        m_builder->addInstance("TkrVecPointsLinks", lyrToNameMap[layer]);

        // If too many total links then draw only the "Hough" links and then quit
        if (layer > -1 && numVecLinks > 25000) continue;

        m_builder->setSubinstancesNumber(lyrToNameMap[layer], int(lyrToLinksItr->second.size()));

        std::vector<Event::TkrVecPointsLink*>::iterator linkItr;

        for(linkItr = lyrToLinksItr->second.begin(); linkItr != lyrToLinksItr->second.end(); linkItr++)
        {
            Event::TkrVecPointsLink* vecLink = *linkItr;

            m_builder->addInstance(lyrToNameMap[layer],"TkrVecPointsLink");
            m_builder->setSubinstancesNumber("TkrVecPointsLink", 2);

            std::vector<Event::TkrTrackElemToLinksRel*> elemToLinksVec = elemsToLinksTab.getRelBySecond(vecLink);
            std::string linkColor = "75,200,255";


            // For right now we change the color of the links which are tagged by hough transform
            if (vecLink->getStatusBits() & 0x01000000) linkColor = "green";
            if (vecLink->getStatusBits() & 0x02000000) linkColor = "red";

            if      (vecLink->skip1Layer())  linkColor = "204,204,204";
            else if (vecLink->skip2Layer())  linkColor = "153,153,153";
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

            drawSingleVectorLink(vecLink, linkColor, true);
        }
    }
/*
    // Loop through all the links in the collection
    for(Event::TkrVecPointsLinkCol::iterator vecLinkItr = vecPointsLinkCol->begin();
        vecLinkItr != vecPointsLinkCol->end();
        vecLinkItr++)
    {
        Event::TkrVecPointsLink* vecLink = *vecLinkItr;

        m_builder->addInstance("TkrVecPointsLinks","TkrVecPointsLink");
        m_builder->setSubinstancesNumber("TkrVecPointsLink", 2);

        std::vector<Event::TkrTrackElemToLinksRel*> elemToLinksVec = elemsToLinksTab.getRelBySecond(vecLink);
//        std::string linkColor = "white";
        std::string linkColor = "75,200,255";

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
        //else if (vecLink->associated()) continue;

        drawSingleVectorLink(vecLink, linkColor);
    }
*/
    return;
}

void TrackElementsFiller::drawSingleVectorLink(const Event::TkrVecPointsLink* vecLink, std::string& linkColor, bool drawExtended)
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

    // Get z position estimated to be in tungsten just above the top of the bilayer
    // What type of bilayer do we have?
    convType lyrType   = m_tgsvc->getLayerType(vecLink->getFirstVecPoint()->getLayer());

    // If no converter then we use the position midway between the sense layers
    double   lyrOffset = -0.5 * fabs(vecLink->getFirstVecPoint()->getXCluster()->position().z()
                       -             vecLink->getFirstVecPoint()->getYCluster()->position().z());

    if      (lyrType == STANDARD) lyrOffset = 0.600;
    else if (lyrType == SUPER   ) lyrOffset = 0.900;

    // Get the slope corrected position at the bottom of this link
    double zTop       = std::max(vecLink->getFirstVecPoint()->getXCluster()->position().z(),
                                 vecLink->getFirstVecPoint()->getYCluster()->position().z())
                      + lyrOffset;

    Point firstPoint = vecLink->getPosition(zTop);
    m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());

    // What type of bilayer do we have?
    lyrType   = m_tgsvc->getLayerType(vecLink->getSecondVecPoint()->getLayer());

    // If no converter then we use the position midway between the sense layers
    lyrOffset = -0.5 * fabs(vecLink->getSecondVecPoint()->getXCluster()->position().z()
              -             vecLink->getSecondVecPoint()->getYCluster()->position().z());

    if      (lyrType == STANDARD) lyrOffset = 0.600;
    else if (lyrType == SUPER   ) lyrOffset = 0.900;

    // Get the slope corrected position at the bottom of this link
    double zBot       = std::max(vecLink->getSecondVecPoint()->getXCluster()->position().z(),
                                 vecLink->getSecondVecPoint()->getYCluster()->position().z())
                      + lyrOffset;

    // Temporary to see intersection points
    if      (drawExtended && vecLink->getStatusBits() & 0x02000000) zBot = -500.;
    else if (drawExtended && vecLink->getStatusBits() & 0x01000000) zBot = -250.;

    Point scndPoint  = vecLink->getPosition(zBot); // vecLink->getBotPosition();
    m_builder->addPoint(scndPoint.x(), scndPoint.y(), scndPoint.z());

    // Add an instance of TkrVecPointsLinkPos
    m_builder->addInstance("TkrVecPointsLink","TkrVecPointsLinkPos");
//    m_builder->addAttValue("Color", "yellow", "");

    // Display the attributes
    m_builder->addAttValue("Position", getPositionString(firstPoint), "");

    // Get size of prism to draw
    float xClusWid = 0.5*vecLink->getFirstVecPoint()->getXCluster()->size()*_dH;
    float yClusWid = 0.5*vecLink->getFirstVecPoint()->getYCluster()->size()*_dH;

    // this is the prism that we pick
    drawPrism(firstPoint.x(), firstPoint.y(), firstPoint.z(), xClusWid, yClusWid, _dH);

    // Add an instance of TkrVecPointsLinkPos
    m_builder->addInstance("TkrVecPointsLink","TkrVecPointsLinkPos");
    m_builder->addAttValue("Color", "75,200,255", "");

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

    // Parent and instance strings
    std::string parent   = "Candidate Tree";
    std::string instance = "Axis";

    // Loop over the collection of head nodes
    for(Event::TkrTreeCol::iterator treeItr = treeCol->begin(); treeItr != treeCol->end(); treeItr++)
    {
        const Event::TkrVecNode* vecNode = (*treeItr)->getHeadNode();

        // Not a real tree
        if (!vecNode) continue;

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

        // Output the information contained in the TkrFilterParams
        drawFilterParams((*treeItr)->getAxisParams(), parent, instance);

        //m_builder->setSubinstancesNumber("Candidate Tree", numVecLinks);

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
        m_builder->addAttValue("Dist to Main",     float(distToMain),                         "");
        m_builder->addAttValue("Num in Best RMS",  float(numAngInRms),                        "");

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
