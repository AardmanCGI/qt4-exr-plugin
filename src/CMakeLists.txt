cmake_policy(SET CMP0020 NEW)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
find_package(Qt4 REQUIRED QtCore QtGui)

add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)
add_definitions(-DQT_SHARED)
list(APPEND CMAKE_CXX_FLAGS_DEBUG -DQT_DEBUG)

find_package(OpenEXR REQUIRED)
include_directories(${OpenEXR_INCLUDE_DIRS})
if(NOT OpenEXR_USE_STATIC_LIBS AND WIN32)
	add_definitions(-DOpenEXR_DLL)
endif()

set(QEXR4_SOURCE_FILES
	exr_plugin.cpp
	exr_handler.cpp
	exr_io.cpp
	)

add_library(qexr4 SHARED ${QEXR4_SOURCE_FILES})

target_link_libraries(qexr4
	Qt4::QtCore
	Qt4::QtGui
	${OpenEXR_LIBRARIES}
	)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${QT_IMAGEFORMATS_PLUGINS_DIR}" CACHE PATH "Default install directory" FORCE)
endif()

install(TARGETS qexr4
	RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}
	)
