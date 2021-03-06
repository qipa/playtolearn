cmake_minimum_required (VERSION 2.8)

ENABLE_TESTING()

project (EduGame)
set (EduGame_VERSION_MAJOR 0)
set (EduGame_VERSION_MINOR 1)


###############################################
#           THIRD PARTY LIBRARIES             #
###############################################

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

include(ExternalProject)

set(BOOST_VERSION 1.47.0)
set(IRRLICHT_VERSION 1.7.2)

######################
## GENERIC PACKAGES ##
######################

#################
## Qt and GLUT ##
#################

SET ( QT_USE_OPENGL true)

find_package(GLUT REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Qt4 REQUIRED)

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${QT_INCLUDE_DIR})

######################
## PYTHON LIBRARIES ##
######################

include(./LocalFindPythonLibs.cmake)

IF(NOT PYTHONLIBS_FOUND)
  MESSAGE(FATAL_ERROR "Could not find python libraries.")
ENDIF()

STRING(REGEX REPLACE ".*/libpython(.)\\...*" "\\1" PYTHON_VERSION_MAJOR "${PYTHON_LIBRARIES}")
STRING(REGEX REPLACE ".*/libpython.\\.(.).*" "\\1" PYTHON_VERSION_MINOR "${PYTHON_LIBRARIES}")
SET(PYTHON_VERSION ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR})

IF (${PYTHON_VERSION_MAJOR} VERSION_EQUAL "3")
  ADD_DEFINITIONS(-DPYTHON_VERSION_3)
ENDIF()

MESSAGE("Python version: ${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
MESSAGE("Python include directory: ${PYTHON_INCLUDE_DIRS}")
MESSAGE("Python library: ${PYTHON_LIBRARIES}")

include_directories(${PYTHON_INCLUDE_DIRS})

######################
## APPLE FRAMEWORKS ##
######################

IF(APPLE)
   INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
   FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
   FIND_LIBRARY(CARBON_LIBRARY Carbon)
   FIND_LIBRARY(COCOA_LIBRARY Cocoa )
   FIND_LIBRARY(IOKIT_LIBRARY IOKit )
   FIND_LIBRARY(CORE_FOUNDATION_LIBRARY CoreFoundation )
   MARK_AS_ADVANCED (CARBON_LIBRARY
                     COCOA_LIBRARY
                     IOKIT_LIBRARY
                     CORE_FOUNDATION_LIBRARY
                     OPENGL_LIBRARY)
   SET(EXTRA_LIBS ${CARBON_LIBRARY} ${COCOA_LIBRARY} ${IOKIT_LIBRARY} ${CORE_FOUNDATION_LIBRARY} ${OPENGL_LIBRARY})
   SET (SDKROOT "/Developer/SDKs/MacOSX10.6.sdk")
   SET (CMAKE_OSX_SYSROOT "${SDKROOT}")
   SET (CMAKE_OSX_ARCHITECTURES "i386")
ENDIF()



####################
## UNIX LIBRARIES ##
####################

if(UNIX)
  FIND_PACKAGE (X11 REQUIRED)
endif()

#######################
## BOOOST LIBRARIES ##
#######################

find_package(Boost REQUIRED COMPONENTS
                              python
                              serialization
                              unit_test_framework)
if (Boost_FOUND)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)
endif(Boost_FOUND)


#set(BOOST_LIBS thread,python,serialization,test)
#set(BOOST_ARGS include=${PYTHON_INCLUDE_DIRS})

#IF(APPLE)
#  set(BOOST_ARGS ${BOOST_ARGS} toolset=darwin architecture=x86 address-model=32 threading=single)
#ENDIF()

# BUILD USING APT-GET
#
#STRING(REGEX REPLACE "(.*)\\.(.*)\\.(.*)" "\\1_\\2_\\3"
#        BOOST_UNDERSCORE_VERSION "${BOOST_VERSION}")

#ExternalProject_Add(boost
#  PREFIX third_party/boost/
#  BUILD_IN_SOURCE 1
#  SVN_REPOSITORY http://svn.boost.org/svn/boost/tags/release/Boost_${BOOST_UNDERSCORE_VERSION}/
#  CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap.sh --with-python-version=${PYTHON_VERSION} --prefix="<INSTALL_DIR>" --libdir="<INSTALL_DIR>/lib" --with-libraries=${BOOST_LIBS}
#  BUILD_COMMAND <SOURCE_DIR>/bjam ${BOOST_ARGS} install
#  INSTALL_COMMAND ""
#  UPDATE_COMMAND ""
#)

