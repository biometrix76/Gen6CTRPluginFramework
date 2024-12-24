#ifndef CTRPLUGINFRAMEWORK_KEYBOARDHANDLER_HPP
#define CTRPLUGINFRAMEWORK_KEYBOARDHANDLER_HPP

#include <Headers.h>

namespace CTRPluginFramework {
    // Callback function types
    using OnCompareCallback = bool (*)(const void *, string &);
    using OnEventsCallback = void(*)(Keyboard &, KeyboardEvent &event);

    // External declaration of the limit array
    extern unsigned int limit[2];

    // Callback function template
    template<typename T> bool Callback(const void *input, string &error);

    template<typename T>
    class KeyboardHandler {
        public:
            // Method declarations
            static bool Set(const string &prompt, bool allowAbort, bool useHex, int maxLength, T &userInput, T defaultValue, u32 minValue = 0, u32 maxValue = 0, bool (*compareCallback)(const void*, string&) = nullptr);
            static bool Set(const string &prompt, bool allowAbort, int maxLength, T &userInput, T defaultValue, void (*eventsCallback)(Keyboard&, KeyboardEvent&) = nullptr);
            static bool Set(const string &prompt, bool allowAbort, int maxLength, T &userInput, T defaultValue);

        private:
            // Private method declarations
            static void ConfigureKeyboard(Keyboard &keyboard, bool allowAbort, bool useHex, int maxLength, OnCompareCallback compareCallback, u32 minValue, u32 maxValue);
            static bool ExecuteKeyboard(Keyboard &keyboard, T &userInput, T defaultValue);
    };

    // Specializations for specific types
    template<> bool KeyboardHandler<u8>::Set(const string &prompt, bool allowAbort, bool useHex, int maxLength, u8 &userInput, u8 defaultValue, u32 minValue, u32 maxValue, bool (*compareCallback)(const void*, string&));
    template<> bool KeyboardHandler<u8>::Set(const string &prompt, bool allowAbort, int maxLength, u8 &userInput, u8 defaultValue, void (*eventsCallback)(Keyboard&, KeyboardEvent&));
    template<> bool KeyboardHandler<u16>::Set(const string &prompt, bool allowAbort, bool useHex, int maxLength, u16 &userInput, u16 defaultValue, u32 minValue, u32 maxValue, bool (*compareCallback)(const void*, string&));
    template<> bool KeyboardHandler<u16>::Set(const string &prompt, bool allowAbort, int maxLength, u16 &userInput, u16 defaultValue, void (*eventsCallback)(Keyboard&, KeyboardEvent&));
    template<> bool KeyboardHandler<u32>::Set(const string &prompt, bool allowAbort, bool useHex, int maxLength, u32 &userInput, u32 defaultValue, u32 minValue, u32 maxValue, bool (*compareCallback)(const void*, string&));
    template<> bool KeyboardHandler<u32>::Set(const string &prompt, bool allowAbort, int maxLength, u32 &userInput, u32 defaultValue, void (*eventsCallback)(Keyboard&, KeyboardEvent&));

    template<> bool KeyboardHandler<string>::Set(const string &prompt, bool allowAbort, int maxLength, string &userInput, string defaultValue, void (*eventsCallback)(Keyboard&, KeyboardEvent&));
    template<> bool KeyboardHandler<string>::Set(const string &prompt, bool allowAbort, int maxLength, string &userInput, string defaultValue);

    // GetArg template functions
    template <typename T> T *GetArg(MenuEntry *entry);
    template <typename T> T *GetArg(MenuEntry *entry, T defaultValue);
}

#endif