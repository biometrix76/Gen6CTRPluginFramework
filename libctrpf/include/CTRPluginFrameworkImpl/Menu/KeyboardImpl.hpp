#ifndef CTRPLUGINFRAMEWORKIMPL_KEYBOARD_HPP
#define CTRPLUGINFRAMEWORKIMPL_KEYBOARD_HPP

#include <Headers.h>
#include "CTRPluginFrameworkImpl/Graphics.hpp"
#include "CTRPluginFrameworkImpl/Graphics/TouchKey.hpp"
#include "CTRPluginFrameworkImpl/Graphics/TouchKeyString.hpp"
#include "CTRPluginFrameworkImpl/System.hpp"

namespace CTRPluginFramework {
    enum Layout {
        QWERTY,
        DECIMAL,
        HEXADECIMAL
    };

    class Keyboard;
    class KeyboardImpl {
        using CompareCallback = bool (*)(const void*, string&);
        using ConvertCallback = void *(*)(string&, bool);
        using OnEventCallback = void(*)(Keyboard&, KeyboardEvent&);
        using KeyIter = vector<TouchKey>::iterator;
        using KeyStringIter = vector<TouchKeyString>::iterator;

        public:
            KeyboardImpl(const string &text = "");
            explicit KeyboardImpl(Keyboard *kb, const string &text = "");
            ~KeyboardImpl(void);

            void SetLayout(Layout layout);
            void SetHexadecimal(bool isHex);
            bool IsHexadecimal(void) const;
            void SetMaxInput(u32 max);
            void CanAbort(bool canAbort);
            void CanChangeLayout(bool canChange);
            string &GetInput(void);
            string &GetMessage(void);
            string &GetTitle(void);
            void SetError(string &error);

            void SetConvertCallback(ConvertCallback callback);
            void SetCompareCallback(CompareCallback callback);
            void OnKeyboardEvent(OnEventCallback callback);
            void ChangeSelectedEntry(int entry);
            int GetSelectedEntry() {return _manualKey;}
            void Populate(const vector<string> &input, bool resetScroll);
            void Populate(const vector<CustomIcon> &input, bool resetScroll);
            void Clear(void);

            int Run(void);
            void Close(void);
            bool operator()(int &out);

            bool DisplayTopScreen;

        private:
            friend class HexEditor;
            friend class ARCodeEditor;

            void _RenderTop(void);
            void _RenderBottom(void);
            void _ProcessEvent(Event &event);
            void _UpdateScroll(float delta, bool ignoreTouch);
            void _Update(float delta);

            // Keyboard layout constructor
            void _Qwerty(void);
            void _QwertyLowCase(void);
            void _QwertyUpCase(void);
            void _QwertySymbols(void);
            void _QwertyNintendo(void);
            static void _DigitKeyboard(vector<TouchKey> &keys);
            void _Decimal(void);
            void _Hexadecimal(void);

            void _ScrollUp(void);
            void _ScrollDown(void);
            void _UpdateScrollInfos(void);
            bool _CheckKeys(void);
            bool _CheckInput(void);

            bool _CheckButtons(int &ret);
            void _HandleManualKeyPress(Key key);
            void _ClearKeyboardEvent();
            void _ChangeManualKey(int newVal);

            Keyboard *_owner{nullptr};

            string _title;
            string _text;
            string _error;
            string _userInput;

            bool _canChangeLayout{false};
            bool _canAbort{true};
            bool _isOpen{false};
            bool _askForExit{false};
            bool _errorMessage{false};
            bool _userAbort{false};
            bool _isHex{true};
            bool _mustRelease{false};
            bool _useCaps{false};
            bool _useSymbols{false};
            bool _useNintendo{false};
            float _offset{0.f};
            u32 _max{0};
            u8 _symbolsPage{0};
            u8 _nintendoPage{0};
            Layout _layout{HEXADECIMAL};
            Clock _blinkingClock;
            int _cursorPositionInString{0};
            int _cursorPositionOnScreen{0};
            bool _showCursor{true};

            CompareCallback _compare{nullptr};
            ConvertCallback _convert{nullptr};
            OnEventCallback _onKeyboardEvent{nullptr};
            KeyboardEvent _KeyboardEvent{};
            vector<TouchKey> *_keys{nullptr};

            static vector<TouchKey> _DecimalKeys;
            static vector<TouchKey> _HexaDecimalKeys;
            static vector<TouchKey> _QwertyKeys;

            // Custom keyboard stuff
            int _manualKey{0};
            int _prevManualKey{-2};
            bool _manualScrollUpdate{false};
            bool _userSelectedKey{false};
            bool _customKeyboard{false};
            bool _displayScrollbar{false};
            bool _isIconKeyboard{false};
            int _currentPosition{0};
            u32 _scrollbarSize{0};
            u32 _scrollCursorSize{0};
            float _scrollSize{0.f};
            float _scrollPosition{0.f};
            float _scrollPadding{0.f};
            float _scrollJump{0.f};
            float _inertialVelocity{0.f};
            float _scrollStart{0.f};
            float _scrollEnd{0.f};
            IntVector _lastTouch;
            Clock _touchTimer;

            vector<TouchKeyString*> _strKeys;
    };
}

#endif