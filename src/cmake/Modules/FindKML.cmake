# Find LIBSHP library and header file
# Sets
#   KML_FOUND               to 0 or 1 depending on the result
#   KML_INCLUDE_DIRECTORIES to directories required for using libpq
#   KML_LIBRARIES           to libpq and any dependent libraries
# If KML_REQUIRED is defined, then a fatal error message will be generated if libpq is not found
#if ( NOT KML_INCLUDE_DIRECTORIES OR NOT KML_LIBRARIES OR NOT KML_FOUND )

  if ( $ENV{KML_DIR} )
    file( TO_CMAKE_PATH "$ENV{KML_DIR}" _KML_DIR )
  endif ( $ENV{KML_DIR} )

  find_library( KML_DOM_LIBRARY
    NAMES  kmldom libkmldom
    PATHS
      ${_KML_DIR}/lib
      ${CMAKE_INSTALL_PREFIX}/lib
      /usr/local/lib
      /usr/lib
      /home/jchen/libs/lib
      /data/jqdu/libs/lib
      ${_KML_DIR}
      ${CMAKE_INSTALL_PREFIX}/bin
    NO_DEFAULT_PATH
  )

  find_library( KML_BASE_LIBRARY
    NAMES kmlbase libkmlbase
    PATHS
      ${_KML_DIR}/lib
      ${CMAKE_INSTALL_PREFIX}/lib
      /usr/local/lib
      /home/jchen/libs/lib
      /data/jqdu/libs/lib

      /usr/lib
      ${_KML_DIR}
      ${CMAKE_INSTALL_PREFIX}/bin
    NO_DEFAULT_PATH
  )

  find_path( KML_INCLUDE_DIRECTORIES
    NAMES kml kml/dom.h kml/engine.h
    PATHS
      ${_KML_DIR}
      /home/jchen/libs/include/kml
      /home/jchen/libs/include

    /data/jqdu/libs/include
    /data/jqdu/libs/include/kml

      ${_KML_DIR}/include
      ${CMAKE_INSTALL_PREFIX}/include
      /usr/local/include
      /usr/include
    NO_DEFAULT_PATH
  )
  include_directories(${KML_INCLUDE_DIRECTORIES})
message ("KML include " ${KML_INCLUDE_DIRECTORIES})
  SET(KML_LIBRARIES ${KML_BASE_LIBRARY} ${KML_DOM_LIBRARY})
  if ( NOT KML_INCLUDE_DIRECTORIES OR NOT KML_LIBRARIES )
    if ( KML_REQUIRED )
      message( FATAL_ERROR "libkml is required. Set KML_DIR" )
    endif ( KML_REQUIRED )
  else ( NOT KML_INCLUDE_DIRECTORIES OR NOT KML_LIBRARIES )
    set( KML_FOUND 1 )
    message("libkml found")
    message(${KML_INCLUDE_DIRECTORIES})
    mark_as_advanced( KML_FOUND )
  endif ( NOT KML_INCLUDE_DIRECTORIES OR NOT KML_LIBRARIES )

#endif ( NOT KML_INCLUDE_DIRECTORIES OR NOT KML_LIBRARIES OR NOT KML_FOUND )

mark_as_advanced( FORCE KML_INCLUDE_DIRECTORIES )
mark_as_advanced( FORCE KML_LIBRARIES )
