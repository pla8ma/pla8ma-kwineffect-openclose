cmake_minimum_required(VERSION 3.22)
project(kwineffect_openclose LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ECM 6.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

find_package(Qt6 6.6 REQUIRED COMPONENTS Core Gui Widgets DBus Quick)
find_package(KF6CoreAddons REQUIRED)
find_package(KWin REQUIRED)

# The factory macro has the Q_OBJECT that AutoMoc looks for.
list(APPEND CMAKE_AUTOMOC_MACRO_NAMES "KWIN_EFFECT_FACTORY_SUPPORTED")

kcoreaddons_add_plugin(kwineffect_openclose
    SOURCES openclose.cpp main.cpp
    INSTALL_NAMESPACE "kwin/effects/plugins")
target_link_libraries(kwineffect_openclose PRIVATE KWin::kwin Qt6::Gui)
