cmake_minimum_required(VERSION 3.26)
project(GAMM)

set(CMAKE_CXX_STANDARD 23)

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
        utilities/Lines.h
        utilities/Lines.cpp
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
        test_cases/Case.h
        test_cases/Case.cpp
        fluid_dynamics/GAMM.cpp
        fluid_dynamics/GAMM.h
        utilities/Instructions.cpp
        utilities/Instructions.h
)
