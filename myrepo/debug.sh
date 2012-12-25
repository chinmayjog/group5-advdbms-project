#!/bin/bash

`g++ -c -g DBHeader.cpp`
`g++ -c -g SysTables.cpp`
`g++ -c -g SysColumns.cpp`
`g++ -c -g SysIndex.cpp`
`g++ -c -g FreeList.cpp`
`g++ -c -g DB.cpp`
`g++ -c -g DBTest.cpp`
`g++ -g SysTables.o SysColumns.o SysIndex.o FreeList.o DBHeader.o DB.o DBTest.o -o DBTestDebug.out`
