// -*- c++ -*-
#pragma once
#ifndef __REFLEX_DEFS_H__
#define __REFLEX_DEFS_H__


#include <rays/defs.h>


namespace Rays
{


	struct Coord2;
	struct Coord3;
	struct Coord4;
	struct Point;
	struct Bounds;
	struct Color;
	struct Matrix;

	class ColorSpace;
	class Polyline;
	class Polygon;
	class Bitmap;
	class Image;
	class Font;
	class Shader;

	class Painter;


}// Rays


namespace Reflex
{


	using namespace Xot::Types;

	using Rays::String;


	using Rays::coord;

	using Rays::Coord2;
	using Rays::Coord3;
	using Rays::Coord4;
	using Rays::Point;
	using Rays::Bounds;
	using Rays::Color;
	using Rays::Matrix;

	using Rays::ColorSpace;
	using Rays::Polyline;
	using Rays::Polygon;
	using Rays::Bitmap;
	using Rays::Image;
	using Rays::Font;
	using Rays::Shader;

	using Rays::Painter;


	enum KeyCode
	{

		KEY_NONE = 0,

		KEY_RETURN,
		KEY_ENTER = KEY_RETURN,
		KEY_ESCAPE,
		KEY_SPACE,
		KEY_BACKSPACE,
		KEY_TAB,

		KEY_LEFT,
		KEY_UP,
		KEY_RIGHT,
		KEY_DOWN,

		KEY_INSERT,
		KEY_DELETE,
		KEY_HOME,
		KEY_END,
		KEY_PAGEUP,
		KEY_PAGEDOWN,

		KEY_SHIFT,
		KEY_LSHIFT,
		KEY_RSHIFT,

		KEY_CONTROL,
		KEY_LCONTROL,
		KEY_RCONTROL,

		KEY_ALT,
		KEY_LALT,
		KEY_RALT,

		KEY_WIN,
		KEY_LWIN,
		KEY_RWIN,

		KEY_OPTION  = KEY_ALT,
		KEY_LOPTION = KEY_LALT,
		KEY_ROPTION = KEY_RALT,

		KEY_COMMAND  = KEY_WIN,
		KEY_LCOMMAND = KEY_LWIN,
		KEY_RCOMMAND = KEY_RWIN,

		KEY_CAPSLOCK,
		KEY_NUMLOCK,
		KEY_SCROLLLOCK,

		KEY_PRINTSCREEN,
		KEY_PAUSE,
		KEY_BREAK,

		KEY_SLEEP,
		KEY_EXEC,//UTE,
		KEY_HELP,
		KEY_PRINT,
		KEY_APPS,
		KEY_SELECT,
		KEY_CLEAR,
		KEY_PLAY,
		KEY_ZOOM,

		KEY_IME_KANA,
		KEY_IME_KANJI,
		KEY_IME_JUNJA,
		KEY_IME_PROCESS,
		KEY_IME_ACCEPT,
		KEY_IME_FINAL,
		KEY_IME_CONVERT,
		KEY_IME_NONCONVERT,
		KEY_IME_MODECHANGE,

		KEY_BROWSER_BACK,
		KEY_BROWSER_FORWARD,
		KEY_BROWSER_REFRESH,
		KEY_BROWSER_STOP,
		KEY_BROWSER_SEARCH,
		KEY_BROWSER_FAVORITES,
		KEY_BROWSER_HOME,

		KEY_VOLUME_MUTE,
		KEY_VOLUME_DOWN,
		KEY_VOLUME_UP,

		KEY_MEDIA_NEXT_TRACK,
		KEY_MEDIA_PREV_TRACK,
		KEY_MEDIA_STOP,
		KEY_MEDIA_PLAY_PAUSE,

		KEY_LAUNCH_MAIL,
		KEY_LAUNCH_MEDIA_SELECT,
		KEY_LAUNCH_APP1,
		KEY_LAUNCH_APP2,

		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_F13,
		KEY_F14,
		KEY_F15,
		KEY_F16,
		KEY_F17,
		KEY_F18,
		KEY_F19,
		KEY_F20,
		KEY_F21,
		KEY_F22,
		KEY_F23,
		KEY_F24,
		KEY_F25,
		KEY_F26,
		KEY_F27,
		KEY_F28,
		KEY_F29,
		KEY_F30,
		KEY_F31,
		KEY_F32,
		KEY_F33,
		KEY_F34,
		KEY_F35,

	};// KeyCode


	enum PointerType
	{

		POINTER_NONE         = 0,

		POINTER_MOUSE_LEFT   = 0x1 << 0,

		POINTER_MOUSE_RIGHT  = 0x1 << 1,

		POINTER_MOUSE_MIDDLE = 0x1 << 2,

		POINTER_TOUCH        = 0x1 << 3,

		POINTER_PEN          = 0x1 << 4,

	};// PointType


	enum Modifier
	{

		MOD_NONE     = 0,

#ifndef MOD_SHIFT
		MOD_SHIFT    = 0x1 << 2,

		MOD_CONTROL  = 0x1 << 1,

		MOD_ALT      = 0x1 << 0,

		MOD_WIN      = 0x1 << 3,
#endif

		MOD_OPTION   = MOD_ALT,

		MOD_COMMAND  = MOD_WIN,

		MOD_HELP     = 0x1 << 4,

		MOD_FUNCTION = 0x1 << 5,

		MOD_NUMPAD   = 0x1 << 6,

		MOD_CAPS     = 0x1 << 7,

	};// Modifier


}// Reflex


#endif//EOH
