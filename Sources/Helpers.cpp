#include <CTRPluginFramework.hpp>
#include <functional>
#include <unordered_map>
#include "Helpers.hpp"

namespace CTRPluginFramework {
    GameName currGameName = GameName::None;
    GameSeries currGameSeries = GameSeries::None;

    // Determines a vector of integers based on the current game series
    vector<u32> AutoGameSet(initializer_list<u32> kalosSet, initializer_list<u32> hoennSet) {
        return (currGameSeries == GameSeries::XY) ? vector<u32>(kalosSet) : vector<u32>(hoennSet);
    }

    // Determines a vector of strings based on the current game series
    vector<string> AutoGameSet(initializer_list<string> kalosSet, initializer_list<string> hoennSet) {
        return (currGameSeries == GameSeries::XY) ? vector<string>(kalosSet) : vector<string>(hoennSet);
    }

    // Returns a value based on whether the game ID corresponds to X/Y or Omega Ruby/Alpha Sapphire
    u32 AutoGame(u32 first, u32 second) {
        return (currGameName == GameName::X || currGameName == GameName::OmegaRuby) ? first : second;
    }

    // Returns a value based on whether the game ID corresponds to X/Y or Omega Ruby/Alpha Sapphire
    string AutoGame(string first, string second) {
        return (currGameName == GameName::X || currGameName == GameName::OmegaRuby) ? first : second;
    }

    // Determines the game ID and series based on the provided title ID
    void DetectGame(u32 titleID) {
        // Map of title IDs to game series and game names
        static const unordered_map<u32, pair<GameSeries, GameName>> gameMap = {
            {0x55D00, {GameSeries::XY, GameName::X}}, // Pokémon X
            {0x55E00, {GameSeries::XY, GameName::Y}}, // Pokémon Y
            {0x11C400, {GameSeries::ORAS, GameName::OmegaRuby}}, // Omega Ruby
            {0x11C500, {GameSeries::ORAS, GameName::AlphaSapphire}} // Alpha Sapphire
        };

        // Look up the title ID in the map
        auto it = gameMap.find(titleID);

        if (it != gameMap.end()) {
            currGameSeries = it->second.first;
            currGameName = it->second.second;
        }

        else {
            currGameSeries = GameSeries::None; // Unknown series
            currGameName = GameName::None; // Unknown game
        }
    }

    // Determines if the game version is up-to-date
    bool IsUpdateSupported(u16 version) {
        switch (currGameName) {
            case GameName::X:
                return (version == 5232); // Compatible version for Pokémon X

            case GameName::Y:
                return (version == 5216); // Compatible version for Pokémon Y

            case GameName::OmegaRuby:
            case GameName::AlphaSapphire:
                return (version == 7820); // Compatible version for ORAS

            default:
                return false; // Unsupported game or invalid version
        }
    }

    vector<u32> battleOffset(2);

    // Function to configure battle offsets based on the current game series
    void SetBattleOffset() {
        if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS)
            battleOffset = {AutoGameSet({0x81FB284, 0x81FB624}, {0x81FB58C, 0x81FB92C})};

