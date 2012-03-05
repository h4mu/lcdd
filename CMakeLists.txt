PROJECT(lcdd)

set(CMAKE_CXX_FLAGS "-m32 -march=i486")
add_definitions("-ansi -pedantic -Wall -DDAEMON")
link_directories(".")
ADD_EXECUTABLE(lcdd lcd.cpp main.cpp)
target_link_libraries(lcdd phidget21)
