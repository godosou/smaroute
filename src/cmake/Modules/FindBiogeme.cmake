# Find LIBBIOGEME library and header file
# Sets
#   BIOGEME_FOUND               to 0 or 1 depending on the result
#   BIOGEME_INCLUDE_DIRECTORIES to directories required for using libpq
#   BIOGEME_LIBRARIES           to libpq and any dependent libraries
# If BIOGEME_REQUIRED is defined, then a fatal error message will be generated if libpq is not found
if ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT BIOGEME_LIBRARIES OR NOT BIOGEME_FOUND )

  if ( $ENV{BIOGEME_DIR} )
    file( TO_CMAKE_PATH "$ENV{BIOGEME_DIR}" _BIOGEME_DIR )
  endif ( $ENV{BIOGEME_DIR} )

  find_library( BIOGEME_LIBRARIES
    NAMES BIOGEME libBIOGEME
    PATHS
      ${_BIOGEME_DIR}/lib
      ${CMAKE_INSTALL_PREFIX}/lib
      /usr/local/lib
      /usr/lib
      ${_BIOGEME_DIR}
      /users/jchen/libs/lib
      ${CMAKE_INSTALL_PREFIX}/bin
    NO_DEFAULT_PATH
  )

  find_path( BIOGEME_INCLUDE_DIRECTORIES
    NAMES shapefil.h
    PATHS
      ${_BIOGEME_DIR}
      /users/jchen/libs/include
      ${_BIOGEME_DIR}/include
      ${CMAKE_INSTALL_PREFIX}/include
      /usr/local/include
      /usr/include
    NO_DEFAULT_PATH
  )

  if ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT BIOGEME_LIBRARIES ) 
    if ( BIOGEME_REQUIRED )
      message( FATAL_ERROR "libshap is required. Set BIOGEME_DIR" )
    endif ( BIOGEME_REQUIRED )
  else ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT BIOGEME_LIBRARIES ) 
    set( BIOGEME_FOUND 1 )
    message("libBIOGEME found")
    mark_as_advanced( BIOGEME_FOUND )
  endif ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT BIOGEME_LIBRARIES )

endif ( NOT BIOGEME_INCLUDE_DIRECTORIES OR NOT BIOGEME_LIBRARIES OR NOT BIOGEME_FOUND )

mark_as_advanced( FORCE BIOGEME_INCLUDE_DIRECTORIES )
mark_as_advanced( FORCE BIOGEME_LIBRARIES )