        else battleOffset.clear(); // Unsupported game series
    }

    bool IfInBattle() {
        static const u32 battlePointer = AutoGameSet(0x81FB170, 0x81FB478);

        // Retrieve the current battle state
        u32 battleState = Process::Read32(battlePointer);

        // Return false immediately if not in a battle state
        if (battleState != 0x40001)
            return false;

        return true;
    }

    // Handle input change for string keyboards
    template<typename T>
    void HandleInputChange(Keyboard &keyboard, KeyboardEvent &event, T &matches, int (*MatchFunction)(T&, const string&), int &outputID) {
        string &input = keyboard.GetInput();
        int matchCount = MatchFunction(matches, input);
        int choice;

        auto handleError = [&](const string &message) {
            keyboard.SetError(message);
        };

        auto handleMatch = [&](int choice) {
            outputID = matches.matchValue[choice] + 1;
            keyboard.Close();
        };

        // Handle character removal
        if (event.type == KeyboardEvent::CharacterRemoved) {
            input.clear();
            handleError("Type something to begin searching");
            return;
        }

        // Handle insufficient input length
        if (input.size() < 3) {
            handleError("You did not type enough to do the search");
            return;
        }

        // Handle no matches
        if (matchCount == 0) {
            handleError("Unfortunately we could not find anything that\nmatches your input.\n\nPlease restart and try again");
            return;
        }

        // Auto-complete if exactly one match
        if (matchCount == 1) {
            handleMatch(0);
            return;
        }

        // Handle multiple matches (10 or fewer)
        if (matchCount <= 10) {
            Keyboard kb(matches.name);
            kb.CanAbort(false);
            kb.DisplayTopScreen = false;
            choice = kb.Open();

            if (choice >= 0) {
                handleMatch(choice);
                return;
            }
        }

        // Handle too many matches
        handleError("Way too many populated results: " + to_string(matchCount) + "\nContinue typing what you are searching for.");
    }

    int speciesID = 0;

    int IterateSpeciesList(Species &output, const string &input) {
        output.name.clear();
        output.matchValue.clear(); // Ensure matchValue is also cleared
        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        int idx = 0;

        for (const string &name : speciesList) {
            string lowerName = name;
            transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            // Check if the lowercased input is a prefix
            if (lowerName.find(lowerInput) == 0) {
                output.name.push_back(name);
                output.matchValue.push_back(idx);
            }

            ++idx;
        }

        return output.matchValue.size();
    }

    void HandleSpeciesInputChange(Keyboard &keyboard, KeyboardEvent &event) {
        Species matches;
        HandleInputChange(keyboard, event, matches, IterateSpeciesList, speciesID);
    }

    void SearchForSpecies(MenuEntry *entry) {
        string output;

        if (KeyboardHandler<string>::Set("Pokemon:", true, 11, output, "", HandleSpeciesInputChange))
            return;
    }

    int abilityName = 0;

    int IterateAbilityList(Ability &output, const string &input) {
        output.name.clear();
        output.matchValue.clear(); // Ensure matchValue is also cleared
        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        int idx = 0;

        for (const string &name : abilityList) {
            string lowerName = name;
            transform(lowerName.begin(), lowerName.end(), lowerName.begin(), ::tolower);

            // Check if the lowercased input is a prefix
            if (lowerName.find(lowerInput) == 0 && idx < AutoGame(188, 191)) {
                output.name.push_back(name);
                output.matchValue.push_back(idx);
            }

            ++idx;
        }

        return output.matchValue.size();
    }

    void HandleAbilityInputChange(Keyboard &keyboard, KeyboardEvent &event) {
        Ability matches;
        HandleInputChange(keyboard, event, matches, IterateAbilityList, abilityName);
    }

    void SearchForAbility(MenuEntry *entry) {
        string output;

        if (KeyboardHandler<string>::Set("Ability:", true, 16, output, "", HandleAbilityInputChange))
            return;
    }

    int heldItemName = 0;

    int IterateItemList(HeldItem &output, const string &input) {
        static const vector<int> ignored = {112, 113, 114, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 425, 426, 621, 806, 807, 808, 809, 810, 811, 812, 813, 814, 815, 816, 817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 836, 837, 838, 839, 847, 858, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870, 871, 872, 873, 874, 875, 876, 877, 884, 885, 886, 887, 888, 889, 890, 891, 892, 893, 894, 895, 896, 897, 898, 899, 900, 901, 902, 926, 927, 928, 929, 930, 931};
        output.name.clear();
        output.matchValue.clear(); // Ensure matchValue is cleared

        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        int idx = 0;

        for (const string &item : heldItemList) {
            // Skip if it's in the ignored list
            if (find(ignored.begin(), ignored.end(), idx) != ignored.end()) {
                ++idx;
                continue;
            }

            string lowerItem = item;
            transform(lowerItem.begin(), lowerItem.end(), lowerItem.begin(), ::tolower);

            // Check if the lowercased input is a prefix
            if (lowerItem.find(lowerInput) == 0 && idx < AutoGameSet(717, 775)) {
                output.name.push_back(item);
                output.matchValue.push_back(idx);
            }

            ++idx;
        }

        return output.matchValue.size();
    }

    void HandleHeldItemInputChange(Keyboard &keyboard, KeyboardEvent &event) {
        HeldItem matches;
        HandleInputChange(keyboard, event, matches, IterateItemList, heldItemName);
    }

    void SearchForItem(MenuEntry *entry) {
        string output;

        if (KeyboardHandler<string>::Set("Item:", true, 18, output, "", HandleHeldItemInputChange))
            return;
    }

    int moveName = 0;

    int IterateMoveList(Moves &output, const string &input) {
        static const vector<int> ignored = {622, 623, 624, 625, 626, 627, 628, 629, 630, 631, 632, 633, 634, 635, 636, 637, 638, 639, 640, 641, 642, 643, 644, 645, 646, 647, 648, 649, 650, 651, 652, 653, 654, 655, 656, 657, 658, 695, 696, 697, 698, 699, 700, 701, 702, 703, 719, 723, 724, 725, 726, 727, 728};
        output.name.clear();
        output.matchValue.clear(); // Ensure matchValue is cleared

        string lowerInput = input;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);
        int idx = 0;

        for (const string &move : movesList) {
            // Skip if it's in the ignored list
            if (find(ignored.begin(), ignored.end(), idx) != ignored.end()) {
                ++idx;
                continue;
            }

            string lowerMove = move;
            transform(lowerMove.begin(), lowerMove.end(), lowerMove.begin(), ::tolower);

            // Check if the lowercased input is a prefix
            if (lowerMove.find(lowerInput) == 0 && idx < AutoGameSet(617, 621)) {
                output.name.push_back(move);
                output.matchValue.push_back(idx);
            }

            ++idx;
        }

        return output.matchValue.size();
    }

    void HandleMoveInputChange(Keyboard &keyboard, KeyboardEvent &event) {
        Moves matches;
        HandleInputChange(keyboard, event, matches, IterateMoveList, moveName);
    }

    void SearchForMove(MenuEntry *entry) {
        string output;

        if (KeyboardHandler<string>::Set("Move:", true, 27, output, "", HandleMoveInputChange))
            return;
    }
}