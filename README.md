ofxMLTK
=====================================

Introduction
------------
A Machine Listening Toolkit for openFrameworks. The goal of this project is to provide a collection of wrappers, interfaces, and tools for applying machine listening techniques within openFrameworks projects. The addon uses the Essentia library for audio processing, however it differs from another Essentia based addon by using the stream processing architecture mode. This mode allows you to define data-flow graphs by programatically connecting algorithm nodes through their inputs and outputs. A scheduler handles processing of the analysis chain allowing for increased efficiency (performance). Analysis values are outputted throughout flow into a Pool data structure which is thread-safe and can be accessed similarly to a hash-table or JSON. This makes the data structure perfect for real-time applications.

The add-on and example are currently setup for real-time applications. For offline tasks, it may be more comfortable to use Essentia's Python bindings or standalone applications. 

License
-------
See LICENSE files included in the libs directory. Consult with UPF MTG for information about commercially licensing Essentia. For non-commercial projects, Essentia's GNU Affero GPLv3 License has information on how to attribute usage.

Installation
------------

1) Drop the 'ofxMLTK' folder into the `openFrameworks/addons/`
2) Use OF Project Generator to add ofxMLTK to a new project and make sure it appears when regenerating the example project.

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