########################
## IRRLICHT LIBRARIES ##
########################

IF(APPLE)
  set(IRRLICHT_BUILD_COMMAND cd <SOURCE_DIR>/source/Irrlicht/MacOSX && xcodebuild -project MacOSX.xcodeproj -sdk ${SDKROOT} -arch i386)
  set(IRRLICHT_INSTALL_COMMAND mkdir -p ../../lib && cp <SOURCE_DIR>/source/Irrlicht/MacOSX/build/Release/libIrrlicht.a ../../lib/ && cp -r <SOURCE_DIR>/include ../../)
ELSE()
  set(IRRLICHT_BUILD_COMMAND cd source/Irrlicht && make sharedlib)
  set(IRRLICHT_INSTALL_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/IrrlichtInstallUnix.sh <SOURCE_DIR> ${IRRLICHT_VERSION})
ENDIF()

IF(NOT Irrlicht_FOUND)

ExternalProject_Add(irrlicht
  PREFIX third_party/irrlicht/
  SVN_REPOSITORY https://irrlicht.svn.sourceforge.net/svnroot/irrlicht/branches/releases/1.7
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  BUILD_COMMAND  ${IRRLICHT_BUILD_COMMAND}
  INSTALL_COMMAND ${IRRLICHT_INSTALL_COMMAND}
)

SET(Irrlicht_FOUND true CACHE STRING "If Irrlicht has been found or not." FORCE)

ENDIF()


##########################
## Adding Library Files ##
##########################

FILE(GLOB _INCLUDE_DIRS "${CMAKE_BINARY_DIR}/third_party/*/include")
MESSAGE("Current working directory: ${CMAKE_BINARY_DIR}")
FOREACH(s ${_INCLUDE_DIRS})
MESSAGE("Including directory: ${s}")
include_directories(${s})
ENDFOREACH()

FILE(GLOB STATIC_LIBS_FILES "${CMAKE_BINARY_DIR}/third_party/*/lib/*.a")
FOREACH(s ${STATIC_LIBS_FILES} )
  STRING(REGEX REPLACE ".*/lib(.*)\\.a" "\\1" name "${s}")
  add_library(${name} STATIC IMPORTED)
   set_property(TARGET ${name}
                PROPERTY IMPORTED_LOCATION ${s})

  MESSAGE("Adding library \"${name}\"\n\tfrom location \"${s}\"")
ENDFOREACH()

FILE(GLOB DYNAMIC_LIBS_DIR "${CMAKE_BINARY_DIR}/third_party/*/lib")
FOREACH(s ${DYNAMIC_LIBS_DIR})
  LINK_DIRECTORIES(${s})
  MESSAGE("Linking directory: ${s}")
ENDFOREACH()

#######################
## SETTING LIBRARIES ##
#######################

SET(DYNAMIC_LIBS dl m util)

IF(UNIX)
  list(APPEND DYNAMIC_LIBS GL GLU Xxf86vm Xext X11)
endif()

#############
## SOURCES ##
#############

# Compiler options
ADD_DEFINITIONS(-g)
ADD_DEFINITIONS(-Wall)

include_directories(src)

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(test_src)
#############
## BUNDLES ##
#############

if(APPLE)
  set (MACOSX_BUNDLE true)
  set (MACOSX_BUNDLE_BUNDLE_NAME playtolearn)
  set (MACOSX_BUNDLE_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/OSXResources/stanfordicon.icns")

  FILE(GLOB_RECURSE MAC_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/OSXResources/*")

  set_source_files_properties("${MAC_RESOURCES}"
                              PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
  LIST(APPEND SRCS ${MEDIA_RESOURCES})
  LIST(APPEND SRCS ${MAC_RESOURCES})
ENDIF()

#################
## Executables ##
#################

add_executable(playtolearn WIN32 MACOSX_BUNDLE "src/frontend/FEEngine.cpp")
target_link_libraries(playtolearn playtolearnlibrary ${DYNAMIC_LIBS})
add_dependencies(playtolearn playtolearnlibrary)

add_executable(playtolearnteacher WIN32 MACOSX_BUNDLE "src/ui/main.cpp")
target_link_libraries(playtolearnteacher playtolearnlibrary ${DYNAMIC_LIBS})
add_dependencies(playtolearnteacher playtolearnlibrary)


if(APPLE)
  set_target_properties(playtolearn PROPERTIES RESOURCE "${MAC_RESOURCES}")
ENDIF()

install(TARGETS playtolearn
        RUNTIME DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
        BUNDLE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}
        RESOURCE DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Resources)