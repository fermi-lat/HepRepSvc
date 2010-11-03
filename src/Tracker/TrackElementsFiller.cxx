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
#include "Event/Recon/TkrRecon/TkrVecNodes.h"

#include <sstream>
#include <iostream>
#include <algorithm>

namespace {
    const float _dH = 0.25; // size of TkrVecPoint hit
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
    m_builder->addType("TkrRecon","TkrVecPoints", "Vector Points","");
    m_builder->addAttDef("NumVecPoints", "Number of Vector Points", "Physics", "");

    m_builder->addType("TkrVecPoints","TkrVecPoint","Vector Point collection","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","red","");
    m_builder->addAttDef("BiLayer", "Bi-layer of point", "Physics", "");
    m_builder->addAttDef("Position", "Position", "Physics", "");

    m_builder->addType("TkrRecon","TkrVecPointsLinks", "Vector links","");
    m_builder->addAttDef("NumVecLinks", "Number of Vector Links", "Physics", "");

    m_builder->addType("TkrVecPointsLinks","TkrVecPointsLink","Vector link collection","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("Status Low", "Vector Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","Vector High Status Bits","Physics","");
    m_builder->addAttDef("MaxScatAngle","Maximum expected scattering angle","Physics","");
    m_builder->addAttDef("Angle2NextLink","Angle to next link","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");
    m_builder->addType("TkrVecPointsLink","TkrVecPointsLinkPos","TkrVecPointsLink Position","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("BiLayer", "Bi-layer of point", "Physics", "");
    m_builder->addAttDef("Position", "Position", "Physics", "");

    m_builder->addType("TkrRecon", "Candidate Trees", "Track Candidate Trees", "");

    m_builder->addType("Candidate Trees", "Candidate Tree", "Track Candidate Tree", "");
    m_builder->addAttDef("Depth", "Depth of Candidate Tree", "Physics", "");
    m_builder->addAttDef("Leaves", "Number of Leaves in Tree", "Physics", "");
    m_builder->addAttDef("Links", "Number of links in Tree", "Physics", "");
    m_builder->addAttDef("Number Daughters", "Number daughters", "Physics", "");
    m_builder->addAttDef("Best RMS angle", "RMS deviations of best branch", "Physics", "");
    m_builder->addAttDef("Best Bi Layers", "Number bi layers of best branch", "physics", "");

    m_builder->addType("Candidate Tree", "Tree Links", "Vector Links in Tree", "");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("Nodes below", "Depth of node tree", "Physics", "");
    m_builder->addAttDef("Leaves below", "Number leaves below this node", "Physics", "");
    m_builder->addAttDef("Links below", "Number links below this node", "Physics", "");
    m_builder->addAttDef("RMS angle", "RMS deviations to this node", "Physics", "");
    m_builder->addAttDef("Number Daughters", "Number daughters", "Physics", "");
    m_builder->addAttDef("Status Low", "Vector Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","Vector High Status Bits","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");
    m_builder->addAttDef("Best RMS angle", "RMS deviations of best branch", "Physics", "");
    m_builder->addAttDef("Best Bi Layers", "Number bi layers of best branch", "physics", "");
}


// This method fill the instance tree Event with the actual TDS content
void TrackElementsFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (hasType(typesList,"Recon/TkrRecon/TkrVecPoints"))    ///TkrVecPointsLink"))
    {
        drawVectorPoints();
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

    // Loop through all the links in the collection
    for(Event::TkrVecPointCol::iterator vecPointItr = vecPointsCol->begin();
        vecPointItr != vecPointsCol->end();
        vecPointItr++)
    {
        Event::TkrVecPoint* vecPoint = *vecPointItr;

        // Get the two attributes: position and layer
        Point position = vecPoint->getPosition();
        int   layer    = vecPoint->getLayer();

        // Add an instance of TkrVecPoint
        m_builder->addInstance("TkrVecPoints","TkrVecPoint");

        // Display the attributes
        m_builder->addAttValue("BiLayer",  (float)(layer),              "");
        m_builder->addAttValue("Position", getPositionString(position), "");

        // this is the prism that we pick
        drawPrism(position.x(), position.y(), position.z(), _dH, _dH, _dH);
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

        //m_builder->addAttValue("LineWidth",          (float)trackWid, "");
        m_builder->addAttValue("Color", linkColor, "");
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

        // this is the prism that we pick
        drawPrism(firstPoint.x(), firstPoint.y(), firstPoint.z(), _dH, _dH, _dH);

        // Add an instance of TkrVecPointsLinkPos
        m_builder->addInstance("TkrVecPointsLink","TkrVecPointsLinkPos");

        // Display the attributes
        m_builder->addAttValue("Position", getPositionString(scndPoint), "");

        // this is the prism that we pick
        drawPrism(scndPoint.x(), scndPoint.y(), scndPoint.z(), _dH, _dH, _dH);

    }

    if (countEm != numVecLinks)
    {
        int canthappenhere = 0;
    }

    return;
}

void TrackElementsFiller::drawNodeTrees()
{
    // Retrieve pointer to the vector of head nodes in the TDS
    Event::TkrVecNodeCol* vecNodeCol = SmartDataPtr<Event::TkrVecNodeCol>(m_dpsvc, "/Event/TkrRecon/TkrVecNodeCol");

    //Now see if we can do the drawing
    if (!vecNodeCol) return;

    m_builder->addInstance("TkrRecon","Candidate Trees");

    // Set the number of nodes so display will know
    int numNodes = vecNodeCol->size();

    if (numNodes == 0) return;

    m_builder->setSubinstancesNumber("Candidate Trees", numNodes);

    // Set the color and width for the first tree
    std::string lineColor = "red";
    int         lineWidth = 3;

    // Loop over the collection of head nodes
    for(Event::TkrVecNodeCol::iterator nodeItr = vecNodeCol->begin(); nodeItr != vecNodeCol->end(); nodeItr++)
    {
        Event::TkrVecNode* vecNode = *nodeItr;

        // Recover the details for this tree
        int   treeDepth     = vecNode->getDepth();
        int   numLeaves     = vecNode->getNumLeaves();
        int   numVecLinks   = vecNode->getNumNodesInTree();
        int   bestNumLayers = vecNode->getBestNumBiLayers();
        float bestRmsAngle  = vecNode->getBestRmsAngle();
        int   numDaughters  = vecNode->size();

        // Add the Candidate Tree instance
        m_builder->addInstance("Candidate Trees", "Candidate Tree");
        m_builder->addAttValue("Depth",            (float)treeDepth,     "");
        m_builder->addAttValue("Leaves",           (float)numLeaves,     "");
        m_builder->addAttValue("Links",            (float)numVecLinks,   "");
        m_builder->addAttValue("Number Daughters", (float)numDaughters,  "");
        m_builder->addAttValue("Best RMS angle",   bestRmsAngle,         "");
        m_builder->addAttValue("Best Bi Layers",   (float)bestNumLayers, "");

        m_builder->setSubinstancesNumber("Candidate Tree", numVecLinks);

        // Now draw the node (which will draw the daughter nodes)
        drawNode(vecNode, lineColor, lineWidth);

        lineColor = "yellow";
        lineWidth = 2;
    }

    return;
}

void TrackElementsFiller::drawNode(const Event::TkrVecNode* vecNode, std::string lineColor, int lineWidth)
{
    // Recover the TkrVecPointsLink associated to this node
    const Event::TkrVecPointsLink* vecLink = vecNode->getAssociatedLink();

    // Note that the top node will not have an associated link as it is the anchor node
    if (vecLink)
    {
        m_builder->addInstance("Candidate Tree","Tree Links");
        
        // Recover the details for this tree
        int   treeDepth     = vecNode->getDepth();
        int   numLeaves     = vecNode->getNumLeaves();
        int   numVecLinks   = vecNode->getNumNodesInTree();
        int   numDaughters  = vecNode->size();
        float rmsAngle      = vecNode->getRmsAngle();
        int   bestNumLayers = vecNode->getBestNumBiLayers();
        float bestRmsAngle  = vecNode->getBestRmsAngle();

        m_builder->addAttValue("LineWidth",        (float)lineWidth,                          "");
        m_builder->addAttValue("Color",            lineColor,                                 "");
        m_builder->addAttValue("Nodes below",      (float)treeDepth,                          "");
        m_builder->addAttValue("Leaves below",     (float)numLeaves,                          "");
        m_builder->addAttValue("Links below",      (float)numVecLinks,                        "");
        m_builder->addAttValue("Number Daughters", (float)numDaughters,                       "");
        m_builder->addAttValue("RMS angle",        rmsAngle,                                  "");
        m_builder->addAttValue("Start Position",   getPositionString(vecLink->getPosition()), "");
        m_builder->addAttValue("Start Direction",  getDirectionString(vecLink->getVector()),  "");
        m_builder->addAttValue("Best RMS angle",   bestRmsAngle,         "");
        m_builder->addAttValue("Best Bi Layers",   (float)bestNumLayers, "");

        //Build strings for status bits
        unsigned int statusBits = vecLink->getStatusBits();

        m_builder->addAttValue("Status Low",  getBits(statusBits, 15, 0),  "");
        m_builder->addAttValue("Status High", getBits(statusBits, 31, 16), "");

        //Point firstPoint = vecLink->getFirstVecPoint()->getPosition();
        Point firstPoint = vecLink->getPosition();
        m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());

        //Point scndPoint  = vecLink->getSecondVecPoint()->getPosition();
        Point scndPoint  = vecLink->getBotPosition();
        m_builder->addPoint(scndPoint.x(), scndPoint.y(), scndPoint.z());
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
