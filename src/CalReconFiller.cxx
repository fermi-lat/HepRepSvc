#include "CalReconFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

#include "Event/TopLevel/EventModel.h"
#include "Event/Recon/CalRecon/CalXtalRecData.h"
#include "Event/Recon/CalRecon/CalCluster.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

#include "idents/VolumeIdentifier.h"

#include <algorithm>

// Constructor
CalReconFiller::CalReconFiller(IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
  m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{
    
    double xtalHeight; 
    int nLayers;
    int eLATTowers;
    int eTowerCAL;
    int eXtal;
    
    double value;
    if(!m_gdsvc->getNumericConstByName(std::string("CALnLayer"), &value))
    {
    } 
    else nLayers = int(value);

    if(!m_gdsvc->getNumericConstByName(std::string("eLATTowers"), &value))
    {
    } 
    else eLATTowers = int(value);
    
    if(!m_gdsvc->getNumericConstByName(std::string("eTowerCAL"), &value))
    {
    } 
    else eTowerCAL = int(value);
    
    if(!m_gdsvc->getNumericConstByName(std::string("eXtal"), &value))
    {
    } 
    else eXtal = int(value);
    
    if(!m_gdsvc->getNumericConstByName(std::string("CsIHeight"),&xtalHeight)) 
    {
    } 
    
    
    int layer=0;
    idents::VolumeIdentifier topLayerId;
    topLayerId.append(eLATTowers);
    topLayerId.append(0);
    topLayerId.append(0);
    topLayerId.append(eTowerCAL);
    topLayerId.append(layer);
    topLayerId.append(layer%2);
    topLayerId.append(0);
    topLayerId.append(eXtal);
    topLayerId.append(0);
    
    HepTransform3D transfTop;
    m_gdsvc->getTransform3DByID(topLayerId,&transfTop);
    Hep3Vector vecTop = transfTop.getTranslation();
    
    layer=nLayers-1;
    idents::VolumeIdentifier bottomLayerId;
    bottomLayerId.append(eLATTowers);
    bottomLayerId.append(0);
    bottomLayerId.append(0);
    bottomLayerId.append(eTowerCAL);
    bottomLayerId.append(layer);
    bottomLayerId.append(layer%2);
    bottomLayerId.append(0);
    bottomLayerId.append(eXtal);
    bottomLayerId.append(0);
    
    HepTransform3D transfBottom;
    m_gdsvc->getTransform3DByID(bottomLayerId,&transfBottom);
    Hep3Vector vecBottom = transfBottom.getTranslation();
    
    
    m_calZtop = vecTop.z();
    m_calZbottom = vecBottom.z();
    m_xtalHeight = xtalHeight;
    
}


// This method build the types for the HepRep
void CalReconFiller::buildTypes()
{
  m_builder->addType("Recon","CalRecon","CalRecon Tree","");

  m_builder->addType("CalRecon","XtalCol","Crystal Collection","");
  m_builder->addType("XtalCol", "Xtal", "Crystal reconstruction", "");
  m_builder->addAttDef("E","Energy reconstructed","Physics","MeV");
  m_builder->addAttValue("DrawAs","Prism","");
  m_builder->addAttValue("Color","red","");

  m_builder->addType("CalRecon","CalClusterCol","Cal Cluster Collection","");
  m_builder->addType("CalClusterCol", "CalCluster", "Cal Cluster", "");
  m_builder->addAttDef("E","Cluster Energy","Physics","MeV");
}


// This method fill the instance tree Event/MC with the actual TDS content
void CalReconFiller::fillInstances (std::vector<std::string>& typesList)
{
  m_builder->addInstance("Recon","CalRecon");

  if (hasType(typesList,"XtalCol"))
    {      
      const HepPoint3D p0(0.,0.,0.);  
      
      // get the pointer to CalXtalRecCol object in TDS
      Event::CalXtalRecCol* cxrc = SmartDataPtr<Event::CalXtalRecCol>(m_dpsvc,
                                                        EventModel::CalRecon::CalXtalRecCol); 
      

      // if pointer is not zero - draw the reconstructed xtal data
      if(cxrc){        
        
        m_builder->addInstance("CalRecon","XtalCol");    
        // drawing red box for each log with a size
        // proportional to energy deposition
    
        double emax = 0.; // reset maximum energy per crystal
    
        // to find maximum energy per crystal
        for (Event::CalXtalRecCol::const_iterator it = cxrc->begin();
             it != cxrc->end(); it++){
      
          // get poiner to the reconstructed data for individual crystal
          Event::CalXtalRecData* recData = *it;
          
          // get reconstructed energy in the crystal
          double eneXtal = recData->getEnergy();
          
          // if energy is bigger than current maximum - update the maximum 
          if(eneXtal>emax)emax=eneXtal;
        }
        
        
        // if maximum crystal energy isn't zero - start drawing 
        if(emax>0){
          // loop over all crystals in reconstructed collection
          // to draw red boxes
          for (Event::CalXtalRecCol::const_iterator it = cxrc->begin();
               it != cxrc->end(); it++){
        
            // get poiner to the reconstructed data for individual crystal
            Event::CalXtalRecData* recData = *it;
        
            // get reconstructed energy in the crystal
            double eneXtal = recData->getEnergy();
        
        
            // draw only crystals containing more than 1% of maximum energy
            if(eneXtal>0.01*emax){
              m_builder->addInstance("XtalCol", "Xtal");    
              m_builder->addAttValue("E", (float)eneXtal, "");

              // get the vector of reconstructed position
              HepVector3D pXtal = recData->getPosition() - p0;
              
              // get reconstructed coordinates
              double x = pXtal.x();
              double y = pXtal.y();
              double z = pXtal.z();
          
          
              // calculate the half size of the box, 
              // taking the 90% of crystal half height
              // as the size corresponding to the maximum energy
              double s = 0.45*m_xtalHeight*eneXtal/emax;
              
              std::cout << s << std::endl;

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
    }

}

bool CalReconFiller::hasType(std::vector<std::string>& list, std::string type) 
{
  if (list.size() == 0) return 1;

  std::vector<std::string>::const_iterator i; 

  i = std::find(list.begin(),list.end(),type);
  if(i == list.end()) return 0;
  else return 1;

}

