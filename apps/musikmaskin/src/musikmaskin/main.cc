#include "core/mat4.h"
#include "core/random.h"
#include "core/mat4.h"
#include "core/axisangle.h"
#include "core/aabb.h"
#include "core/texturetypes.h"
#include "core/vfs.h"
#include "core/vfs_imagegenerator.h"
#include "core/vfs_path.h"
#include "core/os.h"
#include "core/range.h"
#include "core/camera3.h"
#include "core/stringutils.h"
#include "core/stdutils.h"
#include "core/proto.h"
#include "core/log.h"
#include "core/noncopyable.h"
#include "core/cint.h"

#include "render/init.h"
#include "render/debuggl.h"
#include "render/materialshader.h"
#include "render/compiledmesh.h"
#include "render/texturecache.h"
#include "render/shaderattribute3d.h"
#include "render/texture.h"
#include "render/world.h"
#include "render/viewport.h"
#include "render/materialshadercache.h"
#include "render/defaultfiles.h"


#include "window/imguilibrary.h"
#include "window/timer.h"
#include "window/imgui_extra.h"
#include "window/sdllibrary.h"
#include "window/sdlwindow.h"
#include "window/sdlglcontext.h"
#include "window/filesystem.h"
#include "window/engine.h"
#include "window/canvas.h"
#include "window/key.h"

#include "imgui/imgui.h"
#include "SDL.h"
#include <iostream>
#include <memory>


#include "imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"

#include "core/last_n.h"

#include "window/imgui_extra.h"

#include "minsynth/synth.h"

#ifdef max
#undef max
#endif

#include "euph_generated_config.h"

#include "RtMidi.h"

using namespace euphoria::core;
using namespace euphoria::window;
using namespace euphoria::minsynth;


struct application_base
{
    application_base()
    {
        application_base::setup_window("musik maskin");
        setup_audio_callbacks();
    }

    NONCOPYABLE(application_base);

    static void
    setup_window(const std::string&)
    {
        ImGui::StyleColorsDark();
    }

    void
    setup_audio_callbacks()
    {
        SDL_AudioSpec spec;
        SDL_memset(&spec, 0, sizeof(spec));
        spec.freq = sample_frequency;
        spec.format = AUDIO_S16SYS;
        spec.channels = 1;
        spec.samples = 1024;
        spec.callback = sdl_audio_callback;
        spec.userdata = this;

        if(0 != SDL_OpenAudio(&spec, nullptr))
        {
            LOG_ERROR("Failed to setup audio: {0}", SDL_GetError());
            ok = false;
        }

        audio_opened = true;

        const int count = SDL_GetNumAudioDevices(0);
        for(int i = 0; i < count; ++i)
        {
            LOG_INFO
            (
                "Audio device {0}: {1}",
                i,
                SDL_GetAudioDeviceName(i, 0)
            );
        }
    }

    void
    start() const
    {
        if(ok)
        {
            SDL_PauseAudio(0);
        }
    }

    virtual ~application_base()
    {
        if(audio_opened)
        {
            SDL_CloseAudio();
        }
    }

    virtual void
    draw() = 0;

    virtual float
    synth_sample(float time) = 0;

    void
    on_render()
    {
        draw();
    }

    void
    audio_callback(Uint8* stream, int bytes)
    {
        const int len = bytes / 2;
        auto* output = reinterpret_cast<Sint16*>(stream); // NOLINT

        const Sint16 max_amplitude = 32767;

        for(int i = 0; i < len; i += 1)
        {
            const float sample_offset = static_cast<float>(i + samples_consumed)/static_cast<float>(sample_frequency);
            const float sample_time = audio_callback_time + sample_offset;
            auto sample = synth_sample(sample_time);
            if(sample > 1)
            {
                sample = 1;
            }
            if(sample < -1)
            {
                sample = -1;
            }
            output[i] = static_cast<Sint16>(max_amplitude * sample);
            max_sample_time = sample_time;
        }

        samples_consumed += len;
        while(samples_consumed >= sample_frequency)
        {
            samples_consumed -= sample_frequency;
            audio_callback_time += 1;
        }
    }

    static void
    sdl_audio_callback(void* userdata, Uint8* stream, int len)
    {
        auto* app = static_cast<application_base*>(userdata);
        app->audio_callback(stream, len);
    }

    bool ok = true;
    SDL_Window* window = nullptr;
    SDL_GLContext gl_context = nullptr;

    bool audio_opened = false;
    float max_sample_time = 0;
    int sample_frequency = 44100;
    float audio_callback_time = 0;
    int samples_consumed = 0;
};


struct midi_input_node : public euphoria::minsynth::midi_in_node
{
    tone_taker* tones = nullptr;

