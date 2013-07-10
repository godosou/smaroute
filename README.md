newbioroute
===========

This software package contains implementations of route choice modeling methods for smarpthone data, including GPS, accelerometer and bluetooth. The methods are described in Jingmin Chen's PhD thesis "Modeling Route Choice Behavior Using Smartphone Data". Please visit *[EPFL](http://infoscience.epfl.ch/record/183171)* for the manuscript.

Specifically, the software package contains three programs:
* pmm (probabilisic multimodal mapmatching).
* bioroute (biogeme route choice modeling).
* mr.osm (multimodal routeable openstreepmap).

Functionalities
---------------

Installation
------------
The software is implemented in C++. The building environment is [cmake](http://www.cmake.org/) with gcc >4.4. The installation has been tested on Mac OSX (10.6-10.8) and Linux (Ubuntu 10.04.4 LTS).

Requirements. 

* Boost > 1.49
* biogeme 
* Postgresql > 9.0
* libpqxx
* Nnu GSL
* libshp
* libkml
* libconfig
* xmlwrapp
* CXXTest
* libgsl
It is likely that cmake fails to find some dependencies. In this case, please update the corresponding package look-up file in: 382ea014078d0750c6e415f512412cb270e06093 . For more details about how to find dependencies in cmake environemnt, read [cmake documentation](http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries).

MAC OS X

The software has been tested on Mac OSX 10.7, and 10.8. In Mac OSX, the XCode and command line tool have to be installed. The link to the header files of XCode should be given in src/CMakeLists.txt, for example:

   include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/usr/include/")


Usage
-----
