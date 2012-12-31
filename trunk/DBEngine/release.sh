#!/bin/bash

`g++ -c DBHeader.cpp`
`g++ -c SysTables.cpp`
`g++ -c SysColumns.cpp`
`g++ -c SysIndex.cpp`
`g++ -c FreeList.cpp`
`g++ -c DB.cpp`
`g++ -c DBTest.cpp`
`g++ SysTables.o SysColumns.o SysIndex.o FreeList.o DBHeader.o DB.o DBTest.o -o DBTestDebug.out`
