/**
 * @file console.h
 * @brief 3ds stdio support.
 *
 * Provides stdio integration for printing to the 3DS screen as well as debug print
 * functionality provided by stderr.
 *
 * General usage is to initialize the console by:
 * @code
 * consoleDemoInit()
 * @endcode
 * or to customize the console usage by:
 * @code
 * consoleInit()
 * @endcode
 */
#pragma once

#include <3ds/types.h>
#include <3ds/gfx.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CONSOLE_ESC(x) "\x1b[" #x
#define CONSOLE_RESET   CONSOLE_ESC(0m)
#define CONSOLE_BLACK   CONSOLE_ESC(30m)
#define CONSOLE_RED     CONSOLE_ESC(31;1m)
#define CONSOLE_GREEN   CONSOLE_ESC(32;1m)
#define CONSOLE_YELLOW  CONSOLE_ESC(33;1m)
#define CONSOLE_BLUE    CONSOLE_ESC(34;1m)
#define CONSOLE_MAGENTA CONSOLE_ESC(35;1m)
#define CONSOLE_CYAN    CONSOLE_ESC(36;1m)
#define CONSOLE_WHITE   CONSOLE_ESC(37;1m)

/// A callback for printing a character.
typedef bool(*ConsolePrint)(void* con, int c);

/// A font struct for the console.
typedef struct ConsoleFont
{
	u8* gfx;         ///< A pointer to the font graphics
	u16 asciiOffset; ///< Offset to the first valid character in the font table
	u16 numChars;    ///< Number of characters in the font graphics
}ConsoleFont;

/**
 * @brief Console structure used to store the state of a console render context.
 *
 * Default values from consoleGetDefault();
 * @code
 * PrintConsole defaultConsole =
 * {
 * 	//Font:
 * 	{
 * 		(u8*)default_font_bin, //font gfx
 * 		0, //first ascii character in the set
 * 		128, //number of characters in the font set
 *	},
 *	0,0, //cursorX cursorY
 *	0,0, //prevcursorX prevcursorY
 *	40, //console width
 *	30, //console height
 *	0,  //window x
 *	0,  //window y
 *	32, //window width
 *	24, //window height
 *	3, //tab size
 *	0, //font character offset
 *	0,  //print callback
 *	false //console initialized
 * };
 * @endcode
 */
typedef struct PrintConsole
{
	ConsoleFont font;        ///< Font of the console

	u16 *frameBuffer;        ///< Framebuffer address

	int cursorX;             ///< Current X location of the cursor (as a tile offset by default)
	int cursorY;             ///< Current Y location of the cursor (as a tile offset by default)

	int prevCursorX;         ///< Internal state
	int prevCursorY;         ///< Internal state

	int consoleWidth;        ///< Width of the console hardware layer in characters
	int consoleHeight;       ///< Height of the console hardware layer in characters

	int windowX;             ///< Window X location in characters (not implemented)
	int windowY;             ///< Window Y location in characters (not implemented)
	int windowWidth;         ///< Window width in characters (not implemented)
	int windowHeight;        ///< Window height in characters (not implemented)

	int tabSize;             ///< Size of a tab
	u16 fg;                  ///< Foreground color
	u16 bg;                  ///< Background color
	int flags;               ///< Reverse/bright flags

	ConsolePrint PrintChar;  ///< Callback for printing a character. Should return true if it has handled rendering the graphics (else the print engine will attempt to render via tiles).

	bool consoleInitialised; ///< True if the console is initialized
}PrintConsole;

#define CONSOLE_COLOR_BOLD	(1<<0) ///< Bold text
#define CONSOLE_COLOR_FAINT	(1<<1) ///< Faint text
#define CONSOLE_ITALIC		(1<<2) ///< Italic text
#define CONSOLE_UNDERLINE	(1<<3) ///< Underlined text
#define CONSOLE_BLINK_SLOW	(1<<4) ///< Slow blinking text
#define CONSOLE_BLINK_FAST	(1<<5) ///< Fast blinking text
#define CONSOLE_COLOR_REVERSE	(1<<6) ///< Reversed color text
#define CONSOLE_CONCEAL		(1<<7) ///< Concealed text
#define CONSOLE_CROSSED_OUT	(1<<8) ///< Crossed out text
#define CONSOLE_FG_CUSTOM	(1<<9) ///< Foreground custom color
#define CONSOLE_BG_CUSTOM	(1<<10) ///< Background custom color
#define CONSOLE_COLOR_FG_BRIGHT (1<<11) ///< Bright foreground color
#define CONSOLE_COLOR_BG_BRIGHT (1<<12) ///< Bright background color

/// Console debug devices supported by libctru.
typedef enum {
	debugDevice_NULL,    ///< Swallows prints to stderr
	debugDevice_SVC,     ///< Outputs stderr debug statements using svcOutputDebugString, which can then be captured by interactive debuggers
	debugDevice_CONSOLE, ///< Directs stderr debug statements to 3DS console window
	debugDevice_3DMOO = debugDevice_SVC,
} debugDevice;

/**
 * @brief Loads the font into the console.
 * @param console Pointer to the console to update, if NULL it will update the current console.
 * @param font The font to load.
 */
void consoleSetFont(PrintConsole* console, ConsoleFont* font);

/**
 * @brief Sets the print window.
 * @param console Console to set, if NULL it will set the current console window.
 * @param x X location of the window.
 * @param y Y location of the window.
 * @param width Width of the window.
 * @param height Height of the window.
 */
void consoleSetWindow(PrintConsole* console, int x, int y, int width, int height);

/**
 * @brief Gets a pointer to the console with the default values.
 * This should only be used when using a single console or without changing the console that is returned, otherwise use consoleInit().
 * @return A pointer to the console with the default values.
 */
PrintConsole* consoleGetDefault(void);

/**
 * @brief Make the specified console the render target.
 * @param console A pointer to the console struct (must have been initialized with consoleInit(PrintConsole* console)).
 * @return A pointer to the previous console.
 */
PrintConsole *consoleSelect(PrintConsole* console);

/**
 * @brief Initialise the console.
 * @param screen The screen to use for the console.
 * @param console A pointer to the console data to initialize (if it's NULL, the default console will be used).
 * @return A pointer to the current console.
 */
PrintConsole* consoleInit(gfxScreen_t screen, PrintConsole* console);

/**
 * @brief Initializes debug console output on stderr to the specified device.
 * @param device The debug device (or devices) to output debug print statements to.
 */
void consoleDebugInit(debugDevice device);

/// Clears the screen by using iprintf("\x1b[2J");
void consoleClear(void);

#ifdef __cplusplus
}
#endif
