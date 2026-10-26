test(math fann) # ${FANN_LIBRARIES} boost_test_exec_monitor-mt

include(FetchContent)

    FetchContent_Declare(
      NonLinearSystem
      GIT_REPOSITORY https://github.com/ohhmm/NonLinearSystem.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(NonLinearSystem)

    FetchContent_Declare(
      overheat
      GIT_REPOSITORY https://github.com/ohhmm/overheat.git
      GIT_TAG main
    )
    FetchContent_MakeAvailable(overheat)

# TODO: https://github.com/ohhmm/generator
