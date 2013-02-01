# Find LIBBIOGEME library and header file
# Sets
#   BIOGEME_FOUND               to 0 or 1 depending on the result
#   BIOGEME_INCLUDE_DIRECTORIES to directories required for using libpq
#   BIOGEME_LIBRARIES           to libpq and any dependent libraries
# If BIOGEME_REQUIRED is defined, then a fatal error message will be generated if libpq is not found

#  if ( $ENV{BIOGEME_DIR} )
#    file( TO_CMAKE_PATH "$ENV{BIOGEME_DIR}" _BIOGEME_DIR )
#  endif ( $ENV{BIOGEME_DIR} )
  SET(BIOGEME_POSSIBLE_ROOT_DIRS
    ${BIOGEME_ROOT_DIR}
    $ENV{BIOGEME_ROOT_DIR}
    ${BIOGEME_DIR}
    ${BIOGEME_HOME}
    "${CMAKE_SOURCE_DIR}/../libs/biogeme-2.0/libraries/parameters"
    "${CMAKE_SOURCE_DIR}/../libs/biogeme-2.0/libraries/utils"
   	"${CMAKE_SOURCE_DIR}/../libs//biogeme-2.0/libraries/utils"
    "${CMAKE_SOURCE_DIR}/../libs//biogeme-2.0/libraries/parameters"
    "${CMAKE_SOURCE_DIR}/../libs//biogeme-2.0"
    "/usr/local/lib/biogeme/"
    "/data/jqdu/libs/lib/biogeme"
    "/data/jqdu/libs/include/biogeme/"
    "/data/jqdu/libs"
    )

  find_library( LIB_UTILS
    NAMES NAMES utils libutils patUtils libutils.la utils.la libutils.a
    PATHS
      ${BIOGEME_POSSIBLE_ROOT_DIRS}
  )

    find_library(LIB_PARAMETERS
                    NAMES param libparameters patParams parameters libparameters.la  parameters.la libparameters.a
                    PATHS
                          ${BIOGEME_POSSIBLE_ROOT_DIRS})

  find_path(BIOGEME_INCLUDE_DIRECTORIES
    NAMES config.h
    PATHS ${BIOGEME_POSSIBLE_ROOT_DIRS}
    PATH_SUFFIXES include
    DOC "biogeme header include dir"
   )

  find_path(BIOGEME_INCLUDE_UTILS_DIRECTORIES
    NAMES patType.h patDisplay.h patString.h
    PATHS ${BIOGEME_POSSIBLE_ROOT_DIRS}
    PATH_SUFFIXES include
    DOC "biogeme header include dir"
   )

  if ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT LIB_UTILS OR NOT LIB_PARAMETERS )
    if ( BIOGEME_REQUIRED )
      message( FATAL_ERROR "biogeme is required. Set BIOGEME_DIR" )
    endif ( BIOGEME_REQUIRED )
  else ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT LIB_UTILS  OR NOT LIB_PARAMETERS )
    set( BIOGEME_FOUND 1 )
    message("biogeme found")
    mark_as_advanced( BIOGEME_FOUND )
  endif (  NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT LIB_UTILS  OR NOT LIB_PARAMETERS )

if (BIOGEME_FOUND)
    message(${BIOGEME_INCLUDE_DIRECTORIES})
    message(${BIOGEME_INCLUDE_UTILS_DIRECTORIES})
    include_directories(${BIOGEME_INCLUDE_DIRECTORIES})
    include_directories(${BIOGEME_INCLUDE_UTILS_DIRECTORIES})

endif (BIOGEME_FOUND)
mark_as_advanced( FORCE BIOGEME_INCLUDE_DIRECTORIES )
mark_as_advanced( FORCE LIB_PARAMETERS )
mark_as_advanced( FORCE LIB_UTILS )
