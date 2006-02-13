//====================================================================
//  FluxSvc_load.cpp
//--------------------------------------------------------------------
//
//  Package    :  HepRepSvc
//
//  Description: Implementation of <Package>_load routine. This routine 
//               is needed for forcing the linker to load all the components
//               of the library.. 
//
//====================================================================


#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_FACTORY_ENTRIES(HepRepSvc) 
{
    DECLARE_SERVICE( HepRepSvc   );
}


//#include "GaudiKernel/ICnvFactory.h"
//#include "GaudiKernel/ISvcFactory.h"
//#include "GaudiKernel/IAlgFactory.h"


//#define DLL_DECL_SERVICE(x)    extern const ISvcFactory& x##Factory; x##Factory.addRef();
//#define DLL_DECL_CONVERTER(x)  extern const ICnvFactory& x##Factory; x##Factory.addRef();
//#define DLL_DECL_ALGORITHM(x)  extern const IAlgFactory& x##Factory; x##Factory.addRef();


//void HepRepSvc_load() {
//    DLL_DECL_SERVICE( HepRepSvc );
//}

//extern "C" void HepRepSvc_loadRef() {
//    HepRepSvc_load();
//}
