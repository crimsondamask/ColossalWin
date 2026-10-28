# Install script for directory: C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/src/3.3.3-984c301c61.clean

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/pkgs/sqlitecpp_x64-windows-static/debug")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "libraries" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/SQLiteCpp.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "headers" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/src/3.3.3-984c301c61.clean/include/" FILES_MATCHING REGEX ".*\\.(hpp|h)$")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp/SQLiteCppTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp/SQLiteCppTargets.cmake"
         "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/CMakeFiles/Export/31d559fcedc0800e90eb93f071f3b915/SQLiteCppTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp/SQLiteCppTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp/SQLiteCppTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp" TYPE FILE FILES "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/CMakeFiles/Export/31d559fcedc0800e90eb93f071f3b915/SQLiteCppTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp" TYPE FILE FILES "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/CMakeFiles/Export/31d559fcedc0800e90eb93f071f3b915/SQLiteCppTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/SQLiteCpp" TYPE FILE FILES "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/src/3.3.3-984c301c61.clean/package.xml")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/SQLiteCpp" TYPE FILE FILES
    "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/cmake/SQLiteCppConfig.cmake"
    "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/cmake/SQLiteCppConfigVersion.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "C:/Users/Admin/Desktop/code/colossal-740848fa89/colossaal_app/vcpkg_installed/x64-windows-static/vcpkg/blds/sqlitecpp/x64-windows-static-dbg/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
