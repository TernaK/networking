## networking

C++ sockets library.

### standalone use

`mkdir build` and from that folder `cmake .. && make -j4`.

### using the library

I recommend importing the project as a git submodule (or subfolder) in a cmake project. `add_subdirectory(/path/to/networking)` in your  `CMakelists.txt`. Link using `target_link_library(<target> networking_lib)` and this will expose the [networking/include](include) headers path in code.
