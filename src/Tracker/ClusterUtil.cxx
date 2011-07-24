
#include "ClusterUtil.h"
/** 
* @class ClusterUtil
*
* @brief base class for managing clusters
*
* @author(s) L. Rochester
*
*/

namespace {
    const float _ToTScale = 64; // so maxToT = ~4mm
    const float _ToT250Width = 5.0; // "saturated" ToT
}

ClusterUtil::ClusterUtil(HepRepInitSvc* hrisvc, IGlastDetSvc* gsvc) :
m_hrisvc(hrisvc), m_gdsvc(gsvc)
{
    m_markerSize  = 25.0;
    m_markerWidth = 2.0;
    m_scalingMarker  = true;

    int    ladderNStrips;
    double ladderGap;

    m_gdsvc->getNumericConstByName("SiWaferActiveSide", &m_siWaferActiveSide);
    m_gdsvc->getNumericConstByName("stripPerWafer",     &ladderNStrips);
    m_gdsvc->getNumericConstByName("SiThick",           &m_siThickness);
    m_gdsvc->getNumericConstByName("SiWaferSide",       &m_siWaferSide);
    m_gdsvc->getNumericConstByName("ssdGap",            &m_ssdGap);
    m_gdsvc->getNumericConstByName("ladderGap",         &ladderGap);
    m_gdsvc->getNumericConstByName("nWaferAcross",      &m_nWaferAcross);
    m_gdsvc->getNumericConstByName("xNum",              &m_numXTowers);
    m_gdsvc->getNumericConstByName("yNum",              &m_numYTowers);
    m_gdsvc->getNumericConstByName("towerPitch",        &m_towerPitch);

    double deadWidth = m_siWaferSide - m_siWaferActiveSide;
    m_stripLength  = 
        m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*m_ssdGap - deadWidth;
    m_activeWidth  = 
        m_nWaferAcross*m_siWaferSide + (m_nWaferAcross-1)*ladderGap - deadWidth;
    m_siStripPitch = m_siWaferActiveSide / ladderNStrips;
    m_useToTInfo    = hrisvc->getClusterUtil_useToTInfo();
    m_useTriggerInfo    = hrisvc->getClusterUtil_useTriggerInfo();
    m_useDiagnosticInfo = hrisvc->getClusterUtil_useDiagnosticInfo();
}

ClusterUtil::~ClusterUtil() {}

std::string ClusterUtil::getClusterColor(Event::TkrCluster* pCluster, bool isAcc) 
{
    bool isGhost = (pCluster->isSet(Event::TkrCluster::maskGHOST));
    bool showTriggerGhosts = (isGhost&&m_useTriggerInfo);
    bool isDiag  = (pCluster->isSet(Event::TkrCluster::maskDIAGNOSTIC));
    bool showDiagnosticGhosts = (isDiag&&m_useDiagnosticInfo);
    if (isAcc && m_useToTInfo) { return "red"; }
    if (showTriggerGhosts&&showDiagnosticGhosts) {
        return "160,32,240";  // purple (orange+blue!)
    }
    if (showTriggerGhosts) {
        return "255,100,27";  // orange
    }
    if (showDiagnosticGhosts) {
        return "blue";
    }
    if (pCluster->isSet(Event::TkrCluster::maskSAMETRACK) ) {
        return "yellow"; 
    } 
    return "green";
}

void ClusterUtil::buildClusterTypes(IBuilder* builder)
{
    if(m_scalingMarker) {
        builder->addAttValue("DrawAs", "Line", "");
        builder->addAttValue("LineWidth", m_markerWidth, "");
    } else {
        //Here's the real marker 
        builder->addAttValue("DrawAs", "Point", "");
        builder->addAttValue("MarkerName", "Cross", "");
        builder->addAttValue("MarkerSize", "1", "");
    }

    //builder->addAttValue("Color","green","");
    builder->addAttDef("Sequence", "Position in TkrClusterCol", "Physics", "");
    builder->addAttDef("Tower","Cluster Tower #","Physics","");
    builder->addAttDef("Plane","Cluster Plane #","Physics","");
    builder->addAttDef("TkrView","Cluster View","Physics","");
    builder->addAttDef("First Strip","Cluster First Strip","Physics","");
    builder->addAttDef("Last Strip","Cluster Last Strip","Physics","");
    builder->addAttDef("Status-lo", "Cluster Low Status Bits","Physics","");
    builder->addAttDef("Status-hi", "Cluster High Status Bits","Physics","");
    builder->addAttDef("Position","Cluster Global Position","Physics","");
    builder->addAttDef("RawToT","Cluster Time over Threshold","Physics","");
    builder->addAttDef("Mips","ToT converted to Mips","Physics","");

    builder->addType("TkrCluster", "Strip", "Strip", "");
    builder->addType("Strip", "ActiveStrip", "Active Part of Strip", "");
    builder->addAttValue("DrawAs","Prism","");
    builder->addAttDef("TkrView","Cluster View","Physics","");

    builder->addType("TkrCluster","TkrClusterToT"," ","");
    builder->addAttValue("DrawAs","Line","");
    builder->addAttValue("Color","red","");
    builder->addAttDef("TkrView", "tracker view", "Physics", "");

    //builder->addType("TkrCluster", "ClusterMarker", " ", "");
}

