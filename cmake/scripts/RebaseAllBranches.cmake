# Implement proper CMake functions for both Windows and Unix environments
include(CMakeParseArguments)

function(check_git_executable)
    if(NOT GIT_EXECUTABLE)
        message(FATAL_ERROR "Git executable not found. Please install Git.")
    endif()
endfunction()

function(add_rebase_all_branches_target)
    check_git_executable()

    if(WIN32)
        set(SCRIPT_PATH "${CMAKE_CURRENT_LIST_DIR}/rebase_all_branches.ps1")
        if(NOT EXISTS "${SCRIPT_PATH}")
            message(FATAL_ERROR "PowerShell script not found at ${SCRIPT_PATH}")
        endif()

        add_custom_target(rebase-all-branches-to-main
            COMMAND pwsh -NoProfile -NonInteractive -ExecutionPolicy Bypass
                    -File "${SCRIPT_PATH}"
                    -GitExecutable "${GIT_EXECUTABLE}"
                    -MainBranch main
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Rebasing all branches to main using PowerShell"
            VERBATIM
        )
    else()
        set(SCRIPT_PATH "${CMAKE_CURRENT_LIST_DIR}/rebase_all_branches.sh")
        if(NOT EXISTS "${SCRIPT_PATH}")
            message(FATAL_ERROR "Shell script not found at ${SCRIPT_PATH}")
        endif()

        # Ensure script is executable
        execute_process(
            COMMAND chmod +x "${SCRIPT_PATH}"
            RESULT_VARIABLE CHMOD_RESULT
        )
        if(NOT CHMOD_RESULT EQUAL 0)
            message(WARNING "Failed to set executable permissions on ${SCRIPT_PATH}")
        endif()

        add_custom_target(rebase-all-branches-to-main
            COMMAND "${SCRIPT_PATH}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            COMMENT "Rebasing all branches to main using shell script"
            VERBATIM
        )
    endif()

    # Add target properties
    set_target_properties(rebase-all-branches-to-main PROPERTIES
        EXCLUDE_FROM_ALL TRUE
        EXCLUDE_FROM_DEFAULT_BUILD TRUE
        FOLDER "util/git"
    )
endfunction()
