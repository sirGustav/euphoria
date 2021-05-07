#include "core/nlp_line.h"

#include "core/stringutils.h"
#include "core/stringmerger.h"

namespace euphoria::core
{

std::optional<text_line>
parse_line(const std::string& line)
{
    return Split(line, ' ');
}


std::string
line_to_string(const text_line& s)
{
    return StringMerger::Space().Generate(s);
}

}
