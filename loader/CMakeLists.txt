find_package(spdlog CONFIG REQUIRED)

add_executable(loader main.cpp)
target_link_libraries(loader PRIVATE spdlog::spdlog)