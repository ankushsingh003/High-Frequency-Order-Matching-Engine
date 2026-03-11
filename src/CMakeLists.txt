add_library(MatchingEngineCore STATIC OrderBook.cpp)
target_include_directories(MatchingEngineCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(MatchingEngine main.cpp)
target_link_libraries(MatchingEngine PRIVATE MatchingEngineCore)
