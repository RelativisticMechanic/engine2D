# engine2D
## A 2D Graphics Engine for quick prototyping written in C++
![Screenshot](https://files.catbox.moe/i4616z.png)

# Compilation
## Native
``` g++ myapp.cpp -o myapp.exe -lSDL2 -lSDL2_image ```
## Emscripten (for the web)
``` em++ -DENGINE2D_EMSCRIPTEN_IMPLEMENTATION myapp.cpp -o app.html -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS='["bmp", "png"]' -s ALLOW_MEMORY_GROWTH=1 -s ASYNCIFY=1 --preload-file directory-of-resources ```
