# libSimpleSMCE?

A (dubious) proof of concept libSMCE derivative/reimplimentation that uses fork() along with mmap() ANONYMOUS | SHARED instead of Boost.Interprocess.  
This allows us to use std::pmr allocation and simplifies the whole IPC part of libSMCE as its _almost_ as if the child process is running on a thread.

it works in a few steps:
1. mmap some anonymous shared memory
2. placement new a monotonic_buffer_resource and a synchronized_pool_resource on it
3. let the user allocate what they wish
4. call fork() in a std::thread
5. In the parent process, return from the thread instantly; taking note of the child pid
6. In the child process put the user data pointer into an ENV var dlopen the sketch shared library and call its entry point.

This POC actually works, pins and uart are both implemented and working and sketches are compiled as usual; obviously some shortcuts are made..

Along with that this POC makes some api deviations:  
* the SharedBoard is now simply public, not hidden away by a BoardView and functionality is implemented on the data (e.g. see UartChannel.hxx)
* Toolchain is gone, Sketch compiles itself now
* CMake Build log is now streamed out via a provided std::ostream as that is the proper abstraction for it
* CMake scripts / files are embedded into the binary and extracted on demand
* Ardrivo is compiled by the end user so UDDs can be implemented simply by adding a unordered_map<string, base_class> to the shared board
* arduino-prelude expected on PATH (for simplicity..)

To run the provided test script do: `mkdir build && cmake -B build && cmake --build build && ./build/forkmem-test`
It should technically work on your linux machine if you got boost headers and cmake / arduino-prelude on your PATH (install is yoted)

Windows support is theoretically possible if you can convice windows to map the same memory address to 2 processes.
