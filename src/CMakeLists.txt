set(networking_include_dir ${PROJECT_SOURCE_DIR}/include)
set(networking_sources
  networking.cpp ${networking_include_dir}/networking/networking.h
)

add_library(networking_lib ${networking_sources})
target_include_directories(networking_lib PUBLIC ${networking_include_dir})

if(${PROJECT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  add_executable(networking main.cpp)
  target_link_libraries(networking networking_lib)
endif()
