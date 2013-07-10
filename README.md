Introduction
===========

This software package contains implementations of route choice modeling methods for smarpthone data, including GPS, accelerometer and bluetooth. The methods are described in Jingmin Chen's PhD thesis "Modeling Route Choice Behavior Using Smartphone Data". Please visit *[EPFL](http://infoscience.epfl.ch/record/183171)* for the manuscript.

Specifically, the software package contains three programs:
* pmm (probabilisic multimodal map-matching).
* bioroute (biogeme route choice modeling).
* mr.osm (multimodal routeable openstreepmap).

Functionalities
===============

pmm
---------------
pmm standards for "probabilistic multimodal map matching", proposed by Dr. Jingmin Chen and Prof. Michel Bierlaire. It inferes the travelled path and the transport mode of each road simutaneously from smartphone data recorded during a trip. They key features are:
* Infer the traveled path.
* Infer the tranpsort mode of each road, if it is unknown. The tranpsort modes of different roads can be different (multimodal trip). Currenlty, walk, bike, car, bus and metro modes can be recognized.
* Produce a meaninngful likelihood of each path, representing the likelihood that the data have been recorded during the path. 

Several types of data are used:
* GPS data contributes to the identification of both road and mode.
* Transport network and public transport lines, contribute to the identification of mode, besides the road of course.
* Bluetooth data (number of nearby Bluetooth devices), contributes to differentiate public/private transport.
* Accelerometer data, contributes to differentiate motor modes, walk and bike. 
The framework is also capable of integrating more kinds of data if they contribute to identify road/mode.

Please cite the following papers, if you use this program:
* Bierlaire, M., Chen, J., and Newman, J. P (2013). A Probabilistic Map Matching Method for Smartphone GPS data, Transportation Research Part C: Emerging Technologies 26:78–98.
* Chen, J., and Bierlaire, M. (to appear). Probabilistic multimodal map-matching with rich smartphone data, Journal of Intelligent Transportation Systems (accepted for publication on October 22, 2012).


bioroute
---------
bioroute implements several path alternative sampling algorithms for discrete route choice modeling. It also provides interfaces to biogeme by generating biogeme input files from GPS data, map matched path observations, and path alterantive sampling results. The key features include:

* Accept GPS data and pmm results as inputs of route choice observations.
* Implements several importance sampling algorithms for path sampling, including: [Random Walk][], [Metropolis-Hastings][] path.
* Generates biogeme inputs (.mod and .dat files) for discret choice model estimation.

   [Random Walk]: http://www.sciencedirect.com/science/article/pii/S0191261509000381 "E. Frejinger, M. Bierlaire, M. Ben-Akiva, Sampling of alternatives for route choice modeling, Transportation Research Part B: Methodological, Volume 43, Issue 10, December 2009, Pages 984-99"
   [Metropolis-Hastings]: http://www.sciencedirect.com/science/article/pii/S019126151200152X "Gunnar Flötteröd, Michel Bierlaire, Metropolis–Hastings sampling of paths, Transportation Research Part B: Methodological, Volume 48, February 2013, Pages 53-66."


mr.osm
-------
mr.osm stands for multimodal routable openstreetmap (OSM). The public tranposrt lines in OSM are only for visualization purpose. This program will recognize the data into directed graph structure, which can be used for routing purpose (useful for pmm).


Installation
==============

The software is implemented in C++. The building environment is [cmake](http://www.cmake.org/) with gcc >4.4. The installation has been tested on Mac OSX (10.6-10.8) and Linux (Ubuntu 10.04.4 LTS).

Requirements
-------------

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
It is likely that cmake fails to find some dependencies. In this case, please update the corresponding package look-up file in: src/cmake/Modules/. For more details about how to find dependencies in cmake environemnt, read [cmake documentation](http://www.cmake.org/Wiki/CMake:How_To_Find_Libraries).

MAC OS X
-------------
The software has been tested on Mac OSX 10.7, and 10.8. In Mac OSX, the XCode and command line tool have to be installed. The link to the header files of XCode should be given in src/CMakeLists.txt, for example:

   `include_directories("/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk/usr/include/")`

Build
---------
This is a typical cmake building procedure:
* Create and Enter a folder (BUILD_FOLDER) outside of the source code directory.
* Run $cmake PATH_TO_SRC_FOLDER
* You will find executables in BUILD_FOLDER/bin


