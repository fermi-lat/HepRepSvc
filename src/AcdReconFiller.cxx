#include "AcdReconFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/AcdRecon/AcdRecon.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>

// Constructor
AcdReconFiller::AcdReconFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{    
}


// This method build the types for the HepRep
void AcdReconFiller::buildTypes()
{
  m_builder->addType("Recon","AcdRecon","AcdRecon Tree","");

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
  
  if (hasType(typesList,"Recon/AcdRecon/DocaCol/DocaTile"))
    {      

      SmartDataPtr<Event::AcdRecon> acdRec(m_dpsvc, EventModel::AcdRecon::Event);
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
              
          m_builder->addPoint(x+s,y+s,z+s);
          m_builder->addPoint(x-s,y+s,z+s);
          m_builder->addPoint(x-s,y-s,z+s);
          m_builder->addPoint(x+s,y-s,z+s);
          m_builder->addPoint(x+s,y+s,z-s);
          m_builder->addPoint(x-s,y+s,z-s);
          m_builder->addPoint(x-s,y-s,z-s);
          m_builder->addPoint(x+s,y-s,z-s);
          
        }
      }      
    }
}
      

bool AcdReconFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}

