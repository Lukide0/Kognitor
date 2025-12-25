include_guard(GLOBAL)

include(FetchContent)
find_package(Git REQUIRED)

FetchContent_Declare(
  microstd_repo
  GIT_REPOSITORY https://github.com/Lukide0/microstd.git
  GIT_TAG "v0.2.2"
  GIT_SHALLOW TRUE
  GIT_PROGRESS ON)
FetchContent_MakeAvailable(microstd_repo)

