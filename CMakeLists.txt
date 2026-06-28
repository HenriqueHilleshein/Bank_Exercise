cmake_minimum_required(VERSION 3.10.0)

project(Bank_Exercise VERSION 0.1.0 LANGUAGES C CXX)

add_executable(Bank_Exercise
    Source/main.cpp
    Source/Account.cpp
    Source/CustomerAccount.cpp
    Source/EnterpriseAccount.cpp
    Source/FakeDBConnection.cpp
    Source/AccountService.cpp
    Source/Server.cpp
)

target_include_directories(Bank_Exercise
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)

FetchContent_MakeAvailable(httplib)

target_link_libraries(Bank_Exercise PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(Bank_Exercise PRIVATE httplib::httplib)

include(CTest)
enable_testing()