#include "FilterFiller.h"

#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "idents/VolumeIdentifier.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/TopLevel/Event.h"

//#include "OnboardFilterTds/ObfFilterStatus.h"
#include "OnboardFilterTds/ObfFilterTrack.h"

#include "geometry/Point.h"

#include <algorithm>

// Constructor
FilterFiller::FilterFiller(HepRepInitSvc* hrisvc,
                           IGlastDetSvc* gsvc,
                           IDataProviderSvc* dpsvc):
m_hrisvc(hrisvc),m_gdsvc(gsvc),m_dpsvc(dpsvc)
{    
}


// This method build the types for the HepRep
void FilterFiller::buildTypes()
{
    m_builder->addType("Digi","Filter","Filter Stuff","");
    m_builder->addType("Filter", "FilterTrack", "Onboard Track", "");
    m_builder->addAttDef("Filter Track Dir","OBF track direction","","");
    m_builder->addAttValue("DrawAs", "Line", "");
    //m_builder->addAttValue("Color", "255,218,185",""); // sorta peach
    m_builder->addAttValue("Color", "green",""); // sorta peach
    m_builder->addAttValue("LineStyle","Dashed","");
    m_builder->addAttDef("Filter Track Pos","OBF track position","","");
}

// 
void FilterFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList, "Digi/Filter"))
        return;

    m_builder->addInstance("Digi","Filter");

    SmartDataPtr<OnboardFilterTds::ObfFilterTrack> 
        filterTrack(m_dpsvc, "/Event/Filter/ObfFilterTrack");

    if( filterTrack )
    {
        int xHits    = filterTrack->get_nXhits();
        int yHits    = filterTrack->get_nYhits();
        float xSlope = filterTrack->get_slpXZ();
        float ySlope = filterTrack->get_slpYZ();
        float xInt   = filterTrack->get_xInt();
        float yInt   = filterTrack->get_yInt();
        float zInt   = filterTrack->get_z();

        // Calculate direction cosines for GRB track first
        if (xHits > 0 && yHits > 0)
        {
            double alpha = atan2(ySlope, xSlope);

            if (alpha < 0.) alpha += 2. * M_PI;

            double slope = sqrt(pow(xSlope,2) + pow(ySlope,2));
            double beta  = atan(slope);

            double xDir = cos(alpha)*sin(beta);
            double yDir = sin(alpha)*sin(beta);
            double zDir = cos(beta);
            Vector grbDir = Vector(xDir, yDir, zDir);
            Point  grbPos = Point(xInt, yInt, zInt);
            m_builder->addInstance("Filter", "FilterTrack");
            m_builder->addAttValue("FilterTrack Dir", getDirectionString(grbDir), "");
            Point grbEnd = grbPos - zInt/fabs(zDir)*grbDir;
            m_builder->addAttValue("FilterTrack Pos", getPositionString(grbPos), "");
            m_builder->addPoint(xInt, yInt, zInt);
            m_builder->addPoint(grbEnd.x(), grbEnd.y(), grbEnd.z());
        }
    }
}