void ClusterUtil::buildClusterInstance(IBuilder* builder, Event::TkrCluster* pCluster)
{

    double markerOffset = m_siStripPitch;
    double markerSize   = m_markerSize*m_siStripPitch;

    double halfLength   = 0.5 * (m_stripLength);

    Point clusPos  = pCluster->position();

    double halfWidth = 0.5 * pCluster->size() * m_siStripPitch;
    double x    = clusPos.x();
    double y    = clusPos.y();
    double z    = clusPos.z();
    double dz   = 0.5 * m_siThickness;

    double dx   = halfWidth;
    double dy   = halfLength;

    double xToT = x;
    double yToT = y - dy - markerOffset;
    double deltaY = 0.0;
    double deltaX = markerSize;

    int    view   = pCluster->getTkrId().getView();
    if (view == idents::TkrId::eMeasureY) {
        std::swap(dx, dy);
        std::swap(deltaX, deltaY);
        xToT = x - dx - markerOffset;
        yToT = y;
    }

    double rawToT = pCluster->getRawToT();
    double ToT  = rawToT / _ToTScale;


    builder->addAttValue("Tower",       pCluster->tower(),"");
    builder->addAttValue("Plane",       (int)pCluster->getPlane(),"");
    builder->addAttValue("TkrView",        view,"");
    builder->addAttValue("First Strip", pCluster->firstStrip(),"");
    builder->addAttValue("Last Strip",  pCluster->lastStrip(),"");

    unsigned int status = pCluster->getStatusWord();
    builder->addAttValue("Status-lo",getBits(status, 15, 0),"");
    builder->addAttValue("Status-hi",getBits(status, 31, 16),"");
    builder->addAttValue("Position",
        getPositionString(pCluster->position()),"");

    builder->addAttValue("RawToT",         (float) rawToT,"");
    builder->addAttValue("Mips",           (float)(pCluster->getMips()),"");

    //Draw the width of the cluster

    //Now draw the hit strips
    // check for ToT==255
    bool isAcc = (rawToT==255);
    std::string clusterColor = getClusterColor(pCluster,isAcc);

    // quick and dirty wide cluster display.
    bool isWide = (pCluster->size()>4)&&m_hrisvc->getClusterFiller_showWide();
    if(m_scalingMarker) {
        // make the cross in the measured view
        //builder->addInstance("TkrCluster", "ClusterMarker");
        if(isAcc) builder->addAttValue("LineStyle","Dashed","");
        // the following is perhaps a bit too clever...
        // it makes an "X" for normal clusters, and a bowtie for wide ones
        builder->addAttValue("Color", clusterColor, "");
        builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
        builder->addPoint(xToT-deltaX, yToT-deltaY, z-markerSize);
        if(!isWide) builder->addPoint(xToT,yToT,z);
        builder->addPoint(xToT-deltaX, yToT-deltaY, z+markerSize);
        builder->addPoint(xToT+deltaX, yToT+deltaY, z-markerSize);
        if(isWide) builder->addPoint(xToT+deltaX, yToT+deltaY, z+markerSize);
    } else {
        // Here's the code for the real marker
        builder->addInstance("TkrCluster", "ClusterMarker");

        if(isAcc) builder->addAttValue("LineStyle","Dashed","");
        builder->addAttValue("Color", clusterColor, "");
        builder->addPoint(xToT, yToT, z);  
        builder->addAttValue("TkrView", view, "");
    }

    builder->addInstance("TkrCluster", "Strip");
    builder->setSubinstancesNumber("Strip",m_nWaferAcross);
    double waferPitch = m_siWaferSide + m_ssdGap;
    double offset = -0.5*(m_nWaferAcross-1)*waferPitch;
    //builder->addInstance("Strip", "ActiveStrip");
    //if(clusterColor!="green") builder->addAttValue("LineStyle","Dashed","");
    //builder->addAttValue("Color", clusterColor, "");
    for (int wafer=0;wafer<m_nWaferAcross; ++wafer) {
        builder->addInstance("Strip", "ActiveStrip");
        if(clusterColor!="green") builder->addAttValue("LineStyle","Dashed","");
        builder->addAttValue("Color", clusterColor, "");
        builder->addAttValue("TkrView", view, "");
        double delta = offset + wafer*waferPitch;
        x  = clusPos.x();
        y  = clusPos.y() + delta;
        dx = halfWidth;
        dy = 0.5*m_siWaferActiveSide;
        if (view == idents::TkrId::eMeasureY) {
            x = clusPos.x() + delta;
            y = clusPos.y();
            std::swap(dx, dy);
        }
        drawPrism(x, y, z, dx, dy, dz);
    }

    //Time over threshold add here
    if(m_hasTypeToT)
    {
        builder->addInstance("TkrCluster","TkrClusterToT");
        if(rawToT>249) builder->addAttValue("LineWidth", _ToT250Width, "");
        if(isAcc) builder->addAttValue("LineStyle", "Dashed", "");
        builder->addPoint(xToT,yToT,z);
        builder->addPoint(xToT,yToT,z+ToT);
        builder->addAttValue("TkrView", view, "");
    }
}
