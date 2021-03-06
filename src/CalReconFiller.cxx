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
#include "Event/Recon/CalRecon/CalClusterMap.h"
#include "Event/RelTable/RelTable.h"
#include "Event/Recon/CalRecon/CalMipClasses.h" 
#include "Event/Recon/CalRecon/CalRecon.h"

#include "idents/VolumeIdentifier.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

#include "CLHEP/Vector/LorentzVector.h"

// TU: Hacks for CLHEP 1.9.2.2 and beyond
#ifndef HepVector3D
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

#include "idents/VolumeIdentifier.h"

#include "HepRepSvc/HepRepInitSvc.h"

#include <algorithm>

// Constructor
CalReconFiller::CalReconFiller(HepRepInitSvc* hrisvc,
                               IGlastDetSvc* gsvc,
                               IDataProviderSvc* dpsvc,
                               IParticlePropertySvc* ppsvc):
m_hrisvc(hrisvc),m_gdsvc(gsvc),m_dpsvc(dpsvc),m_ppsvc(ppsvc)
{

    double xtalHeight;
    double xtalWidth;
    double xtalLength;
    //int nLayers;
    int eLATTowers;
    int eTowerCAL;
    int eXtal;

    m_gdsvc->getNumericConstByName(std::string("CALnLayer"), &m_nLayers);

    m_gdsvc->getNumericConstByName(std::string("CsIHeight"),&xtalHeight); 
    m_gdsvc->getNumericConstByName(std::string("CsIWidth"),&xtalWidth); 
    m_gdsvc->getNumericConstByName(std::string("CsILength"),&xtalLength); 
    m_gdsvc->getNumericConstByName(std::string("xNum"),&m_xNum); 
    m_gdsvc->getNumericConstByName(std::string("yNum"),&m_yNum); 
    m_gdsvc->getNumericConstByName(std::string("eTowerCAL"),&m_eTowerCAL);
    eTowerCAL = m_eTowerCAL;
    m_gdsvc->getNumericConstByName(std::string("eLATTowers"),&m_eLATTowers);
    eLATTowers = m_eLATTowers;
    m_gdsvc->getNumericConstByName(std::string("eXtal"),&m_eXtal); 
    eXtal = m_eXtal;
    m_gdsvc->getNumericConstByName(std::string("nCsISeg"),&m_nCsISeg); 

    /*
    double value;
    if(!m_gdsvc->getNumericConstByName(std::string("CALnLayer"), &value)){} 
    else nLayers = int(value);
    if(!m_gdsvc->getNumericConstByName(std::string("eLATTowers"), &value)){} 
    else eLATTowers = int(value);
    if(!m_gdsvc->getNumericConstByName(std::string("eTowerCAL"), &value)){} 
    else eTowerCAL = int(value);
    if(!m_gdsvc->getNumericConstByName(std::string("eXtal"), &value)){} 
    else eXtal = int(value);
    if(!m_gdsvc->getNumericConstByName(std::string("CsIHeight"),&xtalHeight)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("CsIWidth"),&xtalWidth)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("CsILength"),&xtalLength)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("xNum"),&m_xNum)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("yNum"),&m_yNum)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("eTowerCAL"),&m_eTowerCAL)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("eLATTowers"),&m_eLATTowers)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("eXtal"),&m_eXtal)) {} 
    if(!m_gdsvc->getNumericConstByName(std::string("nCsISeg"),&m_nCsISeg)) (} 
    */

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

    HepGeom::Transform3D transfTop;
    m_gdsvc->getTransform3DByID(topLayerId,&transfTop);
    CLHEP::Hep3Vector vecTop = transfTop.getTranslation();

    layer=m_nLayers-1;
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
    CLHEP::Hep3Vector vecBottom = transfBottom.getTranslation();


    m_calZtop        = vecTop.z();
    m_calZbottom     = vecBottom.z();
    m_xtalHalfHeight = 0.5 * xtalHeight;
    m_xtalHalfWidth  = 0.5 * xtalWidth;
    m_xtalHalfLength = 0.5 * xtalLength;

    // Set up the color array
//    static std::string colorArray[] = {"white", "255,100,27", "yellow", "green", "red", "160,32,240"};
    static std::string colorArray[] = {"0,0,128", "0,0,255", "65,105,225", "106,90,205", "160,32,240", "148,0,211", "255,153,255"};

    m_maxColors  = 6;
    m_colorIndex = 0;
    m_colorArray = colorArray;

    m_minEnergy  = 10.;
}


