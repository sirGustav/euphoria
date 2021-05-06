#include <array>

#include "core/image.h"
#include "core/image_draw.h"
#include "core/io.h"
#include "core/cint.h"
#include "core/argparse.h"
#include "core/palette_all.h"
#include "core/palette.h"
#include "core/str.h"
#include "core/cint.h"


using namespace euphoria::core;


int
main(int argc, char* argv[])
{
    auto image_size = 32;

    auto parser = argparse::parser {"euphoria palette generator"};
    parser.add("-size", &image_size).set_help("Minimum image size");

    if(const auto r = parser.parse(argc, argv))
    {
        return *r;
    }

    for (auto palette_name : palettes::PaletteNames)
    {
        const auto pal = palettes::get_palette(palette_name);

        image image;
        image.setup_no_alpha_support(image_size * Csizet_to_int(pal.colors.size()), image_size);

        for
        (
            int i = 0;
            i < Csizet_to_int(pal.colors.size());
            i += 1
        )
        {
            draw_square
            (
                &image,
                pal.colors[i],
                image_size * i,
                image_size - 1,
                image_size
            );
        }

        const std::string file = Str{} << "palette_" << pal.name << ".png";

        io::chunk_to_file(image.write(image_write_format::png), file);
    }

    return 0;
}

