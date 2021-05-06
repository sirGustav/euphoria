#include "window/key.h"

#include "core/assert.h"

namespace euphoria::window
{
    core::key
    ToKey(SDL_Keysym key)
    {
        switch(key.sym)
        {
        case SDLK_RETURN: return core::key::return_key;
        case SDLK_ESCAPE: return core::key::escape;
        case SDLK_BACKSPACE: return core::key::backspace;
        case SDLK_TAB: return core::key::tab;
        case SDLK_SPACE: return core::key::space;
        case SDLK_EXCLAIM: return core::key::exclaim;
        case SDLK_QUOTEDBL: return core::key::quotedbl;
        case SDLK_HASH: return core::key::hash;
        case SDLK_PERCENT: return core::key::percent;
        case SDLK_DOLLAR: return core::key::dollar;
        case SDLK_AMPERSAND: return core::key::ampersand;
        case SDLK_QUOTE: return core::key::quote;
        case SDLK_LEFTPAREN: return core::key::left_paren;
        case SDLK_RIGHTPAREN: return core::key::right_paren;
        case SDLK_ASTERISK: return core::key::asterix;
        case SDLK_PLUS: return core::key::plus;
        case SDLK_COMMA: return core::key::comma;
        case SDLK_MINUS: return core::key::minus;
        case SDLK_PERIOD: return core::key::period;
        case SDLK_SLASH: return core::key::slash;
        case SDLK_0: return core::key::num_0;
        case SDLK_1: return core::key::num_1;
        case SDLK_2: return core::key::num_2;
        case SDLK_3: return core::key::num_3;
        case SDLK_4: return core::key::num_4;
        case SDLK_5: return core::key::num_5;
        case SDLK_6: return core::key::num_6;
        case SDLK_7: return core::key::num_7;
        case SDLK_8: return core::key::num_8;
        case SDLK_9: return core::key::num_9;
        case SDLK_COLON: return core::key::colon;
        case SDLK_SEMICOLON: return core::key::semicolon;
        case SDLK_LESS: return core::key::less;
        case SDLK_EQUALS: return core::key::equals;
        case SDLK_GREATER: return core::key::greater;
        case SDLK_QUESTION: return core::key::question;
        case SDLK_AT: return core::key::at;
        case SDLK_LEFTBRACKET: return core::key::left_bracket;
        case SDLK_BACKSLASH: return core::key::backslash;
        case SDLK_RIGHTBRACKET: return core::key::right_bracket;
        case SDLK_CARET: return core::key::caret;
        case SDLK_UNDERSCORE: return core::key::underscore;
        case SDLK_BACKQUOTE: return core::key::backquote;
        case SDLK_a: return core::key::a;
        case SDLK_b: return core::key::b;
        case SDLK_c: return core::key::c;
        case SDLK_d: return core::key::d;
        case SDLK_e: return core::key::e;
        case SDLK_f: return core::key::f;
        case SDLK_g: return core::key::g;
        case SDLK_h: return core::key::h;
        case SDLK_i: return core::key::i;
        case SDLK_j: return core::key::j;
        case SDLK_k: return core::key::k;
        case SDLK_l: return core::key::l;
        case SDLK_m: return core::key::m;
        case SDLK_n: return core::key::n;
        case SDLK_o: return core::key::o;
        case SDLK_p: return core::key::p;
        case SDLK_q: return core::key::q;
        case SDLK_r: return core::key::r;
        case SDLK_s: return core::key::s;
        case SDLK_t: return core::key::t;
        case SDLK_u: return core::key::u;
        case SDLK_v: return core::key::v;
        case SDLK_w: return core::key::w;
        case SDLK_x: return core::key::x;
        case SDLK_y: return core::key::y;
        case SDLK_z: return core::key::z;
        case SDLK_CAPSLOCK: return core::key::capslock;
        case SDLK_F1: return core::key::f1;
        case SDLK_F2: return core::key::f2;
        case SDLK_F3: return core::key::f3;
        case SDLK_F4: return core::key::f4;
        case SDLK_F5: return core::key::f5;
        case SDLK_F6: return core::key::f6;
        case SDLK_F7: return core::key::f7;
        case SDLK_F8: return core::key::f8;
        case SDLK_F9: return core::key::f9;
        case SDLK_F10: return core::key::f10;
        case SDLK_F11: return core::key::f11;
        case SDLK_F12: return core::key::f12;
        case SDLK_PRINTSCREEN: return core::key::print_screen;
        case SDLK_SCROLLLOCK: return core::key::scroll_lock;
        case SDLK_PAUSE: return core::key::pause;
        case SDLK_INSERT: return core::key::insert;
        case SDLK_HOME: return core::key::home;
        case SDLK_PAGEUP: return core::key::page_up;
        case SDLK_DELETE: return core::key::delete_key;
        case SDLK_END: return core::key::end;
        case SDLK_PAGEDOWN: return core::key::page_down;
        case SDLK_RIGHT: return core::key::right;
        case SDLK_LEFT: return core::key::left;
        case SDLK_DOWN: return core::key::down;
        case SDLK_UP: return core::key::up;
        case SDLK_NUMLOCKCLEAR: return core::key::numlock_clear;
        case SDLK_KP_DIVIDE: return core::key::keypad_divide;
        case SDLK_KP_MULTIPLY: return core::key::keypad_multiply;
        case SDLK_KP_MINUS: return core::key::keypad_minus;
        case SDLK_KP_PLUS: return core::key::keypad_plus;
        case SDLK_KP_ENTER: return core::key::keypad_enter;
        case SDLK_KP_1: return core::key::keypad_1;
        case SDLK_KP_2: return core::key::keypad_2;
        case SDLK_KP_3: return core::key::keypad_3;
        case SDLK_KP_4: return core::key::keypad_4;
        case SDLK_KP_5: return core::key::keypad_5;
        case SDLK_KP_6: return core::key::keypad_6;
        case SDLK_KP_7: return core::key::keypad_7;
        case SDLK_KP_8: return core::key::keypad_8;
        case SDLK_KP_9: return core::key::keypad_9;
        case SDLK_KP_0: return core::key::keypad_0;
        case SDLK_KP_PERIOD: return core::key::keypad_period;
        case SDLK_APPLICATION: return core::key::application;
        case SDLK_POWER: return core::key::power;
        case SDLK_KP_EQUALS: return core::key::keypad_equals;
        case SDLK_F13: return core::key::f13;
        case SDLK_F14: return core::key::f14;
        case SDLK_F15: return core::key::f15;
        case SDLK_F16: return core::key::f16;
        case SDLK_F17: return core::key::f17;
        case SDLK_F18: return core::key::f18;
        case SDLK_F19: return core::key::f19;
        case SDLK_F20: return core::key::f20;
        case SDLK_F21: return core::key::f21;
        case SDLK_F22: return core::key::f22;
        case SDLK_F23: return core::key::f23;
        case SDLK_F24: return core::key::f24;
        case SDLK_EXECUTE: return core::key::execute;
        case SDLK_HELP: return core::key::help;
        case SDLK_MENU: return core::key::menu;
        case SDLK_SELECT: return core::key::select;
        case SDLK_STOP: return core::key::stop;
        case SDLK_AGAIN: return core::key::again;
        case SDLK_UNDO: return core::key::undo;
        case SDLK_CUT: return core::key::cut;
        case SDLK_COPY: return core::key::copy;
        case SDLK_PASTE: return core::key::paste;
        case SDLK_FIND: return core::key::find;
        case SDLK_MUTE: return core::key::mute;
        case SDLK_VOLUMEUP: return core::key::volume_up;
        case SDLK_VOLUMEDOWN: return core::key::volume_down;
        case SDLK_KP_COMMA: return core::key::keypad_comma;
        case SDLK_KP_EQUALSAS400: return core::key::keypad_equals_as_400;
        case SDLK_ALTERASE: return core::key::alt_erase;
        case SDLK_SYSREQ: return core::key::sysreq;
        case SDLK_CANCEL: return core::key::cancel;
        case SDLK_CLEAR: return core::key::clear;
        case SDLK_PRIOR: return core::key::prior;
        case SDLK_RETURN2: return core::key::second_return;
        case SDLK_SEPARATOR: return core::key::separator;
        case SDLK_OUT: return core::key::out;
        case SDLK_OPER: return core::key::oper;
        case SDLK_CLEARAGAIN: return core::key::clear_again;
        case SDLK_CRSEL: return core::key::crsel;
        case SDLK_EXSEL: return core::key::exsel;
        case SDLK_KP_00: return core::key::keypad_00;
        case SDLK_KP_000: return core::key::keypad_000;
        case SDLK_THOUSANDSSEPARATOR: return core::key::thousandseparator;
        case SDLK_DECIMALSEPARATOR: return core::key::decimalseparator;
        case SDLK_CURRENCYUNIT: return core::key::currency_unit;
        case SDLK_CURRENCYSUBUNIT: return core::key::currency_subunit;
        case SDLK_KP_LEFTPAREN: return core::key::keypad_leftparen;
        case SDLK_KP_RIGHTPAREN: return core::key::keypad_rightparen;
        case SDLK_KP_LEFTBRACE: return core::key::keypad_leftbrace;
        case SDLK_KP_RIGHTBRACE: return core::key::keypad_rightbrace;
        case SDLK_KP_TAB: return core::key::keypad_tab;
        case SDLK_KP_BACKSPACE: return core::key::keypad_backspace;
        case SDLK_KP_A: return core::key::keypad_a;
        case SDLK_KP_B: return core::key::keypad_b;
        case SDLK_KP_C: return core::key::keypad_c;
        case SDLK_KP_D: return core::key::keypad_d;
        case SDLK_KP_E: return core::key::keypad_e;
        case SDLK_KP_F: return core::key::keypad_f;
        case SDLK_KP_XOR: return core::key::keypad_xor;
        case SDLK_KP_POWER: return core::key::keypad_power;
        case SDLK_KP_PERCENT: return core::key::keypad_percent;
        case SDLK_KP_LESS: return core::key::keypad_less;
        case SDLK_KP_GREATER: return core::key::keypad_greater;
        case SDLK_KP_AMPERSAND: return core::key::keypad_ampersand;
        case SDLK_KP_DBLAMPERSAND: return core::key::keypad_double_ampersand;
        case SDLK_KP_VERTICALBAR: return core::key::keypad_vertical_bar;
        case SDLK_KP_DBLVERTICALBAR:
            return core::key::keypad_double_verticle_bar;
        case SDLK_KP_COLON: return core::key::keypad_colon;
        case SDLK_KP_HASH: return core::key::keypad_hash;
        case SDLK_KP_SPACE: return core::key::keypad_space;
        case SDLK_KP_AT: return core::key::keypad_at;
        case SDLK_KP_EXCLAM: return core::key::keypad_exclam;
        case SDLK_KP_MEMSTORE: return core::key::keypad_mem_store;
        case SDLK_KP_MEMRECALL: return core::key::keypad_mem_recall;
        case SDLK_KP_MEMCLEAR: return core::key::keypad_mem_clear;
        case SDLK_KP_MEMADD: return core::key::keypad_mem_add;
        case SDLK_KP_MEMSUBTRACT: return core::key::keypad_mem_subtract;
        case SDLK_KP_MEMMULTIPLY: return core::key::keypad_mem_multiply;
        case SDLK_KP_MEMDIVIDE: return core::key::keypad_mem_divide;
        case SDLK_KP_PLUSMINUS: return core::key::keypad_plus_minus;
        case SDLK_KP_CLEAR: return core::key::keypad_clear;
        case SDLK_KP_CLEARENTRY: return core::key::keypad_clear_entry;
        case SDLK_KP_BINARY: return core::key::keypad_binary;
        case SDLK_KP_OCTAL: return core::key::keypad_octal;
        case SDLK_KP_DECIMAL: return core::key::keypad_decimal;
        case SDLK_KP_HEXADECIMAL: return core::key::keypad_hexadecimal;
        case SDLK_LCTRL: return core::key::ctrl_left;
        case SDLK_LSHIFT: return core::key::shift_left;
        case SDLK_LALT: return core::key::alt_left;
        case SDLK_LGUI: return core::key::gui_left;
        case SDLK_RCTRL: return core::key::ctrl_right;
        case SDLK_RSHIFT: return core::key::shift_right;
        case SDLK_RALT: return core::key::alt_right;
        case SDLK_RGUI: return core::key::gui_right;
        case SDLK_MODE: return core::key::mode;
        case SDLK_AUDIONEXT: return core::key::audio_next;
        case SDLK_AUDIOPREV: return core::key::audio_prev;
        case SDLK_AUDIOSTOP: return core::key::audio_stop;
        case SDLK_AUDIOPLAY: return core::key::audio_play;
        case SDLK_AUDIOMUTE: return core::key::audio_mute;
        case SDLK_MEDIASELECT: return core::key::media_select;
        case SDLK_WWW: return core::key::www;
        case SDLK_MAIL: return core::key::mail;
        case SDLK_CALCULATOR: return core::key::calculator;
        case SDLK_COMPUTER: return core::key::computer;
        case SDLK_AC_SEARCH: return core::key::ac_search;
        case SDLK_AC_HOME: return core::key::ac_home;
        case SDLK_AC_BACK: return core::key::ac_back;
        case SDLK_AC_FORWARD: return core::key::ac_forward;
        case SDLK_AC_STOP: return core::key::ac_stop;
        case SDLK_AC_REFRESH: return core::key::ac_refresh;
        case SDLK_AC_BOOKMARKS: return core::key::ac_bookmarks;
        case SDLK_BRIGHTNESSDOWN: return core::key::brightness_down;
        case SDLK_BRIGHTNESSUP: return core::key::brightness_up;
        case SDLK_DISPLAYSWITCH: return core::key::displayswitch;
        case SDLK_KBDILLUMTOGGLE: return core::key::keyboard_illum_toggle;
        case SDLK_KBDILLUMDOWN: return core::key::keyboard_illum_down;
        case SDLK_KBDILLUMUP: return core::key::keyboard_illum_up;
        case SDLK_EJECT: return core::key::eject;
        case SDLK_SLEEP: return core::key::sleep;
        default:
            // todo(Gustav): die should have a x version too
            ASSERTX(false && "Invalid keyboard button", key.sym);
            return core::key::invalid;
        }
    }

    core::MouseButton
    ToKey(SDL_MouseButtonEvent mb)
    {
        switch(mb.button)
        {
        case SDL_BUTTON_LEFT: return core::MouseButton::left;
        case SDL_BUTTON_MIDDLE: return core::MouseButton::middle;
        case SDL_BUTTON_RIGHT: return core::MouseButton::right;
        case SDL_BUTTON_X1: return core::MouseButton::x1;
        case SDL_BUTTON_X2: return core::MouseButton::x2;
        default:
            ASSERTX(false && "Invalid mouse button", mb.button);
            return core::MouseButton::invalid;
        }
    }

}  // namespace euphoria::window
