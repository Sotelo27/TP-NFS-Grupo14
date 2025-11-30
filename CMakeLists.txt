cmake_minimum_required(VERSION 3.22)

if (POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

if (NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 20)
endif()

set(CMAKE_CXX_EXTENSIONS ON)

project(taller_tp
    VERSION 1.0
    HOMEPAGE_URL "https://github.com/Sotelo27/TP-NFS-Grupo14"
    LANGUAGES CXX
    )

option(TALLER_TESTS "Enable / disable tests." ON)
option(TALLER_CLIENT "Enable / disable client program." ON)
option(TALLER_SERVER "Enable / disable server program." ON)
option(TALLER_EDITOR "Enable / disable editor program." ON)
option(TALLER_MAKE_WARNINGS_AS_ERRORS "Enable / disable warnings as errors." ON)

message(CMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}")

# Define target: lib common between client, server and editor programs
# HEY TODO XXX: change the name to something that you like
add_library(taller_common STATIC)

# add the source files that make the lib
# HEY!! TODO XXX: add more subfolders here
add_subdirectory(common/)
add_subdirectory(common/dto/)
add_subdirectory(common/enum/)


# set compiler warnings to the targets
include(cmake/CompilerWarnings.cmake)
set_project_warnings(taller_common ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

# Say where the headers will be found required by library target
# Make this PUBLIC so any dependant linking lib will inherit this
# location (so our dependants will not have to explicitly request this)
target_include_directories(taller_common PUBLIC .)

# HEY!! TODO XXX: you need to install some runtime and dev libraries *before*
# compiling the client/editor code.
#
# In xubuntu 24.04 these are:
#
# sudo apt-get install libopus-dev libopusfile-dev libxmp-dev libfluidsynth-dev fluidsynth libwavpack1 libwavpack-dev libfreetype-dev wavpack
#
# Install libSDL2 and libSDL2pp (for client or editor)
# HEY!! TODO XXX: add more dependendies (either for client, server or editor)
if(TALLER_CLIENT OR TALLER_EDITOR)
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.30.8
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
        GIT_TAG release-2.8.2
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
        GIT_TAG release-2.8.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        SDL2_ttf
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
        GIT_TAG release-2.22.0
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        OVERRIDE_FIND_PACKAGE
    )

    FetchContent_Declare(
        libSDL2pp
        OVERRIDE_FIND_PACKAGE
        URL https://github.com/libSDL2pp/libSDL2pp/archive/cc198c9a5657048bee67ece82de620b2d5661084.zip
    )

    FetchContent_MakeAvailable(SDL2 SDL2_image SDL2_mixer SDL2_ttf libSDL2pp)
endif()

# Program section
# ---------------
if(TALLER_CLIENT)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
    find_package(yaml-cpp REQUIRED)
    include_directories(${Qt5Core_INCLUDE_DIRS} ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS})
    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOMOC ON)
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    add_executable(taller_client)

    # Make them depend on taller_common lib
    add_dependencies(taller_client taller_common SDL2pp::SDL2pp)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(client)
    add_subdirectory(client/Game)
    add_subdirectory(client/Game/sdl_wrappers)
    add_subdirectory(client/Game/utils)
    add_subdirectory(client/Game/resources)
    add_subdirectory(client/Game/resources/hud)
    add_subdirectory(client/connection)
    add_subdirectory(client/QT)

    set_project_warnings(taller_client ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # add libSDL2pp's headers
    target_include_directories(taller_client PRIVATE ${libSDL2pp_SOURCE_DIR})
    target_include_directories(taller_client PRIVATE Game)
    target_include_directories(taller_client PRIVATE sdl_wrappers)
    target_include_directories(taller_client PRIVATE utils)
    target_include_directories(taller_client PRIVATE resources)
    target_include_directories(taller_client PRIVATE resources/hud)
    target_include_directories(taller_client PRIVATE connection)
    target_include_directories(taller_client PRIVATE QT)

    # define ASSETS_PATH assets path
    target_compile_definitions(taller_client PRIVATE
            ASSETS_PATH="assets"
    )

    # Link the taller_common lib target
    target_link_libraries(taller_client
            yaml-cpp
            taller_common
            SDL2pp::SDL2pp
            SDL2_image::SDL2_image
            SDL2_ttf::SDL2_ttf
            SDL2_mixer::SDL2_mixer
            taller_common
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
            Qt5::Multimedia
    )
endif()

if(TALLER_SERVER)
    # HEY!! TODO XXX: Change the name of the executable to the one you like!
    find_package(yaml-cpp REQUIRED)
    add_executable(taller_server)

    # Make them depend on taller_common lib
    add_dependencies(taller_server taller_common)

    # Add source files and enable warnings
    # HEY!! TODO XXX: add more subfolders here
    add_subdirectory(server)
    add_subdirectory(server/map)
    add_subdirectory(server/Player)
    add_subdirectory(server/physics)
    add_subdirectory(server/game)

    set_project_warnings(taller_server ${TALLER_MAKE_WARNINGS_AS_ERRORS} FALSE)

    # define PATH collision assets path
    target_compile_definitions(taller_server PRIVATE
        COLLISION_PATH="MapBoundary"
    )

    # Link the taller_common lib target
    target_link_libraries(taller_server taller_common)

    # YAML-CPP (for server track loading)
    include(FetchContent)
#    FetchContent_Declare(
#        yaml_cpp
#        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
#        GIT_TAG 0.8.0
#        GIT_SHALLOW TRUE
#        GIT_PROGRESS TRUE
#    )
#    FetchContent_MakeAvailable(yaml_cpp)

    # Box2D vía pkg-config (garantiza headers+lib 2.4.1 del sistema)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(BOX2D REQUIRED box2d>=2.4)

    # includes y linkeo SIEMPRE con firma "keyword" para este target
    target_include_directories(taller_server PRIVATE
        ${BOX2D_INCLUDE_DIRS}
    )

    target_link_libraries(taller_server yaml-cpp ${BOX2D_LIBRARIES})
endif()

if(TALLER_EDITOR)
    # Dependencias necesarias
    find_package(yaml-cpp REQUIRED)
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

    # Ejecutable del editor
    add_executable(taller_editor)

    # Código fuente del editor
    add_subdirectory(editor)

    # Dependencias del ejecutable
    add_dependencies(taller_editor taller_common SDL2pp::SDL2pp)

    # Copiar carpeta BaseMap al directorio build/editor
    file(COPY ${CMAKE_SOURCE_DIR}/editor/BaseMap
            DESTINATION ${CMAKE_BINARY_DIR}/editor)

    # Definir la ruta para cargar YAML dentro del código
    # Esto hace que el programa busque: build/editor/BaseMap
    target_compile_definitions(taller_editor PRIVATE
            BASE_PATH="editor/BaseMap",
            EDITS_PATH="editor/MapsEdited"
    )

    # Includes necesarios
    target_include_directories(taller_editor PRIVATE
            ${libSDL2pp_SOURCE_DIR}
    )

    # Linkeo final
    target_link_libraries(taller_editor
            PRIVATE
            yaml-cpp
            taller_common
            SDL2pp::SDL2pp
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
    )
endif()


# Testing section
# ---------------

if(TALLER_TESTS)
    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(GoogleTest)

    # target for testing
    add_executable(taller_tests)

    add_dependencies(taller_tests taller_common)

    target_include_directories(taller_tests PUBLIC .)

    add_subdirectory(tests/)

    set_project_warnings(taller_tests FALSE TRUE)

    target_include_directories(taller_tests
            PUBLIC
            ${gtest_SOURCE_DIR}/include
            ${gmock_SOURCE_DIR}/include
    )

    # --- NUEVO: Activar Qt y -fPIC ---
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Widgets)
    set_target_properties(taller_tests PROPERTIES POSITION_INDEPENDENT_CODE ON)
    target_link_libraries(taller_tests
            taller_common
            GTest::gtest_main
            Qt5::Core
            Qt5::Gui
            Qt5::Widgets
    )
endif()

