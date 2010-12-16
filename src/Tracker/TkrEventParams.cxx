#include "TkrEventParamsFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/TkrRecon/TkrEventParams.h"

#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include <sstream>
#include <iostream>
#include <algorithm>

// Constructor
TkrEventParamsFiller::TkrEventParamsFiller(
    HepRepInitSvc* /*hrisvc*/,
    IGlastDetSvc* gsvc,
    IDataProviderSvc* dpsvc,
    IParticlePropertySvc* ppsvc):
m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{


}


// This method build the types for the HepRep
void TkrEventParamsFiller::buildTypes()
{
    m_builder->addType("TkrRecon","TkrEventParams", "Tracker Event Parameters","");
    //m_builder->addType("TkrEventParams","TkrEventParams","Tracker Event Parameters","");
    m_builder->addAttValue("DrawAs","Line","");
//    m_builder->addAttValue("Color","0.98,0.50,0.45",""); // "Salmon"
    m_builder->addAttValue("Color","0.527,0.805,0.977",""); // "Salmon"
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
}


// This method fill the instance tree Event with the actual TDS content
void TkrEventParamsFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (hasType(typesList,"Recon/TkrRecon/TkrEventParams"))
    { 
        Event::TkrEventParams* tkrEventParams = 
                    SmartDataPtr<Event::TkrEventParams>(m_dpsvc, EventModel::TkrRecon::TkrEventParams);

        //Now see if we can do the drawing
        if (tkrEventParams)
        {
            m_builder->addInstance("TkrRecon","TkrEventParams");
            
            double lineWidth = 2;
            
            m_builder->addAttValue("LineWidth", (float)lineWidth, "");

            m_builder->addAttValue("Position",       getPositionString(tkrEventParams->getEventPosition()), "");
            m_builder->addAttValue("Direction",      getDirectionString(tkrEventParams->getEventAxis()), "");
            m_builder->addAttValue("Energy",         (float)tkrEventParams->getEventEnergy(), "");
            m_builder->addAttValue("NumBiLayerHits", tkrEventParams->getNumBiLayers(), "");
            m_builder->addAttValue("NumIterations",  tkrEventParams->getNumIterations(), "");
            m_builder->addAttValue("NumHitsTotal",   tkrEventParams->getNumHitsTotal(), "");
            m_builder->addAttValue("NumDropped",     tkrEventParams->getNumDropped(), "");
            m_builder->addAttValue("ChiSquare",      (float)tkrEventParams->getChiSquare(), "");
            m_builder->addAttValue("TransRms",       (float)tkrEventParams->getTransRms(), "");
            m_builder->addAttValue("LongRmsAve",     (float)tkrEventParams->getLongRmsAve(), "");

            //Build strings for status bits
            unsigned int statBits = tkrEventParams->getStatusBits();
            m_builder->addAttValue("Status Bits",getBits(statBits, 15, 0),"");

            // Set up to draw the track
            Point  position = tkrEventParams->getEventPosition();
            Vector axis     = tkrEventParams->getEventAxis();

            Point  start    = position - 500. * axis;
            Point  stop     = position + 500. * axis;

            m_builder->addPoint(start.x(), start.y(), start.z());
            m_builder->addPoint(stop.x(),  stop.y(),  stop.z());
        }
    }
}