// This method build the types for the HepRep
void CalReconFiller::buildTypes()
{
    m_builder->addType("Recon","CalRecon","CalRecon Tree","");

    m_builder->addType("CalRecon","ClusterCol","Cal Cluster Collection","");
    m_builder->addType("ClusterCol", "Cluster", "Cal Cluster", "");
    m_builder->addAttDef("E","Cluster Energy","Physics","MeV");
    m_builder->addAttDef("rmsTrans","Transverse Moment","Physics","MeV");
    m_builder->addAttDef("rmsLong","Sum Longitudinal Moment","Physics","MeV");
    m_builder->addAttDef("LongAsym","Long Moment Asymmetry","Physics","MeV");
    //m_builder->addAttDef("numTotXtal","Num Xtals","Physics","MeV");
    m_builder->addAttDef("numTruncXtal","Num Truncated Xtals","Physics","MeV");
    m_builder->addAttDef("Centroid","Cluster Centroid","Physics","");
    m_builder->addAttDef("Axis","Cluster Axis","Physics","");
    m_builder->addAttValue("DrawAs","Point","");
    m_builder->addAttValue("Color","green","");
    m_builder->addType("Cluster", "ClusterLayers", "Cal Cluster Layers center","");
    m_builder->addAttValue("DrawAs","Point","");
    m_builder->addAttValue("Color","blue","");// default color for standard display
    m_builder->addAttValue("MarkerName","Cross","");  
    m_builder->addAttValue("MarkerSize", 2, "");
    m_builder->addType("Cluster", "ClusterFit", "Cal Cluster Fit Direction","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","blue","");
    m_builder->addAttValue("LineWidth", 2,"");
    m_builder->addAttDef("Centroid","Fit Centroid","Physics","");
    m_builder->addAttDef("Axis","Fit Axis","Physics","");
    m_builder->addAttDef("nFitLayers","# Fit layers","Physics","MeV");
    m_builder->addAttDef("chiSquare","chi-square of fit","Physics","MeV");
    m_builder->addType("Cluster", "ClusterDir", "Cal Cluster Moments Direction","");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color","green","");
    m_builder->addAttDef("Centroid","Moments Centroid","Physics","");
    m_builder->addAttDef("Axis","Moments Axis","Physics","");

    // Add XtalCol as subset of Cluster to associate xtals in a cluster
    m_builder->addType("Cluster","XtalCol","Crystal Collection","");
    m_builder->addAttDef("#Xtals", "number of Xtals fired","Physics","");
    m_builder->addAttDef("MaxEnergy", "highest energy in any xtal", "Physics", "");
    m_builder->addType("XtalCol", "Xtal", "Crystal reconstruction", "");
    m_builder->addAttDef("E","Energy reconstructed","Physics","MeV");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","red","");
    m_builder->addType("XtalCol", "XtalLog", "Crystal Log", "");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","white","");

    // Add XtalCol as subset of CalRecon in event relations between cluster and xtal don't exist
    m_builder->addType("CalRecon","XtalCol","Crystal Collection","");
    m_builder->addAttDef("#Xtals", "number of Xtals fired","Physics","");
    m_builder->addAttDef("MaxEnergy", "highest energy in any xtal", "Physics", "");
    m_builder->addType("XtalCol", "Xtal", "Crystal reconstruction", "");
    m_builder->addAttDef("E","Energy reconstructed","Physics","MeV");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","red","");
    m_builder->addType("XtalCol", "XtalLog", "Crystal Log", "");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","white","");

    m_builder->addType("CalRecon","CalMipTrackCol","Cal Mip Track Collection","");
    m_builder->addType("CalMipTrackCol", "CalMipTrack", "Cal Mip Tracks", "");
    m_builder->addAttDef("ChiSquare","ChiSquare","Physics"," ");
    m_builder->addAttValue("DrawAs","Line","");
    m_builder->addAttValue("Color", "red","");
    m_builder->addType("CalMipTrack", "Xtal", "Crystal reconstruction", "");
    m_builder->addAttDef("E","Energy reconstructed","Physics","MeV");
    m_builder->addAttValue("DrawAs","Prism","");
    m_builder->addAttValue("Color","blue","");
}