    std::unique_ptr<RtMidiIn> midi;

    static void
    static_callback
    (
        double deltatime,
        std::vector<unsigned char>* message,
        void* user_data
    )
    {
        auto* self = static_cast<midi_in_node*>(user_data);
        if(message != nullptr)
        {
            self->callback(static_cast<float>(deltatime), *message);
            // self->debug_callback(deltatime, *message);
        }
    }

    void
    setup()
    {
        try
        {
            midi = std::make_unique<RtMidiIn>();
            midi->setCallback(&static_callback, this);

            if(open_virtual_port)
            {
                midi->openVirtualPort();
            }

            for(unsigned int i = 0; i < midi->getPortCount(); i += 1)
            {
                const std::string name = midi->getPortName(i);
                LOG_INFO("Midi port {0}: {1}", i, name);
            }

            midi->openPort(port_number);
            LOG_INFO("Using port {0}", midi->getPortName(port_number));
        }
        catch(const RtMidiError& error)
        {
            error.printMessage();
        }
    }
};


void
draw_keys
(
    const std::vector<piano_key>& piano,
    const keyboard_layout& layout,
    float start_x,
    float start_y,
    float width,
    float height,
    float spacing
)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float y = start_y;
    float x_offset = 0;
    for(const auto& row: layout)
    {
        float x = start_x + x_offset;

        for(const auto& key: row)
        {
            const ImVec2 pos {x, y};
            const ImVec2 tone_offset {6, 8};

            const auto found = std::find_if
            (
                piano.begin(), piano.end(),
                [=](const piano_key& p) -> bool
                {
                    return p.keycode == key;
                }
            );

            const auto t = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_TextDisabled]);
            const auto c = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_WindowBg]);
            const auto tt = ImGui::ColorConvertFloat4ToU32(style.Colors[ImGuiCol_Text]);

            const auto key_text = to_string(key);

            draw_list->AddRectFilled(pos, pos + ImVec2(width, height), c);
            draw_list->AddText(pos, t, key_text.c_str());

            if(found != piano.end())
            {
                draw_list->AddText(pos + tone_offset, tt, found->name.c_str());
            }

            x += width + spacing;
        }

        x_offset += (width + spacing) / 2;
        y += height + spacing;
    }
}


class application : public application_base
{
public:
    midi_input_node midi;
    keyboard_input_node piano;
    single_tone_node single_tone;
    arpegiator_node arp;
    tone_to_frequency_converter_node ttf;
    oscilator_node oscilator;
    scaler_effect scaler;
    volume_node master;

    std::vector<node*> nodes;

    float current_time = 0;

    application()
    {
        setup_qwerty_two_octave_layout(&piano.keys, 4, -2);

        midi.tones = &ttf;
        piano.tones = &ttf;
        arp.next_node = &ttf;
        single_tone.next_node = &ttf;
        ttf.next = &oscilator;
        scaler.in = &oscilator;
        master.in = &scaler;

        midi.setup();

        nodes.emplace_back(&piano);
        nodes.emplace_back(&single_tone);
        nodes.emplace_back(&arp);
        nodes.emplace_back(&ttf);
        nodes.emplace_back(&oscilator);
        nodes.emplace_back(&scaler);
        nodes.emplace_back(&master);
    }

