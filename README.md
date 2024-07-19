# Conductor Tool demo application

## Conductor Tool demo application
Conductor Tool demo application is an application which is used for demonstrating the [Conductor Tool](https://conductor.alifsemi.com/). Also referred to as the Device Configuration Tool or DCT.
Application blinks two LEDs and writes output to UART.
Check more information about the application and the Conductor tool from the [Conductor-Tool demo application guide](Conductor-Tool.md).

# Building Conductor Tool demo application
```
git clone https://github.com/alifsemi/alif_conductor-demo.git
cd alif_conductor-demo
git submodule init
git submodule update
```

## Building for XIP
By default application code is executed from TCM. If you want to run code directly from MRAM (XIP), add parameter `-DXIP=1` to your CMake command as seen below.
Both cores must use the same run mode.

## Build Conductor Tool demo for High-Performance M55 (M55_HP)
```
mkdir build_hp
cd build_hp
cmake .. -DENSEMBLE_CORE=M55_HP -DCMAKE_TOOLCHAIN_FILE=../toolchain-gnu.cmake -DCMAKE_BUILD_TYPE=Release -DXIP=1
OR
cmake .. -DENSEMBLE_CORE=M55_HP -DCMAKE_TOOLCHAIN_FILE=../toolchain-gnu.cmake -DCMAKE_BUILD_TYPE=Release
make
```

## Build Conductor Tool demo for High-Efficiency M55 (M55_HE)
```
mkdir build_he
cd build_he
cmake .. -DENSEMBLE_CORE=M55_HE -DCMAKE_TOOLCHAIN_FILE=../toolchain-gnu.cmake -DCMAKE_BUILD_TYPE=Release -DXIP=1
OR
cmake .. -DENSEMBLE_CORE=M55_HE -DCMAKE_TOOLCHAIN_FILE=../toolchain-gnu.cmake -DCMAKE_BUILD_TYPE=Release
make
```

## Build Conductor demo for High-Efficiency M55 (M55_HE) with memory stitching

**NOTE** Currently only HE TCM build is supported
```
mkdir build_he
cd build_he
cmake .. -DENSEMBLE_CORE=M55_HE -DCMAKE_TOOLCHAIN_FILE=../toolchain-gnu.cmake -DCMAKE_BUILD_TYPE=Release -DMEMORY_STITCHING=ON
make

Generate ATOC with command:
./app-gen-toc --filename build/config/conductor_tool_demo_tcm_memory_stitching.json
```

## Building for different board variants
By default, the application is built for DevKit-E7 (Gen 2).
To build the application for different boards, define ENSEMBLE_BOARD variable for cmake.
```
-DENSEMBLE_BOARD="devkit_gen2"
```
Possible options: devkit_gen1/2, appkit_alpha1/2, appkit_gen2 or devkit_b0_cob0

## Flashing the test application
In the root there are two SETOOLS config files for this application, [conductor_tool_demo_tcm.json](conductor_tool_demo_tcm.json) or [conductor_tool_demo_mram.json](conductor_tool_demo_mram.json) which can be used for ISP with the Alif Security Toolkit.
Use either one based on whether your compilation has XIP (MRAM) enabled or do you run code from TCM (default). In root, there is two binaries sram0 and sram1, they need to be copied to same folder as demo binaries when
using SE tools for flashing. [conductor_tool_demo_tcm.json](conductor_tool_demo_tcm.json) or [conductor_tool_demo_mram.json](conductor_tool_demo_mram.json) both require demo application for both cores and sram0/1-binaries.

M55_HE core uses UART2 and M55_HP core uses UART4 for printing the information to demonstrate how the application works.
