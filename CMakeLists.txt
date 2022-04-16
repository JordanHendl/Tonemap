cmake_minimum_required(VERSION 3.13)
project(tonemap)

# Set build config.
set( Architecture "64bit" CACHE STRING "The system architecture."                        )
set( CXX_Standard "17"    CACHE STRING "The C++ standard to use for building."           )
set( Major        "0"     CACHE STRING "The major version of this build."                )
set( Minor        "1"     CACHE STRING "The minor version of this build."                )
set( Branch       "0"     CACHE STRING "The branch version of this build."               )
set( Revision     "0"     CACHE STRING "The revision of this build."                     )
set( Generator    "DEB"   CACHE STRING "The Package Generator to use for this build."    )
set( Sanitizer    "None"  CACHE STRING "The sanitizer to use for this project's build." )

set(CMAKE_CXX_STANDARD          ${CXX_Standard} )
set(CMAKE_CXX_STANDARD_REQUIRED ON              )
add_subdirectory(external)
add_subdirectory(tonemap)
