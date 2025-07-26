cmake_minimum_required(VERSION 3.16)
project(TOmaDO)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets Sql REQUIRED)

# Include directories
include_directories(include)

# Source files
set(SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/database/databasemanager.cpp
        src/timer/compacttimerwidget.cpp
        src/calendar/components/coloreditemdelegate.cpp
        src/calendar/components/calendarstyles.cpp
        src/calendar/components/calendarutils.cpp
        src/calendar/components/calendarnavigation.cpp
        src/calendar/components/daydetailsdialog.cpp
        src/calendar/components/calendarcellrenderer.cpp
        src/calendar/components/calendarinteractionhandler.cpp
        src/calendar/components/customcalendarwidget.cpp
        src/calendar/calendarwidget.cpp
        src/common/colorpickerwidget.cpp
        src/common/colorbuttonrenderer.cpp
        src/common/colorpickergridmanager.cpp
        src/common/colorbutton.cpp
        src/common/colorselectionmanager.cpp
        src/database/databaseconnectionmanager.cpp
        src/database/databaseschemamanager.cpp
        src/database/repositories/goalrepository.cpp
        src/database/repositories/todorepository.cpp
        src/database/repositories/timerrepository.cpp
        src/database/services/colorservices.cpp
        src/dashboard/basedashboardwidget.cpp
        src/dashboard/goaldashboardwidget.cpp
        src/dashboard/dailydashboardwidget.cpp
        src/dashboard/totaldashboardwidget.cpp
        src/dashboard/counterwidget.cpp
        src/dashboard/weeklydashboardwidget.cpp
        src/dialogs/goal/goaldialogvalidator.cpp
        src/dialogs/goal/goaldialogformmanager.cpp
        src/dialogs/goal/goaldialogui.cpp
        src/dialogs/todo/tododialogui.cpp
        src/dialogs/todo/tododialogformmanager.cpp
        src/dialogs/todo/tododialogvalidator.cpp
        src/dialogs/goal/goaldialog.cpp
        src/dialogs/todo/tododialog.cpp
        src/settings/settingsdialog.cpp
        src/statistics/components/statisticsgoalsviewwidget.cpp
        src/statistics/components/statisticsheaderwidget.cpp
        src/statistics/components/statisticstimelyviewwidget.cpp
        src/statistics/components/statisticsgoalsviewwidget.cpp
        src/statistics/components/statisticsdatamanager.cpp
        src/statistics/statisticswidget.cpp
        src/timer/soundmanager.cpp
        src/timer/lavalamppaintwidget.cpp
        src/timer/timerbuttonmanager.cpp
        src/timer/timercore.cpp
        src/timer/timergoalsmanager.cpp
        src/timer/timerdisplaymanager.cpp
)

# Header files
set(HEADERS
        include/mainwindow.h
        include/calendar/components/coloreditemdelegate.h
        include/calendar/components/calendarstyles.h
        include/calendar/components/calendarutils.h
        include/calendar/components/calendarnavigation.h
        include/calendar/components/daydetailsdialog.h
        include/calendar/components/calendarcellrenderer.h
        include/calendar/components/calendarinteractionhandler.h
        include/calendar/components/customcalendarwidget.h
        include/calendar/calendarwidget.h
        include/common/colorpickerwidget.h
        include/common/colorpickerwidgetconstants.h
        include/common/colorbuttonrenderer.h
        include/common/colorpickergridmanager.h
        include/common/colorbutton.h
        include/common/colorselectionmanager.h
        include/dashboard/basedashboardwidget.h
        include/dashboard/goaldashboardwidget.h
        include/dashboard/dailydashboardwidget.h
        include/dashboard/totaldashboardwidget.h
        include/dashboard/weeklydashboardwidget.h
        include/dashboard/counterwidget.h
        include/database/databaseconstants.h
        include/database/databaseconnectionmanager.h
        include/database/databaseschemamanager.h
        include/database/repositories/goalrepository.h
        include/database/repositories/todorepository.h
        include/database/repositories/timerrepository.h
        include/database/services/colorservice.h
        include/database/databasemanager.h
        include/dialogs/goal/goaldialogconstants.h
        include/dialogs/goal/goaldialogvalidator.h
        include/dialogs/goal/goaldialogformmanager.h
        include/dialogs/goal/goaldialogui.h
        include/dialogs/todo/tododialogconstants.h
        include/dialogs/todo/tododialogui.h
        include/dialogs/todo/tododialogformmanager.h
        include/dialogs/todo/tododialogvalidator.h
        include/dialogs/goal/goaldialog.h
        include/dialogs/todo/tododialog.h
        include/settings/settingsdialog.h
        include/statistics/components/statisticsheaderwidget.h
        include/statistics/components/statisticstimelyviewwidget.h
        include/statistics/components/statisticsgoalsviewwidget.h
        include/statistics/components/statisticsdatamanager.h
        include/statistics/statisticswidget.h
        include/timer/soundmanager.h
        include/timer/lavalamppaintwidget.h
        include/timer/compacttimerwidget.h
        include/ui/dialogstyles.h
        include/timer/timerbuttonmanager.h
        include/timer/timercore.h
        include/timer/timergoalsmanager.h
        include/timer/timerdisplaymanager.h
        src/timer/timerdisplaymanager.cpp
)

# Create executable
add_executable(TOmaDO ${SOURCES} ${HEADERS})

# Link Qt libraries
target_link_libraries(TOmaDO Qt5::Core Qt5::Widgets Qt5::Sql)

# Set up Qt MOC
set_target_properties(TOmaDO PROPERTIES
        AUTOMOC ON
        AUTOUIC ON
        AUTORCC ON
)

# Installation configuration
install(TARGETS TOmaDO
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION bin
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/resources/sounds/
        DESTINATION share/tomado/sounds
        FILES_MATCHING PATTERN "*.wav"
        PATTERN "*.mp3"
)

# Create a desktop file for the application
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/packaging/TOmaDO.desktop.in
    ${CMAKE_CURRENT_BINARY_DIR}/TOmaDO.desktop
    @ONLY
)

# Install desktop file and icon
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/TOmaDO.desktop 
    DESTINATION share/applications
)
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/packaging/TOmaDO.png
    DESTINATION share/icons/hicolor/128x128/apps
)

# Package configuration with CPack
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "tomado")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_RELEASE "1")
set(CPACK_PACKAGE_CONTACT "Lazzara Giuliano <giulianolazzara@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "TOmaDO - Task and Time Management Application")
set(CPACK_PACKAGE_DESCRIPTION "A productivity application for task management and time tracking")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5widgets5, libqt5sql5")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")

# Include CPack
include(CPack)

# Create and set permissions for the database directory
install(DIRECTORY DESTINATION ${CMAKE_INSTALL_PREFIX}/share/tomado/db
        DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
                             GROUP_READ GROUP_WRITE GROUP_EXECUTE
                             WORLD_READ WORLD_WRITE WORLD_EXECUTE)


# Set appropriate permissions
install(CODE "execute_process(COMMAND chmod -R 777 \${CMAKE_INSTALL_PREFIX}/share/tomado/db)")