include(find_local_package)

macro(qtect)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    if(NOT QT_FOUND)
        set(QT_COMPONENTS
            Core
            Gui
            Network
            Qml
            CACHE STRING "Required Qt Components"
            )
        dep_find_package(Qt)
        if(NOT Qt_FOUND)
            dep_find_package(Qt6)
            if(NOT Qt6_FOUND)
                dep_find_package(Qt5)
            endif()
        endif()
        find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS "${QT_COMPONENTS}")
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS "${QT_COMPONENTS}")
        message("QT_FOUND: ${QT_FOUND}")
        message("QT_VERSION_MAJOR: ${QT_VERSION_MAJOR}")
        if(QT_VERSION_MAJOR)
            message("Qt${QT_VERSION_MAJOR}_FOUND: ${Qt${QT_VERSION_MAJOR}_FOUND}")
        endif()
    endif()
endmacro()
