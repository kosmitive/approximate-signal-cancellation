cmake_minimum_required(VERSION 3.5.1)
project(UANC)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -msse2")
#set (CMAKE_PREFIX_PATH ${CMAKE_MODULE_PATH} "C:/Qt/5.8/msvc2013_64/lib/cmake")

# apply some settings to generate output in Bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/Bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/Bin/Lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/Bin/Lib)

#set the documentation properties
set(DOCUMENTATION_DIRECTORY ${CMAKE_HOME_DIRECTORY}/Docs)
set(CODE_DOC_ROOT_DIR ${CMAKE_HOME_DIRECTORY}/Code/UANC)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_AUTOMOC ON)

# Find the Qt libraries
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5Network REQUIRED)

#Find the doxygen library
find_package(Doxygen)

# Index all source files
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_HOME_DIRECTORY}  Code/UANC/*)

#Add the Aquila source files
file(GLOB_RECURSE AQUILA_SOURCE Code/libs/aquila/source/*)

#Add Aquila transform files
set(AQUILA_TRANSFORM Code/libs/aquila/transform/FftFactory.h Code/libs/aquila/transform/FftFactory.cpp Code/libs/aquila/transform/OouraFft.h Code/libs/aquila/transform/OouraFft.cpp Code/libs/aquila/source/FramesCollection.cpp Code/libs/aquila/source/FramesCollection.h Code/libs/aquila/transform/Spectrogram.h Code/libs/aquila/transform/Spectrogram.cpp)

#Add the FFT library
set(AQUILA_FFT Code/libs/aquila/fft/fft4g.c)

set(QCUSTOMPLOT_FILES Code/libs/qplot/qcustomplot.h Code/libs/qplot/qcustomplot.cpp Code/UANC/util/SignalManager.cpp Code/UANC/util/SignalManager.h)
set(QPROGRESSINDICATOR Code/libs/qprogressindicator/QProgressIndicator.cpp Code/libs/qprogressindicator/QProgressIndicator.h)

add_executable(UANC ${SOURCE_FILES} ${AQUILA_SOURCE} ${AQUILA_TRANSFORM} ${AQUILA_FFT} ${QCUSTOMPLOT_FILES} ${QPROGRESSINDICATOR} Code/UANC/gui/SignalViewWidget.h Code/UANC/util/event/EventManager.h)

#------------------------------------ Armadillo ------------------------------------

find_package(Armadillo REQUIRED)
include_directories(${ARMADILLO_INCLUDE_DIRS})
if(WIN32) # Check if we are on Windows
	target_include_directories(UANC  PUBLIC "C:/Program Files/armadillo/include")
endif(WIN32)
target_link_libraries(UANC ${ARMADILLO_LIBRARIES})

#------------------------------------- Tests ---------------------------------------

#Create the test files, if Boost has been found
if(Qt5Test_FOUND)
	#Exclude main from the source files of every test, but compile with the other files
	#TODO: Complile the rest of the program as an library and link against it?
	set(MAIN "Code/UANC/main.cpp" "Code/UANC/gui/MainWindow.cpp")
	list(REMOVE_ITEM SOURCE_FILES ${MAIN})
	#Get all test files and iterate over them
	file(GLOB_RECURSE TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}  Code/Tests/*)
	foreach(testSrc ${TEST_FILES})
		#Get the filename for every testfile in this directory and generate the target with the same name.
		get_filename_component(testName ${testSrc} NAME_WE)
		add_executable(${testName} ${testSrc} ${SOURCE_FILES} ${AQUILA_SOURCE} ${AQUILA_TRANSFORM} ${AQUILA_FFT} ${QCUSTOMPLOT_FILES} Code/UANC/util/tests/EpsilonTests.cpp Code/UANC/util/tests/EpsilonTests.h)
		target_link_libraries(${testName} ${Boost_LIBRARIES})
		#Move the testbins to Bin/testbin
		set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testbin)
		target_link_libraries(${testName} ${Qt5Widgets_LIBRARIES})
		target_link_libraries(${testName} ${Qt5PrintSupport_LIBRARIES})
		target_link_libraries(${testName} ${Qt5Network_LIBRARIES})
		target_link_libraries(${testName} ${Qt5Test_LIBRARIES})
		target_link_libraries(${testName} ${ARMADILLO_LIBRARIES})
		if(WIN32)
			target_include_directories(${testName}  PUBLIC "C:/Program Files/armadillo/include")
		endif(WIN32)
		target_link_libraries(${testName} ${Qt5DataVisualization_LIBRARIES})
		#Add the test to the test target, assumed no arguments are needed
		add_test(NAME ${testName}   WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/testbin COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/testBin ${testName})
	endforeach(testSrc)

endif(Qt5Test_FOUND)

# ------------------------------------ QT5 -----------------------------------------

include_directories(
        ${Qt5Widgets_INCLUDE_DIRS}
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Gui_INCLUDE_DIRS}
        ${Qt5Network_INCLUDE_DIRS}
	${Qt5DataVisualization_INCLUDE_DIRS}
)

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
target_link_libraries(UANC ${Qt5Widgets_LIBRARIES})
target_link_libraries(UANC ${Qt5PrintSupport_LIBRARIES})
target_link_libraries(UANC ${Qt5Network_LIBRARIES})
target_link_libraries(UANC ${Qt5DataVisualization_LIBRARIES})

# ----------------------------- CPPLint target ----------------------------
find_package(PythonInterp)
if (PYTHONINTERP_FOUND)
	add_custom_target("stylecheck"
	COMMAND "${CMAKE_COMMAND}" -E chdir
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${PYTHON_EXECUTABLE}"
            "${CMAKE_HOME_DIRECTORY}/misc/cpplint.py"
            "--counting=detailed"
            "--extensions=cpp,hpp,h,cc"
            "--linelength=250"
            ${SOURCE_FILES}
    VERBATIM)
endif(PYTHONINTERP_FOUND)

# ------------------------------------ Doxygen -----------------------------------------

# add a target to generate API documentation with Doxygen
if (DOXYGEN_FOUND)
    configure_file(${DOCUMENTATION_DIRECTORY}/Doxyfile.in ${DOCUMENTATION_DIRECTORY}/Doxyfile @ONLY)
    add_custom_target(doc
            ${DOXYGEN_EXECUTABLE} ${DOCUMENTATION_DIRECTORY}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen" VERBATIM
            )
endif (DOXYGEN_FOUND)

# ------------------------------------ Multiplattform -----------------------------------------

# generate proper gui program on specified platform
if(WIN32) # Check if we are on Windows
    if(MSVC) # Check if we are using the Visual Studio compiler
        set_target_properties(${PROJECT_NAME} PROPERTIES
                WIN32_EXECUTABLE YES
                LINK_FLAGS "/ENTRY:mainCRTStartup"
                )
    elseif(CMAKE_COMPILER_IS_GNUCXX)
        # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
    else()
        message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
    endif(MSVC)
elseif(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            MACOSX_BUNDLE YES
            )
elseif(UNIX)
    # Nothing special required
else()
    message(SEND_ERROR "You are on an unsupported platform! (Not Win32, Mac OS X or Unix)")
endif(WIN32)
