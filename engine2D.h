#ifndef __engine2D_H
#define __engine2D_H

#define ERROR_OUT(...) fprintf(stderr, __VA_ARGS__)
#define MSG_OUT(...) fprintf(stdout, __VA_ARGS__)

#define DEGTORAD(x) (x * 0.01745)

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <cstdint>
#include <vector>
#include <functional>
#ifdef ENGINE2D_EMSCRIPTEN_IMPLEMENTATION
#include <emscripten.h>
#endif
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace engine2D
{
    using namespace std;
    enum class Button
    {
        UP = 0,
        DOWN,
        LEFT,
        RIGHT,
        Q,
        E,
        R,
        SPACE,
        SHIFT,
        RETURN,
        CTRL,
        MOUSE1,
        MOUSE2,
        TOTAL_BUTTONS,
    };

    typedef uint8_t Timer;

    typedef struct
    {
        bool active = false;
        float time;
        float duration;
    } _internal_timer_t;

    float Clamp(float v, float min, float max)
    {
        if(v < min)
            return min;
        if(v > max)
            return max;
        return v;
    }

    float Map(float v, float v_min, float v_max, float m_min, float m_max)
    {
        return m_min + (v - v_min) / (v_max - v_min) * (m_max - m_min);
    }
    
    class Vector2
    {
        public:
        float x, y;
        
        Vector2()
        {
            this->x = 0.0f;
            this->y = 0.0f;
        }

        Vector2(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        void Set(float x, float y)
        {
            this->x = x;
            this->y = y;
        }

        float MagnitudeSquared()
        {
            return (this->x * this->x + this->y * this->y);
        }

        float Magnitude()
        {
            return sqrt(this->MagnitudeSquared());
        }
        
        Vector2 Rotate(float angle)
        {
            Set(this->x * cos(angle) - this->y * sin(angle), this->x * sin(angle) + this->y * cos(angle));
            return *this;
        }

        Vector2 Normalise()
        {
            float l = this->Magnitude();
            if(l == 0)
            {
                Set(0, 0);
                return *this;
            }

            Set(this->x / l, this->y / l);
            return *this;
        }

        Vector2 Ortho()
        {
            Set(y, -x);
            return *this;
        }

        Vector2 SetMagnitude(float mag)
        {
            this->Normalise();
            this->Set(this->x * mag, this->y * mag);
            return *this;
        }

        static float DotProduct(Vector2& v1, Vector2& v2)
        {
            return (v1.x * v2.x + v1.y * v2.y);
        }

    };

    Vector2 operator+ (Vector2 const &v1, Vector2 const &v2)
    {
        return Vector2(v1.x + v2.x, v1.y + v2.y);
    }

    Vector2 operator- (Vector2 const &v1, Vector2 const &v2)
    {
        return Vector2(v1.x - v2.x, v1.y - v2.y);
    }

    Vector2 operator+= (Vector2 const &v1, Vector2 const &v2)
    {
        return Vector2(v1.x + v2.x, v1.y + v2.y);
    }

    Vector2 operator-= (Vector2 const &v1, Vector2 const &v2)
    {
        return Vector2(v1.x - v2.x, v1.y - v2.y);
    }

    class Application
    {
        public:
        string app_name = "engine2D Application";

        virtual void Create(void)
        {

        }
        virtual void Update(float elapsed)
        {

        }

        virtual void Draw(float elapsed)
        {
            
        }

        virtual void OnKeyPress(float elapsed, Button key)
        {

        }

        virtual void OnKeyPressed(float elapsed, Button key)
        {

        }

        virtual void OnMouseMove(float elapsed, int x, int y, int dx, int dy)
        {

        }

        virtual void OnKeyRelease(float elapsed, Button key)
        {

        }

        virtual void OnTextInput(float elapsed, unsigned char key)
        {
            
        }

        virtual void OnTimerTick(float elapsed, Timer timer_id)
        {

        }

        virtual void OnDestroy(void)
        {

        }

        

        Application()
        {

        }
    };

    SDL_Window* application_window = NULL;
    SDL_Renderer* window_renderer = NULL;
    SDL_Texture* screen_texture = NULL;
    
    Application* app = NULL;

    vector<uint16_t> shape_array_x;
    vector<uint16_t> shape_array_y;
    static _internal_timer_t timers[256];

    int shape_x, shape_y;
    bool shape_free = true;
    bool shape_fill;
    uint8_t shape_r, shape_g, shape_b, shape_a;

    bool key_array[static_cast<int>(Button::TOTAL_BUTTONS)];


    unsigned int screen_width = 0;
    unsigned int screen_height = 0;
    unsigned int window_width = 0;
    unsigned int window_height = 0;
    unsigned int window_scale = 0;

    class Image
    {
        public:
        int width, height;
        SDL_Texture* data;
        SDL_Surface* image;
        
        Image(string filename)
        {
            SDL_Surface* im = IMG_Load(filename.c_str());
            this->image = im;
            if(im == NULL)
            {
                ERROR_OUT("Could not load image: %s\nMessage: %s\n", filename.c_str(), SDL_GetError());
                this->data = NULL;
            }
            else
            {
                this->data = SDL_CreateTextureFromSurface(window_renderer, im);
                this->width = im->w;
                this->height = im->h;
            }
        }

        Image(SDL_Surface* im)
        {
            this->image = SDL_CreateRGBSurfaceFrom(im->pixels, im->w, im->h, im->format->BitsPerPixel, im->pitch, im->format->Rmask, im->format->Gmask, im->format->Bmask, im->format->Amask);
            this->width = im->w;
            this->height = im->h;
            this->data = SDL_CreateTextureFromSurface(window_renderer, im);
        }

        void DrawImage(int x, int y, int offsetx=0, int offsety=0, int w=0, int h=0, float angle=0.0f, int pivotx=0, int pivoty=0, float scale = 1.0, bool h_flip=false, bool v_flip=false)
        {
            SDL_Rect src, dest;
            SDL_Point p;
            SDL_RendererFlip flip = SDL_FLIP_NONE;
            p.x = pivotx;
            p.y = pivoty;
            src.x = offsetx; src.y = offsety;
            if(w == 0 || h == 0)
            {
                src.w = this->width - offsetx; src.h = this->height - offsety;
            }
            else
            {
                src.w = w; src.h = h;
            }

            dest.x = x; dest.y = y; dest.w = src.w * scale; dest.h = src.h * scale;
            

            if(h_flip)
                flip = (SDL_RendererFlip)((int)flip | SDL_FLIP_HORIZONTAL);
            if(v_flip)
                flip = (SDL_RendererFlip)((int)flip | SDL_FLIP_VERTICAL);

            SDL_RenderCopyEx(window_renderer, this->data, &src, &dest, angle, &p, flip);
        }

        void GetPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
        {
            int bpp = image->format->BytesPerPixel;
            uint8_t *p = (Uint8 *)image->pixels + y * image->pitch + x * bpp;
            if(bpp != 3 && bpp != 4)
            {
                ERROR_OUT("Unable to get pixel! Non-RGB or Non-RGBA surfaces not supported.\n");
            }
            if(bpp == 3)
            {
                *r = p[0];
                *g = p[1];
                *b = p[2];
                *a = 255;
            }
            else
            {
                *r = p[0];
                *g = p[1];
                *b = p[2];
                *a = p[3];
            }
        }

        void Colourise(uint8_t r, uint8_t g, uint8_t b)
        {
            SDL_SetTextureColorMod(this->data, r, g, b);
        }

        void TransparentColour(bool enable, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            if(enable)
            {
                SDL_SetColorKey(this->image, SDL_TRUE, (uint32_t)((r << 24) + (g << 16) + (b << 8) + (a)));
            }
            else
            {
                SDL_SetColorKey(this->image, SDL_FALSE, 0);
            }
            SDL_DestroyTexture(this->data);
            this->data = SDL_CreateTextureFromSurface(window_renderer, this->image);
        }


        ~Image()
        {
            SDL_FreeSurface(this->image);
            SDL_DestroyTexture(this->data);
        }
    };

    class Sprite
    {
        public:
        Image* im;
        int sheet_width, sheet_height;
        int sprite_width, sprite_height;
        int total_frames;
        float current_frame = 0.0f;

        Sprite(string filename, int vert_lines, int horiz_lines)
        {
            CreateSprite(new Image(filename), vert_lines, horiz_lines);
        }

        Sprite(Image* im, int vert_lines, int horiz_lines)
        {
            CreateSprite(im, vert_lines, horiz_lines);
        }

        void CreateSprite(Image* img, int vert_lines, int horiz_lines)
        {
            this->im = img;
            this->sheet_width = img->width;
            this->sheet_height = img->height;
            this->sprite_width = img->width / vert_lines;
            this->sprite_height = img->height / horiz_lines;
            this->total_frames = img->width * img->height / (this->sprite_width * this->sprite_height);
        }

        void IncrementFrame(float increment)
        {
            this->current_frame += increment;
            if(this->current_frame >= this->total_frames)
                this->current_frame = 0;
        }

        void DrawSprite(int frame, int x, int y, float angle = 0.0f, int pivotx = 0, int pivoty = 0, float scale = 1.0f, bool h_flip = false, bool v_flip = false)
        {
            if(frame == -1)
            {
                frame = this->current_frame;
            }
            int sx = sprite_width * (frame % (sheet_width / (int)sprite_width));
            int sy = sprite_height * (int)(frame / (int) (sheet_width / (int)sprite_width));
            this->im->DrawImage(x, y, sx, sy, sprite_width, sprite_height, angle, pivotx, pivoty, scale, h_flip, v_flip);
        }

        void GetPixel(int frame, int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
        {
            int sx = sprite_width * (frame % (sheet_width / (int)sprite_width));
            int sy = sprite_height * (int)(frame / (int) (sheet_width / (int)sprite_width));
            this->im->GetPixel(sx + x, sy + y, r, g, b, a);
        }
    };

    class BitmapFont
    {
        public:
        Image* im;
        int character_width, character_height;
        int fontsheet_width, fontsheet_height;
        int characters_per_line;

        private:
        bool is_own_image = false;

        public:
        BitmapFont(string s, int ch_w, int ch_h, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0)
        {
            Image* im = new Image(s);
            is_own_image = true;
            CreateBitmapFont(im, ch_w, ch_h, r, g, b, a);
        }

        BitmapFont(Image* im, int ch_w, int ch_h, uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t a = 0)
        {
            CreateBitmapFont(im, ch_w, ch_h, r, g, b, a);
        }

        void CreateBitmapFont(Image* im, int ch_w, int ch_h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            character_width = ch_w;
            character_height = ch_h;
            fontsheet_width = im->width;
            fontsheet_height = im->height;
            characters_per_line = fontsheet_width / character_width;
            this->im = im;
            this->im->TransparentColour(true, r, g, b, a);
        }

        void DrawChar(unsigned char s, int x, int y, float scale = 1)
        {
            int fx = character_width * (s % characters_per_line);
            int fy = character_height * (s / characters_per_line);

            this->im->DrawImage(x, y, fx, fy, character_width, character_height, 0, 0, 0, scale, false, false);
        }

        void Colourise(uint8_t r, uint8_t g, uint8_t b)
        {
            this->im->Colourise(r, g, b);
        }

        void DrawString(char* s, int x, int y, int scale = 1)
        {
            int n = strlen(s);
            int x_now = x;
            int y_now = y;
            for(int i = 0; i < n; i++)
            {
                if(s[i] == '\n')
                {
                    x_now = x;
                    y_now += character_height * scale;
                }
                else if(s[i] == '\r')
                {
                    x_now = x;
                }
                else if(s[i] == '\t')
                {
                    x_now += 4 * character_width * scale;
                }
                else
                {
                    DrawChar((unsigned char)s[i], x_now, y_now, scale);
                    x_now += character_width * scale;
                }
            }
        }

        void printf(int x, int y, int scale, const char* fmt, ...)
        {
            va_list arg1, arg2;
            va_start(arg1, fmt);

            va_copy(arg2, arg1);
            size_t size = vsnprintf(NULL, 0, fmt, arg2) + 1;
            va_end(arg2);

            char* buff = new char[size];
            vsnprintf(buff, size, fmt, arg1);
            va_end(arg1);

            DrawString(buff, x, y, scale);
            delete[] buff;
        }

        ~BitmapFont()
        {
            if(is_own_image)
                delete this->im;
        }
    };

    class PixelBlock
    {
        public:
        uint8_t* pixels;
        int width, height;
        bool blend;
        int pixel_array_size;

        PixelBlock(int w, int h)
        {
            this->pixels = (uint8_t*)calloc(w * h * sizeof(uint32_t), sizeof(uint8_t));
            this->width = w;
            this->height = h;
            this->pixel_array_size = w * h * sizeof(uint32_t);
        }

        ~PixelBlock()
        {
            free(this->pixels);
        }

        void DrawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            int pos = (y * sizeof(uint32_t) * width) + x * sizeof(uint32_t);
            if(pos < pixel_array_size-3 && pos > 0)
            {
                pixels[pos] = r;
                pixels[pos+1] = g;
                pixels[pos+2] = b;
                pixels[pos+3] = a;
            }
        }

        void GetPixel(int x, int y, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
        {
            int pos = (y * 4 * width) + x * 4;
            if(pos < pixel_array_size)
            {
                *r = pixels[pos];
                *g = pixels[pos+1];
                *b = pixels[pos+2];
                *a = pixels[pos+3];
            }
        }

        void Read(int x, int y)
        {
            SDL_Rect rect;
            rect.x = x;
            rect.y = y;
            rect.w = this->width;
            rect.h = this->height;
            SDL_RenderReadPixels(window_renderer, &rect, SDL_PIXELFORMAT_RGBA32, (void*)this->pixels, this->width * sizeof(uint32_t));
        }

        void Write(int x, int y, float scale=1.0f)
        {
            #if BYTE_ORDER == LITTLE_ENDIAN
            SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)this->pixels, this->width, this->height, 32, this->width * sizeof(uint32_t), 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
            #elif BYTE_ORDER == BIG_ENDIAN
            SDL_Surface* surf = SDL_CreateRGBSurfaceFrom((void*)this->pixels, this->width, this->height, 32, this->width * sizeof(uint32_t), 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
            #else
            #error "Endianness not defined for target machine. (Define BYTE_ORDER as LITTLE_ENDIAN or BIG_ENDIAN?)"
            #endif
            SDL_Texture* to_screen = SDL_CreateTextureFromSurface(window_renderer, surf);
            SDL_Rect srect, drect;
            srect.x = 0; srect.y = 0; srect.w = this->width; srect.h = this->height;
            drect.x = x; drect.y = y; drect.w = this->width * scale; drect.h = this->height * scale;
            if(blend)
                SDL_SetRenderDrawBlendMode(window_renderer, SDL_BLENDMODE_BLEND);
            SDL_RenderCopy(window_renderer, to_screen, &srect, &drect);
            SDL_DestroyTexture(to_screen);
            SDL_FreeSurface(surf);  
        }

    };

    class Sound
    {
        SDL_AudioSpec wav_spec;
        uint32_t wav_length;
        uint8_t* wav_buffer;
        SDL_AudioDeviceID device_id;

        public:
        bool is_paused = true;

        Sound(string s)
        {
            SDL_LoadWAV(s.c_str(), &wav_spec, &wav_buffer, &wav_length);
            device_id = SDL_OpenAudioDevice(NULL, 0, &wav_spec, NULL, 0);
            if(!device_id)
            {
                ERROR_OUT("Unable to open sound device!\nMessage: %s\n", SDL_GetError());
            }
        }

        void SetVolume(float volume)
        {
            volume = Clamp(volume, 0.0, 1.0);
            uint8_t* new_buffer = (uint8_t*)calloc(wav_length, sizeof(uint8_t));
            SDL_MixAudioFormat(new_buffer, wav_buffer, wav_spec.format, wav_length, (int)(volume * SDL_MIX_MAXVOLUME) % SDL_MIX_MAXVOLUME);
            SDL_FreeWAV(wav_buffer);
            wav_buffer = new_buffer;
        }

        void Play()
        {
            SDL_QueueAudio(device_id, wav_buffer, wav_length);
            UnPause();
        }

        bool IsFinished()
        {
            if(SDL_GetQueuedAudioSize(device_id) == 0) 
                return true;
            else
                return false;
        }

        void Pause()
        {
            SDL_PauseAudioDevice(device_id, 1);
            is_paused = true;
        }

        void UnPause()
        {
            SDL_PauseAudioDevice(device_id, 0);
            is_paused = false;
        }

        bool IsPaused()
        {
            return is_paused;
        }

        ~Sound()
        {
            SDL_FreeWAV(wav_buffer);
        }
    };

    unsigned int GetWidth() { return screen_width; }
    unsigned int GetHeight() { return screen_height; }
    unsigned int GetScale() { return window_scale; }
    void CaptureMouse() { SDL_SetRelativeMouseMode(SDL_TRUE); }
    void UncaptureMouse() { SDL_SetRelativeMouseMode(SDL_FALSE); }

    void Init(unsigned int w, unsigned int h, unsigned int scale=1.0f);
    void Loop(unsigned int fps);
    
    void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void DrawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void DrawBlock(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool fill = false);
    void DrawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void DrawVLine(int x, int y1, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void DrawHLine(int x1, int x2, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool fill = false);
    void DrawCircle(int x, int y, int rad, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool fill = false);
    void DrawEllipse(int x, int y, int rx, int ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, bool fill = false);

    void PolygonBegin(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a=255, bool fill = false);
    void PolygonVetex(int x, int y);
    void PolygonEnd(void);

    void StartTimer(Timer timer_id, float duration);
    void StopTimer(Timer timer_id);

    void Quit();

    void Init(unsigned int w, unsigned int h, unsigned int scale)
    {
        screen_width = w;
        screen_height = h;
        window_width = screen_width * scale;
        window_height = screen_height * scale;
        window_scale = scale;

        if(SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            ERROR_OUT("Could not initialize SDL renderer! Quiting.\nMessage: %s\n", SDL_GetError());
            exit(-1);
        }

        if(SDL_Init(SDL_INIT_AUDIO) < 0)
        {
            ERROR_OUT("Warning! Could not initialize SDL audio!\nMessage: %s\n", SDL_GetError());
        }

        application_window = SDL_CreateWindow("engine2D Application", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
        if(!application_window)
        {
            ERROR_OUT("Could not initialize SDL window! Quitting.\nMessage: %s\n", SDL_GetError());
            exit(-1);
        }
        window_renderer = SDL_CreateRenderer(application_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        screen_texture = SDL_CreateTexture(window_renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, w, h);
    }

    void _SetDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawBlendMode(window_renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(window_renderer, r, g, b, a);
    }
    
    void Clear(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        _SetDrawColor(r, g, b, a);
        SDL_RenderClear(window_renderer);
    }

    void DrawPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        _SetDrawColor(r, g, b, a);
        SDL_RenderDrawPoint(window_renderer, x, y);
    }

    void DrawLine(int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        _SetDrawColor(r, g, b, a);
        SDL_RenderDrawLine(window_renderer, x1, y1, x2, y2);
    }

    void DrawVLine(int x, int y1, int y2, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        DrawLine(x, y1, x, y2, r, g, b, a);
    }

    void DrawHLine(int x1, int x2, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        DrawLine(x1, y, x2, y, r, g, b, a);
    }

    void DrawBlock(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fill)
    {
        _SetDrawColor(r, g, b, a);
        SDL_Rect rect;
        rect.x = x; rect.y = y; rect.w = w; rect.h = h;
        if(fill)
        {
            SDL_RenderFillRect(window_renderer, &rect);
        }
        else
        {
            SDL_RenderDrawRect(window_renderer, &rect);
        }
    }

    namespace
    {
        int sdl2_gfx_filledPolygonRGBAMT(SDL_Renderer* renderer, const Sint16* vx, const Sint16* vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int **polyInts, int *polyAllocated);
        int sdl2_gfx_filledEllipseRGBA(SDL_Renderer* renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        int sdl2_gfx_ellipseRGBA(SDL_Renderer* renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
        int sdl2_gfx_filledTrigonColor(SDL_Renderer* renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    };
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fill)
    {
        if(!fill)
        {
            _SetDrawColor(r, g, b, a);
            SDL_RenderDrawLine(window_renderer, x1, y1, x2, y2);
            SDL_RenderDrawLine(window_renderer, x2, y2, x3, y3);
            SDL_RenderDrawLine(window_renderer, x3, y3, x1, x2);
        }
        else
        {
            sdl2_gfx_filledTrigonColor(window_renderer, x1, y1, x2, y2, x3, y3, r, g, b, a);
        }

    }

    void DrawCircle(int x, int y, int rad, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fill)
    {
        if(fill)
            sdl2_gfx_filledEllipseRGBA(window_renderer, x, y, rad, rad, r, g, b, a);
        else
            sdl2_gfx_ellipseRGBA(window_renderer, x, y, rad, rad, r, g, b, a);
    }

    void DrawEllipse(int x, int y, int rx, int ry, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fill)
    {
        if(fill)
            sdl2_gfx_filledEllipseRGBA(window_renderer, x, y, rx, ry, r, g, b, a);
        else
            sdl2_gfx_ellipseRGBA(window_renderer, x, y, rx, ry, r, g, b, a);
    }

    void PolygonBegin(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, bool fill)
    {
        if(shape_free)
        {
            shape_r = r;
            shape_g = g;
            shape_b = b;
            shape_a = a;
            shape_array_x.clear();
            shape_array_y.clear();
            shape_x = x;
            shape_y = y;
            shape_fill = fill;
            shape_free = false;
        }
        else
        {
            ERROR_OUT("BeginShape() called without EndShape()!\n");
        }
    }
    
    void PolygonVertex(int x, int y)
    {
        shape_array_x.push_back(shape_x + x);
        shape_array_y.push_back(shape_y + y);
    }

    void PolygonEnd(void)
    {
        if(shape_fill)
            sdl2_gfx_filledPolygonRGBAMT(window_renderer, reinterpret_cast<const Sint16*>(&shape_array_x[0]), reinterpret_cast<const Sint16*>(&shape_array_y[0]), shape_array_y.size(), shape_r, shape_g, shape_b, shape_a, NULL, NULL);
        else
        {
            int n = shape_array_x.size();
            for(unsigned int i = 0; i < n - 1; i++)
            {
                DrawLine(shape_array_x[i], shape_array_y[i], shape_array_x[i+1], shape_array_y[i+1], shape_r, shape_g, shape_b, shape_a);
            }
            DrawLine(shape_array_x[n-1], shape_array_y[n-1], shape_array_x[0], shape_array_y[0], shape_r, shape_g, shape_b, shape_a);
        }
        shape_free = true;

    }

    void _ProcessEvents(float elapsed);
    void MainLoop(void);
    
    void Start(Application* appl)
    {
        UncaptureMouse();
        app = appl;
        app->Create();
        SDL_SetWindowTitle(application_window, app->app_name.c_str());
        #ifdef ENGINE2D_EMSCRIPTEN_IMPLEMENTATION
        emscripten_set_main_loop(MainLoop, 0, 1);
        #else
        MainLoop();
        #endif
    }


    void MainLoop(void)
    {
        SDL_Event e;
        float elapsed = 0.0f;

        while(true)
        {
            uint64_t start = SDL_GetPerformanceCounter();
            _ProcessEvents(elapsed);
            // Update
            app->Update(elapsed);
            // Render
            SDL_SetRenderTarget(window_renderer, screen_texture);
            _SetDrawColor(0, 0, 0, 255);
            SDL_RenderClear(window_renderer);
            // Drawing code goes here
            app->Draw(elapsed);
            // Render to screen
            SDL_SetRenderTarget(window_renderer, NULL);
            //SDL_SetRenderDrawColor(window_renderer, 0, 0, 0, 255);
            //SDL_RenderClear(window_renderer);
            SDL_RenderCopyEx(window_renderer, screen_texture, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
            SDL_RenderPresent(window_renderer);
            SDL_UpdateWindowSurface(application_window);
            uint64_t end = SDL_GetPerformanceCounter();
            elapsed = ((end - start) / (float)SDL_GetPerformanceFrequency());

            for(unsigned int i = 0; i < 256; i++)
            {
                if(timers[i].active == true)
                {
                    timers[i].time -= elapsed;
                    if(timers[i].time <= 0)
                    {
                        timers[i].time = timers[i].duration;
                        app->OnTimerTick(elapsed, i);
                    }
                }
            }
        }
    }

    void StartTimer(Timer timer_id, float duration)
    {
        timers[timer_id].active = true;
        timers[timer_id].duration = duration;
        timers[timer_id].time = duration;
    }

    void StopTimer(Timer timer_id)
    {
        timers[timer_id].active = false;
    }
    
    void Quit()
    {
        SDL_DestroyWindow(application_window);
        SDL_Quit();

        #ifdef ENGINE2D_EMSCRIPTEN_IMPLEMENTATION
        emscripten_cancel_main_loop();
        #else
        exit(0);
        #endif
    }

    void _ProcessEvents(float elapsed)
    {
        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            if(e.type == SDL_QUIT)
            {
                Quit();
            }
            else if(e.type == SDL_MOUSEMOTION)
            {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if(x == window_width / 2 && y == window_height / 2)
                    continue;
                app->OnMouseMove(elapsed, x / window_scale, y / window_scale, e.motion.xrel, e.motion.yrel);
                //SDL_WarpMouseInWindow(application_window, x, y);
            }
            else if(e.type == SDL_TEXTINPUT)
            {
                app->OnTextInput(elapsed, (char)e.text.text[0]);
            }
            else if(e.type == SDL_KEYDOWN)
            {
                if(e.key.repeat)
                    continue;
                
                switch(e.key.keysym.sym)
                {
                    case SDLK_BACKSPACE:
                        app->OnTextInput(elapsed, '\b');
                        break;
                    case SDLK_TAB:
                        app->OnTextInput(elapsed, '\t');
                        break;
                    case SDLK_UP:
                        app->OnKeyPress(elapsed, Button::UP);
                        key_array[static_cast<int>(Button::UP)] = true;
                        break;
                    case SDLK_DOWN:
                        app->OnKeyPress(elapsed, Button::DOWN);
                        key_array[static_cast<int>(Button::DOWN)] = true;
                        break;
                    case SDLK_LEFT:
                        app->OnKeyPress(elapsed, Button::LEFT);
                        key_array[static_cast<int>(Button::LEFT)] = true;
                        break;
                    case SDLK_RIGHT:
                        app->OnKeyPress(elapsed, Button::RIGHT);
                        key_array[static_cast<int>(Button::RIGHT)] = true;
                        break;
                    case SDLK_q:
                        app->OnKeyPress(elapsed, Button::Q);
                        key_array[static_cast<int>(Button::Q)] = true;
                        break;
                    case SDLK_e:
                        app->OnKeyPress(elapsed, Button::E);
                        key_array[static_cast<int>(Button::E)] = true;
                        break;
                    case SDLK_r:
                        app->OnKeyPress(elapsed, Button::R);
                        key_array[static_cast<int>(Button::R)] = true;
                        break;
                    case SDLK_SPACE:
                        app->OnKeyPress(elapsed, Button::SPACE);
                        key_array[static_cast<int>(Button::SPACE)] = true;
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        app->OnKeyPress(elapsed, Button::SHIFT);
                        key_array[static_cast<int>(Button::SHIFT)] = true;
                        break;
                    case SDLK_RETURN:
                    case SDLK_RETURN2:
                        app->OnTextInput(elapsed, '\n');
                        app->OnKeyPress(elapsed, Button::RETURN);
                        key_array[static_cast<int>(Button::RETURN)] = true;
                        break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        app->OnKeyPress(elapsed, Button::CTRL);
                        key_array[static_cast<int>(Button::CTRL)] = true;
                        break;
                    default:
                        break;
                }
            }
            else if(e.type == SDL_KEYUP)
            {
                switch(e.key.keysym.sym)
                {
                    case SDLK_UP:
                    case SDLK_w:
                        app->OnKeyRelease(elapsed, Button::UP);
                        key_array[static_cast<int>(Button::UP)] = false;
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        app->OnKeyRelease(elapsed, Button::DOWN);
                        key_array[static_cast<int>(Button::DOWN)] = false;
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        app->OnKeyRelease(elapsed, Button::LEFT);
                        key_array[static_cast<int>(Button::LEFT)] = false;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        app->OnKeyRelease(elapsed, Button::RIGHT);
                        key_array[static_cast<int>(Button::RIGHT)] = false;
                        break;
                    case SDLK_q:
                        app->OnKeyRelease(elapsed, Button::Q);
                        key_array[static_cast<int>(Button::Q)] = false;
                        break;
                    case SDLK_e:
                        app->OnKeyRelease(elapsed, Button::E);
                        key_array[static_cast<int>(Button::E)] = false;
                        break;
                    case SDLK_r:
                        app->OnKeyRelease(elapsed, Button::R);
                        key_array[static_cast<int>(Button::R)] = false;
                        break;
                    case SDLK_SPACE:
                        app->OnKeyRelease(elapsed, Button::SPACE);
                        key_array[static_cast<int>(Button::SPACE)] = false;
                        break;
                    case SDLK_LSHIFT:
                    case SDLK_RSHIFT:
                        app->OnKeyRelease(elapsed, Button::SHIFT);
                        key_array[static_cast<int>(Button::SHIFT)] = false;
                        break;
                    case SDLK_RETURN:
                    case SDLK_RETURN2:
                        app->OnKeyRelease(elapsed, Button::RETURN);
                        key_array[static_cast<int>(Button::RETURN)] = false;
                        break;
                    case SDLK_LCTRL:
                    case SDLK_RCTRL:
                        app->OnKeyRelease(elapsed, Button::CTRL);
                        key_array[static_cast<int>(Button::CTRL)] = false;
                        break;
                    default:
                        break;
                }
            }
        }
        // Call the OnKeyPressed functions
        for(unsigned int i = 0; i < sizeof(key_array); i++)
        {
            if(key_array[i] == true)
            {
                app->OnKeyPressed(elapsed, static_cast<Button>(i));
            }
        }
    }

    // End of engine2D_c code.

    // Shamelessly copied from SDL2_gfx
    /* SDL2_gfx code */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */

    namespace
    {
        static int *sdl2_gfx_gfxPrimitivesPolyIntsGlobal = NULL;
        static int sdl2_gfx_gfxPrimitivesPolyAllocatedGlobal = 0;

        int sdl2_gfx__gfxPrimitivesCompareInt(const void *a, const void *b)
        {
	        return (*(const int *) a) - (*(const int *) b);
        }
        int sdl2_gfx_pixel(SDL_Renderer* renderer, Sint16 x, Sint16 y)
        {
	        return SDL_RenderDrawPoint(renderer, x, y);
        }

        int sdl2_gfx_hline(SDL_Renderer* renderer, Sint16 x1, Sint16 x2, Sint16 y)
        {
            return SDL_RenderDrawLine(renderer, x1, y, x2, y);;
        }

        int sdl2_gfx_vlineRGBA(SDL_Renderer* renderer, Sint16 x, Sint16 y1, Sint16 y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
        {
            int result = 0;
            result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
            result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
            result |= SDL_RenderDrawLine(renderer, x, y1, x, y2);
            return result;
        }

        int sdl2_gfx_hlineRGBA(SDL_Renderer* renderer, Sint16 x1, Sint16 x2, Sint16 y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
        {
            int result = 0;
            result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
            result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);
            result |= SDL_RenderDrawLine(renderer, x1, y, x2, y);
            return result;
        }

        int sdl2_gfx_ellipseRGBA(SDL_Renderer* renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
        {
            int result;
            int ix, iy;
            int h, i, j, k;
            int oh, oi, oj, ok;
            int xmh, xph, ypk, ymk;
            int xmi, xpi, ymj, ypj;
            int xmj, xpj, ymi, ypi;
            int xmk, xpk, ymh, yph;

            /*
            * Sanity check radii 
            */
            if ((rx < 0) || (ry < 0)) {
                return -1;
            }

            /*
            * Special case for rx=0 - draw a vline 
            */
            if (rx == 0) {
                return sdl2_gfx_vlineRGBA(renderer, x, y - ry, y + ry, r, g, b, a);
            }
            /*
            * Special case for ry=0 - draw a hline 
            */
            if (ry == 0) {
                return sdl2_gfx_hlineRGBA(renderer, x - rx, x + rx, y, r, g, b, a);
            }

            /*
            * Set color
            */
            result = 0;
            result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
            result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

            /*
            * Init vars 
            */
            oh = oi = oj = ok = 0xFFFF;

            /*
            * Draw 
            */
            if (rx > ry) {
                ix = 0;
                iy = rx * 64;

                do {
                    h = (ix + 32) >> 6;
                    i = (iy + 32) >> 6;
                    j = (h * ry) / rx;
                    k = (i * ry) / rx;

                    if (((ok != k) && (oj != k)) || ((oj != j) && (ok != j)) || (k != j)) {
                        xph = x + h;
                        xmh = x - h;
                        if (k > 0) {
                            ypk = y + k;
                            ymk = y - k;
                            result |= sdl2_gfx_pixel(renderer, xmh, ypk);
                            result |= sdl2_gfx_pixel(renderer, xph, ypk);
                            result |= sdl2_gfx_pixel(renderer, xmh, ymk);
                            result |= sdl2_gfx_pixel(renderer, xph, ymk);
                        } else {
                            result |= sdl2_gfx_pixel(renderer, xmh, y);
                            result |= sdl2_gfx_pixel(renderer, xph, y);
                        }
                        ok = k;
                        xpi = x + i;
                        xmi = x - i;
                        if (j > 0) {
                            ypj = y + j;
                            ymj = y - j;
                            result |= sdl2_gfx_pixel(renderer, xmi, ypj);
                            result |= sdl2_gfx_pixel(renderer, xpi, ypj);
                            result |= sdl2_gfx_pixel(renderer, xmi, ymj);
                            result |= sdl2_gfx_pixel(renderer, xpi, ymj);
                        } else {
                            result |= sdl2_gfx_pixel(renderer, xmi, y);
                            result |= sdl2_gfx_pixel(renderer, xpi, y);
                        }
                        oj = j;
                    }

                    ix = ix + iy / rx;
                    iy = iy - ix / rx;

                } while (i > h);
            } else {
                ix = 0;
                iy = ry * 64;

                do {
                    h = (ix + 32) >> 6;
                    i = (iy + 32) >> 6;
                    j = (h * rx) / ry;
                    k = (i * rx) / ry;

                    if (((oi != i) && (oh != i)) || ((oh != h) && (oi != h) && (i != h))) {
                        xmj = x - j;
                        xpj = x + j;
                        if (i > 0) {
                            ypi = y + i;
                            ymi = y - i;
                            result |= sdl2_gfx_pixel(renderer, xmj, ypi);
                            result |= sdl2_gfx_pixel(renderer, xpj, ypi);
                            result |= sdl2_gfx_pixel(renderer, xmj, ymi);
                            result |= sdl2_gfx_pixel(renderer, xpj, ymi);
                        } else {
                            result |= sdl2_gfx_pixel(renderer, xmj, y);
                            result |= sdl2_gfx_pixel(renderer, xpj, y);
                        }
                        oi = i;
                        xmk = x - k;
                        xpk = x + k;
                        if (h > 0) {
                            yph = y + h;
                            ymh = y - h;
                            result |= sdl2_gfx_pixel(renderer, xmk, yph);
                            result |= sdl2_gfx_pixel(renderer, xpk, yph);
                            result |= sdl2_gfx_pixel(renderer, xmk, ymh);
                            result |= sdl2_gfx_pixel(renderer, xpk, ymh);
                        } else {
                            result |= sdl2_gfx_pixel(renderer, xmk, y);
                            result |= sdl2_gfx_pixel(renderer, xpk, y);
                        }
                        oh = h;
                    }

                    ix = ix + iy / ry;
                    iy = iy - ix / ry;

                } while (i > h);
            }

            return (result);
        }

        int sdl2_gfx_filledEllipseRGBA(SDL_Renderer* renderer, Sint16 x, Sint16 y, Sint16 rx, Sint16 ry, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
        {
            int result;
            int ix, iy;
            int h, i, j, k;
            int oh, oi, oj, ok;
            int xmh, xph;
            int xmi, xpi;
            int xmj, xpj;
            int xmk, xpk;

            /*
            * Sanity check radii 
            */
            if ((rx < 0) || (ry < 0)) {
                return (-1);
            }

            /*
            * Special case for rx=0 - draw a vline 
            */
            if (rx == 0) {
                return (sdl2_gfx_vlineRGBA(renderer, x, y - ry, y + ry, r, g, b, a));
            }
            /*
            * Special case for ry=0 - draw a hline 
            */
            if (ry == 0) {
                return (sdl2_gfx_hlineRGBA(renderer, x - rx, x + rx, y, r, g, b, a));
            }

            /*
            * Set color
            */
            result = 0;
            result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
            result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);

            /*
            * Init vars 
            */
            oh = oi = oj = ok = 0xFFFF;

            /*
            * Draw 
            */
            if (rx > ry) {
                ix = 0;
                iy = rx * 64;

                do {
                    h = (ix + 32) >> 6;
                    i = (iy + 32) >> 6;
                    j = (h * ry) / rx;
                    k = (i * ry) / rx;

                    if ((ok != k) && (oj != k)) {
                        xph = x + h;
                        xmh = x - h;
                        if (k > 0) {
                            result |= sdl2_gfx_hline(renderer, xmh, xph, y + k);
                            result |= sdl2_gfx_hline(renderer, xmh, xph, y - k);
                        } else {
                            result |= sdl2_gfx_hline(renderer, xmh, xph, y);
                        }
                        ok = k;
                    }
                    if ((oj != j) && (ok != j) && (k != j)) {
                        xmi = x - i;
                        xpi = x + i;
                        if (j > 0) {
                            result |= sdl2_gfx_hline(renderer, xmi, xpi, y + j);
                            result |= sdl2_gfx_hline(renderer, xmi, xpi, y - j);
                        } else {
                            result |= sdl2_gfx_hline(renderer, xmi, xpi, y);
                        }
                        oj = j;
                    }

                    ix = ix + iy / rx;
                    iy = iy - ix / rx;

                } while (i > h);
            } else {
                ix = 0;
                iy = ry * 64;

                do {
                    h = (ix + 32) >> 6;
                    i = (iy + 32) >> 6;
                    j = (h * rx) / ry;
                    k = (i * rx) / ry;

                    if ((oi != i) && (oh != i)) {
                        xmj = x - j;
                        xpj = x + j;
                        if (i > 0) {
                            result |= sdl2_gfx_hline(renderer, xmj, xpj, y + i);
                            result |= sdl2_gfx_hline(renderer, xmj, xpj, y - i);
                        } else {
                            result |= sdl2_gfx_hline(renderer, xmj, xpj, y);
                        }
                        oi = i;
                    }
                    if ((oh != h) && (oi != h) && (i != h)) {
                        xmk = x - k;
                        xpk = x + k;
                        if (h > 0) {
                            result |= sdl2_gfx_hline(renderer, xmk, xpk, y + h);
                            result |= sdl2_gfx_hline(renderer, xmk, xpk, y - h);
                        } else {
                            result |= sdl2_gfx_hline(renderer, xmk, xpk, y);
                        }
                        oh = h;
                    }

                    ix = ix + iy / ry;
                    iy = iy - ix / ry;

                } while (i > h);
            }

            return (result);
        }
        int sdl2_gfx_filledPolygonRGBAMT(SDL_Renderer* renderer, const Sint16* vx, const Sint16* vy, int n, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int **polyInts, int *polyAllocated)
        {
            int result;
            int i;
            int y, xa, xb;
            int miny, maxy;
            int x1, y1;
            int x2, y2;
            int ind1, ind2;
            int ints;
            int *gfxPrimitivesPolyInts = NULL;
            int *gfxPrimitivesPolyIntsNew = NULL;
            int gfxPrimitivesPolyAllocated = 0;

            /*
            * Vertex array NULL check 
            */
            if (vx == NULL) {
                return (-1);
            }
            if (vy == NULL) {
                return (-1);
            }

            /*
            * Sanity check number of edges
            */
            if (n < 3) {
                return -1;
            }

            /*
            * Map polygon cache  
            */
            if ((polyInts==NULL) || (polyAllocated==NULL)) {
                /* Use global cache */
                gfxPrimitivesPolyInts = sdl2_gfx_gfxPrimitivesPolyIntsGlobal;
                gfxPrimitivesPolyAllocated = sdl2_gfx_gfxPrimitivesPolyAllocatedGlobal;
            } else {
                /* Use local cache */
                gfxPrimitivesPolyInts = *polyInts;
                gfxPrimitivesPolyAllocated = *polyAllocated;
            }

            /*
            * Allocate temp array, only grow array 
            */
            if (!gfxPrimitivesPolyAllocated) {
                gfxPrimitivesPolyInts = (int *) malloc(sizeof(int) * n);
                gfxPrimitivesPolyAllocated = n;
            } else {
                if (gfxPrimitivesPolyAllocated < n) {
                    gfxPrimitivesPolyIntsNew = (int *) realloc(gfxPrimitivesPolyInts, sizeof(int) * n);
                    if (!gfxPrimitivesPolyIntsNew) {
                        if (!gfxPrimitivesPolyInts) {
                            free(gfxPrimitivesPolyInts);
                            gfxPrimitivesPolyInts = NULL;
                        }
                        gfxPrimitivesPolyAllocated = 0;
                    } else {
                        gfxPrimitivesPolyInts = gfxPrimitivesPolyIntsNew;
                        gfxPrimitivesPolyAllocated = n;
                    }
                }
            }

            /*
            * Check temp array
            */
            if (gfxPrimitivesPolyInts==NULL) {        
                gfxPrimitivesPolyAllocated = 0;
            }

            /*
            * Update cache variables
            */
            if ((polyInts==NULL) || (polyAllocated==NULL)) { 
                sdl2_gfx_gfxPrimitivesPolyIntsGlobal =  gfxPrimitivesPolyInts;
                sdl2_gfx_gfxPrimitivesPolyAllocatedGlobal = gfxPrimitivesPolyAllocated;
            } else {
                *polyInts = gfxPrimitivesPolyInts;
                *polyAllocated = gfxPrimitivesPolyAllocated;
            }

            /*
            * Check temp array again
            */
            if (gfxPrimitivesPolyInts==NULL) {        
                return(-1);
            }

            /*
            * Determine Y maxima 
            */
            miny = vy[0];
            maxy = vy[0];
            for (i = 1; (i < n); i++) {
                if (vy[i] < miny) {
                    miny = vy[i];
                } else if (vy[i] > maxy) {
                    maxy = vy[i];
                }
            }

            /*
            * Draw, scanning y 
            */
            result = 0;
            for (y = miny; (y <= maxy); y++) {
                ints = 0;
                for (i = 0; (i < n); i++) {
                    if (!i) {
                        ind1 = n - 1;
                        ind2 = 0;
                    } else {
                        ind1 = i - 1;
                        ind2 = i;
                    }
                    y1 = vy[ind1];
                    y2 = vy[ind2];
                    if (y1 < y2) {
                        x1 = vx[ind1];
                        x2 = vx[ind2];
                    } else if (y1 > y2) {
                        y2 = vy[ind1];
                        y1 = vy[ind2];
                        x2 = vx[ind1];
                        x1 = vx[ind2];
                    } else {
                        continue;
                    }
                    if ( ((y >= y1) && (y < y2)) || ((y == maxy) && (y > y1) && (y <= y2)) ) {
                        gfxPrimitivesPolyInts[ints++] = ((65536 * (y - y1)) / (y2 - y1)) * (x2 - x1) + (65536 * x1);
                    } 	    
                }

                qsort(gfxPrimitivesPolyInts, ints, sizeof(int), sdl2_gfx__gfxPrimitivesCompareInt);

                /*
                * Set color 
                */
                result = 0;
                result |= SDL_SetRenderDrawBlendMode(renderer, (a == 255) ? SDL_BLENDMODE_NONE : SDL_BLENDMODE_BLEND);
                result |= SDL_SetRenderDrawColor(renderer, r, g, b, a);	

                for (i = 0; (i < ints); i += 2) {
                    xa = gfxPrimitivesPolyInts[i] + 1;
                    xa = (xa >> 16) + ((xa & 32768) >> 15);
                    xb = gfxPrimitivesPolyInts[i+1] - 1;
                    xb = (xb >> 16) + ((xb & 32768) >> 15);
                    result |= sdl2_gfx_hline(renderer, xa, xb, y);
                }
            }

            return (result);
        }

        int sdl2_gfx_filledTrigonColor(SDL_Renderer* renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2, Sint16 x3, Sint16 y3, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
        {
            Sint16 vx[3];
            Sint16 vy[3];

            vx[0]=x1;
            vx[1]=x2;
            vx[2]=x3;
            vy[0]=y1;
            vy[1]=y2;
            vy[2]=y3;

            return(sdl2_gfx_filledPolygonRGBAMT(renderer,vx,vy,3,r,g,b,a,NULL,NULL));
        }
    }
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* ........................... */
    /* End of SDL2_gfx code */
};

#endif