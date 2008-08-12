// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepGeometry.cxx,v 1.16 2007/10/27 00:40:14 jrb Exp $
//
// Author(s):
//      R.Giannitrapani

#include "HepRepGeometry.h"
#include "HepRepSvc/IBuilder.h"
#include "Filler.h"

#include "CLHEP/Vector/Rotation.h"
//#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

// TU: Hacks for CLHEP 1.9.2.2 and beyond
#ifndef HepPoint3D
typedef HepGeom::Point3D<double> HepPoint3D;
#endif
#ifndef HepVector3D
typedef HepGeom::Vector3D<double> HepVector3D;
#endif

#include <iomanip>
#include <algorithm>

HepRepGeometry::HepRepGeometry(unsigned int depth, Filler* filler, std::string mode): 
m_mode(mode), m_filler(filler), m_depth(depth)
{
    m_actualDepth = 0;
    m_builder = 0;
    HepGeom::Transform3D start;
    m_actualTransform.push_back(start);
}

HepRepGeometry::~HepRepGeometry() {}

IGeometry::VisitorRet 
HepRepGeometry::pushShape(ShapeType s, const UintVector& idvec, 
                          std::string name, std::string material, 
                          const DoubleVector& params, VolumeType type,
                          SenseType sense)
{
    if(m_hrMode == "type")
    {
        // Build the full path name in the list of already built types
        std::string fullName = "";
        for(unsigned int j = 0; j<m_actualType.size();++j)
        {
            fullName = fullName + m_actualType[j] + "/";
        }

        fullName = fullName + name;

        std::vector<std::string>::const_iterator i;
        i = std::find(m_types.begin(),m_types.end(),fullName);
        if (i == m_types.end())
        {          
            m_types.push_back(fullName);          
            std::string father;
            if (m_actualType.size() == 0)
                father = "";
            else
                father = m_actualType.back();

            m_actualDepth++;

            m_actualType.push_back(name);          
            m_builder->addType(father,name,
                "A volume of the geometry; it can be simple, a composition or a stack","");

            // The color is set only to the root of the geometry, so it will be
            // possible to change the color of subtypes 
            if (name == "LAT")
            {
                m_builder->addAttValue("Color","0.98,0.92,0.8","");
                m_builder->addAttValue("Layer","Geometry","");
            }

            m_builder->addAttDef("Volume type",
                "The kind of volume (simple, composition or stack)",
                "Physics","");
            m_builder->addAttDef("Sensitivity",
                "One of Nonsensitive, intSensitive, or posSensitive",
                "Physics","");
            m_builder->addAttDef("Material","The material name of the volume",
                "Physics","");
            m_builder->addAttDef("Shape",
                "Currently this can be Box,Tube,Sphere or Trap",
                "Physics","");

            if (m_actualDepth > m_depth)
                m_builder->addAttValue("Visibility", false, "");

            m_builder->addAttValue("Material",material,"");

            switch(type)
            {
            case Simple:
                m_builder->addAttValue("Volume type","Simple","");
                break;
            case Composite: 
                m_builder->addAttValue("Volume type","Composite","");
                break;
            case Xstack: 
                m_builder->addAttValue("Volume type","Xstack","");
                break;
            case Ystack: 
                m_builder->addAttValue("Volume type","Ystack","");
                break;
            case Zstack:
                m_builder->addAttValue("Volume type","Zstack","");
                break;
            }  
            switch(sense) {
            case posSensitive:
                m_builder->addAttValue("Sensitivity","posSensitive","");
                break;
            case intSensitive:
                m_builder->addAttValue("Sensitivity","intSensitive","");
                break;
            case Nonsensitive:
                m_builder->addAttValue("Sensitivity","Nonsensitive","");
                break;
            }

            switch(s)
            {
            case Box:
                m_builder->addAttValue("DrawAs","Prism","");
                m_builder->addAttValue("Shape","Box","");
                break;
            case Tube:
                m_builder->addAttValue("DrawAs","Prism","");
                m_builder->addAttValue("Shape","Tube","");
                break;                            
            case Sphere:
                m_builder->addAttValue("DrawAs","Point","");
                m_builder->addAttValue("Shape","Sphere","");
                break;           
            case Trap:   
                m_builder->addAttValue("DrawAs","Prism","");
                m_builder->addAttValue("Shape","Trap","");
                break;
            }
        }
        else
        {
            m_actualDepth++;
            m_actualType.push_back(name);
        }
    }

    else if (m_hrMode == "instance")
    {
        std::string temp;
        for(unsigned int k=0; k<m_actualInstance.size(); k++)
            temp = temp + m_actualInstance[k] + "/";

        double x=params[0], y=params[1], z=params[2];
        double rx=params[3], ry=params[4], rz=params[5];  

        CLHEP::HepRotation rot;
        rot.rotateX(rx*M_PI/180);
        rot.rotateY(ry*M_PI/180);
        rot.rotateZ(rz*M_PI/180);

        CLHEP::Hep3Vector t(x, y, z);  
        HepGeom::Transform3D tr(rot,t);
        HepGeom::Transform3D atr = (m_actualTransform.back())*tr;

        std::string father;
        if (m_actualInstance.size() == 0)
            father = "";
        else
            father = m_actualInstance.back();


        m_actualInstance.push_back(name);
        m_actualDepth++;            

        // Verify that this instance is in the typesList
        if (m_filler->hasType(m_typesList, temp+name))
        {                                    
            m_builder->addInstance(father,name);

            if ((s == Box))
            {
                double dx = params[6]/2;
                double dy = params[7]/2;
                double dz = params[8]/2;

                HepPoint3D v;
                HepPoint3D v1;

                //   if ((((type == Simple) || (type == posSensitive) 
                //  || (type == intSensitive)) 
                //    && (m_actualDepth < m_depth)) || (m_actualDepth == m_depth)) 
                // if ((type == Simple) || (type == posSensitive) 
                //   || (type == intSensitive))
                if (type == Simple)
                {
                    m_filler->drawTransformedPrism(dx, dy, dz, atr);
                }
            } // End of Box case
            else if (s == Trap)
            {
                double dx1 = params[6]/2;
                double dx2 = params[7]/2;
                double dxDiff = params[8]/2;
                double dy = params[9]/2;
                double dz = params[10]/2;

                m_filler->drawTransformedTrap(dx1, dx2, dxDiff, dy, dz, atr);

            }   // end Trap (trapezoidal prism)
            else if (s == Tube)
            {
                double dz = params[6]/2;
                double rin = params[7];
                double rout = params[8];

                HepPoint3D v;
                HepPoint3D v1;

                unsigned int n = 20;
                double angle;

                if (type == Simple)
                {
                    for(unsigned int i=0; i<n; i++)
                    {
                        angle = 2*M_PI*((double)i/n);
                        v.setX(rout*sin(angle)); 
                        v.setY(rout*cos(angle)); 
                        v.setZ(-dz); 
                        v1 = (atr*v);           
                        m_builder->addPoint(v1.x(), v1.y(), v1.z());
                    }
                    for(unsigned int i=0; i<n; i++)
                    {
                        angle = 2*M_PI*((double)i/n);
                        v.setX(rout*sin(angle)); 
                        v.setY(rout*cos(angle)); 
                        v.setZ(dz); v1 = (atr*v);           
                        m_builder->addPoint(v1.x(), v1.y(), v1.z());
                    }            
                }
            } // End of Tube case          
            else if (s == Sphere)            
            {
                double rin = params[6];
                double rout = params[7];
                double phimin = params[8];
                double phimax = params[9];
                double thetamin = params[10];
                double thetamax = params[11];

                HepPoint3D v;
                HepPoint3D v1;

                unsigned int m = 10;
                unsigned int n = 20;
                double anglet, anglep;

                if (type == Simple)
                {
                    for(unsigned int j=0; j<m+1; j++)
                    {
                        for(unsigned int i=0; i<n+1; i++)
                        {
                            anglet = thetamin + (thetamax-thetamin)*((double)j/m);
                            anglep = phimin + (phimax-phimin)*((double)i/n);

                            v.setX(rin*sin(anglet)*cos(anglep)); 
                            v.setY(rin*sin(anglet)*sin(anglep)); 
                            v.setZ(rin*cos(anglet)); 
                            v1 = (atr*v);
                            m_builder->addPoint(v1.x(), v1.y(), v1.z());

                            v.setX(rout*sin(anglet)*cos(anglep)); 
                            v.setY(rout*sin(anglet)*sin(anglep)); 
                            v.setZ(rout*cos(anglet)); 
                            v1 = (atr*v);
                            m_builder->addPoint(v1.x(), v1.y(), v1.z());
                        }
                    }
                }
            }// End of Sphere case

        }            
        m_actualTransform.push_back(atr);
    }
    else
        return AbortSubtree;

    if ((m_typesList.size() == 0) && (m_actualDepth == m_depth))
        return AbortSubtree;      
    else 
        return More;
}


void HepRepGeometry::popShape()
{
    if(m_hrMode == "type")
    {
        m_actualType.pop_back();
    }
    else if (m_hrMode == "instance")
    {
        m_actualTransform.pop_back();      
        m_actualInstance.pop_back();
    }

    m_actualDepth--;

}

void HepRepGeometry::reset()
{
    m_types.clear();
    m_actualType.clear();
    m_actualInstance.clear();
    m_actualDepth = 0;
}
