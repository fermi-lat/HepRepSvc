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
#include "Event/Recon/CalRecon/CalRecon.h"


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
  m_builder->addAttValue("DrawAs","Point","");
  m_builder->addAttValue("Color","green","");
  m_builder->addType("CalCluster", "CalClusterLayers", "Cal Cluster Layers center","");
  m_builder->addAttValue("DrawAs","Point","");
  m_builder->addAttValue("Color","blue","");
  m_builder->addAttValue("MarkerName","Cross","");  
  m_builder->addType("CalCluster", "CalClusterDir", "Cal Cluster Direction","");
  m_builder->addAttValue("DrawAs","Line","");
  m_builder->addAttValue("Color","green","");
}


// This method fill the instance tree Event/MC with the actual TDS content
void CalReconFiller::fillInstances (std::vector<std::string>& typesList)
{
  if (!hasType(typesList, "Recon/CalRecon"))
    return;

  m_builder->addInstance("Recon","CalRecon");

  if (hasType(typesList,"Recon/CalRecon/XtalCol"))
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
            if((eneXtal>0.01*emax) && (hasType(typesList, "Recon/CalRecon/XtalCol/Xtal"))){
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

  if (hasType(typesList,"Recon/CalRecon/CalClusterCol"))
  {      
    m_builder->addInstance("CalRecon","CalClusterCol");    

    // drawing the cross in the average position for each layer 

    //  get pointer to the cluster reconstructed collection
    Event::CalClusterCol* cls = SmartDataPtr<Event::CalClusterCol>(m_dpsvc,
        EventModel::CalRecon::CalClusterCol);


    // if pointer is not zero, start drawing
    if(cls){
      m_builder->addInstance("CalClusterCol", "CalCluster");    

      // get pointer to the cluster 0 - the only one exiting now
      Event::CalCluster* cl = cls->getCluster(0); 

      // get total energy in the calorimeter
      double energy_sum = cl->getEnergySum();

      // get vector of layer energies
      const std::vector<double>& eneLayer = cl->getEneLayer();

      // get layer positions
      const std::vector<Vector>& posLayer = cl->getPosLayer();

      // draw only if there is some energy in the calorimeter        
      if(energy_sum > 0){

        // Draw the cluster center
        double x = (cl->getPosition()).x();
        double y = (cl->getPosition()).y();
        double z = (cl->getPosition()).z();
        m_builder->addPoint(x,y,z);            
        
        // Draw the layers reconstructed positions
        m_builder->addInstance("CalCluster", "CalClusterLayers");    
        // loop over calorimeter layers
        for( int l=0;l<8;l++){

          // if energy in this layer is not zero - draw blue cross at
          // the average reconstructed position for this layer
          if (eneLayer[l]>0){            
            double x=(posLayer[l]).x();
            double y=(posLayer[l]).y();
            double z=(posLayer[l]).z();
            m_builder->addPoint(x,y,z);            
          }
        }

        // drawing the reconstructed shower direction
        // as a green line
        double dirX = (cl->getDirection()).x();
        double dirY = (cl->getDirection()).y();
        double dirZ = (cl->getDirection()).z();

        // non display for non-physical or horizontal direction
        if(dirZ >= -1. && dirZ != 0.){
          // Draw the cluster direction
          m_builder->addInstance("CalCluster", "CalClusterDir");    


          // calculate x and y coordinates for the beginning and the end
          // of line in the top and bottom calorimeter layers
          double xTop = x+dirX*(m_calZtop-z)/dirZ;
          double yTop = y+dirY*(m_calZtop-z)/dirZ;
          double xBottom = x+dirX*(m_calZbottom-z)/dirZ;
          double yBottom = y+dirY*(m_calZbottom-z)/dirZ;

          m_builder->addPoint(xTop,yTop,m_calZtop);            
          m_builder->addPoint(xBottom,yBottom,m_calZbottom);                            
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

