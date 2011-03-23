# - Find casacore
#
# Find the native CASACORE includes and library
#
#  CASACORE_INCLUDE_DIR  - where to find casacore.h, etc.
#  CASACORE_LIBRARY_PATH - Specify to choose a non-standard location to
#                          search for libraries
#  CASACORE_LIBRARIES    - List of libraries when using casacore.
#  CASACORE_FOUND        - True if casacore found.

# http://www.mail-archive.com/cmake@cmake.org/msg26187.html
#message("========== version: ${CasaCore_FIND_VERSION}")
#message("========== ver major: ${CasaCore_FIND_VERSION_MAJOR}")
#message("========== ver minor: ${CasaCore_FIND_VERSION_MINOR}")
#message("========== ver path: ${CasaCore_FIND_VERSION_PATCH}")
#message("========== exact: ${CasaCore_VERSION_EXACT}")
#message("========== required: ${CasaCore_FIND_REQUIRED}")

include(FindPkgConfig)
pkg_check_modules(CFITSIO cfitsio>=3.0 REQUIRED)
pkg_check_modules(WCSLIB wcslib>=4.7 REQUIRED)


IF (CASACORE_INCLUDE_DIR)
    # Already in cache, be silent
    SET(CASACORE_FIND_QUIETLY TRUE)
ENDIF (CASACORE_INCLUDE_DIR)

if(CASACORE_FIND_QUIETLY OR NOT CASACORE_FIND_REQUIRED)
  find_package(LAPACK)
else(CASACORE_FIND_QUIETLY OR NOT CASACORE_FIND_REQUIRED)
  find_package(LAPACK REQUIRED)
endif(CASACORE_FIND_QUIETLY OR NOT CASACORE_FIND_REQUIRED)

if(LAPACK_FOUND)
  set(CASACORE_LINKER_FLAGS ${LAPACK_LINKER_FLAGS})

FIND_PATH(CASACORE_INCLUDE_DIR casacore)

SET(CASACORE_NAMES
    casa_images
    casa_mirlib
    casa_components
    casa_coordinates
    casa_lattices
    casa_msfits
    casa_ms
    casa_fits
    casa_measures
    casa_tables
    casa_scimath
    casa_scimath_f
    casa_casa
)
FOREACH( lib ${CASACORE_NAMES} )
    FIND_LIBRARY(CASACORE_LIBRARY_${lib} NAMES ${lib} PATHS ENV CASACORE_LIBRARY_PATH )
    MARK_AS_ADVANCED(CASACORE_LIBRARY_${lib})
    LIST(APPEND CASACORE_LIBRARIES ${CASACORE_LIBRARY_${lib}})
ENDFOREACH(lib)
LIST(APPEND CASACORE_LIBRARIES ${LAPACK_LIBRARIES})
endif(LAPACK_FOUND)

# handle the QUIETLY and REQUIRED arguments and set CASACORE_FOUND to TRUE if.
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CASACORE DEFAULT_MSG CASACORE_LIBRARIES CASACORE_INCLUDE_DIR)

IF(CASACORE_FOUND)
ELSE(CASACORE_FOUND)
    SET( CASACORE_LIBRARIES )
ENDIF(CASACORE_FOUND)

