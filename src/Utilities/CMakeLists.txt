
add_library(Utilities
				patPostGreSqlDataType.cc
                patTimeFunctions.cc
                patCoordinates.cc
                patDisplay.cc
                patTransportMode.cc
                patCreateKmlStyles.cc
                patListDirectory.cc
                patNBParameters.cc
                patPostGISDataType.cc
                patPostGreSQLConnector.cc
               patSampleFromDiscreteUntilCdf.cc
                )
#target_link_libraries (Utilities  ${LIB_EXTRA})
