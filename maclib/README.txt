Follow instructions to install the libraries and compile from scratch.

To just have things work:

Stick all the files dylib/a/etc into /usr/local/lib
Then, put the out file in your own directory and run it with ./out

The compile line for macosx is: g++ -o out -std=c++11 -I/usr/local/include/SDL2 -I/usr/local/include -L/usr/local/lib main.cpp conf.cpp Engine.cpp -lSDL2main -lSDL2 -lSDL2_image -lpng
