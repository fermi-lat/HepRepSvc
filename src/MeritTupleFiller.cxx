#include "MeritTupleFiller.h"

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

#include "ntupleWriterSvc/INTupleWriterSvc.h"

#include "geometry/Point.h"

#include "TLeaf.h"
#include "TFile.h"
#include "TTree.h"

#include <algorithm>
#include <iomanip>
#include <fstream>

// Constructor
MeritTupleFiller::MeritTupleFiller(HepRepInitSvc* hrisvc,
                                   INTupleWriterSvc* rtsvc):
m_hrisvc(hrisvc),m_rtsvc(rtsvc)
{
    m_names.resize(0,"");
    m_prefixes.resize(0,"");
    m_types.resize(0,"");
    m_dims.resize(0,0);
    m_conversionFlags.resize(0,0);
    m_prefixCount = 0;
}

// This method build the types for the HepRep
void MeritTupleFiller::buildTypes()
{
    if(!m_hrisvc->getMeritTupleFiller_doIt()) return;

    m_builder->addType("","Merit","merit variables","");
    //m_builder->addAttValue("Layer","Event",""); //kludge

    m_tree = 0;
    std::string treeName = "MeritTuple";
    void* ptr;
    // check for output file first
    m_rtsvc->getOutputTreePtr(ptr);
    if (ptr==0) {
        //now check for input tuple
        m_rtsvc->getInputTreePtr(ptr);
        if(ptr==0) return;
    }
    m_tree = reinterpret_cast<TTree*>(ptr);
    if(!parseTree(m_tree)) return;

    int size = m_names.size();
    m_prefixes.resize(size,"");
    m_conversionFlags.resize(size,0);

    std::string prefix = "";
    std::string thisPrefix;
    bool tkrHits = false;
    int i;
    for(i=0; i<size; ++i) {
        // choose the prefix
        std::string thisName = m_names[i];
        if(thisName=="TkrNumHits") tkrHits = true;
        if(thisName=="TkrNumTracks") tkrHits = false;
        if(tkrHits) thisPrefix = "TkrHits";
        else if(thisName.substr(0,4)=="McTH") thisPrefix = "McTH";
        else if(thisName.substr(0,4)=="User") thisPrefix = "User";
        else if(thisName.substr(0,2)=="Mc")   thisPrefix = "Mc";
        else if(thisName.substr(0,2)=="Pt")   thisPrefix = "Pt";
        else                                  thisPrefix = thisName.substr(0,3); 
        m_prefixes[i] = thisPrefix;
        if(prefix!=thisPrefix) {
            std::string comment = thisPrefix+" variables";
            m_builder->addType("Merit",thisPrefix, comment ,"");
            m_prefixCount++;
            prefix = thisPrefix;
        }

        // kludge
        //m_builder->addAttValue("DrawAs","Prism","");
        //m_builder->addAttValue("Color","red","");

        if( thisName=="GltGemSummary" || thisName=="GltWord")      { // F->lo
            m_conversionFlags[i] = 1;
        } else if (thisName=="Tkr1Type" || thisName=="Tkr2Type")   { // F->lo,hi
            m_conversionFlags[i] = 2;
        } else if (thisName=="VtxStatus" ||thisName=="Vtx2Status") { // F->lo
            m_conversionFlags[i] = 1;
        } else if (thisName.find("Status")!=std::string::npos)     { // Int-> lo,hi
            m_conversionFlags[i] = 3;
        }
    }
}

