// File and Version Information:
// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/src/HepRepGeometry.cxx,v 1.6 2004/07/21 15:31:29 riccardo Exp $
//
// Author(s):
//      R.Giannitrapani

#include "HepRepGeometry.h"
#include "HepRepSvc/IBuilder.h"


#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "CLHEP/Geometry/Transform3D.h"

#include <iomanip>
#include <algorithm>

HepRepGeometry::HepRepGeometry(unsigned int depth, std::string mode): 
  m_mode(mode), m_depth(depth)
{
  m_actualDepth = 0;
  m_builder = 0;
  HepTransform3D start;
  m_actualTransform.push_back(start);
}

HepRepGeometry::~HepRepGeometry()
{

}

IGeometry::VisitorRet 
HepRepGeometry::pushShape(ShapeType s, const UintVector& idvec, 
                      std::string name, std::string material, 
                      const DoubleVector& params, VolumeType type)
{
  if(m_hrMode == "type")
    {
      std::vector<std::string>::const_iterator i;
      i = std::find(m_types.begin(),m_types.end(),name);
      if (i == m_types.end())
        {
          m_types.push_back(name);          
          std::string father;
          if (m_actualType.size() == 0)
            father = "";
          else
            father = m_actualType.back();
          
          m_actualDepth++;

          m_actualType.push_back(name);          
          m_builder->addType(father,name,"A volume of the geometry; it can be simple, a composition or a stack","");


					// The color is set only to the root of the geometry, so it will be possible
					// to change the color of subtypes 
					if (name == "LAT")
						m_builder->addAttValue("Color","0.98,0.92,0.8","");

          m_builder->addAttDef("Volume type","The kind of volume (simple, composition, stack or sensitive)","Physics","");
          m_builder->addAttDef("Material","The material name of the volume","Physics","");
          m_builder->addAttDef("Shape","At the moment this can be just a Box","Physics","");
          
          m_builder->addAttValue("Material",material,"");
          
          switch(type)
            {
            case Simple:
              m_builder->addAttValue("Volume type","Simple","");
              break;
            case posSensitive:
              m_builder->addAttValue("Volume type","posSensitive","");
              break;
            case intSensitive:
              m_builder->addAttValue("Volume type","intSensitive","");
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
          
          switch(s)
            {
            case Box:
              m_builder->addAttValue("DrawAs","Prism","");
              m_builder->addAttValue("Shape","Box","");
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
      double x=params[0], y=params[1], z=params[2];
      double rx=params[3], ry=params[4], rz=params[5];  
      
      HepRotation rot(rx*M_PI/180, ry*M_PI/180, rz*M_PI/180);
      Hep3Vector t(x, y, z);  
      HepTransform3D tr(rot,t);
      HepTransform3D atr = (m_actualTransform.back())*tr;

      std::string father;
      if (m_actualInstance.size() == 0)
        father = "";
      else
        father = m_actualInstance.back();

      m_actualInstance.push_back(name);
      m_actualDepth++;            

      m_builder->addInstance(father,name);
      
      if ((s == Box))
        {
          double dx = params[6]/2;
          double dy = params[7]/2;
          double dz = params[8]/2;

          HepPoint3D v;
          HepPoint3D v1;
      
          if ((((type == Simple) || (type == posSensitive) || (type == intSensitive)) 
               && (m_actualDepth < m_depth)) || (m_actualDepth == m_depth)) 
            
            {
          
              v.setX(dx); v.setY(dy); v.setZ(dz); v1 = (atr*v);
              m_builder->addPoint(v1.x(), v1.y(), v1.z());
              v.setX(-dx); v.setY(dy); v.setZ(dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());
              v.setX(-dx); v.setY(-dy); v.setZ(dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());
              v.setX(dx); v.setY(-dy); v.setZ(dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());

          
              v.setX(dx); v.setY(dy); v.setZ(-dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());
              v.setX(-dx); v.setY(dy); v.setZ(-dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());              
              v.setX(-dx); v.setY(-dy); v.setZ(-dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());
              v.setX(dx); v.setY(-dy); v.setZ(-dz); v1 = atr*v;
              m_builder->addPoint(v1.x(), v1.y(), v1.z());              
            }
        }
      m_actualTransform.push_back(atr);
    }

  if (m_actualDepth == m_depth)
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


