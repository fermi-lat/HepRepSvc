#ifndef _H_IRegister
#define _H_IRegister

// includes
#include "GaudiKernel/IAlgTool.h"

class IHepRepSvc;

static const InterfaceID IID_IRegister("IRegister", 1 , 0); 

/** 
 * @class IRegister
 *
 * @brief The Gaudi tool interface to be used to register a concrete
 * streamer or server
 *
 *    
 */
class IRegister : virtual public IAlgTool {
public:
    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IRegister; }
    /// Pass a pointer to the service to the tool. 
    virtual StatusCode registerMe(IHepRepSvc*) = 0;
};

#endif  // _H_IRegister
