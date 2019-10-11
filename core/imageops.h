#ifndef CORE_IMAGEOPS_H
#define CORE_IMAGEOPS_H

#include "core/table.h"
#include "core/rgb.h"

// todo(Gustav): Rename module to something like console image or something...

namespace euphoria::core
{
    struct Image;

    struct ImageMapAction
    {
        char to;
        Rgbi from_color;
    };

    Table<char>
    ImageToStringTable(
            const Image&                       img,
            const std::vector<ImageMapAction>& map);

    Table<char>
    ImageToStringTable(const Image& img, bool shorter);

    // todo(Gustav): add function to print image using colored unicode blocks

}  // namespace euphoria::core

#endif  // CORE_IMAGEOPS_H
