project(ChatBot)

file(GLOB src *.cpp)
add_executable(chatbot ${src})
target_link_libraries(chatbot libOpenMind)