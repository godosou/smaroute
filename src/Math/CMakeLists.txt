include_directories("${NewBioroute_SOURCE_DIR}/Utilities")
add_library(Math
                patLogProbaToProba.cc
                patMixtureNormal.cc
                patRandomNumber.cc
                patSampleDiscreteDistribution.cc)
