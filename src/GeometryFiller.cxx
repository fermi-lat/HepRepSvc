#include "GlastSvc/GlastDetSvc/IGlastDetSvc.h"
#include "HepRepGeometry.h"
#include "GeometryFiller.h"
#include "HepRepSvc/IBuilder.h"

#include "HepRepSvc/HepRepInitSvc.h"

#include <algorithm>

GeometryFiller::GeometryFiller( unsigned int depth,
                               HepRepInitSvc* hrisvc,
                               IGlastDetSvc* gsvc):m_gsvc(gsvc)
{
  m_geometry = new HepRepGeometry(depth, this);
}

void GeometryFiller::buildTypes()
{
  m_geometry->setBuilder(m_builder);
  m_geometry->setHepRepMode("type"); 
  m_gsvc->accept(*m_geometry);
  m_geometry->reset();
}


void GeometryFiller::fillInstances (std::vector<std::string>& typesList)
{
  m_geometry->setBuilder(m_builder);
  m_geometry->setHepRepMode("instance"); 
  m_geometry->setTypesList(typesList);
  m_gsvc->accept(*m_geometry);  
  m_geometry->reset();
}
