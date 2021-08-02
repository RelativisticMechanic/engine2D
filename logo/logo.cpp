#include "engine2D.h"

class engine2D_Logo : public engine2D::Application
{
    engine2D::Image* background = new engine2D::Image("./bgfar.png");
    engine2D::Image* background_city = new engine2D::Image("./bgnear.png");
    engine2D::Image* background_neighbourhood = new engine2D::Image("./bgnearest.png");
    engine2D::Image* logo = new engine2D::Image("./logo.png");

    engine2D::PixelBlock* px_in = new engine2D::PixelBlock(320, 100);
    engine2D::PixelBlock* px_out = new engine2D::PixelBlock(320, 100);

    float x1 = 0.0f;
    float x2 = 0.0f;

    float global_time = 0.0f;

    void Create()
    {
        app_name = "engine2D Logo";
    }

    void Update(float elapsed)
    {
        x1 -= elapsed * 30.0f;
        x2 -= elapsed * 40.0f;
        if(x1 < -320)
        {
            x1 = 0;
        }
        if(x2 < -320)
        {
            x2 = 0;
        }
    }

    void Draw(float elapsed)
    {
        global_time += elapsed;
        background->DrawImage(0, 0);
        background_city->DrawImage(x1, 0);
        background_city->DrawImage(background->width + x1, 0);
        background_neighbourhood->DrawImage(x2, 0);
        background_neighbourhood->DrawImage(background_neighbourhood->width + x2, 0);
        logo->DrawImage(80, 160);
        px_in->Read(0, 100);
        for(int x = 0; x < px_in->width; x++)
        {
            for(int y = 0; y < px_in->width; y++)
            {
                uint8_t r, g, b, a;
                px_in->GetPixel(x + 2.0f * sin(0.2 * x + global_time / 1000.0f), y, &r, &g, &b, &a);
                int out_x = x + rand() % 2 - 2;
                int out_y = px_in->height - 1 - y;
                px_out->DrawPixel(out_x, out_y, r / 2, g / 2, b / 2, a);
            }
        }
        px_out->Write(0, 200);
    }

};

int main(void)
{
    engine2D::Init(320, 300, 2);
    engine2D::Start(new engine2D_Logo());
}
