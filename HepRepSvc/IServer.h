// $Header:
//
// Description: 
//
// Author:R.Giannitrapni

#ifndef ISERVER_H
#define ISERVER_H

#include <string>
#include <vector>
#include <map>

class IRegistry;
class ISvcAdapter;

/** 
 * @class IServer
 *
 * @brief A server interface for interactive HepRep implementations
 *    
 */
class IServer
{
 public:

  /// Set the fillers registry
  void setRegistry(IRegistry* r){m_registry = r;};
  /// Set the Gaudi Service Adapter
  void setSvcAdapter(ISvcAdapter* s){m_svcAdapter = s;};
  /// Get the SvcAdapter
  const ISvcAdapter* getSvcAdapter(){return m_svcAdapter;};

  /// The pure virtual method run
  virtual void run() = 0;

 protected:
  IRegistry* m_registry;
  ISvcAdapter* m_svcAdapter;
};

#endif //ISERVER_H
