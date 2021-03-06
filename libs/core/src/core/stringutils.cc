#include "core/stringutils.h"

#include <string>
#include <algorithm>
#include <cstring>
#include <vector>
#include <sstream>
#include <iterator>

#include "core/assert.h"


namespace euphoria::core
{

std::pair<std::string, std::string>
last_strings(const std::string& str, char sep)
{
    auto result = str.find(sep);
    if(result == std::string::npos)
    {
        return std::make_pair(str, "");
    }

    const auto parent = str.substr(0, result);
    const auto child = str.substr(result, str.length() - parent.length());
    return std::make_pair(parent, child);
}


std::string
first_chars(const std::string& str, std::size_t count)
{
    if(str.length() < count) { return str; }
    else { return str.substr(0, count); }
}


std::string
first_chars_with_ellipsis(const std::string& str, unsigned int count)
{
    if (str.length() > count)
    {
        return str.substr(0, count) + "...";
    }

    return str;
}


std::string
strip_last_string(const std::string& str, char sep)
{
    auto result = str.find(sep);
    if(result == std::string::npos)
    {
        return "";
    }

    return str.substr(0, result);
}


std::string
trim_right(const std::string& string_to_trim, const std::string& trim_characters)
{
    return std::string(string_to_trim).erase(string_to_trim.find_last_not_of(trim_characters) + 1);
}


std::string
trim_left(const std::string& string_to_trim, const std::string& trim_characters)
{
    return std::string(string_to_trim).erase(0, string_to_trim.find_first_not_of(trim_characters));
}


std::string
trim(const std::string& string_to_trim, const std::string& trim_characters)
{
    return trim_right(trim_left(string_to_trim, trim_characters), trim_characters);
}


bool
starts_with(const std::string& string_to_test, const std::string& start)
{
    const std::string::size_type length = start.length();
    const std::string::size_type other_length = string_to_test.length();
    if(other_length < length)
    {
        return false;
    }
    const std::string actual_start = string_to_test.substr(0, length);
    return start == actual_start;
}


bool
ends_with(const std::string& string_to_test, const std::string& end)
{
    const std::string::size_type length = end.length();
    const std::string::size_type other_length = string_to_test.length();
    if(other_length < length)
    {
        return false;
    }
    const std::string actual_end
            = string_to_test.substr(other_length - length, length);
    return end == actual_end;
}


char
to_lower_char(char b)
{
    if(b >= 'A' && b <= 'Z')
    {
        return static_cast<char>((static_cast<int>(b) - 'A') + 'a');
    }
    else
    {
        return b;
    }
}

char
to_upper_char(char b)
{
    if(b >= 'a' && b <= 'z')
    {
        return static_cast<char>(b + ('A' - 'a'));
    }
    else
    {
        return b;
    }
}


std::string
to_lower(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), to_lower_char);
    return result;
}


std::vector<std::string>
to_lower(const std::vector<std::string>& str)
{
    return to_string_vector(str, [](const std::string& s) { return to_lower(s); });
}


std::string
to_upper(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), to_upper_char);
    return result;
}


std::string
char_to_string(char c, char_to_string_style style)
{
    constexpr std::string_view smart_characters =
        "abcdefghijklmnopqrstuwxyz"
        "ABCDEFGHIJKLMNOPQRSTUWXYZ"
        " "
        "~!@#$%^&*()_+"
        "`123456790-="
        ",.<>/?"
        "{}[]:;\"'\\|"
        "\n\r\t"
        ;
    std::ostringstream ss;
    switch(c)
    {
    case 0:
        ss << "<null>";
        if(style == char_to_string_style::smart)
        {
            return ss.str();
        }
        break;
    case '\n':
        ss << "<\\n>";
        break;
    case '\r':
        ss << "<\\r>";
        break;
    case '\t':
        ss << "<tab>";
        break;
    // source: http://www.asciitable.com/
    case  1: ss << "<start of heading>"; break;
    case  2: ss << "<start of text>"; break;
    case  3: ss << "<end of text>"; break;
    case  4: ss << "<end of transmission>"; break;
    case  5: ss << "<enquiry>"; break;
    case  6: ss << "<acknowledge>"; break;
    case  7: ss << "<bell>"; break;
    case  8: ss << "<backspace>"; break;
    // case  9: ss << "<horizontal tab>"; break;
    // case 10: ss << "<newline>"; break;
    case 11: ss << "<vertical tab>"; break;
    case 12: ss << "<new page>"; break;
    // case 13: ss << "<carriage return>"; break;
    case 14: ss << "<shift out>"; break;
    case 15: ss << "<shift in>"; break;
    case 16: ss << "<data link esqape>"; break;
    case 17: ss << "<device control 1>"; break;
    case 18: ss << "<device control 2>"; break;
    case 19: ss << "<device control 3>"; break;
    case 20: ss << "<device control 4>"; break;
    case 21: ss << "<negative acknowledge>"; break;
    case 22: ss << "<synchronous idle>"; break;
    case 23: ss << "<end of trans. block>"; break;
    case 24: ss << "<cancel>"; break;
    case 25: ss << "<end of medium>"; break;
    case 26: ss << "<substitute>"; break;
    case 27: ss << "<escape>"; break;
    case 28: ss << "<file separator>"; break;
    case 29: ss << "<group separator>"; break;
    case 30: ss << "<record separator>"; break;
    case 31: ss << "<unit separator>"; break;
    case 127: ss << "<DEL>"; break;

    case ' ':
        ss << "<space>";
        break;
    default:
        ss << c;
        break;
    }

    if(style == char_to_string_style::include_hex || smart_characters.find(c) == std::string_view::npos)
    {
        ss << "(0x" << std::hex << static_cast<int>(c) << ")";
    }
    return ss.str();
}


