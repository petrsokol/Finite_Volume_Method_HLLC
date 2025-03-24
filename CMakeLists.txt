cmake_minimum_required(VERSION 3.26)
project(GAMM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-Wall -O3 -fopenmp")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG_MODE -fopenmp") # -D + name
set(CMAKE_C_FLAGS_RELEASE "-O3")

find_package(OpenMP COMPONENTS CXX)

add_executable(GAMM main.cpp
        geometry/Point.cpp
        geometry/Point.h
        geometry/Cell.cpp
        geometry/Cell.h
        geometry/Interface.cpp
        geometry/Interface.h
        fluid_dynamics/Def.cpp
        fluid_dynamics/Def.h
        structures/Conservative.cpp
        structures/Conservative.h
        structures/Primitive.cpp
        structures/Primitive.h
        structures/Vector.cpp
        structures/Vector.h
        fluid_dynamics/Scheme.cpp
        fluid_dynamics/Scheme.h
        fluid_dynamics/Bound.cpp
        fluid_dynamics/Bound.h
        fluid_dynamics/NACA.cpp
        fluid_dynamics/NACA.h
        utilities/DataIO.cpp
        utilities/DataIO.h
        fluid_dynamics/GAMM.cpp
        fluid_dynamics/GAMM.h
        utilities/Instructions.cpp
        utilities/Instructions.h
        geometry/Mesh.cpp
        geometry/Mesh.h
        geometry/MeshParams.cpp
        geometry/MeshParams.h
        utilities/Timer.cpp
        utilities/Timer.h
)

set(TARGETS
        GAMM
)

target_link_libraries(GAMM PRIVATE OpenMP::OpenMP_CXX)
