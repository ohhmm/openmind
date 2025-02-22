if(NOT GIT_EXECUTABLE)
	find_package(Git QUIET)
endif()

if(NOT GIT_EXECUTABLE AND WIN32)
	find_program(GIT_EXECUTABLE git PATHS 
		"$ENV{ProgramFiles}/Microsoft Visual Studio/*/*/Common7/IDE/CommonExtensions/Microsoft/TeamFoundation/Team Explorer/Git/cmd/"
		)
	find_package(Git)
endif()

if(GIT_EXECUTABLE AND WIN32)
	string(REGEX REPLACE "/" "\\\\" GIT_EXE_CMD "\"${GIT_EXECUTABLE}\"")
	message("GIT_EXE_CMD : ${GIT_EXE_CMD}")
endif()

macro(add_git_target_multiple_commands)
	set(args ${ARGN})
	set(new_target ${ARGV0})
	list(POP_FRONT args)
	add_custom_target(${new_target}
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		${args}
	)
	set_target_properties(${new_target} PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")
endmacro()

macro(add_git_target)
	set(args ${ARGN})
	set(new_target ${ARGV0})
	list(POP_FRONT args)
	add_git_target_multiple_commands(${new_target}
		COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
		COMMAND ${GIT_EXECUTABLE} ${args}
		COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
		)
endmacro()

macro(add_force_push_head_to_develop_target)
	set(new_target force-push-head${ARGV0}-to-develop)
	add_custom_target(${new_target}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD~${ARGV0}:develop -f || ${GIT_EXECUTABLE} push origin HEAD~${ARGV0}:refs/heads/develop
		COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
		)
	set_target_properties(${new_target} PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git/develop")
endmacro()

if(GIT_EXECUTABLE)
	message("GIT_EXECUTABLE: ${GIT_EXECUTABLE}")

	add_git_target_multiple_commands(git-gui
		COMMAND ${GIT_EXECUTABLE} gc
		COMMAND ${GIT_EXECUTABLE} gui
	)
	add_git_target(stash stash)
	add_git_target(unstash stash pop)
	add_git_target(rebase-origin-main pull --rebase --autostash origin main)
	add_git_target(rebase-main-interactive rebase -i --autostash origin/main)
	add_git_target(offline-rebase-origin-main-interactive rebase -i --autostash origin/main)
	add_git_target(update pull --rebase --autostash)
	add_git_target(bring-to-develop rebase HEAD develop --autostash)
	set_target_properties(bring-to-develop PROPERTIES FOLDER "util/git/develop")
	add_git_target_multiple_commands(bring-to-develop-interactive 
		COMMAND ${GIT_EXECUTABLE} rebase HEAD develop --autostash || ${GIT_EXECUTABLE} rebase --continue
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main || ${GIT_EXECUTABLE} rebase --continue
		COMMAND ${GIT_EXECUTABLE} rebase -i --autostash origin/main || ${GIT_EXECUTABLE} rebase --continue
		COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
	)
	set_target_properties(bring-to-develop-interactive PROPERTIES FOLDER "util/git/develop")

	if(openmind_SOURCE_DIR AND NOT openmind_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
		add_custom_target(update-openmind
			WORKING_DIRECTORY ${openmind_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash || ${GIT_EXECUTABLE} pull --rebase --autostash origin HEAD
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin HEAD
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash https://github.com/ohhmm/openmind HEAD
			COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
		)
		set_target_properties(update-openmind PROPERTIES
			EXCLUDE_FROM_ALL 1
			EXCLUDE_FROM_DEFAULT_BUILD 1
			FOLDER "util/git")
		add_dependencies(update update-openmind)

		add_git_target(push-openmind-develop push origin develop)
		add_git_target(force-push-openmind-develop push origin develop -f)
	endif()

	if(WIN32)
		set(PS_GIT_CMD ".'${GIT_EXECUTABLE}' branch --merged origin/main | Select-String -NotMatch '^\\s*\\*?\\s*main$$' | ForEach-Object { .'${GIT_EXECUTABLE}' branch -D $$_.Line.Trim() }")
		add_custom_target(delete-merged-branches
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main
			COMMAND ${GIT_EXECUTABLE} rebase --autostash origin/main main
			COMMAND powershell -Command "${PS_GIT_CMD}"
			COMMENT "Deleting branches that already are in main using powershell."
		)
	else(WIN32)
		add_custom_target(delete-merged-branches
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
			COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main
			COMMAND ${GIT_EXECUTABLE} rebase --autostash origin/main main
			COMMAND ${GIT_EXECUTABLE} branch --merged origin/main | grep -v "^* main" | xargs -n 1 -r ${GIT_EXECUTABLE} branch -d
			COMMENT "Deleting branches that already are in main using bash."
		)
	endif(WIN32)
	set_target_properties(delete-merged-branches PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(push-to-develop
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} push origin HEAD:develop || ${GIT_EXECUTABLE} push origin HEAD:refs/heads/develop
		COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
	)
	set_target_properties(push-to-develop PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git/develop")

	foreach(NUM RANGE 0 9)
		add_force_push_head_to_develop_target(${NUM})
	endforeach()

	add_git_target(force-push-head push -f)
	add_git_target(force-push-origin push -f origin)

	add_git_target(rebase-continue rebase --continue)

	add_custom_target(rebase-and-force-push-origin
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		COMMAND ${GIT_EXECUTABLE} pull --rebase --autostash origin main && ${GIT_EXECUTABLE} push -f origin
	)
	set_target_properties(rebase-and-force-push-origin PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	if(WIN32)
		set(CMD_LIST_BRANCHES "${GIT_EXE_CMD} for-each-ref --format=%(refname:short) refs/heads/ --no-contains main")
		set(CMD_REBASE_BRANCH "${GIT_EXE_CMD} rebase --autostash origin/main %b")
		set(CMD_ITERATE_BRANCHES "for /f \"usebackq tokens=*\" %b in ( branches.txt ) do ( ${CMD_REBASE_BRANCH} )")
		set(CMD_LINE_SUFFIX " ^|^| ${GIT_EXE_CMD} rebase --abort")
		add_custom_target(rebase-all-branches
			DEPENDS process-cmd-rebase-or-abort

            COMMAND ${CMAKE_COMMAND} -E echo "Rebasing all branches onto origin/main"
			COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error

			COMMAND cmd /c ${CMD_LIST_BRANCHES} > branches.txt
			COMMAND type NUL > rebase.cmd
			COMMAND cmd /c ( for /f "usebackq tokens=*" %b in ( branches.txt ) do @echo ${CMD_REBASE_BRANCH} ) >> rebase.cmd
			COMMAND $<TARGET_FILE:process-cmd-rebase-or-abort>

            COMMENT "Rebasing all branches onto origin/main using cmd with improved branch handling."
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
	else()
		add_custom_target(rebase-all-branches
			COMMAND ${GIT_EXECUTABLE} fetch --all || echo git fetch error
			COMMAND ${CMAKE_COMMAND} -E echo "Rebasing all branches onto main"
			COMMAND ${CMAKE_COMMAND} -E env bash "${CMAKE_SOURCE_DIR}/cmake/rebase-all-branches.sh"
			WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		)
	endif()
	set_target_properties(rebase-all-branches PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")

	add_custom_target(origin-remote-maintain
		DEPENDS maintain
		COMMAND ${CMAKE_COMMAND} -E echo "Maintain branches including origin remote"
		COMMAND $<TARGET_FILE:maintain> --silent

		COMMENT "Maintain branches including origin remote"
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	)
	set_target_properties(origin-remote-maintain PROPERTIES
		EXCLUDE_FROM_ALL 1
		EXCLUDE_FROM_DEFAULT_BUILD 1
		FOLDER "util/git")
endif()
