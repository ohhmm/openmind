
macro(qtect)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC ON)
    set(QT_COMPONENTS
        Core
        Gui
        Network
        Qml
        CACHE STRING "Required Qt Components"
        )
    if(NOT QT_FOUND)
        find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS "${QT_COMPONENTS}")
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS "${QT_COMPONENTS}")
        message("QT_FOUND: ${QT_FOUND}")
        message("QT_VERSION_MAJOR: ${QT_VERSION_MAJOR}")
        if(QT_VERSION_MAJOR)
            message("Qt${QT_VERSION_MAJOR}_FOUND: ${Qt${QT_VERSION_MAJOR}_FOUND}")
        endif()
    endif()
endmacro()
