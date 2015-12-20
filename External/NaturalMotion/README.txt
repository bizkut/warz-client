Welcome to the Morpheme Runtime distribution
============================================

The documentation for the product is located in the docs sub-folder.
 * For information on what has changed in this version, refer to CHANGES.txt.
 * For details on how these changes may affect your use of morpheme, please refer to UPGRADE.txt

To get started with looking at and building code there are some key locations where Visual Studio solutions (and build files for other platforms) are located:

  - morpheme/samples/tutorials
  - morpheme/tools/assetCompiler
  - morpheme/tools/runtimeTarget

The basic contents of this distribution are as follows:

\---NaturalMotion
    +---common                        <- General purpose utility libraries
    |   +---NMNumerics                  <- Algorithms to support offline processing
    |   +---NMPlatform                  <- Cross platform base library for use in runtime
    |   +---NMRuntimeUtils              <- Algorithms to support runtime processing
    |   +---NMTL                        <- Template library for containers
    |   +---tinyxml                     <- A slightly modified version of TinyXML
    |   \---XMD                         <- Library for reading/writing XMD format
    +---docs                          <- Detailed documentation, CHANGES.txt and UPGRADE.txt
    \---morpheme                      <- all morpheme libraries and tools
        +---samples                     <- Tutorials
        +---SDK                         <- Core morpheme SDK
        |   +---assetProcessor            <- Support library for assetCompiler
        |   +---core                      <- Core morpheme library and nodes
        |   +---export                    <- Support library for assetCompiler for reading connect-exported XML assets
        |   +---physics                   <- Core morpheme physics support library. Only available in skus with physics.
        |   \---sharedDefines             <- Global morpheme runtime headers, eg mVersion.h
        +---tools                       <- Offline tools and runtime targets
        |   +---assetCompiler             <- Core asset compilation tool. Generates runtime assets for target platforms.
        |   +---AssetExporter             <- C# GUI app for batch processing assets with morpheme connect and asset compilers
        |   +---debugging                 <- Visual Studio debugging macros
        |   +---NodeWizard                <- C# GUI app for quickly generating runtime nodes
        |   \---runtimeTarget             <- Reference implementation app to host runtime on target platforms
        \---utils                       <- Utility libraries
            +---comms2                    <- Comms layer for network communication with runtime
            +---physicsComms2             <- As above, including physics support. Only available in skus with physics.
            \---simpleBundle              <- Library for managing bundling runtime assets into a single file

For support and sales enquiries, contact us via the Support Portal, send us an email or give us a call.

Support Portal: https://support.naturalmotion.com/
Downloads: https://files.naturalmotion.com/
Email: morphemesupport@naturalmotion.com 
Phone: (UK) +44 1865 250 575 

Address:
NaturalMotion Ltd
10 St. Ebbes Street
Oxford
OX1 1PT
United Kingdom


