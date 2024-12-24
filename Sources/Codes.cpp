#include <optional>
#include "Codes.hpp"
#include "PKHeX.hpp"

namespace CTRPluginFramework {
    static int selectedIcon = 0;

    void UpdateIcon(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C79C43, 0x8C81347);
        vector<string> iconName;
        Keyboard keyboard;

        for (const auto &icon : iconList)
            iconName.push_back(icon.description);

        while (true) {
            int choice = keyboard.Setup("Icon:", true, iconName, selectedIcon);

            if (choice == -1)
                return; // Exit on cancel

            if (choice >= 0) {
                Process::Write8(address, iconList[choice].identifier);
                OSD::Notify("Icon: " + iconName[choice]);
                break;
            }
        }
    }

    static u32 profileNices;

    void UpdateNices(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C82B90, 0x8C8B35C);

        if (KeyboardHandler<u32>::Set("Nices:", true, false, 5, profileNices, 0, 0, 99999, nullptr)) {
            Process::Write32(address, profileNices);
            OSD::Notify("Nices: " + to_string(profileNices));
        }
    }

    static u32 profileWishes;

    void UpdateWishes(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C82B94, 0x8C8B360);

        if (KeyboardHandler<u32>::Set("Wishes:", true, false, 5, profileWishes, 0, 0, 99999, nullptr)) {
            Process::Write32(address, profileWishes);
            OSD::Notify("Wishes: " + to_string(profileWishes));
        }
    }

    static string profileMessage;

    void UpdateProfileMessage(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C79CB8, 0x8C813BC);

        if (KeyboardHandler<string>::Set("Message:", true, 16, profileMessage, "", nullptr)) {
            Process::WriteString(address, profileMessage, StringFormat::Utf16);
            OSD::Notify("Message: " + profileMessage);
        }
    }

    void UpdateHistory(MenuEntry *entry) {
        Keyboard keyboard;
        int option;

        if (keyboard.Setup("History:", true, {"Clear"}, option) == 0) {
            if (Process::Write32(AutoGameSet(0x8C84C7C, 0x8C8D448), 0x0) && Process::Write32(AutoGameSet(0x8C84C80, 0x8C8D44C), 0x0))
                OSD::Notify("History cleared!");
        }
    }

    static int selectedActivity, destinationType;
    static u32 activityAmount[2], currentActivityValue[3];

    void UpdateLinks(MenuEntry *entry) {
        static const vector<u32> address = {AutoGameSet({0x8C82ADC, 0x8C82AD4}, {0x8C8B2A8, 0x8C8B2A0})};
        static const vector<string> activityOption = {"Battles", "Trades"};
        Keyboard keyboard;

        while (true) {
            // Prompt user to select an activity (Battles or Trades)
            int activityChoice = keyboard.Setup("Links:", true, activityOption, selectedActivity);

            if (activityChoice == -1)
                break; // Exit if user cancels

            // Read current values for the selected activity
            for (int i = 0; i < 3; ++i)
                currentActivityValue[i] = Process::Read32(address[selectedActivity] + (i * 0x10));

            // Construct the prompt to display destination options and their values
            string destinationPrompt =  "Select destination:\n\n"
                                        "Link: " + to_string(currentActivityValue[0]) + "\n"
                                        "WiFi: " + to_string(currentActivityValue[1]) + "\n"
                                        "IR: " + to_string(currentActivityValue[2]) + "\n\n"
                                        "Total: " + to_string(currentActivityValue[0] + currentActivityValue[1] + currentActivityValue[2]);

            // Prompt user to select a destination (Link, WiFi, or IR)
            int destinationChoice = keyboard.Setup(destinationPrompt, true, {"Link", "WiFi", "IR"}, destinationType);

            if (destinationChoice == -1)
                continue; // Skip to next iteration if user cancels

            // Prompt user to enter a new value for the selected activity and destination
            if (KeyboardHandler<u32>::Set("Amount:", true, false, 6, activityAmount[selectedActivity], 0, 0, 999999, nullptr)) {
                // Write the new value to the appropriate address
                if (Process::Write32(address[selectedActivity] + (destinationType * 0x10), activityAmount[selectedActivity])) {
                    string destinationTypeString = (destinationType == 0) ? "Link" : (destinationType == 1) ? "WiFi" : "IR";
                    OSD::Notify(activityOption[selectedActivity] + " - " + destinationTypeString + ": " + to_string(activityAmount[selectedActivity]));
                }
            }
        }
    }

    static u8 surveyCopiedFrom, destinationSurvey;

    void UpdateMiniSurvey(MenuEntry *entry) {
        static const vector<u32> address = {AutoGameSet({0x8C7D228, 0x8C7D22E}, {0x8C8545C, 0x8C85462})};

        // Prompt the user to select the survey to copy from
        if (KeyboardHandler<u8>::Set("Duplicate (Source):", true, false, 1, surveyCopiedFrom, 1, 1, 6, nullptr)) {
            // Prompt the user to select the destination survey
            if (KeyboardHandler<u8>::Set("To (Destination):", true, false, 1, destinationSurvey, 1, 1, 6, nullptr)) {
                // Process each survey address
                for (const auto &address : address) {
                    if (destinationSurvey != surveyCopiedFrom) {
                        u8 value = Process::Read8(address + (surveyCopiedFrom - 1)); // Read value from source
                        Process::Write8(address + (destinationSurvey - 1), value); // Write value to destination
                    }

                    else return; // Exit if source and destination are the same
                }

                OSD::Notify("Mini Survey(s) duplicated!");
            }
        }
    }

    static int selectedGreetOption;
    static u32 greetValue[2];

    void UpdateGreets(MenuEntry *entry) {
        static const vector<u32> address = {AutoGameSet({0x8C82CA0, 0x8C82B98}, {0x8C8B46C, 0x8C8B364})};
        static const vector<string> options = {"Today", "Total"};
        Keyboard keyboard;

        while (true) {
            // Prompt the user to select "Today" or "Total"
            int greetingChoice = keyboard.Setup("Greets:", true, options, selectedGreetOption);

            if (greetingChoice == -1)
                break; // Exit if the user cancels

            // Determine maximum value based on the selected option
            u32 maxValue = (selectedGreetOption == 0 ? 9999 : 999999999);

            // Prompt the user to enter a new value
            if (KeyboardHandler<u32>::Set(options[selectedGreetOption] + ":", true, false, (selectedGreetOption == 0 ? 4 : 9), greetValue[selectedGreetOption], 0, 0, maxValue, nullptr)) {
                if (selectedGreetOption == 0)
                    Process::Write16(address[selectedGreetOption], greetValue[selectedGreetOption]);

                else Process::Write32(address[selectedGreetOption], greetValue[selectedGreetOption]);

                OSD::Notify(options[selectedGreetOption] + ": " + to_string(greetValue[selectedGreetOption]) + " greeted!");
            }
        }
    }

    static int selectedFunction;

    void PlayerSearchSystem(MenuEntry *entry) {
        static const vector<string> options = {"Icons", "Nices", "Wishes", "Message", "History", "Links", "Mini Survey", "Greets"};

        // Map function indices to corresponding functions
        using FunctionPointer = void (*)(MenuEntry *);
        static const vector<FunctionPointer> functions = {
            UpdateIcon,
            UpdateNices,
            UpdateWishes,
            UpdateProfileMessage,
            UpdateHistory,
            UpdateLinks,
            UpdateMiniSurvey,
            UpdateGreets
        };

        Keyboard keyboard;
        bool inSubmenu = false; // Tracks if the user is inside a submenu

        while (true) {
            int selection;

            if (!inSubmenu)
                selection = keyboard.Setup(Color::Orange << "[Profile]", true, options, selectedFunction); // Show the main menu

            else selection = -1;

            if (selection == -1) {
                if (inSubmenu)
                    inSubmenu = false; // Return to the main menu

                else break;
            }

            else {
                inSubmenu = true; // Entering a submenu

                // Call the corresponding function
                if (selection >= 0 && selection < functions.size())
                    functions[selection](entry);
            }
        }
    }

    // Vector holding pointers to battle offsets
    static vector<u32> pointer(2);
    // Flag indicating whether to revert changes
    static bool isRevert = false;
    static int validPosition; // How many valid positions

    vector<string> GetBattleParty(vector<string> party) {
        // Initialize validPosition counter
        validPosition = 0;

        // Process each member
        for (int index = 0; index < party.size(); ++index) {
            u32 offset = battleOffset[0] + (index * sizeof(u32));
            u32 location = Process::Read32(offset);

            if (location != 0) {
                int species = Process::Read16(location + 0xC);

                if (species > 0 && species <= 721)
                    party[index] = speciesList[species - 1], ++validPosition;

                else party[index] = "[Empty]";
            }

            else  party[index] = "[Empty]";
        }

        return party;
    }

    static u8 position;

    void PartyPosition(MenuEntry *entry) {
        vector<string> party(6); // Initialize vector to hold party

        // Lambda to generate prompt text for party positions
        auto generatePromptText = [&](const vector<string> &party) -> string {
            string promptText = entry->Name() <<
                Color::White << "\n\n1: " + party[0] <<
                Color::White << "\n2: " + party[1] <<
                Color::White << "\n3: " + party[2] <<
                Color::White << "\n4: " + party[3] <<
                Color::White << "\n5: " + party[4] <<
                Color::White << "\n6: " + party[5] << "\n";
            return promptText;
        };

        // Lambda to update pointer offsets based on the selected position
        auto updatePointerOffsets = [&](u8 position) {
            for (int index = 0; index < pointer.size(); ++index)
                pointer[index] = battleOffset[index] + ((position - 1) * sizeof(u32));
        };

        // Check if the player is in battle
        if (IfInBattle()) {
            party = GetBattleParty(party); // Get battle party
            // Generate the prompt text for user selection
            string promptText = generatePromptText(party);

            // Handle keyboard input and update the selected position if valid
            if (KeyboardHandler<u8>::Set(promptText, true, false, 1, position, 0, 1, validPosition, Callback<u8>)) {
                updatePointerOffsets(position); // Adjust pointer offsets based on selection

                // Update the menu entry with the newly set position
                entry->Name() = "Position: " << Color::Gray << Utils::ToString(position, 0);
                OSD::Notify("Selected in-battle position: " + party[position - 1]);
            }
        }
    }

    static int statusFlag, selectedCondition;

    void StatusCondition(MenuEntry *entry) {
        static const vector<pair<string, int>> statusConditions = {{"Paralyzed", 0x20}, {"Asleep", 0x24}, {"Frozen", 0x28}, {"Burned", 0x2C}, {"Poisoned", 0x30}};
        static const vector<string> statusChoice = {"None", "Affected"};
        vector<string> options;
        Keyboard keyboard;

        // Fill options with the names of each condition
        for (const auto &condition : statusConditions)
            options.push_back(condition.first);

        // Check if in battle and pointers are valid
        if (IfInBattle() && all_of(pointer.begin(), pointer.end(), [](int ptr) {return ptr != 0;})) {
            int userSelection = keyboard.Setup(entry->Name() + ":", true, statusChoice, statusFlag);

            if (userSelection != -1) {
                if (statusFlag == 0) {
                    // Clear any existing conditions
                    for (int ptr : pointer)
                        for (const auto &condition : statusConditions)
                            Process::Write8(ptr, condition.second, 0);

                    OSD::Notify("Status conditions removed");
                }

                else if (statusFlag == 1) {
                    // Apply the chosen condition
                    int conditionSelection = keyboard.Setup(entry->Name() + ":", true, options, selectedCondition);

                    if (conditionSelection != -1) {
                        for (int ptr : pointer) {
                            for (const auto &condition : statusConditions)
                                Process::Write8(ptr, condition.second, 0);

                            Process::Write8(ptr, statusConditions[selectedCondition].second, 1);
                        }

                        OSD::Notify("Applied status condition: " + statusConditions[selectedCondition].first);
                    }
                }
            }
        }
    }

    int statChoice, mainStat, statBoost;
    // Array to store main stat values for 5 different stats
    static u16 mainStatVal[5];
    // Array to store boosted stat values for 7 different stats
    static u8 statBoostVal[7];

    void Stats(MenuEntry *entry) {
        static const vector<string> statOptions = {"Base", "Boosts"};
        // Options for base stats
        static const vector<string> mainStats = {"Attack", "Defense", "Sp. Atk", "Sp. Def", "Speed"};
        // Options for boost stats: initialize with mainStats
        static const vector<string> statBoosts = []() {
            vector<string> combinedStats = mainStats;
            combinedStats.push_back("Accuracy");
            combinedStats.push_back("Evasiveness");
            return combinedStats;
        }();

        // Get user inputs
        u16 value16;
        u8 value8;

        // Check if in battle and pointers are valid and not = 0
        if (IfInBattle() && pointer[0] != 0 && pointer[1] != 0) {
            Keyboard keyboard;

            // While keyboard is not aborted()
            while (keyboard.Setup(entry->Name() + ":", true, statOptions, statChoice) != -1) {
                if (statChoice == 0) {
                    // While keyboard is not aborted()
                    while (keyboard.Setup(entry->Name() + ":", true, mainStats, mainStat) != -1) {
                        // Get and validate base stat value
                        if (KeyboardHandler<u16>::Set(mainStats[mainStat] + ":", true, false, 3, value16, 0, 1, 999, Callback<u16>)) {
                            mainStatVal[mainStat] = value16; // Store base stat value
                            entry->SetGameFunc(UpdateStats);
                            OSD::Notify("Updated " << mainStats[mainStat] << " to: " << to_string(value16));
                            break; // Exit loop after successful input
                        }
                    }
                }

                else if (statChoice == 1) {
                    // While keyboard is not aborted()
                    while (keyboard.Setup(entry->Name() + ":", true, statBoosts, statBoost) != -1) {
                        // Get and validate boost stat value
                        if (KeyboardHandler<u8>::Set(statBoosts[statBoost] + ":", true, false, 1, value8, 0, 0, 6, Callback<u8>)) {
                            statBoostVal[statBoost] = value8 + 6; // Adjust and store boost stat value
                            entry->SetGameFunc(UpdateStats);
                            OSD::Notify("Boosted " << statBoosts[statBoost] << " by: x" << to_string(value8));
                            break; // Exit loop after successful input
                        }
                    }
                }
            }
        }
    }

    void UpdateStats(MenuEntry *entry) {
        // Check if in battle and pointers are valid and not = 0
        if (IfInBattle() && pointer[0] != 0 && pointer[1] != 0) {
            // Loop through all pointers
            for (int i = 0; i < pointer.size(); i++) {
                for (int k = 0; k < sizeof(mainStatVal) / sizeof(mainStatVal[0]); k++)
                    if (mainStatVal[k] != 0)
                        Process::Write16(pointer[i], 0xF6 + (k * 2), mainStatVal[k]);

                for (int l = 0; l < sizeof(statBoostVal) / sizeof(statBoostVal[0]); l++)
                    if (statBoostVal[l] != 0)
                        Process::Write8(pointer[i], 0x104 + (l * 1), statBoostVal[l]);
            }
        }
    }

    static u16 health;
    static u8 mana;

    void HealthAndMana(MenuEntry *entry) {
        static const vector<string> options = {"Health", "Mana"};
        static int choice;

        // Get user inputs
        u16 tempHealth;
        u8 tempMana;

        // Check if the player is in battle and if the pointers are valid and not = 0
        if (IfInBattle() && pointer[0] != 0 && pointer[1] != 0) {
            Keyboard keyboard;

            // While keyboard is not aborted()
            while (keyboard.Setup(entry->Name() + ":", true, options, choice) != -1) {
                if (choice == 0) {
                    // Prompt the user for Health input and update the health variable if valid
                    if (KeyboardHandler<u16>::Set("Health:", true, false, 3, tempHealth, 0, 1, 999, Callback<u16>)) {
                        health = tempHealth;
                        entry->SetGameFunc(UpdateHealthAndMana);
                        OSD::Notify("Health set to: " << to_string(tempHealth));
                        break; // Exit the loop after
                    }
                }

                else if (choice == 1) {
                    // Prompt the user for Mana input and update the mana variable if valid
                    if (KeyboardHandler<u8>::Set("Mana:", true, false, 2, tempMana, 0, 1, 99, Callback<u8>)) {
                        mana = tempMana;
                        entry->SetGameFunc(UpdateHealthAndMana);
                        OSD::Notify("PP set to: " << to_string(tempMana));
                        break; // Exit the loop after
                    }
                }
            }
        }
    }

    void UpdateHealthAndMana(MenuEntry *entry) {
        static const int healthPosition = 2, manaPosition = 4; // Number of positions to update for Health and Mana

        // Ensure the player is in battle and pointers are valid and not = 0
        if (IfInBattle() && pointer[0] != 0 && pointer[1] != 0) {
            // Iterate through each pointer
            for (const auto &ptr : pointer) {
                if (health != 0) {
                    for (int pos = 0; pos < healthPosition; ++pos) {
                        const u32 healthOffset = 0x10 - (pos * 2);
                        Process::Write16(ptr, healthOffset, health);
                    }
                }

                if (mana != 0) {
                    for (int pos = 0; pos < manaPosition; ++pos) {
                        const u32 manaOffset = 0x11E + (pos * 0xE);
                        const u16 manaValue = (mana << 8) | mana;
                        Process::Write16(ptr, manaOffset, manaValue);
                    }
                }
            }
        }
    }

    static u16 item;

    void HeldItem(MenuEntry *entry) {
        // Ensure the player is in a battle and pointers are valid
        if (!IfInBattle() || pointer[0] == 0 || pointer[1] == 0)
            return;

        // Prompt the user to search for the desired held item
        SearchForItem(entry);

        // If a valid item is selected, apply it to all relevant pointers
        if (heldItemName > 0) {
            for (const auto &ptr : pointer)
                Process::Write16(ptr, 0x12, heldItemName);

            OSD::Notify("Item changed: " + string(heldItemList[heldItemName - 1]));
        }
    }

    static u16 move;
    static int moveSlot; // 4 slots available, variable used to determine which slot to write to

    void Moves(MenuEntry *entry) {
        const vector<string> options = {"Move 1", "Move 2", "Move 3", "Move 4"};
        Keyboard keyboard;

        // Ensure the player is in battle and pointers are valid and not = 0
        if (IfInBattle() && pointer[0] != 0 && pointer[1] != 0) {
            while (true) {
                // Allow user to select a move slot
                int selectedSlot = keyboard.Setup(entry->Name() + ":", true, options, moveSlot);

                if (selectedSlot == -1)
                    return; // Exit if user cancels selection

                // Allow user to input the move
                SearchForMove(entry);
                move = moveName;

                // Check if the selected move is valid
                if (move > 0) {
                    // Update all relevant pointers with the selected move
                    for (const auto &ptr : pointer) {
                        const u32 offset = 0x11C + (moveSlot * 0xE);
                        Process::Write16(ptr, offset, move);
                    }

                    // Notify user of the move change
                    OSD::Notify("Move " + to_string(moveSlot + 1) + ": " + movesList[move - 1]);
                    break;
                }
            }
        }
    }

    // Default multiplier is set to 1, which is the game's default
    static u8 multiplier = 1;

    // Handle input changes for experience multiplier
    void ExpMultiplier(MenuEntry *entry) {
        // Check if in battle first
        if (IfInBattle()) {
            // Prompt user to enter a new multiplier value
            if (KeyboardHandler<u8>::Set(entry->Name() + ":", true, false, 3, multiplier, 0, 1, 100, Callback<u8>)) {
                entry->SetGameFunc(UpdateExpMultiplier);
                OSD::Notify("Exp: x" + to_string(multiplier));
            }
        }
    }

    void UpdateExpMultiplier(MenuEntry *entry) {
        static const vector<u32> address = AutoGameSet({0x53EDA0, 0x175FB0}, {0x579860, 0x16B81C});
        vector<u32> instruction = {0xE1D002B2, 0xE92D4002, 0xE3A01000 + multiplier, 0xE0000190, 0xE8BD8002};

        // Create MemoryManager instances
        static vector<MemoryManager> managers;
        static bool initialized = false;

        // Initialize MemoryManager instances if not already done
        if (!initialized) {
            for (const auto &addr : address) {
                for (int i = 0; i < instruction.size(); ++i)
                    managers.emplace_back(addr + (i * 0x4)); // Increment address for each instruction
            }

            initialized = true;
        }

        // Update multiplier in the instruction
        instruction[2] = 0xE3A01000 + multiplier;

        // Check if in battle and entry is activated
        if (IfInBattle() && entry->IsActivated()) {
            bool success = true;
            size_t index = 0;

            // Write new instruction to the addresses
            for (const auto &instruction : instruction) {
                if (index >= managers.size() || !managers[index].Write(instruction)) {
                    success = false;
                    break; // Exit loop if writing fails
                }

                ++index;
            }

            // Write to the second set of addresses if applicable
            if (success && managers.size() > address.size()) {
                if (index < managers.size() && !managers[index].Write(AutoGameSet(0xEB0F237A, 0xEB10380F)))
                    return; // Exit if writing fails
            }
        }

        // Restore original values if the entry is not active
        else if (!entry->IsActivated()) {
            for (auto &manager : managers) {
                if (manager.HasOriginalValue())
                    manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
            }

            managers.clear(); // Clear managers to reset initialization
            initialized = false;
        }
    }

    // Function to check if the given pointer is valid for a Pokemon
    bool IsValid(u32 pointer, PK6 *pkmn) {
        return GetPokemon(pointer, pkmn);
    }

    // Static variables to manage Pokemon info display
    static bool isInfoViewOn = false; // Flag to indicate if the Pokemon info view is active
    static int  infoViewState = 0, // State of the Pokemon info view (0: Enable, 1: Disable)
                screenDisplay = 0; // Current information screen

    // Callback function to display Pokemon information
    bool ViewInfoCallback(const Screen &screen) {
        // Static data for Pokemon and UI settings
        static PK6 *pokemon = new PK6; // Pointer to Pokemon data
        static const u32 address = AutoGameSet(0x81FF744, 0x81FEEC8); // Base address for Pokemon data
        static u32 currentOffset = address; // Current offset in the Pokemon data
        static const vector<string> statNames = {"HP", "Atk", "Def", "SpD", "SpA", "Spd"}; // Names of stats

        // Early exit if the current screen is not the top screen
        if (!screen.IsTop)
            return false;

        // Only proceed if in battle and info view is active
        if (!IfInBattle() || !isInfoViewOn)
            return false;

        // Info for navigating the position of Pokemon infos
        const u32 stepSize = 0x1E4; // Step size for navigating through Pokemon data
        const u32 maxOffset = address + (stepSize * 0x5); // Maximum offset in Pokemon data

        // Move to the next screen if the R key is pressed and within bounds
        if (Controller::IsKeyPressed(Key::R) && (currentOffset + stepSize) <= maxOffset)
            currentOffset += stepSize;

        // Move to the previous screen if the L key is pressed and within bounds
        if (Controller::IsKeyPressed(Key::L) && (currentOffset - stepSize) >= address)
            currentOffset -= stepSize;

        // Fetch the current Pokemon data
        if (!IsValid(currentOffset, pokemon))
            return false;

        // Get UI settings for colors and positions, for screen drawing
        const FwkSettings &settings = FwkSettings::Get();
        Color textColor = Color::Gainsboro; // Text color
        Color headerColor = Color::DarkGrey; // Header color
        int xPos = 5, yPos = 5; // X and Y positions for drawing text
        int lineHeight = 12; // Line height for text spacing

        // Display basic Pokemon information for the first and default screen
        if (screenDisplay == 0) {
            const CTRPluginFramework::Screen &screen = CTRPluginFramework::OSD::GetTopScreen();
            u32 slotIndex = (currentOffset - address) / stepSize; // Calculate Pokemon slot index
            screen.DrawSysfont(headerColor << "[Slot: " << Utils::ToString(slotIndex + 1, 0) << "]", xPos, yPos, textColor);
            yPos += lineHeight;
            screen.DrawSysfont("Species: " << Color(0xF2, 0xCE, 0x70) << speciesList[pokemon->species - 1], xPos, yPos, textColor);
            yPos += lineHeight;
            screen.DrawSysfont("Nature: " << textColor << natureList[pokemon->nature], xPos, yPos, textColor);
            yPos += lineHeight;
            screen.DrawSysfont("Item: " << (pokemon->heldItem == 0 ? Color::Gray : textColor) << (pokemon->heldItem == 0 ? "None" : heldItemList[pokemon->heldItem - 1]), xPos, yPos, textColor);
            yPos += lineHeight;
            screen.DrawSysfont("Ability: " << textColor << abilityList[pokemon->ability - 1], xPos, yPos, textColor);
        }

        else if (screenDisplay == 1) {
            screen.DrawSysfont(headerColor << "[Moves]", xPos, yPos, textColor);
            yPos += lineHeight;

            // Loop through and display each move
            for (int i = 0; i < 4; i++) {
                string moveDisplay = pokemon->move[i] > 0 ? movesList[pokemon->move[i] - 1] : "None";
                screen.DrawSysfont(to_string(i + 1) + ": " << (pokemon->move[i] > 0 ? textColor : Color::Gray) << moveDisplay, xPos, yPos, textColor);
                yPos += lineHeight;
            }
        }

        // Display Pokemon IVs
        else if (screenDisplay == 2) {
            screen.DrawSysfont(headerColor << "[IV]", xPos, yPos, textColor);
            yPos += lineHeight;

            // Loop through and display each IV value
            for (int i = 0; i < statNames.size(); i++) {
                u8 ivValue = (u8)(pokemon->iv32 >> (5 * i)) & 0x1F; // Extract IV value
                screen.DrawSysfont(statNames[i] + ": " + to_string(ivValue), xPos, yPos, textColor);
                yPos += lineHeight;
            }
        }

        // Display Pokemon EVs
        else if (screenDisplay == 3) {
            screen.DrawSysfont(headerColor << "[EV]", xPos, yPos, textColor);
            yPos += lineHeight;

            // Loop through and display each EV value
            for (int i = 0; i < statNames.size(); i++) {
                screen.DrawSysfont(statNames[i] + ": " + to_string(pokemon->EV[i]), xPos, yPos, textColor);
                yPos += lineHeight;
            }
        }

        return true; // Successful execution of the callback function
    }

    void TogglePokemonInfo(MenuEntry *entry) {
        // Check if currently in battle and if entry is activated or not
        if (IfInBattle() && entry->IsActivated()) {
            // Check if the first hotkey is pressed
            if (entry->Hotkeys[0].IsPressed())
                screenDisplay = (screenDisplay + 1) % 4; // Move to the next info screen, loop back if needed

            // Run the callback function to display Pokemon info
            OSD::Run(ViewInfoCallback);
        }

        // Stop displaying Pokemon info if not in battle or if entry is not activated anymore
        else OSD::Stop(ViewInfoCallback);
    }

    void ViewPokemonInfo(MenuEntry *entry) {
        static const vector<string> options = {"Enable", "Disable"}; // Options for toggling Pokemon info view
        Keyboard keyboard;

        // Check if currently in battle
        if (IfInBattle()) {
            if (keyboard.Setup(entry->Name() + ":", false, options, infoViewState) != -1) {
                // Update the flag to enable or disable Pokemon info view based on selection
                isInfoViewOn = (infoViewState == 0);
                // Set the function to toggle Pokemon info
                entry->SetGameFunc(TogglePokemonInfo);
                OSD::Notify("Info view: " + string(options[infoViewState]));
            }
        }
    }

    static u8 data8;

    void AllowMultipleMegas(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C79D86, 0x8C79D86);
        static const u32 pointer = 0x8000178;

        if (IfInBattle()) {
            if (Nibble::Read8(address, data8, true) && data8 != 1) {
                if (Nibble::Read8(address, data8, true))
                    Nibble::Write8(address, 1, true);
            }

            Process::Write8(pointer, 0xEDD, 1);
        }
    }

    void NoWildPokemon(MenuEntry *entry) {
        static const u32 address = AutoGameSet<u32>(0x436AC8, AutoGame(0x4640EC, 0x4640E4));
        static u32 original;
        static bool saved = false;

        if (!Process::Write32(address, 0xE12FFF1E, original, entry, saved))
            return;
    }

    bool IsObtained(int value) {
        u32 obtainedOffset = Process::Read32(AutoGameSet(0x8CC8D4C, 0x8CE0CB8));

        if (obtainedOffset < 0x8000000 || obtainedOffset >= 0x8DF0000)
            return false;

        // Check if all Pokémon are obtained
        for (int i = 0; i < 721; ++i) {
            u8 byte = Process::Read8(obtainedOffset + 0xC + (i / 8));

            if ((byte & (1 << (i % 8))) == 0)
                break;

            if (i == 720)
                return false; // All Pokémon obtained
        }

        // Check the specific Pokémon's obtained status
        u8 byte = Process::Read8(obtainedOffset + 0xC + ((value - 1) / 8));
        return (byte & (1 << ((value - 1) % 8))) != 0;
    }

    unsigned short last = 0xFFFF, poke[721];

    void UpdateWildSpawner(int spawn, int form, int level, bool updateRadar) {
        static const u32 pointer = AutoGameSet(0x8CEC564, 0x8D06468);
        static const int tableLength = AutoGameSet(0x178, 0xF4);

        // Read the base offset
        u32 baseOffset = Process::Read32(pointer);

        if (baseOffset < 0x8000000 || baseOffset >= 0x8DF0000)
            return; // Invalid pointer, exit early

        // Validate the ZO file encounter data
        u32 encounterDataOffset = baseOffset + Process::Read32(baseOffset + 0x10);

        if (Process::Read32(baseOffset + 0x10) == Process::Read32(baseOffset + 0x14)) {
            last = 0xFFFF; // Reset last if no valid encounter data
            return;
        }

        // Update last Pokemon ID and calculate encounter offset
        last = Process::Read16(pointer + 4);
        u32 encounterOffset = encounterDataOffset + AutoGameSet(0x10, 0xE);

        // Handle DexNav table updates for ORAS
        if (currGameSeries == GameSeries::ORAS && updateRadar) {
            u32 dexNavOffset = 0x16B3DF40 + Process::Read32(0x16B3DF40 + 4 + Process::Read16(pointer + 4) * 4) + AutoGameSet(0x10, 0xE);
            Process::CopyMemory((void *)dexNavOffset, (void *)encounterOffset, 0xF4);
        }

        // Iterate over the encounter table to update data
        for (int index = 0; index < tableLength; index += 4) {
            u32 entryOffset = encounterOffset + index;

            // Skip if the encounter is already valid
            if (Process::Read8(entryOffset + 2) == 1)
                continue;

            // Update encounter details
            Process::Write8(entryOffset + 2, level);
            Process::Write8(entryOffset + 3, 1); // Mark as valid encounter

            // Associate the species with the modified Pokemon
            poke[spawn - 1] = spawn;

            // Check and update species and form
            u16 newSpecies = spawn + (0x800 * form);

            if (Process::Read16(entryOffset) != newSpecies)
                Process::Write16(entryOffset, newSpecies);
        }

        // Update DexNav table again if required
        if (updateRadar && currGameSeries == GameSeries::ORAS) {
            u32 dexNavBase = 0x16B3DF40 + Process::Read32(0x16B3DF40 + 4 + Process::Read16(pointer + 4) * 4) + AutoGameSet(0x10, 0xE);

            for (int j = 0; j < tableLength; j += 4) {
                u32 dexNavEntry = dexNavBase + j;
                u16 existingID = Process::Read16(dexNavEntry);

                if (existingID != 0)
                    Process::Write16(dexNavEntry, poke[existingID - 1]);
            }
        }
    }

    static int pokemon = 0, form = 0;
    static u8 level = 1;

    void WildSpawner(MenuEntry *entry) {
        Keyboard keyboard;

        // Search for a Pokémon species
        SearchForSpecies(entry);
        pokemon = speciesID;

        if (pokemon <= 0) {
            OSD::Notify("Invalid Pokémon selected.");
            return; // Exit if no valid Pokémon is found
        }

        // Prompt user to select the Pokémon's form
        if (keyboard.Setup("Select Form", true, formList(pokemon), form) == -1)
            return; // Exit if the user cancels the form selection

        // Prompt user to input the Pokémon's level
        if (!KeyboardHandler<u8>::Set("Enter Level", true, false, 3, level, 0, 1, 100, Callback<u8>))
            return; // Exit if the level input is invalid or canceled

        // Update the wild spawner configuration
        UpdateWildSpawner(pokemon, form, level, currGameSeries == GameSeries::ORAS);
        OSD::Notify("Spawning: " + string(speciesList[pokemon - 1]) + " (Form: " + to_string(form + 1) + ", Level: " + to_string(level) + ")");
    }

    void AlwaysShiny(MenuEntry *entry) {
        static MemoryManager manager(AutoGameSet(0x14F6A4, 0x14ECA4));

        if (entry->IsActivated()) {
            // Write the new instruction
            if (!manager.Write(0xEA00001C))
                return; // Exit if writing fails
        }

        // When deactivated, restore the original value
        else if (!entry->IsActivated()) {
            if (manager.HasOriginalValue())
                manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
        }
    }

    void CaptureRate(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8073334, 0x80737A4);

        // Ensure the player is in battle
        if (!IfInBattle())
            return;

        // Retrieve the current value at the address
        u32 currentValue = Sudo::Read32(address);

        if (entry->IsActivated()) {
            if (currentValue == 0xA000004 && !Sudo::Write32(address, 0xEA000004))
                return; // Exit if the write operation fails
        }

        else {
            if (currentValue == 0xEA000004 && !Sudo::Write32(address, 0xA000004))
                return; // Exit if the write operation fails
        }
    }

    void CatchTrainerPokemon(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8075474, 0x8075858);
        // Vectors to store expected values for validation.
        vector<u32> check(3);

        // Check if the player is in battle.
        if (IfInBattle()) {
            // If the entry is activated, set and check the expected values.
            if (entry->IsActivated()) {
                check = {0xE5D00000, 0xE3500000, 0xA00000E};

                if (Sudo::Read32(address, 3) == check) {
                    if (!Sudo::Write32(address, {0xE3A0B000, 0xE5C0B000, 0xEA00000E}))
                        return;
                }
            }

            // If the entry is not activated, revert to original values.
            else {
                check = {0xE3A0B000, 0xE5C0B000, 0xEA00000E};

                if (Sudo::Read32(address, 3) == check) {
                    if (!Sudo::Write32(address, {0xE5D00000, 0xE3500000, 0xA00000E}))
                        return;
                }
            }
        }
    }

    void FastWalkRun(MenuEntry *entry) {
        static const vector<u32> address = AutoGameSet({0x8092DE4, 0x8092F34}, {0x80845E8, 0x808475C});

        // Function to update memory with validation.
        auto updateMemory = [](bool isActivated, const vector<u32> &addr, const vector<vector<u32>> &current, const vector<vector<u32>> &target) {
            if (Sudo::Read32(addr[0], 2) == current[0] && Sudo::Read32(addr[1], 2) == current[1])
                return Sudo::Write32(addr[0], target[0]) && Sudo::Write32(addr[1], target[1]);

            return false;
        };

        if (entry->IsActivated())
            updateMemory(true, address, {{0x13A05006, 0x3A05004}, {0x13A0500A, 0x3A05007}}, {{0x13A05003, 0x3A05002}, {0x13A05005, 0x3A05003}});

        else updateMemory(false, address, {{0x13A05003, 0x3A05002}, {0x13A05005, 0x3A05003}}, {{0x13A05006, 0x3A05004}, {0x13A0500A, 0x3A05007}});
    }

    void WalkThroughWalls(MenuEntry *entry) {
        static const vector<u32> address = {
            AutoGameSet(
                {0x53ED50, AutoGame(0x80B5820, 0x80B5824), AutoGame(0x80B3A1C, 0x80B3A20)},
                {0x579820, 0x80BB414, 0x80B87F8}
        )};

        static vector<u32> original(4);
        static bool saved = false;

        auto updateMemory = [](bool isActivated, const vector<u32> &addr, u32 check1, u32 check2, u32 target1, u32 target2) {
            if (Sudo::Read32(addr[1]) == check1 && Sudo::Read32(addr[2]) == check2)
                return Sudo::Write32(addr[1], target1) && Sudo::Write32(addr[2], target2);

            return false;
        };

        if (Process::Write32(address[0], {0xE3A01000, 0xE12FFF1E, 0xE3A06000, 0xE12FFF1E}, original, entry, saved)) {
            if (entry->IsActivated()) {
                updateMemory(
                    true,
                    address,
                    0xE1A01000,
                    0xE1A06000,
                    AutoGameSet<u32>(AutoGame(0xEBF8914A, 0xEBF89149), 0xEBF83D01),
                    AutoGameSet<u32>(AutoGame(0xEBF898CD, 0xEBF898CC), 0xEBF8480A)
                );
            }

            else {
                updateMemory(
                    false,
                    address,
                    AutoGameSet<u32>(AutoGame(0xEBF8914A, 0xEBF89149), 0xEBF83D01),
                    AutoGameSet<u32>(AutoGame(0xEBF898CD, 0xEBF898CC), 0xEBF8480A),
                    0xE1A01000,
                    0xE1A06000
                );
            }
        }
    }

    void StayInAction(MenuEntry *entry) {
        static MemoryManager manager(AutoGameSet<u32>(0x3B9C30, 0x3D5EC8));

        if (entry->IsActivated()) {
            if (!manager.Write(0xE3A00000))
                return; // Exit if writing fails
        }

        // When deactivated, restore the original value
        else if (!entry->IsActivated()) {
            if (manager.HasOriginalValue())
                manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
        }
    }

    void UnlockFullFlyMap(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C7A81C, 0x8C81F24);
        static vector<u8> locationFlags;
        int unchangedCount = 0; // Tracks how many flags are already correct

        if (currGameSeries == GameSeries::XY)
             locationFlags = {0xF7, 0xFF, 0xF};

        else locationFlags = {0xCA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF3, 0xFB, 0x81};

        for (size_t i = 0; i < locationFlags.size(); ++i) {
            u8 currentFlag;

            if (Process::Read8(address + i, currentFlag) && currentFlag != locationFlags[i])
                // Update flag if it doesn't match the expected value
                Process::Write8(address + i, locationFlags[i]);

            else ++unchangedCount; // Increment if the flag is already correct
        }

        // If all flags are already correct, display a warning message
        if (unchangedCount == locationFlags.size())
            OSD::Notify("Already unlocked full fly map!");
    }

    void RenameAnyPokemon(MenuEntry *entry) {
        static MemoryManager manager(AutoGameSet<u32>(0x4B1680, AutoGame(0x4EA990, 0x4EA998)));

        if (entry->IsActivated()) {
            if (!manager.Write(0xE3A00001))
                return; // Exit if writing fails
        }

        // When deactivated, restore the original value
        else if (!entry->IsActivated()) {
            if (manager.HasOriginalValue())
                manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
        }
    }

    void LearnAnyTeachable(MenuEntry *entry) {
        static MemoryManager manager(AutoGameSet<u32>(0x4A0540, AutoGame(0x4D051C, 0x4D0514)));

        if (entry->IsActivated()) {
            if (!manager.Write(0xE3A00001))
                return; // Exit if writing fails
        }

        // When deactivated, restore the original value
        else if (!entry->IsActivated()) {
            if (manager.HasOriginalValue())
                manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
        }
    }

    void InstantEgg(MenuEntry *entry) {
        static const u32 address = AutoGameSet<u32>(0x438C50, AutoGame(0x4658A4, 0x46589C));
        static const vector<u32> instruction = {0xE3A01001, 0xE5C011E8, 0xEA00209B};

        // Create MemoryManager instances for each address
        static vector<MemoryManager> managers;
        static bool initialized = false;

        if (!initialized) {
            for (int i = 0; i < instruction.size(); ++i)
                managers.emplace_back(address + (i * 0x4)); // Increment address for each instruction

            initialized = true;
        }

        if (entry->IsActivated()) {
            bool success = true;
            size_t index = 0;

            // Write new instruction to the addresses
            for (const auto &instruction : instruction) {
                if (index >= managers.size() || !managers[index].Write(instruction)) {
                    success = false;
                    break; // Exit loop if writing fails
                }

                ++index;
            }

            if (!success)
                return; // Exit if writing fails
        }

        // When deactivated, restore the original values
        else if (!entry->IsActivated()) {
            for (auto &manager : managers) {
                if (manager.HasOriginalValue())
                    manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
            }

            managers.clear(); // Clear managers to reset initialization
            initialized = false;
        }
    }

    void InstantEggHatch(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x4A22F4, 0x4D2278);
        static u32 original;
        static bool saved = false;

        if (!Process::Write32(address, 0xE3A00000, original, entry, saved))
            return;
    }

    void ViewValuesInSummary(MenuEntry *entry) {
        static const vector<u32> address = {
            AutoGameSet(
                {0x153184, 0x153230, 0x4A23E4, 0x4A2430, 0x4A25E4, 0x4A2630, 0x4A2398},
                {0x1531AC, 0x15328C, AutoGame(0x4D2370, 0x4D2368), AutoGame(0x4D23BC, 0x4D23B4), AutoGame(0x4D2534, 0x4D252C), AutoGame(0x4D2580, 0x4D2578), AutoGame(0x4D2324, 0x4D231C)}
        )};

        static const vector<vector<u32>> instruction = {
            {{AutoGameSet(0xEA0060EC, 0xEA006283)}, {AutoGameSet(0xEA0060C1, 0xEA00624B)}, {AutoGameSet<u32>(0xEAF2C304, AutoGame(0xEAF2032B, 0xEAF2032D))}, {AutoGameSet<u32>(0xEAF2C2FE, AutoGame(0xEAF20325, 0xEAF20327))}, {AutoGameSet<u32>(0xEAF2C2B4, AutoGame(0xEAF202EA, 0xEAF202EC))}, {AutoGameSet<u32>(0xEAF2C2AE, AutoGame(0xEAF202E4, 0xEAF202E6))}, {AutoGameSet<u32>(0xEAF2C30A, AutoGame(0xEAF20331, 0xEAF20333))}},
            {{AutoGameSet(0xEA0060E1, 0xEA006278)}, {AutoGameSet(0xEA0060B6, 0xEA006240)}, {AutoGameSet<u32>(0xEAF2C2E1, AutoGame(0xEAF20308, 0xEAF2030A))}, {AutoGameSet<u32>(0xEAF2C2D9, AutoGame(0xEAF20300, 0xEAF20302))}, {AutoGameSet<u32>(0xEAF2C29E, AutoGame(0xEAF202D4, 0xEAF202D6))}, {AutoGameSet<u32>(0xEAF2C296, AutoGame(0xEAF202CC, 0xEAF202CE))}, {AutoGameSet<u32>(0xEAF2C2E9, AutoGame(0xEAF20310, 0xEAF20312))}}
        };

        static const vector<u32> original(7, 0xE92D4070);

        if (entry->Hotkeys[0].IsDown())
            Process::Write32(address, instruction[0]);

        else if (entry->Hotkeys[1].IsDown())
            Process::Write32(address, instruction[1]);

        else Process::Write32(address, original);
    }

    void NoOutlines(MenuEntry *entry) {
        static MemoryManager manager(AutoGameSet<u32>(0x362ED8, 0x37A140));

        if (entry->IsActivated()) {
            // Write new instruction when activated
            if (!manager.Write(0))
                return; // Exit if writing fails
        }

        // Restore the original value when deactivated
        else if (!entry->IsActivated()) {
            if (manager.HasOriginalValue())
                manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
        }
    }

    void FastDialogs(MenuEntry *entry) {
        static const vector<u32> address = AutoGameSet({0x3F6FB4, 0x3F7818}, {0x419A34, 0x41A2A4});
        static const vector<u32> instruction = {0xE3A04003, 0xE3A05003};

        // Create MemoryManager instances
        static vector<MemoryManager> managers;
        static bool initialized = false;

        if (!initialized) {
            for (int i = 0; i < address.size(); ++i)
                managers.emplace_back(address[i]); // Create a MemoryManager instance for each address

            initialized = true;
        }

        if (entry->IsActivated()) {
            bool success = true;

            // Write new instruction to the addresses
            for (int i = 0; i < address.size(); ++i) {
                if (i >= managers.size() || !managers[i].Write(instruction[i])) {
                    success = false;
                    break; // Exit loop if writing fails
                }
            }

            if (!success)
                return; // Exit if writing fails
        }

        // When deactivated, restore the original values
        else if (!entry->IsActivated()) {
            for (auto &manager : managers) {
                if (manager.HasOriginalValue())
                    manager.~MemoryManager(); // Explicitly call the destructor to restore the original value
            }

            managers.clear(); // Clear managers to reset initialization
            initialized = false;
        }
    }
}