    void
    draw() override
    {
        // imgui: demo window
        ImGui::ShowDemoWindow();

        // imgui: style window
        if(ImGui::Begin("ImGui Style"))
        {
            ImGui::ShowStyleEditor();
        }
        ImGui::End();

        // musik maskin ui
        if(ImGui::Begin("UI"))
        {
            if(ImGui::Button("Light"))
            {
                ImGui::StyleColorsLight();
            }
            ImGui::SameLine();
            if(ImGui::Button("Dark"))
            {
                ImGui::StyleColorsDark();
            }
        }
        ImGui::End();

        // keyboard input
        if(ImGui::Begin("Keyboard"))
        {
            if(imgui::canvas_begin(ImVec4(0, 0, 0, 0.5f), "canvas_piano"))
            {
                const auto p = ImGui::GetCursorScreenPos();
                constexpr float keysize = 30;
                draw_keys
                (
                    piano.keys,
                    create_qwerty_keyboard_layout(),
                    p.x + 10,
                    p.y + 10,
                    keysize,
                    keysize,
                    3
                );
            }
            imgui::canvas_end();
        }
        ImGui::End();

        // musik maskin main window
        if(ImGui::Begin("Main"))
        {
            static last_n<float> time_history(100);
            static float max_diff = 0;
            ImGui::Text
            (
                "Tones: %d, %d alive and %d dead",
                oscilator.get_total_tones(),
                oscilator.get_alive_tones(),
                oscilator.get_dead_tones()
            );
            imgui::label(fmt::format("Time: {.1f}", current_time));
            imgui::label(fmt::format("Sample time: {.1f}", max_sample_time));
            imgui::label(fmt::format("Max diff: {.1f}", max_diff));
            const auto f = current_time - audio_callback_time;
            time_history.push(f);
            if(f > max_diff)
            {
                max_diff = f;
            }
            ImGui::PlotLines("Callback diff", time_history.data(), time_history.size());
            // ImGui::Text("Callback time: %.1f", time - audio_callback_time);

            // horizontal line?

            ImGui::SliderFloat("master", &master.volume, 0.0f, 1.0f);

            imgui::knob("Master", &master.volume, 0.0f, 1.0f);

            imgui::custom_dropdown
            (
                "tuning", &ttf.tuning, tuning::max_value,
                [](auto t)
                {
                    return to_string(t);
                }
            );

            ImGui::DragFloat
            (
                "Time to start",
                &oscilator.envelope.time_to_start,
                0.01f,
                0.0f,
                1.0f
            );
            ImGui::DragFloat
            (
                "Time to end",
                &oscilator.envelope.time_to_end,
                0.01f,
                0.0f,
                1.0f
            );

            imgui::custom_dropdown
            (
                "Oscilator",
                &oscilator.oscilator,
                oscilator_type::max_value,
                [](auto t)
                {
                    return to_string(t);
                }
            );

            imgui::custom_dropdown
            (
                "Chord emulation",
                &piano.chords_emulation,
                chord_emulation::max_value,
                [](auto t)
                {
                    return to_string(t);
                }
            );

            ImGui::InputInt("Times", &scaler.times, 1, 5);

            ImGui::InputInt("Arp octaves", &arp.octaves);
            imgui::custom_dropdown
            (
                "Arp mode", &arp.mode, arp_mode::max_value,
                [](auto t)
                {
                    return to_string(t);
                }
            );
            imgui::knob("Update time", &arp.update_time, 0, 1);
            ImGui::SameLine();
            imgui::knob("Tone time", &arp.tone_time, 0, 1);

            {
                ImGui::BeginChild("audio devices", ImVec2(0, 0), true);
                int count = SDL_GetNumAudioDevices(0);

                for(int i = 0; i < count; ++i)
                {
                    ImGui::Text("%s", SDL_GetAudioDeviceName(i, 0));
                }
                ImGui::EndChild();
            }
        }

        ImGui::End();
    }

    float
    synth_sample(float time) override
    {
        return master.get_output(time);
    }

    void
    update(float dt)
    {
        for(auto* node: nodes)
        {
            node->update(dt, current_time);
        }
    }

    void
    on_key(key key, bool down)
    {
        piano.on_input(key, down, current_time);
    }
};


int
main(int argc, char** argv)
{
    engine engine;

    if (const auto r = engine.setup(argparse::name_and_arguments::extract(argc, argv)); r != 0)
    {
        return r;
    }

    int window_width = 1280;
    int window_height = 720;

    if(!engine.create_window("Musikmaskin", window_width, window_height, true))
    {
        return -1;
    }

    application app;
    app.start();

    bool running = true;

    //////////////////////////////////////////////////////////////////////////////
    // main loop

    float time = 0;
    Uint64 current_time = SDL_GetPerformanceCounter();
    Uint64 last_time = 0;

    while(running)
    {
        last_time = current_time;
        current_time = SDL_GetPerformanceCounter();

        const float dt = euphoria::core::c_u64_to_float(current_time - last_time) / euphoria::core::c_u64_to_float(SDL_GetPerformanceFrequency());

        time += dt;
        app.current_time = time;


        SDL_Event e;
        while(SDL_PollEvent(&e) != 0)
        {
            imgui::process_imgui_events(&e);

            if(engine.on_resize(e, &window_width, &window_height))
            {
            }

            // key handling
            switch(e.type)
            {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if(!ImGui::GetIO().WantCaptureKeyboard)
                {
                    if(e.key.repeat == 0)
                    {
                        app.on_key(to_key(e.key.keysym), e.type == SDL_KEYDOWN);
                    }
                }
                break;
            default: break;
            }

            switch(e.type)
            {
            case SDL_QUIT: running = false; break;
            default: break;
            }
        }

        app.update(dt);

        engine.imgui->start_new_frame();

        if(ImGui::BeginMainMenuBar())
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit", "Ctrl+Q"))
                {
                    running = false;
                }
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();

        app.on_render();
        // ImGui::End();

        engine.init->clear_screen(color::light_gray);
        imgui::imgui_render();

        SDL_GL_SwapWindow(engine.window->window);
    }

    return 0;
}
