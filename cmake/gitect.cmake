
find_package(Git QUIET)
if(NOT GIT_EXECUTABLE AND WIN32)
	find_program(GIT_EXECUTABLE git PATHS 
		"$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/Git/cmd/"
		)
	find_package(Git)
endif()
if(GIT_EXECUTABLE)
	message("GIT_EXECUTABLE: ${GIT_EXECUTABLE}")

	add_custom_target(git-gui
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} gc
		COMMAND ${GIT_EXECUTABLE} gui
	)
	set_target_properties(git-gui PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	add_custom_target(rebase-main
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash https://github.com/ohhmm/openmind main
	)
	set_target_properties(rebase-main PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	add_custom_target(push-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push https://github.com/ohhmm/openmind develop
	)
	set_target_properties(push-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	add_custom_target(force-push-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push https://github.com/ohhmm/openmind develop -f
	)
	set_target_properties(force-push-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	add_custom_target(update
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(update PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util")

	if(openmind_SOURCE_DIR AND NOT openmind_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
		add_custom_target(update-openmind
			WORKING_DIRECTORY ${openmind_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash https://github.com/ohhmm/openmind main
			COMMAND ${GIT_EXECUTABLE} fetch --all
		)
		set_target_properties(update-openmind PROPERTIES
			EXCLUDE_FROM_ALL 1
			EXCLUDE_FROM_DEFAULT_BUILD 1
			FOLDER "util")
		add_dependencies(update update-openmind)

	endif()
endif()
