#include "gui/textdata.h"

#include "core/assert.h"
#include "core/log.h"

#include "render/font.h"


namespace euphoria::gui
{
    TextData::TextData()
        : font_(nullptr)
        , size(-1.0f)
    {
    }


    TextData::~TextData() = default;


    void
    TextData::SetFont(std::shared_ptr<render::Font> font)
    {
        font_ = font;
        text_.reset();
        UpdateText();
    }


    const render::Font&
    TextData::GetFont() const
    {
        ASSERT(font_);
        return *font_;
    }


    void
    TextData::SetString(const std::string& str)
    {
        string_ = str;
        UpdateText();
    }


    const std::string&
    TextData::GetString() const
    {
        return string_;
    }


    bool
    TextData::HasText() const
    {
        return text_ != nullptr;
    }


    const render::Text&
    TextData::GetText() const
    {
        ASSERT(text_);
        return *text_;
    }


    render::Text&
    TextData::GetText()
    {
        ASSERT(text_);
        return *text_;
    }


    void
    TextData::SetSize(float new_size)
    {
        this->size = new_size;
        if(HasText())
        {
            GetText().SetSize(new_size);
        }
    }


    void
    TextData::UpdateText()
    {
        if(text_ == nullptr && font_ != nullptr)
        {
            text_ = std::make_shared<render::Text>(font_.get());
        }

        if(text_ != nullptr)
        {
            // button assumes this is bottom left
            text_->SetAlignment(render::Align::BOTTOM_LEFT);

            core::ui_text text;
            if(false == text.init_by_parsing_source(string_))
            {
                LOG_ERROR("Failed to parse {0}", string_);
            }
            LOG_INFO("Loaded {0}", core::textparser::visitor_debug_string::accept_all_nodes(&text));
            text_->SetText(text);
            text_->SetSize(size);
        }
    }
}

