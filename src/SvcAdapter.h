// $Header:

#ifndef SVCADAPTER_H
#define SVCADAPTER_H

#include <vector>
#include <string>
#include "HepRepSvc/ISvcAdapter.h"

class HepRepSvc;

/* @class: SvcAdapter 
 *
 * @brief: The HepRepSvc service adapter
 *
 * @author:R.Giannitrapni
 */ 
class SvcAdapter: public ISvcAdapter
{
 public:
  SvcAdapter(HepRepSvc* h):m_hrsvc(h){};
  virtual void nextEvent(int);
  virtual std::vector<std::string>& getSources();
  virtual void setSource(std::string);
  
 private:
  HepRepSvc* m_hrsvc;
};

#endif //SVCADAPTER_H