// This method fill the instance tree Event/MC with the actual TDS content
void CalReconFiller::fillInstances (std::vector<std::string>& typesList)
{
    //bool printEnergy = false; // diagnostic

    if (!hasType(typesList, "Recon/CalRecon"))
        return;

    m_builder->addInstance("Recon","CalRecon");

    if (hasType(typesList,"Recon/CalRecon/ClusterCol"))
    {      
        m_builder->addInstance("CalRecon","ClusterCol");    

        // drawing the cross in the average position for each layer 

        //  get pointer to the cluster reconstructed collection
        Event::CalClusterMap* clusterMap = SmartDataPtr<Event::CalClusterMap>(m_dpsvc,
            EventModel::CalRecon::CalClusterMap);

        // Recover the list of xTal to Cluster relations
        Event::CalClusterHitTabList* xTal2ClusTabList = 
            SmartDataPtr<Event::CalClusterHitTabList>(m_dpsvc, EventModel::CalRecon::CalClusterHitTab);

        // Draw the clusters
        drawClusters(clusterMap, xTal2ClusTabList);

        // Look for the case where no relations exist, then we need to explicitly draw the xTals. 
        if (!xTal2ClusTabList && hasType(typesList, "Recon/CalRecon/XtalCol")) 
        {
            // get the pointer to CalXtalRecCol object in TDS
            Event::CalXtalRecCol* cxrc = SmartDataPtr<Event::CalXtalRecCol>(m_dpsvc,
                EventModel::CalRecon::CalXtalRecCol); 

            drawXtals(cxrc);
        }
    }

    if (hasType(typesList,"Recon/CalRecon/CalMipTrackCol"))
    {      
        m_builder->addInstance("CalRecon","CalMipTrackCol");    

        //  get pointer to the CalMIPs collection
        Event::CalMipTrackCol* CalMipTrackCol = 
            SmartDataPtr<Event::CalMipTrackCol>(m_dpsvc, 
            EventModel::CalRecon::CalMipTrackCol);

        drawMips(CalMipTrackCol);
    }
}

void CalReconFiller::drawClusters(Event::CalClusterMap* clusterMap, Event::CalClusterHitTabList* xTal2ClusTabList)
{
    // Make the relational table
    Event::CalClusterHitTab* xTal2ClusTab = 0;
    
    if (xTal2ClusTabList) xTal2ClusTab = new Event::CalClusterHitTab(xTal2ClusTabList);

    // if pointer is not zero, start drawing
    if(clusterMap != 0 && !clusterMap->empty())
    {
        // Retrieve the CalClusterVec first
        Event::CalClusterVec clusterVec = clusterMap->getRawClusterVec();

        int numClusters = clusterVec.size();

        m_builder->setSubinstancesNumber("ClusterCol", numClusters);

        std::string clusColor = "";

        m_colorIndex = 0;

        Event::CalClusterVec::iterator clusIter = clusterVec.begin();
        while(clusIter != clusterVec.end())
        {
            m_builder->addInstance("ClusterCol", "Cluster");  

            // get pointer to the cluster 
            Event::CalCluster* cl = *clusIter++; 

            // Use color options if asked for
            if (m_hrisvc->getCalReconFiller_useColors())
            {
                int j = std::min(m_colorIndex++,m_maxColors-1);

                // Special cases, first see if we have "uber" cluster
                if (clusIter == clusterVec.end() && clusterVec.size() > 1) j = m_maxColors;
                // Check that energy is low
                else if (cl->getMomParams().getEnergy() < m_minEnergy) j = m_maxColors - 1;

                clusColor = m_colorArray[j];
            }

            drawCluster(cl, clusColor);

            // if we have more than one cluster then skip drawing crystals for the first one
            if ((clusIter == clusterVec.end() && clusterVec.size() > 1) || !xTal2ClusTab) continue;

            // Now draw the associated crystals. 
            // Start by getting the list for this cluster
            std::vector<Event::CalClusterHitRel*> xTal2ClusVec = xTal2ClusTab->getRelBySecond(cl);

            // Draw them
            drawXtals(xTal2ClusVec, clusColor);
        }
    }

    return;
}
    
