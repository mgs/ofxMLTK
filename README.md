ofxMLTK
=====================================

Introduction
------------
A Machine Listening Toolkit for OpenFrameworks. The goal of this project is to provide a collection of wrappers, interfaces, and tools for applying machine listening techniques in OpenFrameworks projects. The addon currently uses the Essentia library for audio processing, however it differs from other addons using essentia byy exposing the stream processing architecture. This mode allows you to define data-flow graphs by connecting algorithm node's through their inputs and outputs. A scheduler handles efficient processing of the analysis chain and dumps values into a thread-safe Pool data structure which can then be accessed at your pleasure.

The add-on is currently setup for real-time analysis but will handle off-line tasks and recording data to external files in the very near future.

License
-------
See LICENSE files included in the libs directory. This addon cannot currently be used in commercial project due to the Essentia library. Consult with UPF MTG for information about commercially licensing Essentia. For non-commercial projects, Essentia's GNU Affero GPLv3 License has information on how to attribute usage.

Installation
------------
Just drop the 'ofxMLTK' folder into the `openFrameworks/addons/` folder. Documentation will be coming soon but in the meantime an example is included.

Dependencies
------------
No other addons are needed. Static libraries are included, compilation instructions coming soon.

Compatibility
------------
OpenFrameworks v0.10+

Known issues
------------

Version history
------------

### Version 0.1 (Date):
Initial Release
