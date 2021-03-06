#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/image_canvas.h"


using namespace euphoria::core;


int
main(int, char*[])
{
    constexpr auto image_size = 300;
    image image;
    image.setup_no_alpha_support(image_size, image_size);
    clear(&image, {color::yellow});

    auto ctx = canvas{&image};

    ctx.fill_style = color::white;
    ctx.fill_rect(0, 0, 300, 300);

    // grass
    ctx.fill_style = color::green;
    ctx.fill_rect(0, 200, 300, 100);

    // Wall
    ctx.fill_style = color::gray;
    ctx.fill_rect(75, 140, 150, 110);

    // Door
    ctx.fill_style = color::black;
    ctx.fill_rect(130, 190, 40, 60);

    // Roof
    ctx.fill_style = color::red;
    ctx.begin_path();
    ctx.move_to(50, 140);
    ctx.line_to(150, 60);
    ctx.line_to(250, 140);
    ctx.close_path();
    ctx.fill();

    io::chunk_to_file(image.write(image_write_format::png), "house.png");
    return 0;
}

