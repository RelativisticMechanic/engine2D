# engine2D
2D Graphics Engine for quick prototyping written in C++

# Building
## Native
``` g++ myapp.cpp -o myapp.exe -lSDL2 -lSDL2_image ```
## Emscripten 
``` em++ myapp.cpp -o app.html -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp", "png"]' -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 --preload-file directory-of-resources ```
