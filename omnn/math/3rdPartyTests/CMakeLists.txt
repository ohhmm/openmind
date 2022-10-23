
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

dep(fann)
test(math fann) # ${FANN_LIBRARIES} boost_test_exec_monitor-mt

# TODO: https://github.com/ohhmm/generator

# TODO: https://github.com/ohhmm/NonLinearSystem
