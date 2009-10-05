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

#include <sstream>
#include <iostream>
#include <algorithm>

namespace {
    const float _dH = 0.25; // size of TkrTrackHit box
    const float _hE = 0.1;  //halfheight of caps on hit "sigmas"
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
    m_builder->addType("TkrRecon","TkrVecPointsLinks", "Vector links","");

    m_builder->addType("TkrVecPointsLinks","TkrVecPointsLink","Vector link collection","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","white","");
    m_builder->addAttDef("Status Low", "Vector Low Status Bits","Physics","");
    m_builder->addAttDef("Status High","Vector High Status Bits","Physics","");
    m_builder->addAttDef("MaxScatAngle","Maximum expected scattering angle","Physics","");
    m_builder->addAttDef("Angle2NextLink","Angle to next link","Physics","");
    m_builder->addAttDef("Start Position","Track start position","Physics","");
    m_builder->addAttDef("Start Direction","Track start direction","Physics","");
}


// This method fill the instance tree Event with the actual TDS content
void TrackElementsFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList,"Recon/TkrRecon/TkrVecPointsLinks/TkrVecPointsLink")) return;

    Event::TkrVecPointsLinkCol* vecPointsLinkCol = 
        SmartDataPtr<Event::TkrVecPointsLinkCol>(m_dpsvc, EventModel::TkrRecon::TkrVecPointsLinkCol);

    //Now see if we can do the drawing
    if (!vecPointsLinkCol) return;

    Event::TkrTrackElemToLinksTabList* elemsToLinksList = 
        SmartDataPtr<Event::TkrTrackElemToLinksTabList>(m_dpsvc, EventModel::TkrRecon::TkrTrackElemsToLinksTab);

    if (!elemsToLinksList) return;

    Event::TkrTrackElemToLinksTab elemsToLinksTab(elemsToLinksList);

    m_builder->addInstance("TkrRecon","TkrVecPointsLinks");

    int numVecLinks = vecPointsLinkCol->size();

    if (numVecLinks == 0) return;

    m_builder->setSubinstancesNumber("TkrVecPointsLinks", numVecLinks);

    // Loop through all the links in the collection
    for(Event::TkrVecPointsLinkCol::iterator vecLinkItr = vecPointsLinkCol->begin();
        vecLinkItr != vecPointsLinkCol->end();
        vecLinkItr++)
    {
        Event::TkrVecPointsLink* vecLink = *vecLinkItr;

        m_builder->addInstance("TkrVecPointsLinks","TkrVecPointsLink");

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

        Point firstPoint = vecLink->getFirstVecPoint()->getPosition();
        m_builder->addPoint(firstPoint.x(), firstPoint.y(), firstPoint.z());

        Point scndPoint  = vecLink->getSecondVecPoint()->getPosition();
        m_builder->addPoint(scndPoint.x(), scndPoint.y(), scndPoint.z());
    }
}
