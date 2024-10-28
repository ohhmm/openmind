include(${CMAKE_CURRENT_LIST_DIR}/scripts/RebaseAllBranches.cmake)

find_package(Git)
if(GIT_FOUND)
    if(WIN32)
        add_rebase_all_branches_target()
    else()
        add_custom_target(rebase-all-branches-to-main
            COMMAND ${CMAKE_CURRENT_LIST_DIR}/scripts/rebase_all_branches.sh
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Rebasing all branches onto main using shell script."
        )
    endif()
endif()
