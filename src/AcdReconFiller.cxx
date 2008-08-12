#include "AcdReconFiller.h"
#include "HepRepSvc/IBuilder.h"
#include "HepRepSvc/HepRepInitSvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/AcdRecon/AcdRecon.h"
#include "Event/Digi/AcdDigi.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "idents/VolumeIdentifier.h"


#include <algorithm>

// Constructor
AcdReconFiller::AcdReconFiller(HepRepInitSvc* hrisvc,
                               IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
m_hrisvc(hrisvc),m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{    
}


// This method build the types for the HepRep
void AcdReconFiller::buildTypes()
{
    m_builder->addType("Recon","AcdRecon","AcdRecon Tree","");

    m_builder->addType("AcdRecon", "HitCol", "Hit Collection", "");
    m_builder->addType("HitCol", "AcdHit","","");
    m_builder->addAttDef("Id","ID of the ACD Detector","","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addType("AcdHit", "PMT_A","","");
    m_builder->addType("AcdHit", "PMT_B", "", "");

    m_builder->addType("AcdRecon","DocaCol","Doca Collection","");  
    m_builder->addType("DocaCol","DocaTile","","");
    m_builder->addAttDef("Doca","Doca of the tile","Physics","");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","red","");
}


// This method fill the instance tree Event/MC with the actual TDS content
void AcdReconFiller::fillInstances (std::vector<std::string>& typesList)
{
    if (!hasType(typesList, "Recon/AcdRecon"))
        return;

    m_builder->addInstance("Recon","AcdRecon");

    SmartDataPtr<Event::AcdRecon> acdRec(m_dpsvc, EventModel::AcdRecon::Event);

    if (hasType(typesList, "Recon/AcdRecon/HitCol/AcdHit"))
    {

        SmartDataPtr<Event::AcdDigiCol> acdDigiCol(m_dpsvc, EventModel::Digi::AcdDigiCol);
        if (acdDigiCol && acdDigiCol->size()>0) {
            m_builder->addInstance("AcdRecon","HitCol");    
            m_builder->setSubinstancesNumber("HitCol", acdDigiCol->size());
            Event::AcdDigiCol::const_iterator acdDigiIt;
            for (acdDigiIt = acdDigiCol->begin(); acdDigiIt != acdDigiCol->end(); acdDigiIt++) {
                m_builder->addInstance("HitCol","AcdHit");  
                idents::AcdId id = (*acdDigiIt)->getId();
                m_builder->addAttValue("Id", (float)id.id(), "");

                // Redraw the ACD detector in color depending upon level of threshold
                if (id.tile() && !id.na()) {
                    if ( (*acdDigiIt)->getCno(Event::AcdDigi::A) || 
                        (*acdDigiIt)->getCno(Event::AcdDigi::B) )
                        m_builder->addAttValue("Color","orange","");
                    else if ((*acdDigiIt)->getHitMapBit(Event::AcdDigi::A) || 
                        (*acdDigiIt)->getHitMapBit(Event::AcdDigi::B))
                        m_builder->addAttValue("Color","red","");
                    else if ((*acdDigiIt)->getAcceptMapBit(Event::AcdDigi::A) || 
                        (*acdDigiIt)->getAcceptMapBit(Event::AcdDigi::B))
                        m_builder->addAttValue("Color","160,32,240",""); // this is *really* purple!
                    if(((*acdDigiIt)->getHitMapBit(Event::AcdDigi::A) &&
                        !(*acdDigiIt)->getAcceptMapBit(Event::AcdDigi::A)) ||
                        ((*acdDigiIt)->getHitMapBit(Event::AcdDigi::B) &&
                        !(*acdDigiIt)->getAcceptMapBit(Event::AcdDigi::B))) {
                            m_builder->addAttValue("LineStyle", "Dashed","");
                            m_builder->addAttValue("LineWidth", "3.0","");
                        }

                    HepGeom::Transform3D global;
                    idents::VolumeIdentifier volid = id.volId();
                    m_gdsvc->getTransform3DByID(volid, &global);

                    std::string shape;
                    // getShapeById returns the params by push_back()
                    // so the vector needs to be empty to start with!
                    std::vector<double> params(0);

                    m_gdsvc->getShapeByID(volid, &shape, &params); 

                    double dx = params[0]*0.5;
                    double dy = params[1]*0.5;
                    double dz = params[2]*0.5;

                    drawTransformedPrism(dx, dy, dz, global);

                 }

                // Fill in attributes of the two PMTs
                m_builder->addInstance("AcdHit", "PMT_A");
                Event::AcdDigi digi = **acdDigiIt;
                m_builder->addAttValue("PHA_A",(*acdDigiIt)->getPulseHeight(Event::AcdDigi::A),"");
                m_builder->addAttValue("Range",(*acdDigiIt)->getRange(Event::AcdDigi::A),"");

                if ((*acdDigiIt)->getAcceptMapBit(Event::AcdDigi::A) == true)  
                    m_builder->addAttValue("AcceptBit","true","");
                else
                    m_builder->addAttValue("AcceptBit","false","");

                if ((*acdDigiIt)->getHitMapBit(Event::AcdDigi::A) == true) 
                    m_builder->addAttValue("HitMapBit","true","");
                else 
                    m_builder->addAttValue("HitMapBit","false","");

                if ((*acdDigiIt)->getCno(Event::AcdDigi::A) == true) 
                    m_builder->addAttValue("CNO","true","");
                else
                    m_builder->addAttValue("CNO","false","");

                m_builder->addInstance("AcdHit", "PMT_B");
                m_builder->addAttValue("PHA_B",(*acdDigiIt)->getPulseHeight(Event::AcdDigi::B),"");
                m_builder->addAttValue("Range",(*acdDigiIt)->getRange(Event::AcdDigi::A),"");
                if ((*acdDigiIt)->getAcceptMapBit(Event::AcdDigi::B) == true) 
                    m_builder->addAttValue("AcceptBit","true","");
                else
                    m_builder->addAttValue("AcceptBit","false","");

                if ((*acdDigiIt)->getHitMapBit(Event::AcdDigi::B) == true) 
                    m_builder->addAttValue("HitMapBit","true","");
                else 
                    m_builder->addAttValue("HitMapBit","false","");

                if ((*acdDigiIt)->getCno(Event::AcdDigi::B) == true) 
                    m_builder->addAttValue("CNO","true","");
                else
                    m_builder->addAttValue("CNO","false","");

            }
        }
    }


    if (hasType(typesList,"Recon/AcdRecon/DocaCol/DocaTile"))
    {      
        if (acdRec)
        {        
            if( acdRec->getDoca()<=1000. )
            {
                m_builder->addInstance("AcdRecon","DocaCol");    
                m_builder->addInstance("DocaCol","DocaTile");    
                m_builder->addAttValue("Doca", (float)acdRec->getDoca(), "");

                // get the VolumeIdentifier of the closest doca
                idents::AcdId id = acdRec->getMinDocaId();
                const idents::VolumeIdentifier volid = id.volId();

                HepPoint3D tilecenter;
                HepTransform3D transform;

                m_gdsvc->getTransform3DByID(volid, &transform);
                tilecenter = transform*tilecenter;

                // represent the doca as a box of side 10
                double x = tilecenter.x();
                double y = tilecenter.y();
                double z = tilecenter.z();
                double s = 10;

                drawPrism(x, y, z, s, s, s);
            }
        }      
    }

}
