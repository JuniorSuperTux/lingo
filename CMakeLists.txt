cmake_minimum_required(VERSION 3.16)

project(lingo VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick)

add_subdirectory(dependencies/tidy-html5)

qt_standard_project_setup()

include_directories(include)

qt_add_executable(applingo
	src/main.cpp
	src/parser.cpp
	src/dict_types.cpp
	src/xhtml_converter.cpp
	include/parser.hpp
	include/dict_types.hpp
	include/xhtml_converter.hpp
)

qt_add_qml_module(applingovocab
	URI lingo.dict_types
	VERSION 1.0
	SOURCES
		include/dict_types.hpp
		src/dict_types.cpp
	STATIC
)

qt_add_qml_module(applingo
	URI lingo
	VERSION 1.0
	QML_FILES
		qml/Main.qml
		qml/Vocab.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(applingo PROPERTIES
	#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.applingo
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

set_target_properties(applingo PROPERTIES
	CXX_STANDARD 20
)

target_link_libraries(applingo PRIVATE
	Qt6::Quick
	applingovocabplugin
	tidy-static
)

include(GNUInstallDirs)
install(TARGETS applingo
	BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