void CalReconFiller::drawCluster(Event::CalCluster* cluster, std::string color)
{
    // Specific vector of layer data
    Event::CalClusterLayerDataVec& lyrDataVec = *cluster;

    // get total energy in the calorimeter: 
    // energySum is not filled when reading from Root!
    double clusEnergy = cluster->getMomParams().getEnergy();

    m_builder->addAttValue("E", (float)clusEnergy, "");

    // draw only if there is some energy in the calorimeter        
    if(clusEnergy > 0)
    {
        // Fill in the other output values 
        double rmsLong = cluster->getRmsLong();
        double rmsTran = cluster->getRmsTrans();
        double longAsy = cluster->getRmsLongAsym();
        int    nTrunc  = cluster->getNumTruncXtals();

        m_builder->addAttValue("rmsTrans",     (float)rmsTran, "");
        m_builder->addAttValue("rmsLong",      (float)rmsLong, "");
        m_builder->addAttValue("LongAsym",     (float)longAsy, "");
        //m_builder->addAttValue("numTotXtal",   cluster->size(),     "");
        m_builder->addAttValue("numTruncXtal", nTrunc,         "");
        m_builder->addAttValue("Centroid",     
            getPositionString(cluster->getMomParams().getCentroid()), "");
        m_builder->addAttValue("Axis",         
            getDirectionString(cluster->getMomParams().getAxis()), "");

        // Draw the cluster center
        double x = (cluster->getPosition()).x();
        double y = (cluster->getPosition()).y();
        double z = (cluster->getPosition()).z();

        // Use color options if asked for
        if (m_hrisvc->getCalReconFiller_useColors())
        {
            m_builder->addAttValue("Color", color, "");
        }
        m_builder->addPoint(x,y,z);            

        // Draw the layers reconstructed positions
        m_builder->addInstance("Cluster", "ClusterLayers");    

        // Use color options if asked for
        if (m_hrisvc->getCalReconFiller_useColors())
        {
            m_builder->addAttValue("Color", color, "");
        }

        // loop over calorimeter layers
        for( int l=0;l<m_nLayers;l++){

            // if energy in this layer is not zero - draw blue cross at
            // the average reconstructed position for this layer
            if (lyrDataVec[l].getEnergy() > 0) {            
                double x=lyrDataVec[l].getPosition().x();
                double y=lyrDataVec[l].getPosition().y();
                double z=lyrDataVec[l].getPosition().z();
                m_builder->addPoint(x,y,z);            
            }
        }

        // drawing the reconstructed shower direction
        // as a green line
        double dirX = (cluster->getDirection()).x();
        double dirY = (cluster->getDirection()).y();
        double dirZ = (cluster->getDirection()).z();

        // non display for non-physical or horizontal direction
        if(dirZ >= -1. && dirZ != 0.)
        {
            // Draw the cluster direction
            m_builder->addInstance("Cluster", "ClusterDir");    
            m_builder->addAttValue("Centroid",     
                getPositionString(cluster->getMomParams().getCentroid()), "");
            m_builder->addAttValue("Axis",         
                getDirectionString(cluster->getMomParams().getAxis()), "");

            // No point in drawing if moments not done
            if (rmsTran > 0. && rmsLong > 0.)
            {
                // Use color options if asked for
                if (m_hrisvc->getCalReconFiller_useColors()) m_builder->addAttValue("Color", color, "");

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

        // One more to draw the "fit" axis
        double xFitPos = cluster->getFitParams().getCentroid().x();
        double yFitPos = cluster->getFitParams().getCentroid().y();
        double zFitPos = cluster->getFitParams().getCentroid().z();

        double xFitDir = cluster->getFitParams().getAxis().x();
        double yFitDir = cluster->getFitParams().getAxis().y();
        double zFitDir = cluster->getFitParams().getAxis().z();

        // non display for non-physical or horizontal direction
        if(zFitDir >= -1. && zFitDir != 0.)
        {
            // Draw the cluster direction
            m_builder->addInstance("Cluster", "ClusterFit");    
            m_builder->addAttValue("Centroid",     
                getPositionString(cluster->getFitParams().getCentroid()), "");
            m_builder->addAttValue("Axis",         
                getDirectionString(cluster->getFitParams().getAxis()), "");
            m_builder->addAttValue("nFitLayers",  (float)cluster->getFitParams().getFitLayers(), "");
            m_builder->addAttValue("chiSquare",   (float)cluster->getFitParams().getChiSquare(), "");

            // Don't bother drawing if the fit was not performed
            if (cluster->getFitParams().getFitLayers() >= 4)
            {
                // Use color options if asked for
                if (m_hrisvc->getCalReconFiller_useColors()) m_builder->addAttValue("Color", color, "");

                // calculate x and y coordinates for the beginning and the end
                // of line in the top and bottom calorimeter layers
                double xTop    = xFitPos + xFitDir * (m_calZtop    - zFitPos) / zFitDir;
                double yTop    = yFitPos + yFitDir * (m_calZtop    - zFitPos) / zFitDir;
                double xBottom = xFitPos + xFitDir * (m_calZbottom - zFitPos) / zFitDir;
                double yBottom = yFitPos + yFitDir * (m_calZbottom - zFitPos) / zFitDir;

                m_builder->addPoint(xTop, yTop, m_calZtop);            
                m_builder->addPoint(xBottom, yBottom, m_calZbottom);                            
            }
        }
    }

    return;
}

void CalReconFiller::drawXtals(std::vector<Event::CalClusterHitRel*>& xTalRelVec, std::string color)
{
    // If list is empty there is nothing to do...
    if (!xTalRelVec.empty())
    {
        int    nXtals = xTalRelVec.size();
        double eTot   = 0.0;

        m_builder->addInstance("Cluster","XtalCol");    
        m_builder->setSubinstancesNumber("XtalCol", nXtals);
        // drawing red box for each log with a size
        // proportional to energy deposition

        double emax = 0.; // reset maximum energy per crystal
        std::vector<Event::CalClusterHitRel*>::const_iterator it = xTalRelVec.begin();

        // to find maximum energy per crystal
        for (; it != xTalRelVec.end(); it++)
        {
            // get poiner to the reconstructed data for individual crystal
            Event::CalXtalRecData* recData = (*it)->getFirst();

            // get reconstructed energy in the crystal
            double eneXtal = recData->getEnergy();
            //if(printEnergy) std::cout << eneXtal << std::endl;
            eTot += eneXtal;

            // if energy is bigger than current maximum - update the maximum 
            if(eneXtal>emax)emax=eneXtal;
        }
        m_builder->addAttValue("#Xtals",nXtals,"");
        m_builder->addAttValue("MaxEnergy",(float)emax,"");

        // if maximum crystal energy isn't zero - start drawing 
        if(emax > 0)
        {
            // loop over all crystals in reconstructed collection
            // to draw red boxes
            std::vector<Event::CalClusterHitRel*>::iterator it = xTalRelVec.begin();
            for (;it!=xTalRelVec.end(); ++it)
            {
                // get poiner to the reconstructed data for individual crystal
                Event::CalXtalRecData* recData = (*it)->getFirst();

                drawXtal(recData, emax, color);
            }
        }
    }
    return;
}
    
void CalReconFiller::drawXtals(Event::CalXtalRecCol* xTalCol, std::string color)
{
    if (xTalCol)
    {
        int nXtals = xTalCol->size();
        double eTot = 0.0;
        m_builder->addInstance("CalRecon","XtalCol");    
        m_builder->setSubinstancesNumber("XtalCol", nXtals);
        // drawing red box for each log with a size
        // proportional to energy deposition

        double emax = 0.; // reset maximum energy per crystal
        Event::CalXtalRecCol::const_iterator it = xTalCol->begin();

        // to find maximum energy per crystal
        for (; it != xTalCol->end(); it++){

            // get poiner to the reconstructed data for individual crystal
            Event::CalXtalRecData* recData = *it;

            // get reconstructed energy in the crystal
            double eneXtal = recData->getEnergy();
            //if(printEnergy) std::cout << eneXtal << std::endl;
            eTot += eneXtal;

            // if energy is bigger than current maximum - update the maximum 
            if(eneXtal>emax)emax=eneXtal;
        }
        m_builder->addAttValue("#Xtals",nXtals,"");
        m_builder->addAttValue("MaxEnergy",(float)emax,"");

        // if maximum crystal energy isn't zero - start drawing 
        if(emax>0)
        {
            // loop over all crystals in reconstructed collection
            // to draw red boxes
            for (Event::CalXtalRecCol::const_iterator it = xTalCol->begin();it!=xTalCol->end(); ++it)
            {
                // get poiner to the reconstructed data for individual crystal
                Event::CalXtalRecData* recData = *it;

                drawXtal(recData, emax);
            }
        }
    }

    return;
}

void CalReconFiller::drawXtal(Event::CalXtalRecData* recData, double eMax, std::string color)
{
    // This is really a constant for this method
    static const HepPoint3D p0(0.,0.,0.);  

    // get reconstructed energy in the crystal
    double eneXtal = recData->getEnergy();

    // draw crystals containing less than 1% of maximum energy dashed
    m_builder->addInstance("XtalCol", "Xtal"); 
    //if(eneXtal<0.01*emax) m_builder->addAttValue("LineStyle","Dashed","");
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
    //double s = 0.45*m_xtalHeight*eneXtal/emax;
    double s = 2. * 0.45*m_xtalHalfHeight*pow(eneXtal/eMax,0.333);

    // check for saturation... for now just use the energy, since
    // the individual xtal ends are not available in the recon class
    // so no orange boxes
    // we can do better by using the digi info (maybe)

    //double ePos = recData->getEnergy(0, idents::CalXtalId::POS);
    //double eNeg = recData->getEnergy(0, idents::CalXtalId::NEG);

    double eSat = 68000.0;
    //if(ePos>eSat) satCount++;
    //if(eNeg>eSat) satCount++;

    std::string satColor[3] = {"red", "orange", "yellow"};
    double energy = recData->getEnergy();
    int satCount = (energy>eSat ? 2 : 0);
    m_builder->addAttValue("Color", satColor[satCount],"");
    drawPrism(x, y, z, s, s, s);

    // Draw the faint outline of the entire log
    m_builder->addInstance("XtalCol", "XtalLog");
    //if(eneXtal<0.01*emax) m_builder->addAttValue("LineStyle","Dashed","");
    if (color != "") m_builder->addAttValue("Color", color, "");

    // Get the volume identifier
    const idents::CalXtalId xtalId = recData->getPackedId();

    // unpack crystal identification into tower, layer and column number
    int layer = xtalId.getLayer();
    int tower = xtalId.getTower();
    int col   = xtalId.getColumn();

    // create Volume Identifier for segment 0 of this crystal
    idents::VolumeIdentifier segm0Id;
    segm0Id.append(m_eLATTowers);
    segm0Id.append(tower/m_xNum);
    segm0Id.append(tower%m_xNum);
    segm0Id.append(m_eTowerCAL);
    segm0Id.append(layer);
    segm0Id.append(layer%2); 
    segm0Id.append(col);
    segm0Id.append(m_eXtal);
    segm0Id.append(0);

    HepTransform3D transf;

    //get 3D transformation for segment 0 of this crystal
    m_gdsvc->getTransform3DByID(segm0Id,&transf);
    //get position of the center of the segment 0
    CLHEP::Hep3Vector vect0 = transf.getTranslation();

    // create Volume Identifier for the last segment of this crystal
    idents::VolumeIdentifier segm11Id;
    // copy all fields from segm0Id, except segment number
    for(int ifield = 0; ifield < fSegment; ifield++)
        segm11Id.append(segm0Id[ifield]);
    segm11Id.append(m_nCsISeg-1); // set segment number for the last segment

    //get 3D transformation for the last segment of this crystal
    m_gdsvc->getTransform3DByID(segm11Id,&transf);
    //get position of the center of the last segment
    CLHEP::Hep3Vector vect1 = transf.getTranslation();

    // Crystal center is what we want
    CLHEP::Hep3Vector xtalCtr = 0.5 * (vect0 + vect1);

    double xXtal = xtalCtr.x();
    double yXtal = xtalCtr.y();
    double zXtal = xtalCtr.z();

    // Start drawing this side of the log. 
    // How we do this depends on if x or y 
    if (layer % 2 == 1) // y face is constant
    {
        drawPrism(xXtal, yXtal, zXtal,
            m_xtalHalfWidth, m_xtalHalfLength, m_xtalHalfHeight);
    }
    else // x face is constant
    {
        drawPrism(xXtal, yXtal, zXtal,
            m_xtalHalfLength, m_xtalHalfWidth, m_xtalHalfHeight);
    }
 
    return;
}

void CalReconFiller::drawMips(Event::CalMipTrackCol* CalMipTrackCol)
{
    // if pointer is not zero, start drawing
    if(CalMipTrackCol)
    {
        // Retrieve the CalMIPs to CalXtalRecData relational table
        //SmartDataPtr<Event::CalXtalMIPsTabList> 
        //    CalXtalMipsTable(m_dpsvc, EventModel::CalRecon::CalXtalMIPsTab);
        //Event::CalXtalMIPsTab* CalXtalMIPsTab 
        //    = new Event::CalXtalMIPsTab(CalXtalMipsTable);

        int numMIPs = CalMipTrackCol->size();
        m_builder->setSubinstancesNumber("CalMipTrackCol", numMIPs);

        Event::CalMipTrackColItr mipIter = CalMipTrackCol->begin();            
        for(; mipIter != CalMipTrackCol->end(); mipIter++)
        {
            Event::CalMipTrack* calMipTrack = *mipIter;

            m_builder->addInstance("CalMipTrackCol", "CalMipTrack");  

            // Position and direction
            Point  mipPos   = calMipTrack->getPoint();
            Vector mipDir   = calMipTrack->getDir();

            //                 // Number of crystals
            //                 int    numXtals = calMipTrack->size();

            //      // Update the point
            //          double delta  = 2 * numXtals * m_xtalHeight;
            //          Point  topPos = mipPos + delta * mipDir;

            //      // Bottom point
            //      mipPos -= delta * mipDir;

            //                  // Add the starting point to the display
            //                  m_builder->addPoint(mipPos.x(), mipPos.y(), mipPos.z());

            //                  // Add the endpoint
            //                  m_builder->addPoint(topPos.x(), topPos.y(), topPos.z());


            //        //projection of this hit on track
            //        Point Hp=C+((Hit[hid].P-C)*dir)*dir;
            //        for (int ihh=ih+1; ihh<tr[itr].nh; ihh++)
            //      {
            //        int hidd=tr[itr].hid[ihh];
            //        //projection of this hit on track
            //        Point HHp=C+((Hit[hidd].P-C)*dir)*dir;
            //        //vv=Hit[hid].P-Hit[hidd].P;
            //        vv=HHp-Hp;
            //        double d=sqrt(vv*vv);
            //        if (d>tr[itr].length)
            //          {
            //            tr[itr].length=d;
            //            tr[itr].H1=Hp;
            //            tr[itr].H2=HHp;
            //          }

            Point startPos;
            Point endPos;
            double dmax=-1.;
            // Find start and end of track
            for(Event::CalMipXtalVec::iterator xTalIter1  = calMipTrack->begin(); 
                xTalIter1 != calMipTrack->end(); 
                xTalIter1++)
            {
                Event::CalMipXtal& xTal1 = *xTalIter1;
                // get the vector of reconstructed position
                HepVector3D pXtal1 = xTal1.getXtal()->getPosition();
                Point H1=xTal1.getXtal()->getPosition();
                // projection on the track
                Point P1=mipPos+((H1-mipPos)*mipDir)*mipDir;
                for(Event::CalMipXtalVec::iterator xTalIter2  = calMipTrack->begin(); 
                    xTalIter2 != calMipTrack->end(); 
                    xTalIter2++)
                {
                    Event::CalMipXtal& xTal2 = *xTalIter2;
                    // get the vector of reconstructed position
                    HepVector3D pXtal2 = xTal2.getXtal()->getPosition();
                    Point H2=xTal2.getXtal()->getPosition();
                    // projection on the track
                    Point P2=mipPos+((H2-mipPos)*mipDir)*mipDir;            
                    Vector vv=P2-P1;
                    double dist=sqrt(vv*vv);
                    if (dist>dmax)
                    {
                        startPos=P1;
                        endPos=P2;
                        dmax=dist;
                    }
                }
            }

            // Add the starting point to the display
            m_builder->addPoint(startPos.x(), startPos.y(), startPos.z());

            // Add the endpoint
            m_builder->addPoint(endPos.x(), endPos.y(), endPos.z());

            // Draw these crystals
            for(Event::CalMipXtalVec::iterator xTalIter  = calMipTrack->begin(); 
                xTalIter != calMipTrack->end(); 
                xTalIter++)
            {
                Event::CalMipXtal& xTal = *xTalIter;

                // get reconstructed energy in the crystal
                double eneXtal = xTal.getXtal()->getEnergy();

                m_builder->addInstance("CalMipTrack", "Xtal");    
                m_builder->addAttValue("E", (float)eneXtal, "");

                // get the vector of reconstructed position
                HepVector3D pXtal = xTal.getXtal()->getPosition();

                // get reconstructed coordinates
                double x = pXtal.x();
                double y = pXtal.y();
                double z = pXtal.z();

                // calculate the half size of the box, 
                // taking the 90% of crystal half height
                // as the size corresponding to the maximum energy
                //double s = 0.45*m_xtalHalfHeight*eneXtal/emax;
                double s = m_xtalHalfHeight;
                drawPrism(x, y, z, s, s, s);
            }
        }
    }

    return;
}

