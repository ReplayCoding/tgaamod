add_library(
    tgaamod SHARED
    dll.cpp
    tgaamod.cpp
    hook.cpp
)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(tgaamod PRIVATE spdlog::spdlog)

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)
target_include_directories(tgaamod PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(tgaamod PRIVATE ${DETOURS_LIBRARY})

find_package(fmt CONFIG REQUIRED)
target_link_libraries(tgaamod PRIVATE fmt::fmt)