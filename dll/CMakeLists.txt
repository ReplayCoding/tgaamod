find_package(spdlog CONFIG REQUIRED)

add_library(
    tgaamod SHARED
    dll.cpp
)

target_link_libraries(tgaamod PRIVATE spdlog::spdlog)