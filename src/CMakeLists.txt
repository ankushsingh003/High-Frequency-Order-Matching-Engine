add_library(MatchingEngineCore STATIC OrderBook.cpp)
target_include_directories(MatchingEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

include(FetchContent)
FetchContent_Declare(
  asio
  URL https://github.com/chriskohlhoff/asio/archive/asio-1-28-0.zip
)
FetchContent_MakeAvailable(asio)

add_executable(MatchingEngine main.cpp)
target_link_libraries(MatchingEngine PRIVATE MatchingEngineCore asio)
target_compile_definitions(MatchingEngine PRIVATE ASIO_STANDALONE)
