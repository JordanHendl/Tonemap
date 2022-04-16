SET( EXT_STBTOOLS_NAME        stb_tools                                                   )
SET( EXT_STBTOOLS_VER         1                                                           )
SET( EXT_STBTOOLS_REL         21                                                          )
SET( EXT_STBTOOLS_REV         1                                                           )
SET( EXT_STBTOOLS_REL_VER     ${EXT_STBTOOLS_VER}.${EXT_STBTOOLS_REL}.${EXT_STBTOOLS_REV} )
SET( EXT_STBTOOLS_UNTAR_NAME  ${EXT_STBTOOLS_NAME}                                        )

SET( EXT_STBTOOLS_TAR_DIR   ${CMAKE_CURRENT_SOURCE_DIR}/${EXT_STBTOOLS_UNTAR_NAME}.tgz                   ) 
SET( EXT_STBTOOLS_UNTAR_DIR ${CMAKE_BINARY_DIR}/ext/${EXT_STBTOOLS_NAME}/src/                            )
SET( EXT_STBTOOLS_BLD_DIR   ${CMAKE_BINARY_DIR}/ext/${EXT_STBTOOLS_NAME}/build                           )
SET( EXT_STBTOOLS_SRC_DIR   ${CMAKE_BINARY_DIR}/ext/${EXT_STBTOOLS_NAME}/src/${EXT_STBTOOLS_UNTAR_NAME}/ )
SET( EXT_STBTOOLS_CMAKE_DIR ${EXT_STBTOOLS_SRC_DIR}                                                      )
SET( EXT_STBTOOLS_INCS      ${EXT_STBTOOLS_SRC_DIR}/include                                              )

IF( UNIX )
  SET( EXT_STBTOOLS_LIBS ${EXT_STBTOOLS_BLD_DIR}/libstb_tools.a
     )

  SET( EXT_STBTOOLS_LIB  ${EXT_STBTOOLS_BLD_DIR}/libstb_tools.a
     )
ENDIF()

FILE( MAKE_DIRECTORY ${EXT_STBTOOLS_BLD_DIR} )
FILE( MAKE_DIRECTORY ${EXT_STBTOOLS_SRC_DIR} )

MESSAGE( "Untarring STB_TOOLS library source" )
EXECUTE_PROCESS( 
  COMMAND tar -xf ${EXT_STBTOOLS_TAR_DIR}
  WORKING_DIRECTORY ${EXT_STBTOOLS_UNTAR_DIR}
  )

EXECUTE_PROCESS( 
  COMMAND ${CMAKE_COMMAND} ${EXT_STBTOOLS_CMAKE_DIR}
  WORKING_DIRECTORY ${EXT_STBTOOLS_BLD_DIR}
  )

IF( WIN32 )
  ADD_CUSTOM_COMMAND(
    PRE_BUILD
    OUTPUT ${EXT_STBTOOLS_LIBS}
    COMMAND MSBuild Stb_image.sln /property:Configuration=Release -maxcpucount:7
    WORKING_DIRECTORY ${EXT_STBTOOLS_BLD_DIR}
    )
ENDIF()

IF( UNIX )
  ADD_CUSTOM_COMMAND(
    PRE_BUILD
    OUTPUT ${EXT_STBTOOLS_LIBS}
    COMMAND $(MAKE)
    WORKING_DIRECTORY ${EXT_STBTOOLS_BLD_DIR}
    )
ENDIF()

ADD_CUSTOM_TARGET(
  EXT_Stb_Tools ALL
  DEPENDS ${EXT_STBTOOLS_LIBS}
)

SET( Stb_Tools_LIBRARIES    ${EXT_STBTOOLS_LIBS} CACHE INTERNAL "STBTOOLS Libaries"     )
SET( Stb_Tools_INCLUDE_DIRS ${EXT_STBTOOLS_INCS} CACHE INTERNAL "STBTOOLS Include Dirs" )