
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
		FOLDER "util/git")

	add_custom_target(rebase-main
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main
	)
	set_target_properties(rebase-main PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(rebase-main-interactive
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} fetch --all
		COMMAND ${GIT_EXECUTABLE} rebase -i --autostash origin/main
	)
	set_target_properties(rebase-main-interactive PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(update
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(update PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	if(openmind_SOURCE_DIR AND NOT openmind_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
		add_custom_target(update-openmind
			WORKING_DIRECTORY ${openmind_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash || ${GIT_EXECUTABLE} pull --rebase --autostash origin HEAD
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin HEAD
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash https://github.com/ohhmm/openmind HEAD
			COMMAND ${GIT_EXECUTABLE} fetch --all
		)
		set_target_properties(update-openmind PROPERTIES
			EXCLUDE_FROM_ALL 1
			EXCLUDE_FROM_DEFAULT_BUILD 1
			FOLDER "util/git")
		add_dependencies(update update-openmind)

		add_custom_target(push-openmind-develop
			WORKING_DIRECTORY ${openmind_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} push origin develop
			COMMAND ${GIT_EXECUTABLE} fetch --all
		)
		set_target_properties(push-openmind-develop PROPERTIES
			EXCLUDE_FROM_ALL 1
			EXCLUDE_FROM_DEFAULT_BUILD 1
			FOLDER "util/git")

		add_custom_target(force-push-openmind-develop
			WORKING_DIRECTORY ${openmind_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} push origin develop -f
			COMMAND ${GIT_EXECUTABLE} fetch --all
		)
		set_target_properties(force-push-openmind-develop PROPERTIES
			EXCLUDE_FROM_ALL 1
			EXCLUDE_FROM_DEFAULT_BUILD 1
			FOLDER "util/git")
	endif()

	add_custom_target(push-to-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD:develop
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(push-to-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(force-push-head-to-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD:develop -f
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(force-push-head-to-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(force-push-head1-to-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD~1:develop -f
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(force-push-head1-to-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(force-push-head2-to-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD~2:develop -f
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(force-push-head2-to-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(force-push-head3-to-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD~3:develop -f
		COMMAND ${GIT_EXECUTABLE} fetch --all
	)
	set_target_properties(force-push-head3-to-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

endif()
