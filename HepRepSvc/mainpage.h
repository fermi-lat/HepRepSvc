// $Header: /nfs/slac/g/glast/ground/cvs/FluxSvc/FluxSvc/mainpage.h,v 1.5 2002/04/04 19:10:28 srobinsn Exp $
// Mainpage for doxygen

/** @mainpage package HepRepSvc

  @authors R.Giannitrapani
 
  @section description Description

  This package implements a GAUDI service to provide HepRep functionalities for
  GLAST. HepRep is a standard HEP format for graphic description of an event; it
  is easy, extendable and flexible enought to accomodate all the possible event
  features. 

  The HepRepSvc help to build an HepRep representation that can be passed (in
  various ways) to an external graphical client for event display; known HepRep
  compliant client are WIRED (100% HepRep compliant) and FRED (90% HepRep
  compliant).

  The HepRepSvc works as follows: an internal register hold a list of IFiller
  implementations that can be called to actually build an HepRep representation
  of the Event. These fillers can use Gaudi services and the TDS to retrieve
  relevant informations; for example the two fillers provided at the moment
  (MonteCarloFiller and GeometryFiller) use the GlastDetSvc and the TDS to build
  an HepRep representation of the LAT geometry and of the MonteCarlo data
  present in the TDS (mainly collections of McPositionHit, McIntegratingHit and
  McParticle).

  The fillers can build the HepRep representation in various format; the
  abstraction is provided by the use of the IBuilder interface, so that the
  concrete fillers does not depend on any concrete implementation of the
  HepRep. The HepRepSvc provide one format as XML persistency files; a CORBA
  format is provided in another package (CorbaServer).

  To use this service just include it in the list of services in a GAUDI job.

  Please note that the production of HepRep (both as XML or CORBA) can have a
  big impact on the job performance; the normal use of the HepRepSvc so is
  during the analysis of a particular event that the user want to browse, not
  during production of big runs.

  The actual version of the HepRepSvc is quite incomplete and it has been
  committed on the repository for test purpouses. Expect big changes and
  improvments in future releases.

  @section properties properties

  There are 3 properties that can be set in the jobOptions.txt file:

  - <b>saveXml</b>: it is a boolean and specify if the XML persistency files
    must be saved or not. By default is 0.

  - <b>xmlPath</b>: it is a string that set the local path where to save the XML
    files; please note that you must provide a valid path for your operating
    system and that the path must end with a "\\" or "/" depending on your os
    (ex. xmlPath="c:\\riccardo\\xmlfiles\\" or
    xmlPath="/scratch/users/riccardo/xmlfiles/"). By default is empty, so that
    the files are saved in the home of HepRepSvc

  - <b>geometryDepth</b>: it is an unsigned integer that specify the depth to
    which the geometry filler will descend into the geometry tree of GLAST. By
    default is set to 4. Please remember that a too high depth means lots of
    volumes (that can be dangerous for both performances of the graphical
    clients and memory usage)

  @section test test

  A simple test program produce an XML HepRep file with the LAT geometry (no event). 
  
  <br>

  <br>
  <hr>
  @section notes release.notes
  release.notes
  <hr>
  @section requirements requirements
  @verbinclude requirements
  <hr>
*/

