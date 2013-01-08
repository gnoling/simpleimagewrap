CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(simpleimagewrap)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwindows -s")
ADD_EXECUTABLE(simpleimagewrap ini.c simpleimagewrap.c)

OPTION(SELF_PACK_EXECUTABLES "Self-pack executables." ON)

IF (SELF_PACK_EXECUTABLES)
   INCLUDE(${CMAKE_ROOT}/Modules/FindSelfPackers.cmake)
   IF (SELF_PACKER_FOR_EXECUTABLE)
     IF (WIN32)
       ADD_CUSTOM_COMMAND(
           SOURCE simpleimagewrap
           COMMAND ${SELF_PACKER_FOR_EXECUTABLE}
           ARGS ${SELF_PACKER_FOR_EXECUTABLE_FLAGS}
                simpleimagewrap.exe
           TARGET simpleimagewrap)
     ENDIF (WIN32)
   ENDIF (SELF_PACKER_FOR_EXECUTABLE)
ENDIF (SELF_PACK_EXECUTABLES)

ADD_CUSTOM_COMMAND(
    SOURCE simpleimagewrap
    COMMAND cp
    ARGS ../simpleimagewrap.ini ./
    TARGET simpleimagewrap)
