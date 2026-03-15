cmake_minimum_required(VERSION 3.10)
project(mycpu_simulator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
include_directories(tests/unit)

set(CORE_SOURCES
    src/Bus.cpp
    src/CPU.cpp
    src/Memory.cpp
    src/Timer.cpp
    src/Uart.cpp
    src/TestDevice.cpp
    src/ProgramLoader.cpp
)

add_executable(mycpu_sim
    ${CORE_SOURCES}
    src/simulator_main.cpp
)

add_executable(test_memory
    ${CORE_SOURCES}
    tests/unit/test_memory.cpp
)

add_executable(test_cpu_basic
    ${CORE_SOURCES}
    tests/unit/test_cpu_basic.cpp
)

add_executable(test_mmu
    ${CORE_SOURCES}
    tests/unit/test_mmu.cpp
)

add_executable(program_test_runner
    tests/program/program_test_runner.cpp
)

add_executable(test_cpu_state
    ${CORE_SOURCES}
    tests/unit/test_cpu_state.cpp
)

add_executable(test_testdevice
    ${CORE_SOURCES}
    tests/unit/test_testDevice.cpp
)

add_executable(test_runtime_loop
    ${CORE_SOURCES}
    tests/unit/test_runtime_loop.cpp
)

add_executable(test_runtime_halt
    ${CORE_SOURCES}
    tests/unit/test_runtime_halt.cpp
)

add_executable(test_program_loader
    ${CORE_SOURCES}
    tests/unit/test_program_loader.cpp
)

add_executable(test_runtime_protocol
    ${CORE_SOURCES}
    tests/unit/test_runtime_protocol.cpp
)

add_executable(test_trap_protocol
    ${CORE_SOURCES}
    tests/unit/test_trap_protocol.cpp
)