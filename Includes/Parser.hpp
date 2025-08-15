#ifndef PARSER_HPP
#define PARSER_HPP

/*
All credit for those functions goes to PabloMK7
*/

#pragma once

#include <CTRPluginFramework.hpp>
#include <functional>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#define PATH_LANGUAGE_SETTINGS  "E:/Gen6CTRPluginFramework/Language/Settings.txt"
#define PATH_ENGLISH_TEXT       "E:/Gen6CTRPluginFramework/Language/English.txt"
#define PATH_FRENCH_TEXT        "E:/Gen6CTRPluginFramework/Language/French.txt"
#define PATH_ITALIAN_TEXT       "E:/Gen6CTRPluginFramework/Language/Italian.txt"
#define PATH_SPANISH_TEXT       "E:/Gen6CTRPluginFramework/Language/Spanish.txt"
#define PATH_GERMAN_TEXT        "E:/Gen6CTRPluginFramework/Language/German.txt"
#define PATH_JAPANESE_TEXT      "E:/Gen6CTRPluginFramework/Language/Japanese.txt"
#define GITHUB                  "github.com/biometrix76/Gen6CTRPluginFramework"

namespace CTRPluginFramework {
    class TextFileParser {
    public:
        using TextMap = map<string, vector<string>>;
        using TextMapIter = TextMap::iterator;
        using TextMapConstIter = TextMap::const_iterator;

        TextFileParser();
        bool Parse(string filename);
        const string &Get(const string &key, u32 element = 0);
        TextMapIter begin();
        TextMapIter end();
        TextMapConstIter cbegin();
        TextMapConstIter cend();
        TextMapConstIter erase(TextMapIter first, TextMapIter last);

        ~TextFileParser();
        static bool IsNumerical(const string &str, bool isHex);

    private:
        TextMap dataMap;
        vector<string> vecData;
        const vector<string> &_FindKey(const string &key);
    };

    string &Trim(string &str);
    extern TextFileParser *getLanguage;

    enum f_Language {
        l_None = 0, // If no language was chosen yet
        l_English,
        l_French,
        l_Italian,
        l_Spanish,
        l_German,
        l_Japanese,
        l_Undefined
    };
}

#endif