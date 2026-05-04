cmake_minimum_required(VERSION 3.12)
project(taxi-service CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find userver (preinstalled in Docker image)
find_package(userver REQUIRED COMPONENTS core postgresql mongo redis)


# JWT-CPP is header-only, we just need to include it
# It will be installed in /usr/local/include/jwt-cpp
message(STATUS "JWT-CPP will be used as header-only library")

# User Service executable
add_executable(${PROJECT_NAME}-user
    src/user/main.cpp
    src/user/handlers.cpp
    src/common/models.cpp
    src/common/database.cpp
    src/common/mongo_database.cpp
    src/common/redis_cache.cpp
    src/common/rate_limiter.cpp
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
)

target_include_directories(${PROJECT_NAME}-user PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}-user PRIVATE
    userver::core
    userver::postgresql
    userver::mongo
    userver::redis
)

# Driver Service executable
add_executable(${PROJECT_NAME}-driver
    src/driver/main.cpp
    src/driver/handlers.cpp
    src/common/models.cpp
    src/common/database.cpp
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
)

target_include_directories(${PROJECT_NAME}-driver PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}-driver PRIVATE
    userver::core
    userver::postgresql
)

# Ride Service executable
add_executable(${PROJECT_NAME}-ride
    src/ride/main.cpp
    src/ride/handlers.cpp
    src/common/models.cpp
    src/common/database.cpp
    src/common/redis_cache.cpp
    src/common/rate_limiter.cpp
    src/auth/jwt_auth_checker.cpp
    src/auth/jwt_auth_factory.cpp
)

target_include_directories(${PROJECT_NAME}-ride PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}-ride PRIVATE
    userver::core
    userver::postgresql
    userver::redis
)

# Install targets
install(TARGETS ${PROJECT_NAME}-user ${PROJECT_NAME}-driver ${PROJECT_NAME}-ride
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PROJECT_NAME}
)

# Install configs
install(DIRECTORY configs/
    DESTINATION ${CMAKE_INSTALL_SYSCONFDIR}/${PROJECT_NAME}
    COMPONENT ${PROJECT_NAME}
)
