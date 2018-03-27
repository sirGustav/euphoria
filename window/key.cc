#include "window/key.h"

#include "core/assert.h"

Key
ToKey(SDL_Keysym key)
{
  switch(key.sym)
  {
    case SDLK_RETURN:
      return Key::RETURN;
    case SDLK_ESCAPE:
      return Key::ESCAPE;
    case SDLK_BACKSPACE:
      return Key::BACKSPACE;
    case SDLK_TAB:
      return Key::TAB;
    case SDLK_SPACE:
      return Key::SPACE;
    case SDLK_EXCLAIM:
      return Key::EXCLAIM;
    case SDLK_QUOTEDBL:
      return Key::QUOTEDBL;
    case SDLK_HASH:
      return Key::HASH;
    case SDLK_PERCENT:
      return Key::PERCENT;
    case SDLK_DOLLAR:
      return Key::DOLLAR;
    case SDLK_AMPERSAND:
      return Key::AMPERSAND;
    case SDLK_QUOTE:
      return Key::QUOTE;
    case SDLK_LEFTPAREN:
      return Key::LEFT_PAREN;
    case SDLK_RIGHTPAREN:
      return Key::RIGHT_PAREN;
    case SDLK_ASTERISK:
      return Key::ASTERIX;
    case SDLK_PLUS:
      return Key::PLUS;
    case SDLK_COMMA:
      return Key::COMMA;
    case SDLK_MINUS:
      return Key::MINUS;
    case SDLK_PERIOD:
      return Key::PERIOD;
    case SDLK_SLASH:
      return Key::SLASH;
    case SDLK_0:
      return Key::NUM_0;
    case SDLK_1:
      return Key::NUM_1;
    case SDLK_2:
      return Key::NUM_2;
    case SDLK_3:
      return Key::NUM_3;
    case SDLK_4:
      return Key::NUM_4;
    case SDLK_5:
      return Key::NUM_5;
    case SDLK_6:
      return Key::NUM_6;
    case SDLK_7:
      return Key::NUM_7;
    case SDLK_8:
      return Key::NUM_8;
    case SDLK_9:
      return Key::NUM_9;
    case SDLK_COLON:
      return Key::COLON;
    case SDLK_SEMICOLON:
      return Key::SEMICOLON;
    case SDLK_LESS:
      return Key::LESS;
    case SDLK_EQUALS:
      return Key::EQUALS;
    case SDLK_GREATER:
      return Key::GREATER;
    case SDLK_QUESTION:
      return Key::QUESTION;
    case SDLK_AT:
      return Key::AT;
    case SDLK_LEFTBRACKET:
      return Key::LEFT_BRACKET;
    case SDLK_BACKSLASH:
      return Key::BACKSLASH;
    case SDLK_RIGHTBRACKET:
      return Key::RIGHT_BRACKET;
    case SDLK_CARET:
      return Key::CARET;
    case SDLK_UNDERSCORE:
      return Key::UNDERSCORE;
    case SDLK_BACKQUOTE:
      return Key::BACKQUOTE;
    case SDLK_a:
      return Key::A;
    case SDLK_b:
      return Key::B;
    case SDLK_c:
      return Key::C;
    case SDLK_d:
      return Key::D;
    case SDLK_e:
      return Key::E;
    case SDLK_f:
      return Key::F;
    case SDLK_g:
      return Key::G;
    case SDLK_h:
      return Key::H;
    case SDLK_i:
      return Key::I;
    case SDLK_j:
      return Key::J;
    case SDLK_k:
      return Key::K;
    case SDLK_l:
      return Key::L;
    case SDLK_m:
      return Key::M;
    case SDLK_n:
      return Key::N;
    case SDLK_o:
      return Key::O;
    case SDLK_p:
      return Key::P;
    case SDLK_q:
      return Key::Q;
    case SDLK_r:
      return Key::R;
    case SDLK_s:
      return Key::S;
    case SDLK_t:
      return Key::T;
    case SDLK_u:
      return Key::U;
    case SDLK_v:
      return Key::V;
    case SDLK_w:
      return Key::W;
    case SDLK_x:
      return Key::X;
    case SDLK_y:
      return Key::Y;
    case SDLK_z:
      return Key::Z;
    case SDLK_CAPSLOCK:
      return Key::CAPSLOCK;
    case SDLK_F1:
      return Key::F1;
    case SDLK_F2:
      return Key::F2;
    case SDLK_F3:
      return Key::F3;
    case SDLK_F4:
      return Key::F4;
    case SDLK_F5:
      return Key::F5;
    case SDLK_F6:
      return Key::F6;
    case SDLK_F7:
      return Key::F7;
    case SDLK_F8:
      return Key::F8;
    case SDLK_F9:
      return Key::F9;
    case SDLK_F10:
      return Key::F10;
    case SDLK_F11:
      return Key::F11;
    case SDLK_F12:
      return Key::F12;
    case SDLK_PRINTSCREEN:
      return Key::PRINTSCREEN;
    case SDLK_SCROLLLOCK:
      return Key::SCROLLLOCK;
    case SDLK_PAUSE:
      return Key::PAUSE;
    case SDLK_INSERT:
      return Key::INSERT;
    case SDLK_HOME:
      return Key::HOME;
    case SDLK_PAGEUP:
      return Key::PAGEUP;
    case SDLK_DELETE:
      return Key::DELETE;
    case SDLK_END:
      return Key::END;
    case SDLK_PAGEDOWN:
      return Key::PAGEDOWN;
    case SDLK_RIGHT:
      return Key::RIGHT;
    case SDLK_LEFT:
      return Key::LEFT;
    case SDLK_DOWN:
      return Key::DOWN;
    case SDLK_UP:
      return Key::UP;
    case SDLK_NUMLOCKCLEAR:
      return Key::NUMLOCK_CLEAR;
    case SDLK_KP_DIVIDE:
      return Key::KEYPAD_DIVIDE;
    case SDLK_KP_MULTIPLY:
      return Key::KEYPAD_MULTIPLY;
    case SDLK_KP_MINUS:
      return Key::KEYPAD_MINUS;
    case SDLK_KP_PLUS:
      return Key::KEYPAD_PLUS;
    case SDLK_KP_ENTER:
      return Key::KEYPAD_ENTER;
    case SDLK_KP_1:
      return Key::KEYPAD_1;
    case SDLK_KP_2:
      return Key::KEYPAD_2;
    case SDLK_KP_3:
      return Key::KEYPAD_3;
    case SDLK_KP_4:
      return Key::KEYPAD_4;
    case SDLK_KP_5:
      return Key::KEYPAD_5;
    case SDLK_KP_6:
      return Key::KEYPAD_6;
    case SDLK_KP_7:
      return Key::KEYPAD_7;
    case SDLK_KP_8:
      return Key::KEYPAD_8;
    case SDLK_KP_9:
      return Key::KEYPAD_9;
    case SDLK_KP_0:
      return Key::KEYPAD_0;
    case SDLK_KP_PERIOD:
      return Key::KEYPAD_PERIOD;
    case SDLK_APPLICATION:
      return Key::APPLICATION;
    case SDLK_POWER:
      return Key::POWER;
    case SDLK_KP_EQUALS:
      return Key::KEYPAD_EQUALS;
    case SDLK_F13:
      return Key::F13;
    case SDLK_F14:
      return Key::F14;
    case SDLK_F15:
      return Key::F15;
    case SDLK_F16:
      return Key::F16;
    case SDLK_F17:
      return Key::F17;
    case SDLK_F18:
      return Key::F18;
    case SDLK_F19:
      return Key::F19;
    case SDLK_F20:
      return Key::F20;
    case SDLK_F21:
      return Key::F21;
    case SDLK_F22:
      return Key::F22;
    case SDLK_F23:
      return Key::F23;
    case SDLK_F24:
      return Key::F24;
    case SDLK_EXECUTE:
      return Key::EXECUTE;
    case SDLK_HELP:
      return Key::HELP;
    case SDLK_MENU:
      return Key::MENU;
    case SDLK_SELECT:
      return Key::SELECT;
    case SDLK_STOP:
      return Key::STOP;
    case SDLK_AGAIN:
      return Key::AGAIN;
    case SDLK_UNDO:
      return Key::UNDO;
    case SDLK_CUT:
      return Key::CUT;
    case SDLK_COPY:
      return Key::COPY;
    case SDLK_PASTE:
      return Key::PASTE;
    case SDLK_FIND:
      return Key::FIND;
    case SDLK_MUTE:
      return Key::MUTE;
    case SDLK_VOLUMEUP:
      return Key::VOLUME_UP;
    case SDLK_VOLUMEDOWN:
      return Key::VOLUME_DOWN;
    case SDLK_KP_COMMA:
      return Key::KEYPAD_COMMA;
    case SDLK_KP_EQUALSAS400:
      return Key::KEYPAD_EQUALS_AS_400;
    case SDLK_ALTERASE:
      return Key::ALT_ERASE;
    case SDLK_SYSREQ:
      return Key::SYSREQ;
    case SDLK_CANCEL:
      return Key::CANCEL;
    case SDLK_CLEAR:
      return Key::CLEAR;
    case SDLK_PRIOR:
      return Key::PRIOR;
    case SDLK_RETURN2:
      return Key::SECOND_RETURN;
    case SDLK_SEPARATOR:
      return Key::SEPARATOR;
    case SDLK_OUT:
      return Key::OUT;
    case SDLK_OPER:
      return Key::OPER;
    case SDLK_CLEARAGAIN:
      return Key::CLEAR_AGAIN;
    case SDLK_CRSEL:
      return Key::CRSEL;
    case SDLK_EXSEL:
      return Key::EXSEL;
    case SDLK_KP_00:
      return Key::KEYPAD_00;
    case SDLK_KP_000:
      return Key::KEYPAD_000;
    case SDLK_THOUSANDSSEPARATOR:
      return Key::THOUSANDSEPARATOR;
    case SDLK_DECIMALSEPARATOR:
      return Key::DECIMALSEPARATOR;
    case SDLK_CURRENCYUNIT:
      return Key::CURRENCY_UNIT;
    case SDLK_CURRENCYSUBUNIT:
      return Key::CURRENCY_SUBUNIT;
    case SDLK_KP_LEFTPAREN:
      return Key::KEYPAD_LEFTPAREN;
    case SDLK_KP_RIGHTPAREN:
      return Key::KEYPAD_RIGHTPAREN;
    case SDLK_KP_LEFTBRACE:
      return Key::KEYPAD_LEFTBRACE;
    case SDLK_KP_RIGHTBRACE:
      return Key::KEYPAD_RIGHTBRACE;
    case SDLK_KP_TAB:
      return Key::KEYPAD_TAB;
    case SDLK_KP_BACKSPACE:
      return Key::KEYPAD_BACKSPACE;
    case SDLK_KP_A:
      return Key::KEYPAD_A;
    case SDLK_KP_B:
      return Key::KEYPAD_B;
    case SDLK_KP_C:
      return Key::KEYPAD_C;
    case SDLK_KP_D:
      return Key::KEYPAD_D;
    case SDLK_KP_E:
      return Key::KEYPAD_E;
    case SDLK_KP_F:
      return Key::KEYPAD_F;
    case SDLK_KP_XOR:
      return Key::KEYPAD_XOR;
    case SDLK_KP_POWER:
      return Key::KEYPAD_POWER;
    case SDLK_KP_PERCENT:
      return Key::KEYPAD_PERCENT;
    case SDLK_KP_LESS:
      return Key::KEYPAD_LESS;
    case SDLK_KP_GREATER:
      return Key::KEYPAD_GREATER;
    case SDLK_KP_AMPERSAND:
      return Key::KEYPAD_AMPERSAND;
    case SDLK_KP_DBLAMPERSAND:
      return Key::KEYPAD_DOUBLE_AMPERSAND;
    case SDLK_KP_VERTICALBAR:
      return Key::KEYPAD_VERTICAL_BAR;
    case SDLK_KP_DBLVERTICALBAR:
      return Key::KEYPAD_DOUBLE_VERTICLE_BAR;
    case SDLK_KP_COLON:
      return Key::KEYPAD_COLON;
    case SDLK_KP_HASH:
      return Key::KEYPAD_HASH;
    case SDLK_KP_SPACE:
      return Key::KEYPAD_SPACE;
    case SDLK_KP_AT:
      return Key::KEYPAD_AT;
    case SDLK_KP_EXCLAM:
      return Key::KEYPAD_EXCLAM;
    case SDLK_KP_MEMSTORE:
      return Key::KEYPAD_MEM_STORE;
    case SDLK_KP_MEMRECALL:
      return Key::KEYPAD_MEM_RECALL;
    case SDLK_KP_MEMCLEAR:
      return Key::KEYPAD_MEM_CLEAR;
    case SDLK_KP_MEMADD:
      return Key::KEYPAD_MEM_ADD;
    case SDLK_KP_MEMSUBTRACT:
      return Key::KEYPAD_MEM_SUBTRACT;
    case SDLK_KP_MEMMULTIPLY:
      return Key::KEYPAD_MEM_MULTIPLY;
    case SDLK_KP_MEMDIVIDE:
      return Key::KEYPAD_MEM_DIVIDE;
    case SDLK_KP_PLUSMINUS:
      return Key::KEYPAD_PLUS_MINUS;
    case SDLK_KP_CLEAR:
      return Key::KEYPAD_CLEAR;
    case SDLK_KP_CLEARENTRY:
      return Key::KEYPAD_CLEAR_ENTRY;
    case SDLK_KP_BINARY:
      return Key::KEYPAD_BINARY;
    case SDLK_KP_OCTAL:
      return Key::KEYPAD_OCTAL;
    case SDLK_KP_DECIMAL:
      return Key::KEYPAD_DECIMAL;
    case SDLK_KP_HEXADECIMAL:
      return Key::KEYPAD_HEXADECIMAL;
    case SDLK_LCTRL:
      return Key::CTRL_LEFT;
    case SDLK_LSHIFT:
      return Key::SHIFT_LEFT;
    case SDLK_LALT:
      return Key::ALT_LEFT;
    case SDLK_LGUI:
      return Key::GUI_LEFT;
    case SDLK_RCTRL:
      return Key::CTRL_RIGHT;
    case SDLK_RSHIFT:
      return Key::SHIFT_RIGHT;
    case SDLK_RALT:
      return Key::ALT_RIGHT;
    case SDLK_RGUI:
      return Key::GUI_RIGHT;
    case SDLK_MODE:
      return Key::MODE;
    case SDLK_AUDIONEXT:
      return Key::AUDIO_NEXT;
    case SDLK_AUDIOPREV:
      return Key::AUDIO_PREV;
    case SDLK_AUDIOSTOP:
      return Key::AUDIO_STOP;
    case SDLK_AUDIOPLAY:
      return Key::AUDIO_PLAY;
    case SDLK_AUDIOMUTE:
      return Key::AUDIO_MUTE;
    case SDLK_MEDIASELECT:
      return Key::MEDIASELECT;
    case SDLK_WWW:
      return Key::WWW;
    case SDLK_MAIL:
      return Key::MAIL;
    case SDLK_CALCULATOR:
      return Key::CALCULATOR;
    case SDLK_COMPUTER:
      return Key::COMPUTER;
    case SDLK_AC_SEARCH:
      return Key::AC_SEARCH;
    case SDLK_AC_HOME:
      return Key::AC_HOME;
    case SDLK_AC_BACK:
      return Key::AC_BACK;
    case SDLK_AC_FORWARD:
      return Key::AC_FORWARD;
    case SDLK_AC_STOP:
      return Key::AC_STOP;
    case SDLK_AC_REFRESH:
      return Key::AC_REFRESH;
    case SDLK_AC_BOOKMARKS:
      return Key::AC_BOOKMARKS;
    case SDLK_BRIGHTNESSDOWN:
      return Key::BRIGHTNESS_DOWN;
    case SDLK_BRIGHTNESSUP:
      return Key::BRIGHTNESS_UP;
    case SDLK_DISPLAYSWITCH:
      return Key::DISPLAYSWITCH;
    case SDLK_KBDILLUMTOGGLE:
      return Key::KEYBOARD_ILLUM_TOGGLE;
    case SDLK_KBDILLUMDOWN:
      return Key::KEYBOARD_ILLUM_DOWN;
    case SDLK_KBDILLUMUP:
      return Key::KEYBOARD_ILLUM_UP;
    case SDLK_EJECT:
      return Key::EJECT;
    case SDLK_SLEEP:
      return Key::SLEEP;
    default:
      ASSERT(0 && key.sym && "Invalid keyboard button");
      return Key::INVALID;
  }
}

MouseButton
ToKey(SDL_MouseButtonEvent mb)
{
  switch(mb.button)
  {
    case SDL_BUTTON_LEFT:
      return MouseButton::LEFT;
    case SDL_BUTTON_MIDDLE:
      return MouseButton::MIDDLE;
    case SDL_BUTTON_RIGHT:
      return MouseButton::RIGHT;
    case SDL_BUTTON_X1:
      return MouseButton::X1;
    case SDL_BUTTON_X2:
      return MouseButton::X2;
    default:
      ASSERT(0 && mb.button && "Invalid mouse button");
      return MouseButton::INVALID;
  }
}