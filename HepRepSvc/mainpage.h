// $Header: /nfs/slac/g/glast/ground/cvs/HepRepSvc/HepRepSvc/mainpage.h,v 1.4 2004/08/02 08:39:40 riccardo Exp $
// Mainpage for doxygen

/** @mainpage package HepRepSvc

  @authors R.Giannitrapani, L.Rochester, others
 
  @section description Description

  This package implements a GAUDI service to provide HepRep functionalities for
  GLAST. HepRep is a standard HEP format for graphic description of an event; it
  is easy, extendable and flexible enough to accommodate all the possible event
  features. 

  HepRepSvc helps to build an HepRep representation that can be
  passed (in various ways) to an external graphical client for event
  display; known HepRep-compliant clients are WIRED (100% HepRep
  compliant) and FRED (not completely HepRep compliant, but almost).

  HepRepSvc works as follows: an internal register holds a list of IFiller
  implementations that can be called to actually build an HepRep representation
  of the Event. These fillers can use Gaudi services and the TDS to retrieve
  relevant information; for example two of the fillers provided
  (MonteCarloFiller and GeometryFiller) use the GlastDetSvc and the TDS to build
  a HepRep representation of the LAT geometry and of the MonteCarlo data
  present in the TDS (mainly collections of McPositionHit, McIntegratingHit and
  McParticle).

  The fillers can build the HepRep representation in various formats; the
  abstraction is provided by the use of the IBuilder interface, so that the
  concrete fillers do not depend on any concrete implementation of
  HepRep. 

  Runtime the HepRepSvc can load different implementations of the
  HepRep format; for now we are providing an Xml one (for persistency)
  and a CORBA one (for interactive sessions). More implementations can
  come in the future. 

  To have a complete decoupling of this Gaudi service and the concrete
  implementations, HepRepSvc exposes the following abstract
  interfaces:

    - <b>IFiller</b>: the basic filler abstract class to be used to
      derive a concrete filler.

    - <b>IBuilder</b>: this one is used by the fillers to
      delegate at runtime the actual building of the HepRep 

    - <b>IRegistry</b>: the interface to the Registry object that holds
     and manages the list of registered fillers.

    - <b>IRegister</b>: an abstract interface to a Gaudi Tool that can
     be used to register a concrete implementation of the HepRep
     format

    - <b>IStreamer</b>: an interface to a concrete implementation of
      the HepRep; a streamer is not interactive, it just streams out the
      HepRep on a file directly by command or automatically at the end
      of every event (see the property autoStream later). The command
      to stream a file is:
      <p>
      HepRepSvc::saveHepRep(std::string streamerName, std::string fileName);
      <p>
      where streamerName is the name that identifies the streamer, and
      fileName is the name of the file to save into; note that the
      fileName must not include the extension, is up to the streamer to
      add the proper extension.
      <p>
      Example of a streamer is the Xml one that can be found in the
      HepRepXml package.
      <p>
      HepRepSvc supports any number of registered streamers.

    - <b>IServer</b>: an interface to a concrete implementation of the
     HepRep; a server is an interactive session of HepRep, and it is
     supposed to be connected by some client in order to work. As
     such, if a server is registered to the HepRepSvc it will be in
     charge of managing the event loop (if the HepRepSvc has been set as
     runnable in the jobOptions). HepRepSvc supports only one server at
     a time, so the last one registered is the one used.
	

  A concrete IServer or IStreamer must be registered to the HepRepSvc
  in order to be usable; this can be done with a Gaudi Tool inheriting
  from IRegister.
 
  To use this service just include it in the list of services in a
  GAUDI job and include in the DLL list as many streamers or servers as
  you need.

  Please note that the production of HepRep (both as XML or CORBA) can have a
  big impact on the job performance; the normal use of the HepRepSvc is
  during the analysis of a particular event that the user wants to browse, not
  during production.

  For a concrete example of usage of this service, look at the
  HepRepXml package.

  @section properties properties

  There are 3 properties that can be set in the jobOptions.txt file:

  - <b>HepRepSvc.autoStream</b>: a string that specifies whether or not the HepRep must
    be automatically saved to a file at the end of each event;
    if the string is null this feature is disabled (the default),
    otherwise the string specifies the name of the streamer to be
    used. 

  - <b>HepRepSvc.streamPath</b>: a string that sets the local path of the
    saved streamer files; please note that you must provide a valid
    path for your operating system and that the path must end with a
    "\\" or "/" depending on your OS
    (ex. streamPath="c:\\riccardo\\xmlfiles\\" or
    streamPath="/scratch/users/riccardo/xmlfiles/"). By default it's empty,
    so that the files are typically saved in the cmt of HepRepSvc

  - <b>unsigned HepRepSvc.geometryDepth</b>: an unsigned integer that specifies the depth to
    which the geometry filler will descend into the geometry tree of GLAST. By
    default it's set to 4. Please remember that a too high depth means lots of
    volumes (that can be dangerous for both performances of the graphical
    clients and memory usage)

  HepRepInitSvc provides a mechanism for passing jobOption parameters to the fillers,
  which are not Gaudi components. One such paramater is currently implemented:

  - <b>bool HepRepInitSvc.VertexFiller_dashes</b> causes the extrapolated vertices to be drawn
  as dashed lines. Default is false.

  @section heprep What is HepRep?

  HepRep is an abstract protocol suitable to describe a high energy physics 
  event (although it is quite general and can be useful for other kinds of 
  situations). This abstract protocol can be implemented in various ways, from
  a persistent file (e.g. XML) to a communication protocol between clients and
  servers (such as CORBA). You can look at it as a format that is quite flexible to 
  describe a hierarchy of graphics objects, each one with various graphics attributes
  (color, shading, etc.) and possibly augmented with physics attributes.

  A HepRep event is formed from a certain number of trees of two possible kinds:

  - <b>TypeTree</b>: this kind of tree specifies the types of objects that you
    can represent in your event (a track, a geometry box and so on); it is a tree, so 
    that you can have a hierarchy (for example for the geometry, or for the montecarlo 
    tree, and so on). Each TypeTree has a name and a version (contained in a compact 
    object called <e>id</e>), and each TypeTree, as we have said, contains a hierarchy
    of <b>Type</b>s that we will describe in details later. The important thing to know
    is that a TypeTree doesn't represent actual objects, just possible types of 
    objects.
  
  - <b>InstanceTree</b>: this second kind of tree represents a true hierarchy of "real"
    objects, instances of the types described in a TypeTree; this InstanceTree is
    always connected (by a property) to a TypeTree and its objects, the <b>Instance</b>s,
    are of the Type described by the that TypeTree.


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

