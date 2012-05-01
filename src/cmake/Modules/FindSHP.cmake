# Find LIBSHP library and header file
# Sets
#   SHP_FOUND               to 0 or 1 depending on the result
#   SHP_INCLUDE_DIRECTORIES to directories required for using libpq
#   SHP_LIBRARIES           to libpq and any dependent libraries
# If SHP_REQUIRED is defined, then a fatal error message will be generated if libpq is not found
if ( NOT SHP_INCLUDE_DIRECTORIES OR NOT SHP_LIBRARIES OR NOT SHP_FOUND )

  if ( $ENV{SHP_DIR} )
    file( TO_CMAKE_PATH "$ENV{SHP_DIR}" _SHP_DIR )
  endif ( $ENV{SHP_DIR} )

  find_library( SHP_LIBRARIES
    NAMES shp libshp
    PATHS
      ${_SHP_DIR}/lib
      ${CMAKE_INSTALL_PREFIX}/lib
      /usr/local/lib
      /usr/lib
      ${_SHP_DIR}
      /users/jchen/libs/lib
      ${CMAKE_INSTALL_PREFIX}/bin
    NO_DEFAULT_PATH
  )

  find_path( SHP_INCLUDE_DIRECTORIES
    NAMES shapefil.h
    PATHS
      ${_SHP_DIR}
      /users/jchen/libs/include
      ${_SHP_DIR}/include
      ${CMAKE_INSTALL_PREFIX}/include
      /usr/local/include
      /usr/include
    NO_DEFAULT_PATH
  )

  if ( NOT SHP_INCLUDE_DIRECTORIES OR NOT SHP_LIBRARIES ) 
    if ( SHP_REQUIRED )
      message( FATAL_ERROR "libshap is required. Set SHP_DIR" )
    endif ( SHP_REQUIRED )
  else ( NOT SHP_INCLUDE_DIRECTORIES OR NOT SHP_LIBRARIES ) 
    set( SHP_FOUND 1 )
    message("libshp found")
    mark_as_advanced( SHP_FOUND )
  endif ( NOT SHP_INCLUDE_DIRECTORIES OR NOT SHP_LIBRARIES )

endif ( NOT SHP_INCLUDE_DIRECTORIES OR NOT SHP_LIBRARIES OR NOT SHP_FOUND )

mark_as_advanced( FORCE SHP_INCLUDE_DIRECTORIES )
mark_as_advanced( FORCE SHP_LIBRARIES )
