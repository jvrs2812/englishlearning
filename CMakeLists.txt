cmake_minimum_required(VERSION 3.16)
project(english_buddy_bot LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# =============================================================================
# Dependencies via FetchContent
# =============================================================================
include(FetchContent)

# nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG        v3.11.3
)

# cpp-httplib (header-only HTTP client/server)
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.18.3
)

# tgbot-cpp (Telegram Bot API)
FetchContent_Declare(
    tgbot
    GIT_REPOSITORY https://github.com/reo7sp/tgbot-cpp.git
    GIT_TAG        v1.8
)

FetchContent_MakeAvailable(json httplib tgbot)

# =============================================================================
# System dependencies
# =============================================================================
find_package(SQLite3 REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Boost REQUIRED COMPONENTS system thread)

# =============================================================================
# Sources
# =============================================================================
set(SOURCES
    src/main.cpp
    src/telegram/TelegramBot.cpp
    src/llm/OllamaService.cpp
    src/storage/SQLiteConversationStore.cpp
    src/scheduler/DailyScheduler.cpp
    src/config/JsonConfig.cpp
    src/core/ChatController.cpp
    src/core/PromptBuilder.cpp
)

# =============================================================================
# Executable
# =============================================================================
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${SQLite3_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    TgBot
    nlohmann_json::nlohmann_json
    httplib::httplib
    SQLite::SQLite3
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Boost::system
    Boost::thread
    pthread
)
