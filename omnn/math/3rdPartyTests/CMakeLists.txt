test(math fann) # ${FANN_LIBRARIES} boost_test_exec_monitor-mt

if(UNIX)
    include(FetchContent)

    FetchContent_Declare(
      overheat
      GIT_REPOSITORY https://github.com/ohhmm/overheat.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(overheat)

    test(math overheat)
endif()

# TODO: https://github.com/ohhmm/generator

# TODO: https://github.com/ohhmm/NonLinearSystem
