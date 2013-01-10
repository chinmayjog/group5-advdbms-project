#!/bin/bash

`g++ -c -g DBHeader.cpp`
`g++ -c -g SysTables.cpp`
`g++ -c -g SysColumns.cpp`
`g++ -c -g SysIndex.cpp`
`g++ -c -g FreeList.cpp`
`g++ -c -g DB.cpp`
`g++ -c -g DataPage.cpp`
`g++ -c -g DataTypes.cpp`
`g++ -c -g dataDirectoryPage.cpp`
`g++ -c -g main.cpp`
`g++ -g SysTables.o SysColumns.o SysIndex.o FreeList.o DataPage.o DBHeader.o dataDirectoryPage.o DataTypes.o DB.o ../cachemgr/obj/BufferManager*.o main.o -o DBTestDebug.out`
