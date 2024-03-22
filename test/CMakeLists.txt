add_executable(TestSimulation data.cpp test_simulation.cpp)
target_include_directories(PM_Simulation PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(TestSimulation PUBLIC PM_Simulation Catch2 Catch2::Catch2WithMain)