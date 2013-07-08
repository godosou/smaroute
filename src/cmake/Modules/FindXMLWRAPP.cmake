# Try to find xmlwrapp C++ library
# See 
# http://sourceforge.net/projects/xmlwrapp/
#
# Once run this will define: 
# 
# XMLWRAPP_FOUND       = system has XMLWRAPP lib
#
# XMLWRAPP_LIBRARIES   = full path to the libraries
#    on Unix/Linux with additional linker flags from "XMLWRAPP-config --libs"
# 
# CMAKE_XMLWRAPP_CXX_FLAGS  = Unix compiler flags for XMLWRAPP, essentially "`XMLWRAPP-config --cxxflags`"
#
# XMLWRAPP_INCLUDE_DIR      = where to find headers 
#
# XMLWRAPP_LINK_DIRECTORIES = link directories, useful for rpath on Unix
# XMLWRAPP_EXE_LINKER_FLAGS = rpath on Unix
#
# Jingmin Chen September 2011
# jingmin.chen@epfl.ch
# --------------------------------

  
  IF(UNIX) 
    SET(XMLWRAPP_CONFIG_PREFER_PATH 
      "$ENV{XMLWRAPP_DIR}/bin"
      "$ENV{XMLWRAPP_DIR}"
      "$ENV{XMLWRAPP_HOME}/bin" 
      "$ENV{XMLWRAPP_HOME}" 
      CACHE STRING "preferred path to XMLWRAPP (xmlwrapp-config)")
    FIND_PROGRAM(XMLWRAPP_CONFIG XMLWRAPP-config
      ${XMLWRAPP_CONFIG_PREFER_PATH}
      /usr/bin/
      )
    # MESSAGE("DBG XMLWRAPP_CONFIG ${XMLWRAPP_CONFIG}")
    
    IF (XMLWRAPP_CONFIG) 
      # set CXXFLAGS to be fed into CXX_FLAGS by the user:
      SET(XMLWRAPP_CXX_FLAGS "`${XMLWRAPP_CONFIG} --cflags`")
      
      # set link libraries and link flags
      SET(XMLWRAPP_LIBRARIES "`${XMLWRAPP_CONFIG} --libs`")
      
      # extract link dirs for rpath  
      EXECUTE_PROCESS(
      COMMAND ${XMLWRAPP_CONFIG} --libs
        OUTPUT_VARIABLE XMLWRAPP_CONFIG_LIBS )
        MESSAGE(${XMLWRAPP_CONFIG_LIBS})
      # split off the link dirs (for rpath)
      # use regular expression to match wildcard equivalent "-L*<endchar>"
      # with <endchar> is a space or a semicolon
      STRING(REGEX MATCHALL "[-][L]([^ ;])+" 
        XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX 
        "${XMLWRAPP_CONFIG_LIBS}" )
    #       MESSAGE("DBG  XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX=${XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX}")
    MESSAGE(${XMLWRAPP_CXX_FLAGS})
      # remove prefix -L because we need the pure directory for LINK_DIRECTORIES
      
      IF (XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX)
          SET(XMLWRAPP_LINK_DIRECTORIES_FIRST TRUE)
          SET(XMLWRAPP_EXE_LINKER_FLAGS_FIRST TRUE)
          foreach (a ${XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX})
              STRING(REGEX REPLACE "[-][L]" "" XMLWRAPP_LINK_DIRECTORIES_TEMP ${a} )
              IF(NOT ${XMLWRAPP_LINK_DIRECTORIES_FIRST})
                  SET(XMLWRAPP_LINK_DIRECTORIES "${XMLWRAPP_LINK_DIRECTORIES} ${XMLWRAPP_LINK_DIRECTORIES_TEMP}")
              ELSE(NOT ${XMLWRAPP_LINK_DIRECTORIES_FIRST})
                  SET(XMLWRAPP_LINK_DIRECTORIES "${XMLWRAPP_LINK_DIRECTORIES_TEMP}")
                  SET(XMLWRAPP_LINK_DIRECTORIES_FIRST FALSE)
              ENDIF(NOT ${XMLWRAPP_LINK_DIRECTORIES_FIRST})
              
              IF(NOT ${XMLWRAPP_EXE_LINKER_FLAGS_FIRST})
                  SET(XMLWRAPP_EXE_LINKER_FLAGS "${XMLWRAPP_EXE_LINKER_FLAGS} -Wl,-rpath,${XMLWRAPP_LINK_DIRECTORIES_TEMP}")
              ELSE(NOT ${XMLWRAPP_EXE_LINKER_FLAGS_FIRST})
                  SET(XMLWRAPP_EXE_LINKER_FLAGS " -Wl,-rpath,${XMLWRAPP_LINK_DIRECTORIES_TEMP}")
                  SET(XMLWRAPP_EXE_LINKER_FLAGS_FIRST FALSE)
                  
              ENDIF(NOT ${XMLWRAPP_EXE_LINKER_FLAGS_FIRST})
        endforeach (a ${XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX})
        
      ENDIF (XMLWRAPP_LINK_DIRECTORIES_WITH_PREFIX)
            MESSAGE("DBG  XMLWRAPP_LINK_DIRECTORIES=${XMLWRAPP_LINK_DIRECTORIES}")
            MESSAGE("DBG  XMLWRAPP_EXE_LINKER_FLAGS=${XMLWRAPP_EXE_LINKER_FLAGS}")

      #      ADD_DEFINITIONS("-DHAVE_XMLWRAPP")
      #      SET(XMLWRAPP_DEFINITIONS "-DHAVE_XMLWRAPP")
      MESSAGE(${XMLWRAPP_LINK_DIRECTORIES})
      MARK_AS_ADVANCED(
        XMLWRAPP_CXX_FLAGS
        XMLWRAPP_INCLUDE_DIR
        XMLWRAPP_LIBRARIES
        XMLWRAPP_LINK_DIRECTORIES
        XMLWRAPP_DEFINITIONS
	)
      
    ELSE(XMLWRAPP_CONFIG)
      MESSAGE("FindXMLWRAPP.cmake: XMLWRAPP-config not found. Please set it manually. XMLWRAPP_CONFIG=${XMLWRAPP_CONFIG}")
    ENDIF(XMLWRAPP_CONFIG)

  ENDIF(UNIX)


IF(XMLWRAPP_LIBRARIES)
  IF(XMLWRAPP_INCLUDE_DIR OR XMLWRAPP_CXX_FLAGS)

    SET(XMLWRAPP_FOUND 1)
    
  ENDIF(XMLWRAPP_INCLUDE_DIR OR XMLWRAPP_CXX_FLAGS)
ENDIF(XMLWRAPP_LIBRARIES)
