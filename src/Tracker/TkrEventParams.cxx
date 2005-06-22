#include "TkrEventParamsFiller.h"
#include "HepRepSvc/IBuilder.h"

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
TkrEventParamsFiller::TkrEventParamsFiller(IGlastDetSvc* gsvc,
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
    m_builder->addAttValue("Color","blue","");
    m_builder->addAttDef("Status Bits", "Status Bits","Physics","");
    m_builder->addAttDef("Position","Track position","Physics","");
    m_builder->addAttDef("Direction","Track direction","Physics","");
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

            m_builder->addAttValue("Position",  getPositionString(tkrEventParams->getEventPosition()), "");
            m_builder->addAttValue("Direction", getDirectionString(tkrEventParams->getEventAxis()), "");

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


bool TkrEventParamsFiller::hasType(std::vector<std::string>& list, std::string type) 
{
    if (list.size() == 0) return 1;

    std::vector<std::string>::const_iterator i; 

    i = std::find(list.begin(),list.end(),type);
    if(i == list.end()) return 0;
    else return 1;

}

std::string TkrEventParamsFiller::getTripleString(int precis, double x, double y, double z)
{
    std::stringstream triple;
    triple.setf(std::ios::fixed);
    triple.precision(precis);
    triple << " (" << x << "," << y << "," << z << ")";

    return triple.str();
}

std::string TkrEventParamsFiller::getPositionString(const Point& position)
{
    int precis = 3;
    return getTripleString(precis, position.x(), position.y(), position.z());
}

std::string TkrEventParamsFiller::getDirectionString(const Vector& direction)
{
    int precis = 5;
    return getTripleString(precis, direction.x(), direction.y(), direction.z());
}

std::string TkrEventParamsFiller::getBits(unsigned int statBits, int highBit, int lowBit)
{                    
    std::stringstream outString;
    int bit;
    for (bit=highBit; bit>=lowBit; --bit) {
        outString << (statBits>>(bit)&1) ;
        if (bit%4==0) outString << " ";
    }
    return outString.str();
}
