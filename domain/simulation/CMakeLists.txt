add_library(simulation_lib SHARED
    SimulationGraph.h
    GraphBuilder.h
    GraphBuilder.cpp
    IComponentModel.h
    LinearModels.h
    ComponentModelFactory.h
    Solver.h
    Solver.cpp
)

target_include_directories(simulation_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# ДОБАВИЛИ component_library В КОНЕЦ СТРОКИ:
target_link_libraries(simulation_lib PUBLIC domain_lib component_library)