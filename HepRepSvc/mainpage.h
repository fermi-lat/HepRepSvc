// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/HepRepSvc/mainpage.h,v 1.3 2004/05/20 08:13:47 riccardo Exp $
// Mainpage for doxygen

/** @mainpage package HepRepSvc

  @authors R.Giannitrapani
 
  @section description Description

  This package implements a GAUDI service to provide HepRep functionalities for
  GLAST. HepRep is a standard HEP format for graphic description of an event; it
  is easy, extendable and flexible enought to accomodate all the possible event
  features. 

  The HepRepSvc help to build an HepRep representation that can be
  passed (in various ways) to an external graphical client for event
  display; known HepRep compliant client are WIRED (100% HepRep
  compliant) and FRED (not completely HepRep compliant, but almost).

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
  HepRep. 

  Runtime the HepRepSvc can load different implementations of the
  HepRep format; for now we are providing an Xml one (for persistency)
  and a CORBA one (for interactive sessions). More implementations can
  come in the future. 

  To have a complete decoupling of this Gaudi service and the concrete
  implementations, HepRepSvc exposte the following abstract
  interfaces:

    - <b>IFiller</b>: the basic filler abstract class to be used to
      derive a concrete filler.

    - <b>IBuilder</b>: this one is used by the fillers mechanism to
      delegate runtime the real building of the HepRep 

    - <b>IRegistry</b>: the interface to the Registry object that holds
     and manage the list of registered fillers.

    - <b>IRegister</b>: an abstract interface to a Gaudi Tool that can
     be used to register a concrete implementation of the HepRep
     format

    - <b>IStreamer</b>: an interface to a concrete implementation of
      the HepRep; a streamer is not interactive, it just stream out the
      HepRep on a file directly by command or automatically at the end
      of every event (see the property autoStream later). The command
      to stream a file is the following one
      <p>
      HepRepSvc::saveHepRep(std::string streamerName, std::string fileName);
      <p>
      where streamerName is the name that identify the streamer, while
      fileName is the name of the file to save on; note that the
      fileName must not include the extension, is up to the streamer to
      add the proper extension.
      <p>
      Example of streamer is the Xml one that can be found in the
      HepRepXml package.
      <p>
      HepRepSvc support any number of registered streamer.

    - <b>IServer</b>: an interface to a concrete implementation of the
     HepRep; a server is an interactive session of HepRep, and it is
     supposed to be connected by some client in order to work. As
     such, if a server is registered to the HepRepSvc it will be in
     charge to manage the event loop (if the HepRepSvc has been set as
     runnable in the jobOptions). HepRepSvc support only one server at
     a time, so the last one registered is the one used.
	

  A concrete IServer or IStreamer must be registered to the HepRepSvc
  in order to be usable; this can be done with a Gaudi Tool hineriting
  by the IRegister one.
 
  To use this service just include it in the list of services in a
  GAUDI job and include in the DLL list as many streamer or server do
  you need.

  Please note that the production of HepRep (both as XML or CORBA) can have a
  big impact on the job performance; the normal use of the HepRepSvc so is
  during the analysis of a particular event that the user want to browse, not
  during production of big runs.

  For a concrete example of usage of this service, look at the
  HepRepXml package.

  @section properties properties

  There are 3 properties that can be set in the jobOptions.txt file:

  - <b>autoStream</b>: it is a string that specify if the HepRep must
    be saved or not on a file automatically at the end of each event;
    if the string is null this feature is disabled (the default),
    otherwise the string specify the name of the streamer to be
    used. 

  - <b>streamPath</b>: it is a string that set the local path where to
    save the streamer files; please note that you must provide a valid
    path for your operating system and that the path must end with a
    "\\" or "/" depending on your os
    (ex. streamPath="c:\\riccardo\\xmlfiles\\" or
    streamPath="/scratch/users/riccardo/xmlfiles/"). By default is empty,
    so that the files are saved in the cmt of HepRepSvc

  - <b>geometryDepth</b>: it is an unsigned integer that specify the depth to
    which the geometry filler will descend into the geometry tree of GLAST. By
    default is set to 4. Please remember that a too high depth means lots of
    volumes (that can be dangerous for both performances of the graphical
    clients and memory usage)

  @section heprep What is HepRep?

  HepRep is an abstract protocol suitable to describe an high energy physics 
  event (althought it is quite general to be useful also for other kind of 
  situations). This abstract protocol can be implemented in various way, from
  a persistent file (like XML) to a communication protocol between clients and
  servers (like in CORBA). You can see it as a format that is quite flexible to 
  describe a hierarchy of graphics object each one with various graphics attributes
  (color, shading etc.etc) and possibly augmented with physics attributes.

  An HepRep event is formed by a certain number of trees of two possible kind:

  - <b>TypeTree</b>: this kind of tree specify what are the types of object that you
    can represent in your event (a track, a geometry box and so on); it is a tree, so 
    that you can have a hierarchy (for example for the geometry, or for the montecarlo 
    tree, and so on). Each TypeTree has a name and a version (contained in a compact 
    object called <e>id</e>) and each TypeTree, as we have said, contains a hierarchy
    of <b>Type</b> that we will describe in details later. The important thing to know
    is that a TypeTree doesn't represent true objects, but just possible types of 
    objects.
  
  - <b>InstanceTree</b>: this second kind of tree represent a true hierarchy of "real"
    objects, instances of the types described in a TypeTree; for this an InstanceTree is
    always connected (by a property) to a TypeTree and its objects, the <b>Instance</b> 
    ones, are of the Type described by the that TypeTree.

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