// 
void MeritTupleFiller::fillInstances (std::vector<std::string>& typesList)
{

    if(!m_hrisvc->getMeritTupleFiller_doIt()) return;

    //MsgStream log = *(m_hrisvc->getMsgStream());
    //log << MSG::INFO << "Hello from MeritTupleFiller!" << endreq;

    if (!hasType(typesList,"Merit")) return;

    std::string pref = "";
    m_builder->addInstance("","Merit");
    m_builder->setSubinstancesNumber("Merit", m_prefixCount);

    double delta = -320.; // for kludge
 
    int i;
    for(i=0;i<(int)m_names.size();++i) {
        std::string thisName = m_names[i];
        void * ptr;
        m_rtsvc->getItem("MeritTuple",thisName, ptr);
        if(!ptr) continue;
        if(m_prefixes[i]!=pref) {
            pref = m_prefixes[i];
            m_builder->addInstance("Merit",pref);


            // kludge
            //drawPrism(0.+delta, 0.+delta, 1000., 10., 10., 10.);
            //delta += 40.;
        }

        std::string thisType = m_types[i];

        if(thisName=="PtPos") {
            Point pos = Point(*reinterpret_cast<float*>(ptr)*.001,
                *(reinterpret_cast<float*>(ptr)+1)*.001,
                *(reinterpret_cast<float*>(ptr)+2)*.001);
            std::string stPos = getPositionString(pos)+" km";
            m_builder->addAttValue(thisName,stPos,"");
        }

        //bit words, some encoded as floats!
        //floating pt -> 16 bit integer
        if(m_conversionFlags[i]==1) {
            m_builder->addAttValue(thisName,
                getBits( (unsigned)*reinterpret_cast<float*>(ptr),15,0),"");
            continue;
        }

        // 32 bits from int
        if(m_conversionFlags[i]==3) {
            m_builder->addAttValue(thisName+"-lo",
                getBits( *reinterpret_cast<unsigned*>(ptr),15,0),"");
            m_builder->addAttValue(thisName+"-hi",
                getBits( *reinterpret_cast<unsigned*>(ptr),31,16),"");
            continue;
        }

        // 32 bits from float
        if(m_conversionFlags[i]==2) {
            m_builder->addAttValue(thisName+"-lo",
                getBits( (unsigned)*reinterpret_cast<float*>(ptr),15,0),"");
            m_builder->addAttValue(thisName+"-hi",
                getBits( (unsigned)*reinterpret_cast<float*>(ptr),31,16),"");
            continue;
        }

        // Native HepRepTypes
        if(thisType=="Int_t") {
            m_builder->addAttValue(thisName, *reinterpret_cast<int*>(ptr),"");
        }else if (thisType=="Float_t") {
            m_builder->addAttValue(thisName, *reinterpret_cast<float*>(ptr),"");
        }else {
            // the remaining types need to be converted to strings
            std::stringstream tempStr;
            if (thisType=="Double_t") {
                double dTemp = *reinterpret_cast<double*>(ptr);
                tempStr.precision(4);
                tempStr.setf(std::ios::fixed);
                tempStr << dTemp;
                m_builder->addAttValue(thisName, tempStr.str()  ,"");
            }else if (thisType=="UInt_t") {
                unsigned iTemp = *reinterpret_cast<unsigned int*>(ptr);
                tempStr  << iTemp;
                m_builder->addAttValue(thisName, tempStr.str()  ,"");
            }else if (thisType=="ULong64_t") {
                unsigned long long  iLTemp = *reinterpret_cast<unsigned long long*>(ptr);
                tempStr  << iLTemp;
                m_builder->addAttValue(thisName, tempStr.str()  ,"");
            }else if (thisType=="Char_t") {
                std::string  sTemp = std::string(reinterpret_cast<char*>(ptr));
                m_builder->addAttValue(thisName, sTemp  ,"");
            }
        }
    }
}

bool MeritTupleFiller::parseTree( TTree* tree)
{
    //std::vector<std::string*> nameVec;

    TObjArray* brCol = tree->GetListOfBranches();
    int numBranches = brCol->GetEntries();
    int iBranch;
    std::string leafName;
    std::string typeName;
    std::string dimStr;
    int dim;
    for (iBranch=0;iBranch<numBranches;iBranch++) {
        std::string branchName(((TBranch*)(brCol->At(iBranch)))->GetName());
        leafName = branchName;
        int index = leafName.find("[");
        dim = 1;
        dimStr = "1";
        if((unsigned)index!=std::string::npos) {
            dim = -1;
            int index1 = leafName.find("]");
            dimStr = leafName.substr(index+1,index1-index-1);
            leafName = leafName.substr(0,index);
            dim = atoi(dimStr.c_str());
        }
        //std::cout << "setting branch: " << branchName
        //    << " and Leaf: " << leafName << std::endl;
        TLeaf *leaf = ((TBranch*)brCol->At(iBranch))->GetLeaf(leafName.c_str());
        typeName = "Not Found";
        if (leaf) {
            typeName = leaf->GetTypeName();
        }
        m_names.push_back(leafName);
        m_types.push_back(typeName);
        m_dims.push_back(dim);
    }
    return true;
}