std::string::size_type
find_first_index_of_mismatch(const std::string& lhs, const std::string& rhs)
{
    const auto end = std::min(lhs.size(), rhs.size());

    std::string::size_type index = 0;
    for(; index < end; index+=1)
    {
        if(lhs[index]!=rhs[index])
        {
            return index;
        }
    }

    if(index >= lhs.size() && index >= rhs.size())
    {
        return std::string::npos;
    }
    else
    {
        return end;
    }
}


void
replace_all(std::string* string, const std::string& to_find, const std::string& to_replace)
{
    std::size_t index = string->find(to_find);
    const std::size_t find_length = to_find.length();
    ASSERT(find_length > 0);
    while(index != std::string::npos)
    {
        string->erase(index, find_length);
        string->insert(index, to_replace);
        index = string->find(to_find, index);
    }
}


std::string
replace_all(const std::string& string, const std::string& to_find, const std::string& to_replace)
{
    std::string temp = string;
    replace_all(&temp, to_find, to_replace);
    return temp;
}


void
copy(char* dst, const std::string& src, const std::string::size_type& count)
{
    strncpy(dst, src.c_str(), count - 1);
    dst[count - 1] = 0;
}


std::string
replace_with_character(const std::string& string, const std::string& to_find, char to_replace)
{
    std::string s = string;
    for(char c: to_find)
    {
        std::replace(s.begin(), s.end(), c, to_replace);
    }
    return s;
}


std::string
remove_from_end(const std::string& str, const std::string& end)
{
    if(ends_with(str, end))
    {
        const auto new_length = str.length() - end.length();
        if(new_length == 0)
        {
            return "";
        }
        ASSERT(new_length > 0);
        return str.substr(0, new_length);
    }

    return str;
}


std::string
strip(const std::string& str, const std::string& ch)
{
    std::stringstream ss;
    for(const char c: str)
    {
        if(ch.find(c) == std::string::npos)
        {
            ss << c;
        }
    }
    return ss.str();
}


// remove all characters in ch except the first one in a chain from str
std::string
remove_consecutive(const std::string& str, const std::string& ch)
{
    std::stringstream ss;
    bool skip = false;
    for(const char c: str)
    {
        if(ch.find(c) == std::string::npos)
        {
            ss << c;
            skip = false;
        }
        else
        {
            if(!skip)
            {
                ss << c;
                skip = true;
            }
        }
    }
    return ss.str();
}


template <typename Out>
void
split(const std::string& s, char delim, Out result)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim))
    {
        *(result++) = item;
    }
}


std::vector<std::string>
split(const std::string& s, char delim)
{
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}


std::vector<std::string>
split_on_spaces(const std::string& string)
{
    std::istringstream iss(string);
    return std::vector<std::string>
    (
        std::istream_iterator<std::string>{iss},
        std::istream_iterator<std::string>()
    );
}


std::string
optional_string(bool b, const std::string& str)
{
    if(b)
    {
        return str;
    }
    else
    {
        return "";
    }
}


bool
is_number(char b)
{
    return b >= '0' && b <= '9';
}


int
parse_number(const char** aa)
{
    const char*& a = *aa;

    int result = *a - '0';
    ++a;

    while(is_number(*a))
    {
        result *= 10;
        result += *a - '0';
        ++a;
    }

    --a;
    return result;
}


int
string_compare(const std::string& lhs, const std::string& rhs)
{
    const char* a = lhs.c_str();
    const char* b = rhs.c_str();

    if(a == b)    { return  0; }
    if(a == nullptr) { return -1; }
    if(b == nullptr) { return  1; }

    while(*a != 0 && *b != 0)
    {
        // will contain either a number or a letter
        const int a0 = is_number(*a) ? parse_number(&a) + 256 : to_lower_char(*a);
        const int b0 = is_number(*b) ? parse_number(&b) + 256 : to_lower_char(*b);

        if(a0 < b0) { return -1; }
        if(a0 > b0) { return  1; }

        ++a;
        ++b;
    }

    if(*a != 0) { return  1; }
    if(*b != 0) { return -1; }

    return 0;
}

}
