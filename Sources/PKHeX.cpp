#include <CTRPluginFramework.hpp>
#include <unordered_set>
#include "Helpers.hpp"
#include <functional>
#include <array>

namespace CTRPluginFramework {
    static int selectedID;
    static u32 trainerID[2];

    static void UpdateIDValue(const vector<u32> &addresses, const vector<string> &options) {
        // Check if a valid ID is selected.
        if (selectedID >= 0) {
            // Prompt the user to enter a value then write it to memory if successful.
            if (KeyboardHandler<u32>::Set(options[selectedID] + ":", true, false, 5, trainerID[selectedID], 0, 1, 65535, nullptr)) {
                Process::Write16(addresses[selectedID], trainerID[selectedID]);
                OSD::Notify("Success!");
            }
        }
    }

    void Identity(MenuEntry *entry) {
        static const vector<u32> addresses = {AutoGameSet({0x8C79C3C, 0x8C79C3E}, {0x8C81340, 0x8C81342})};
        static const vector<string> options = {"TID", "SID"};
        Keyboard keyboard;

        if (keyboard.Setup(entry->Name() + ":", true, options, selectedID) != -1)
            UpdateIDValue(addresses, options);
    }

    string playerName;

    void InGameName(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C79C84, 0x8C81388);

        // Prompt the user to enter a value then write it to memory if successful.
        if (KeyboardHandler<string>::Set(entry->Name() + ":", true, 16, playerName, "", nullptr)) {
            Process::WriteString(address, playerName, StringFormat::Utf16);
            OSD::Notify("In-game name changed to: " + playerName);
        }
    }

    // Returns the time limit based on the selected time unit (hours, minutes, seconds).
    static int GetTimeLimit(int index) {
        return (index == 0 ? 999 : 59);
    }

    // Static variables for play time management and update flags.
    static u16 playTime[3];
    static int timeSelection;
    static int canUpdateTime[3];

    static void ConfigurePlayTime(const vector<string> &options) {
        // Check if a valid time unit is selected.
        if (timeSelection >= 0) {
            int limit = GetTimeLimit(timeSelection);
            // Calculate the number of digits needed for the limit value.
            int numDigits = (limit > 0) ? static_cast<int>(log10(limit)) + 1 : 1;

            // Prompt the user to enter a value then write it to memory if successful.
            if (KeyboardHandler<u16>::Set(options[timeSelection] + ":", true, false, numDigits, playTime[timeSelection], 0, 1, limit, nullptr)) {
                canUpdateTime[timeSelection] = 1;
                OSD::Notify("Time played updated");
            }
        }
    }

    void PlayTime(MenuEntry *entry) {
        static const vector<string> options = {"Hours", "Minutes", "Seconds"};
        Keyboard keyboard;
        keyboard.Setup(entry->Name() + ":", true, options, timeSelection);
        ConfigurePlayTime(options);
        UpdatePlayTime(entry);
    }

    void UpdatePlayTime(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8CE2814, 0x8CFBD88);

        for (int i = 0; i < 3; ++i) {
            if (canUpdateTime[i]) {
                if (i == 0)
                    Process::Write16(address, playTime[0]);

                else Process::Write8(address + 1 + i, playTime[i]);
            }
        }
    }

    static int language;

    void GameLanguage(MenuEntry *entry) {
        static const vector<string> options = {"Japanese", "English", "French", "Italian", "German", "Spanish", "Korean"}; // Korean
        Keyboard keyboard;

        // Setup the keyboard input for language selection.
        if (keyboard.Setup(entry->Name() + ":", true, options, language) != -1) {
            entry->SetGameFunc(UpdateLanguage);
            OSD::Notify("Save and restart to change language to: " + string(options[language]));
        }
    }

    void UpdateLanguage(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C79C69, 0x8C8136D);
        u8 currentLanguage;

        // Read the current language setting and update if necessary.
        if (Process::Read8(address, currentLanguage)) {
            u8 newLanguage = (language >= 5 ? language + 2 : language + 1); // Spanish & Korean = 7 & 8

            if (currentLanguage != newLanguage && newLanguage != 6) // 6 = N/A
                Process::Write8(address, newLanguage);
        }
    }

    static u32 amountOfMoney;

    void Money(MenuEntry *entry) {
        // Prompt the user to enter a value then write it to memory if successful.
        if (KeyboardHandler<u32>::Set(entry->Name() + ":", true, false, 7, amountOfMoney, 0, 0, 9999999, nullptr)) {
            entry->SetGameFunc(ApplyMoney);
            OSD::Notify("Money set to: " + to_string(amountOfMoney));
        }
    }

    void ApplyMoney(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C6A6AC, 0x8C71DC0);
        u32 currentMoney;

        // Read the current money value and update if necessary.
        if (Process::Read32(address, currentMoney) && currentMoney != amountOfMoney)
            Process::Write32(address, amountOfMoney);
    }

    static u16 battlePoints;

    void BattlePoints(MenuEntry *entry) {
        // Prompt the user to enter a value then write it to memory if successful.
        if (KeyboardHandler<u16>::Set(entry->Name() + ":", true, false, 4, battlePoints, 0, 0, 9999, nullptr)) {
            entry->SetGameFunc(ApplyBattlePoints);
            OSD::Notify("Battle Points set to: " + to_string(battlePoints));
        }
    }

    void ApplyBattlePoints(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C6A6E0, 0x8C71DE8);
        u16 currentPoints;

        // Read the current battle points value and update if necessary.
        if (Process::Read16(address, currentPoints) && currentPoints != battlePoints)
            Process::Write16(address, battlePoints);
    }

    static u32 miles;

    void PokeMiles(MenuEntry *entry) {
        // Prompt the user to enter a value then write it to memory if successful.
        if (KeyboardHandler<u32>::Set(entry->Name() + ":", true, false, 7, miles, 0, 0, 9999999, nullptr)) {
            entry->SetGameFunc(ApplyPokeMiles);
            OSD::Notify("P-Miles set to: " + to_string(miles));
        }
    }

    void ApplyPokeMiles(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C82BA0, 0x8C8B36C);
        u32 currentMiles;

        // Read the current value and update if necessary.
        if (Process::Read32(address, currentMiles) && currentMiles != miles)
            Process::Write32(address, miles);
    }

    static u16 itemAmount;

    void Items(MenuEntry *entry) {
        // Prompt the user to enter a value then write it to memory if successful.
        if (KeyboardHandler<u16>::Set(entry->Name() + ":", true, false, 3, itemAmount, 0, 0, 999, Callback<u16>)) {
            entry->SetGameFunc(UpdateItems);
            OSD::Notify("Applied changes to Items!");
        }
    }

    static const vector<int> items = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 112, 116, 117, 118, 119, 135, 136, 213, 214, 215, 217, 218, 219, 220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327, 492, 493, 494, 495, 496, 497, 498, 499, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 548, 549, 550, 551, 552, 553, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 564, 571, 572, 573, 576, 577, 580, 581, 582, 583, 584, 585, 586, 587, 588, 589, 590, 639, 640, 644, 646, 647, 648, 649, 650, 656, 657, 658, 659, 660, 661, 662, 663, 664, 665, 666, 667, 668, 669, 670, 671, 672, 673, 674, 675, 676, 677, 678, 679, 680, 681, 682, 683, 684, 685, 699, 704, 710, 711, 715};

    void UpdateItems(MenuEntry *entry) {
        static const vector<u32> address = {AutoGameSet({0x8C67564, 0x8C67566}, {0x8C6EC70, 0x8C6EC72})};
        vector<int> getItems;
        copy(items.begin(), items.end(), back_inserter(getItems));

        // Add specific items based on the currGameSeries
        if (currGameSeries == GameSeries::XY)
            getItems.insert(getItems.end(), {65, 66, 67}); // XY-specific items

        else if (currGameSeries == GameSeries::ORAS)
            getItems.insert(getItems.end(), {534, 535, 752, 753, 754, 755, 756, 757, 758, 759, 760, 761, 762, 763, 764, 767, 768, 769, 770}); // ORAS-specific items

        // Add common items for XY and ORAS
        if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS)
            getItems.insert(getItems.end(), {500, 652, 653, 654, 655});

        // Sort items
        sort(getItems.begin(), getItems.end());

        // Write items to memory
        for (int counter = 0; counter < getItems.size(); counter++)
            Process::Write32(address[0] + (0x4 * counter), getItems[counter] | (itemAmount << 16));
    }

    static u16 medAmount;

    void Medicines(MenuEntry *entry) {
        if (KeyboardHandler<u16>::Set(entry->Name() + ":", true, false, 3, medAmount, 0, 0, 999, Callback<u16>)) {
            entry->SetGameFunc(UpdateMedicines);
            OSD::Notify("Applied changes to Medicines!");
        }
    }

    static const vector<int> medicines = {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 134, 504, 565, 566, 567, 568, 569, 570, 591, 645, 708, 709};

    void UpdateMedicines(MenuEntry *entry) {
        static const vector<u32> address = {AutoGameSet({0x8C67ECC, 0x8C67ECE}, {0x8C6F5E0, 0x8C6F5E2})};
        vector<int> getMedicines;
        copy(medicines.begin(), medicines.end(), back_inserter(getMedicines));

        // Add specific medicine based on the current game series
        if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS)
            getMedicines.push_back(571); // XY and ORAS-specific medicine

        // Add additional items if not in XY (handled explicitly for ORAS)
        if (currGameSeries == GameSeries::ORAS)
            getMedicines.insert(getMedicines.end(), {65, 66, 67});

        // Sort medicines
        sort(getMedicines.begin(), getMedicines.end());

        // Write medicines to memory
        for (int counter = 0; counter < getMedicines.size(); counter++)
            Process::Write32(address[0] + (0x4 * counter), getMedicines[counter] | (medAmount << 16));
    }

    vector<int> berries = {149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212, 686, 687, 688};
    static u16 berryAmount;

    void Berries(MenuEntry *entry) {
        if (KeyboardHandler<u16>::Set(entry->Name() + ":", true, false, 3, berryAmount, 0, 0, 999, Callback<u16>)) {
            entry->SetGameFunc(UpdateBerries);
            OSD::Notify("Applied changes to Berries!");
        }
    }

    void UpdateBerries(MenuEntry *entry) {
        static const vector<u32> address = {AutoGameSet({0x8C67FCC, 0x8C67FCE}, {0x8C6F6E0, 0x8C6F6E2})};
        sort(berries.begin(), berries.end());

        for (int counter = 0; counter < berries.size(); counter++) {
            if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS)
                Process::Write32(address[0] + (0x4 * counter), berries[counter] | (berryAmount << 16));

            else Process::Write32(address[0] + (0x4 * counter), berries[counter] + (berryAmount << 10));
        }
    }

    static const vector<int> teachables = {328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354, 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380, 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 618, 619, 620, 690, 691, 692, 693, 694};

    void UnlockTMsAndHMs(void) {
        static const vector<u32> address = {AutoGameSet({0x8C67D24, 0x8C67D26}, {0x8C6F430, 0x8C6F432})};
        vector<int> getTeachables;

        // Copy the teachables vector to getTeachables
        copy(teachables.begin(), teachables.end(), back_inserter(getTeachables));

        // If the game is either XY or ORAS, then append specific TMs & HMs for those games accordingly
        if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS) {
            getTeachables.insert(getTeachables.end(), {420, 421, 422, 423, 424});

            // ORAS has some additionals
            if (currGameSeries == GameSeries::ORAS)
                getTeachables.insert(getTeachables.end(), {425, 737});
        }

        // Sort the list via ID no.
        sort(getTeachables.begin(), getTeachables.end());

        // Get TMs & HMs
        for (int counter = 0; counter < getTeachables.size(); counter++) {
            if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS)
                Process::Write32(address[0] + (0x4 * counter), getTeachables[counter] | (1 << 16));

            else Process::Write32(address[0] + (0x4 * counter), getTeachables[counter] + (1 << 10));
        }

        OSD::Notify("Unlocked all TMs & HMs!");
    }

    void UnlockKeyItems(void) {
        static const vector<u32> address = {AutoGameSet({0x8C67BA4, 0x8C67BA6}, {0x8C6F2B0, 0x8C6F2B2})};
        // List of universal Key Items
        vector<int> getKeyItems = {216, 465, 466, 628, 629, 631, 632, 638};

        // Append Key Items specific to XY
        if (currGameSeries == GameSeries::XY)
            getKeyItems.insert(getKeyItems.end(), {431, 442, 450, 641, 642, 643, 689, 695, 696, 698, 700, 701, 702, 703, 705, 712, 713, 714});

        // Append Key Items specific to ORAS
        if (currGameSeries == GameSeries::ORAS)
            getKeyItems.insert(getKeyItems.end(), {457, 474, 503, 718, 719, 720, 721, 722, 724, 725, 726, 727, 728, 729, 730, 731, 732, 733, 734, 735, 736, 738, 739, 740, 741, 742, 743, 744, 751, 771, 772, 774, 775});

        // Append Key Items common to both XY and ORAS
        if (currGameSeries == GameSeries::XY || currGameSeries == GameSeries::ORAS)
            getKeyItems.insert(getKeyItems.end(), {447, 471, 697});

        // Sort the list via ID no.
        sort(getKeyItems.begin(), getKeyItems.end());

        // Get Key Items
        for (int counter = 0; counter < getKeyItems.size(); counter++)
            Process::Write32(address[0] + (0x4 * counter), getKeyItems[counter] | (1 << 16));

        OSD::Notify("Unlocked all Key Items!");
    }

    static int unlockCase;

    void KeyItems(MenuEntry *entry) {
        static const vector<string> options = {"TMs & HMs", "Key Items"};
        Keyboard keyboard;

        if (keyboard.Setup(entry->Name() + ":", true, options, unlockCase) != -1)
            (unlockCase == 0 ? UnlockTMsAndHMs() : UnlockKeyItems());
    }

    void RemoveOldBackups(const string &directory, const vector<string> &fileList) {
        for (const auto &file : fileList)
            File::Remove(directory + file); // Remove each file from the specified directory
    }

    bool ExportData(const string &directory, u32 address, const string &fileExtension) {
        string fileName;

        // Prompt user to enter a file name
        if (KeyboardHandler<string>::Set("Name:", true, 16, fileName, "", nullptr)) {
            // Create a backup file and write data to it
            File backupFile(directory + fileName + fileExtension, File::RWC);
            backupFile.Dump(address, 215760);
            OSD::Notify("Data exported successfully!"); // Notify user of success
            return true;
        }

        return false;
    }

    bool ImportData(const string &directory, u32 address, const vector<string> &fileList) {
        Keyboard keyboard;
        keyboard.Populate(fileList); // Populate keyboard with file list
        int fileIndex = keyboard.Open(); // Prompt user to select a file

        if (fileIndex >= 0) {
            // Open the selected file and restore data from it
            File restoreFile(directory + fileList[fileIndex]);
            restoreFile.Inject(address, 215760);
            OSD::Notify("Data imported successfully!"); // Notify user of success
            return true;
        }

        return false;
    }

    void ExportImport(MenuEntry *entry) {
        // Constants for file paths and addresses
        const u32 titleID = Process::GetTitleID(); // Get the game title ID
        const string backupDirectory = "/luma/plugins/00040000" + Utils::ToHex(titleID) + "/Bin/"; // Directory for backups
        const u32 backupAddr = AutoGameSet(0x8C861C8, 0x8C9E134);
        const string fileExtension = ".bin"; // Backup file extension
        const size_t maxBackups = 20; // Maximum number of backups allowed

        // Function to show a message box
        auto DisplayMessage = [](const string &message, DialogType dialogType) {
            MessageBox(CenterAlign(message), dialogType, ClearScreen::Both)();
        };

        // List backup files in the directory
        vector<string> backupFiles;
        Directory directory(backupDirectory);
        directory.ListFiles(backupFiles, fileExtension);

        // Check if the number of backups has reached the limit
        if (backupFiles.size() >= maxBackups) {
            if (MessageBox(CenterAlign("Limit reached! Erase old backups?"), DialogType::DialogYesNo, ClearScreen::Both)()) {
                RemoveOldBackups(backupDirectory, backupFiles); // Remove old backups
                DisplayMessage("All backups have been erased", DialogType::DialogOk); // Notify user
            }

            return; // Exit if limit is reached and old backups are erased
        }

        // Present options to user and handle their choice
        vector<string> options = {"Export", "Import"};
        Keyboard keyboard;
        int userSelection = -1;

        // Set up the keyboard with the export/import options
        if (keyboard.Setup(entry->Name() + ":", true, options, userSelection) != -1) {
            switch (userSelection) {
                case 0: // Export case
                    if (!Directory::IsExists(backupDirectory))
                        Directory::Create(backupDirectory); // Create directory if it doesn't exist

                    if (backupFiles.size() < maxBackups)
                        ExportData(backupDirectory, backupAddr, fileExtension); // Export data

                    break;

                case 1: // Import case
                    if (backupFiles.empty()) {
                        DisplayMessage("No backups available for recovery", DialogType::DialogOk); // Notify user of no backups
                        return;
                    }

                    ImportData(backupDirectory, backupAddr, backupFiles); // Import data
                    break;

                default:
                    DisplayMessage("Invalid choice. Please try again", DialogType::DialogOk); // Notify user of invalid choice
                    break;
            }
        }
    }

    // Helper function to set a specific bit field
    template<typename T, typename U>
    void SetBitField(T &field, U mask, int value, int shift) {
        field = (field & ~static_cast<T>(mask)) | (static_cast<T>(value) << shift);
    }

    // Helper function to set a simple value
    template<typename T, typename U>
    void SetValue(T &field, U value) {
        field = value;
    }

    const u8 blockPosition[4][24] = {{0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3, 1, 1, 2, 3, 2, 3}, {1, 1, 2, 3, 2, 3, 0, 0, 0, 0, 0, 0, 2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2}, {2, 3, 1, 1, 3, 2, 2, 3, 1, 1, 3, 2, 0, 0, 0, 0, 0, 0, 3, 2, 3, 2, 1, 1}, {3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 3, 2, 3, 2, 1, 1, 0, 0, 0, 0, 0, 0}};

    void Unshuffle(u8 *shuffled, u8 *buffer, u32 value) {
        // Copies 0xE8 bytes from shuffled array to buffer array
        Process::CopyMemory(buffer, shuffled, 0xE8);

        // Rearranges data in buffer array based on blockPosition and shuffle value
        for (short block = 0; block < 4; block++) {
            for (short i = 0; i < 56; i++)
                *(buffer + i + 8 + 56 * block) = *(shuffled + i + 8 + 56 * blockPosition[block][value]);
        }
    }

    template <class Pokemon>
    void DecryptPokemon(u8 *encryptedData, Pokemon data) {
        // Extracts encryption key from the first 4 bytes of encryptedData
        u32 encryptionKey = *(u32*)encryptedData;

        // Computes shuffle value based on encryption key
        u32 shuffleValue = (((encryptionKey >> 0xD) & 0x1F) % 24);

        // Initializes seed with encryption key
        u32 seed = encryptionKey;

        // Decrypts data using a linear congruential generator
        for (int i = 8; i < 232; i += 2)  {
            seed = (0x41C64E6D * seed) + 0x00006073;
            encryptedData[i] ^= (u8)(seed >> 16);
            encryptedData[i + 1] ^= (u8)(seed >> 24);
        }

        // Unshuffles the decrypted data into the structure
        Unshuffle(encryptedData, (u8*)data, shuffleValue);
    }

    template <class Pokemon>
    void EncryptPokemon(Pokemon data, u8 *encryptedData) {
        // Extracts encryption key from the first 4 bytes of encryptedData
        u32 encryptionKey = *(u32*)encryptedData;

        // Computes shuffle value based on encryption key
        u32 shuffleValue = (((encryptionKey >> 0xD) & 0x1F) % 24);

        // Initializes seed with encryption key
        u32 seed = encryptionKey;

        // Reshuffles block data into encryptedData from the structure
        for (int i = 0; i < 11; i++) {
            Unshuffle(encryptedData, (u8*)data, shuffleValue);

            if (!Process::CopyMemory(encryptedData, data, 232))
                return;
        }

        // Encrypts data using a linear congruential generator
        for (int i = 8; i < 232; i += 2)  {
            seed = (0x41C64E6D * seed) + 0x00006073;
            encryptedData[i] ^= (u8)(seed >> 16);
            encryptedData[i + 1] ^= (u8)(seed >> 24);
        }
    }

    template <class Pokemon>
    u16 Checksum(Pokemon data) {
        // Calculates checksum of the structure
        u16 chksum = 0;

        for (u8 i = 4; i < 232 / 2; i++)
            chksum += *((u16*)(data) + i);

        return chksum;
    }

    template <class Pokemon>
    bool VerifyPokemonData(Pokemon data) {
        // Checks if poke pointer is valid and not already marked as invalid
        if (!data || data->sanity)
            return false;

        // Verifies the checksum and the validity of the species
        return (Checksum(data) == data->checksum) &&
           (data->species >= 1 && data->species <= 721);
    }

    template <class Pokemon>
    bool GetPokemon(u32 pointer, Pokemon data) {
        if (data == 0 || pointer == 0)
            return false;

        // Loads encrypted game data into encryptedData array
        u8 encryptedData[232];

        if (!Process::CopyMemory(&encryptedData, (u8*)pointer, 232))
            return false;

        // Decrypts the data and stores it in the structure
        DecryptPokemon(encryptedData, data);

        // Validates the decrypted structure and returns the result
        return (VerifyPokemonData(data));
    }

    template <class Pokemon>
    bool SetPokemon(u32 pointer, Pokemon data) {
        if (data == 0 || pointer == 0)
            return false;

        // Updates checksum in the structure
        data->checksum = Checksum(data);

        // Prepares encrypted data for storage
        u8 encryptedData[232];

        if (!Process::CopyMemory(&encryptedData, data, 232))
            return false;

        // Encrypts the data and stores it in the encryptedData array
        EncryptPokemon(data, encryptedData);

        // Writes encrypted data back to the game at the specified pointer
        if (Process::Patch(pointer, encryptedData, 232))
            return true;

        // Displays an error message if encryption or writing fails
        MessageBox(CenterAlign("Failed to encrypt or write data!"), DialogType::DialogOk)();
        return false;
    }

    template <class Pokemon>
    u8 IsShiny(Pokemon data) {
        // Determines if the Pokemon is shiny based on TID, SID, and PID
        return (data->TID ^ data->SID ^ (u16)(data->PID & 0x0000FFFF) ^ (u16)((data->PID & 0xFFFF0000) >> 16)) < 16;
    }

    namespace PKHeX {
        const int growthTable[100][6] = {{0, 0, 0, 0, 0, 0}, {8, 15, 4, 9, 6, 10}, {27, 52, 13, 57, 21, 33}, {64, 122, 32, 96, 51, 80}, {125, 237, 65, 135, 100, 156}, {216, 406, 112, 179, 172, 270}, {343, 637, 178, 236, 274, 428}, {512, 942, 276, 314, 409, 640}, {729, 1326, 393, 419, 583, 911}, {1000, 1800, 540, 560, 800, 1250}, {1331, 2369, 745, 742, 1064, 1663}, {1728, 3041, 967, 973, 1382, 2160}, {2197, 3822, 1230, 1261, 1757, 2746}, {2744, 4719, 1591, 1612, 2195, 3430}, {3375, 5737, 1957, 2035, 2700, 4218}, {4096, 6881, 2457, 2535, 3276, 5120}, {4913, 8155, 3046, 3120, 3930, 6141}, {5832, 9564, 3732, 3798, 4665, 7290}, {6859, 11111, 4526, 4575, 5487, 8573}, {8000, 12800, 5440, 5460, 6400, 10000}, {9261, 14632, 6482, 6458, 7408, 11576}, {10648, 16610, 7666, 7577, 8518, 13310}, {12167, 18737, 9003, 8825, 9733, 15208}, {13824, 21012, 10506, 10208, 11059, 17280}, {15625, 23437, 12187, 11735, 12500, 19531}, {17576, 26012, 14060, 13411, 14060, 21970}, {19683, 28737, 16140, 15244, 15746, 24603}, {21952, 31610, 18439, 17242, 17561, 27440}, {24389, 34632, 20974, 19411, 19511, 30486}, {27000, 37800, 23760, 21760, 21600, 33750}, {29791, 41111, 26811, 24294, 23832, 37238}, {32768, 44564, 30146, 27021, 26214, 40960}, {35937, 48155, 33780, 29949, 28749, 44921}, {39304, 51881, 37731, 33084, 31443, 49130}, {42875, 55737, 42017, 36435, 34300, 53593}, {46656, 59719, 46656, 40007, 37324, 58320}, {50653, 63822, 50653, 43808, 40522, 63316}, {54872, 68041, 55969, 47846, 43897, 68590}, {59319, 72369, 60505, 52127, 47455, 74148}, {64000, 76800, 66560, 56660, 51200, 80000}, {68921, 81326, 71677, 61450, 55136, 86151}, {74088, 85942, 78533, 66505, 59270, 92610}, {79507, 90637, 84277, 71833, 63605, 99383}, {85184, 95406, 91998, 77440, 68147, 106480}, {91125, 100237, 98415, 83335, 72900, 113906}, {97336, 105122, 107069, 89523, 77868, 121670}, {103823, 110052, 114205, 96012, 83058, 129778}, {110592, 115015, 123863, 102810, 88473, 138240}, {117649, 120001, 131766, 109923, 94119, 147061}, {125000, 125000, 142500, 117360, 100000, 156250}, {132651, 131324, 151222, 125126, 106120, 165813}, {140608, 137795, 163105, 133229, 112486, 175760}, {148877, 144410, 172697, 141677, 119101, 186096}, {157464, 151165, 185807, 150476, 125971, 196830}, {166375, 158056, 196322, 159635, 133100, 207968}, {175616, 165079, 210739, 169159, 140492, 219520}, {185193, 172229, 222231, 179056, 148154, 231491}, {195112, 179503, 238036, 189334, 156089, 243890}, {205379, 186894, 250562, 199999, 164303, 256723}, {216000, 194400, 267840, 211060, 172800, 270000}, {226981, 202013, 281456, 222522, 181584, 283726}, {238328, 209728, 300293, 234393, 190662, 297910}, {250047, 217540, 315059, 246681, 200037, 312558}, {262144, 225443, 335544, 259392, 209715, 327680}, {274625, 233431, 351520, 272535, 219700, 343281}, {287496, 241496, 373744, 286115, 229996, 359370}, {300763, 249633, 390991, 300140, 240610, 375953}, {314432, 257834, 415050, 314618, 251545, 393040}, {328509, 267406, 433631, 329555, 262807, 410636}, {343000, 276458, 459620, 344960, 274400, 428750}, {357911, 286328, 479600, 360838, 286328, 447388}, {373248, 296358, 507617, 377197, 298598, 466560}, {389017, 305767, 529063, 394045, 311213, 486271}, {405224, 316074, 559209, 411388, 324179, 506530}, {421875, 326531, 582187, 429235, 337500, 527343}, {438976, 336255, 614566, 447591, 351180, 548720}, {456533, 346965, 639146, 466464, 365226, 570666}, {474552, 357812, 673863, 485862, 379641, 593190}, {493039, 367807, 700115, 505791, 394431, 616298}, {512000, 378880, 737280, 526260, 409600, 640000}, {531441, 390077, 765275, 547274, 425152, 664301}, {551368, 400293, 804997, 568841, 441094, 689210}, {571787, 411686, 834809, 590969, 457429, 714733}, {592704, 423190, 877201, 613664, 474163, 740880}, {614125, 433572, 908905, 636935, 491300, 767656}, {636056, 445239, 954084, 660787, 508844, 795070}, {658503, 457001, 987754, 685228, 526802, 823128}, {681472, 467489, 1035837, 710266, 545177, 851840}, {704969, 479378, 1071552, 735907, 563975, 881211}, {729000, 491346, 1122660, 762160, 583200, 911250}, {753571, 501878, 1160499, 789030, 602856, 941963}, {778688, 513934, 1214753, 816525, 622950, 973360}, {804357, 526049, 1254796, 844653, 643485, 1005446}, {830584, 536557, 1312322, 873420, 664467, 1038230}, {857375, 548720, 1354652, 902835, 685900, 1071718}, {884736, 560922, 1415577, 932903, 707788, 1105920}, {912673, 571333, 1460276, 963632, 730138, 1140841}, {941192, 583539, 1524731, 995030, 752953, 1176490}, {970299, 591882, 1571884, 1027103, 776239, 1212873}, {1000000, 600000, 1640000, 1059860, 800000, 1250000}};

        template <class Pokemon>
        void MakeShiny(Pokemon data, bool shinify) {
            // Compute value for altering PID based on TID, SID, and PID
            u16 sxor = (data->TID ^ data->SID ^ (u16)(data->PID & 0x0000FFFF)) & 0xFFF0;

            // Conditionally set PID based on shinify flag
            data->PID = shinify
                ? (data->PID & 0x000FFFFF) | (sxor << 16) // Modify PID to make shiny
                : Utils::Random(1, 0xFFFFFFFF); // Generate random PID if not making shiny
        }

        template <class Pokemon>
        void MarkAsNicknamed(Pokemon data, bool isNicknamed) {
            const u32 NICKNAMED_MASK = 0x80000000u; // Mask for the most significant bit
            SetBitField(data->iv32, NICKNAMED_MASK, isNicknamed ? 1 : 0, 31);
        }

        template <class Pokemon>
        void SetNickname(Pokemon data, string name) {
            // Converts the provided 'name' to UTF-16 format before writing it.
            Process::WriteString((u32)data->nickname, name, StringFormat::Utf16);
        }

        template <class Pokemon>
        void AssignSpecies(Pokemon data, u16 getSpecies) {
            // Updates Pokemon species if getSpecies is valid and different from the current species
            if (getSpecies > 0 && getSpecies <= 721 && data->species != getSpecies)
                SetValue(data->species, speciesID);
        }

        template<class Pokemon>
        void AssignExperience(Pokemon data, int level, int type) {
            SetValue(data->exp, growthTable[level - 1][type]);
        }

        template<class Pokemon>
        void AssignNature(Pokemon data, int getNature) {
            SetValue(data->nature, getNature);
        }

        template<class Pokemon>
        void AssignHeldItem(Pokemon data, int getItem) {
            SetValue(data->heldItem, getItem);
        }

        template<class Pokemon>
        void AssignGender(Pokemon data, int gender) {
            SetBitField(data->fatefulEncounterGenderForm, 0x6, gender, 1);
        }

        template<class Pokemon>
        void AssignForm(Pokemon data, int form) {
            SetBitField(data->fatefulEncounterGenderForm, 0x38, form, 3);
        }

        template<class Pokemon>
        void AssignAbility(Pokemon data, int getAbility) {
            SetValue(data->ability, getAbility);
        }

        template<class Pokemon>
        void AdjustFriendship(Pokemon data, int friendshipLevel) {
            SetValue(data->hiddenTrainerFriendship, friendshipLevel);
        }

        template<class Pokemon>
        void SpecifyLanguage(Pokemon data, int getLanguage) {
            SetValue(data->language, getLanguage);
        }

        template<class Pokemon>
        void MarkAsEgg(Pokemon data, bool isEgg) {
            SetBitField(data->iv32, 0x40000000u, isEgg, 30);
        }

        template<class Pokemon>
        void SetPokerusStatus(Pokemon data, int days, int strain) {
            SetBitField(data->infected, 0xF, days, 0);
            SetBitField(data->infected, 0xF0, strain, 4);
        }

        template<class Pokemon>
        void SetCountry(Pokemon data, int countryFrom) {
            SetValue(data->country, countryFrom);
        }

        template<class Pokemon>
        void SetConsoleRegion(Pokemon data, int consoleRegionFrom) {
            SetValue(data->consoleRegion, consoleRegionFrom);
        }

        template<class Pokemon>
        void DefineOriginVersion(Pokemon data, int versionFrom) {
            SetValue(data->version, versionFrom);
        }

        template<class Pokemon>
        void AssignSecretID(Pokemon data, int value) {
            if (value <= 65535)
                SetValue(data->SID, value);
        }

        template<class Pokemon>
        void AssignTrainerID(Pokemon data, int value) {
            if (value <= 65535)
                SetValue(data->TID, value);
        }

        template<class Pokemon>
        void AssignPokeBall(Pokemon data, int ballType) {
            SetValue(data->ball, ballType);
        }

        template<class Pokemon>
        void SpecifyMetLevel(Pokemon data, int level) {
            SetBitField(data->metLevel, 0x7F, level, 0);
        }

        enum MetType {MET_DATE, EGG_DATE};

        template<class Pokemon>
        void SpecifyMetOrEggDate(Pokemon data, bool isEgg, u8 date[3], bool setYear, bool setMonth, bool setDay) {
            MetType dateType = isEgg ? EGG_DATE : MET_DATE;
            const bool canSet[3] = {setYear, setMonth, setDay};

            for (int i = 0; i < 3; ++i) {
                if (canSet[i]) {
                    if (dateType == EGG_DATE)
                        SetValue(data->eggDate[i], date[i]);

                    else SetValue(data->metDate[i], date[i]);
                }
            }
        }

        template<class Pokemon>
        void SetFatefulEncounterFlag(Pokemon data, bool isFateful) {
            SetBitField(data->fatefulEncounterGenderForm, 0x1, isFateful, 0);
        }

        template<class Pokemon>
        void AssignIndividualValue(Pokemon data, int index, int value) {
            if (value <= 31)
                SetBitField(data->iv32, 0x1F << (5 * index), value, 5 * index);
        }

        template<class Pokemon>
        void AssignEffortValue(Pokemon data, int index, int value) {
            if (value <= 252) {
                int maxCount = count_if(data->EV, data->EV + 6, [](int value) {return value == 252;});

                if (maxCount < 2 || (maxCount == 2 && value <= 6))
                    SetValue(data->EV[index], value);
            }
        }

        template<class Pokemon>
        void AssignContestStat(Pokemon data, int index, int value) {
            if (index < 6 && value <= 255)
                SetValue(data->contest[index], value);
        }

        template<class Pokemon>
        void AssignMove(Pokemon data, int index, int getMove, bool isRelearnable) {
            if (index < 4) {
                if (isRelearnable)
                    SetValue(data->relearn[index], getMove);

                else SetValue(data->move[index], getMove);
            }
        }

        template<class Pokemon>
        void AssignPPUp(Pokemon data, int index, int value) {
            if (index < 4)
                SetValue(data->movePPUp[index], value);
        }

        template<class Pokemon>
        void SetOriginalTrainerName(Pokemon data, const string &name) {
            Process::WriteString((u32)data->originalTrainerName, name, StringFormat::Utf16);
        }

        template<class Pokemon>
        void SetHiddenTrainerName(Pokemon data, const string &name) {
            Process::WriteString((u32)data->hiddenTrainerName, name, StringFormat::Utf16);
        }

        u32 DetermineSpeciesPointer() {
            return AutoGameSet(0x8C861C8, 0x8C9E134);
        }

        static u8 gBoxNumber = 0, gPositionNumber = 0;
        static u32 dataPointer = 0;

        bool ConfigureBoxAndPosition() {
            // Prompt for box number and validate input
            if (!KeyboardHandler<u8>::Set("Box:", true, false, 2, gBoxNumber, 0, 1, 31))
                return false; // Invalid box number input

            // Prompt for position number and validate input
            if (!KeyboardHandler<u8>::Set("Position:", true, false, 2, gPositionNumber, 0, 1, 30))
                return false; // Invalid position number input

            // Calculate the pointer to the Pokemon data based on box and position
            dataPointer = ((gPositionNumber - 1) * 0xE8) + ((gBoxNumber - 1) * 6960) + DetermineSpeciesPointer();
            return true; // Setup successful
        }

        void Position(MenuEntry *entry) {
            if (ConfigureBoxAndPosition()) // Handle setup failure if necessary
                entry->Name() = "Position: " << Color::Gray << "B" << to_string(gBoxNumber) << " at: " << to_string(gPositionNumber);

            else OSD::Notify("Invalid position");
        }

        bool VerifyPokemonData(u32 pointer, PK6 *pokemon) {
            // Check if the Pokemon data is valid and not in battle
            if (!GetPokemon(dataPointer, pokemon) && !IfInBattle())
                return false; // Data invalid or Pokemon is in battle

            return true; // Data is valid
        }

        bool HandlePokemonData(PK6 &pokemon, MenuEntry *entry, const function<bool(PK6&)> &processFunc, const function<string()> &getMessage) {
            if (VerifyPokemonData(dataPointer, &pokemon)) {
                if (processFunc(pokemon)) {
                    if (SetPokemon(dataPointer, &pokemon)) {
                        OSD::Notify(getMessage());
                        return true;
                    }
                }
            }

            return false;
        }

        void Shiny(MenuEntry *entry) {
            static const vector<string> options = {"No", "Yes"};
            Keyboard keyboard;
            PK6 pokemon;
            int makeShiny;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, makeShiny) != -1) {
                    if (IsShiny(&data) != makeShiny) {
                        MakeShiny(&data, makeShiny);
                        return true;
                    }
                }

                return false;
            }, [&]() -> string {
                return "Species made shiny? " + options[makeShiny];
            });
        }

        void Species(MenuEntry *entry) {
            PK6 pokemon;
            static u16 getSpecies = 0;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                SearchForSpecies(entry);

                if (Controller::IsKeyPressed(Key::B))
                    return false;

                getSpecies = speciesID;

                if (getSpecies > 0) {
                    AssignSpecies(&data, getSpecies);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Species updated successfully to: " + string(speciesList[getSpecies - 1]);
            });
        }

        void IsNicknamed(MenuEntry *entry) {
            static const vector<string> options = {"No", "Yes"};
            Keyboard keyboard;
            PK6 pokemon;
            int isNick;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, isNick) != -1) {
                    MarkAsNicknamed(&data, isNick); // Update nickname status
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Nickname status updated to: " + options[isNick];
            });
        }

        void Nickname(MenuEntry *entry) {
            PK6 pokemon;
            string nickname;

            if (VerifyPokemonData(dataPointer, &pokemon)) {
                if (KeyboardHandler<string>::Set(entry->Name() + ":", true, 26, nickname, "", nullptr)) {
                    HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                        SetNickname(&data, nickname); // Update Pokemon nickname
                        return true;
                    }, [&]() -> string {
                        return "Nickname updated to: " + nickname;
                    });
                }
            }
        }

        void Gender(MenuEntry *entry) {
            static const vector<string> options = {"Male", "Female"};
            Keyboard keyboard;
            PK6 pokemon;
            int genderChoice = 0; // Declare genderChoice outside the lambda

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                // Check for fixed gender
                for (const auto &fixed : genderCannotChange) {
                    if (data.species == fixed) {
                        OSD::Notify("Cannot change gender");
                        return false; // Exit if gender cannot be changed
                    }
                }

                int choice = keyboard.Setup(entry->Name() + ":", true, options, genderChoice);

                if (choice != -1) {
                    AssignGender(&data, genderChoice); // Update Pokemon gender
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Gender updated to: " + options[genderChoice];
            });
        }

        void Level(MenuEntry *entry) {
            PK6 pokemon;
            static u8 level = 0;

            // Lambda function for processing the level update
            auto processLevel = [&](PK6 &data) -> bool {
                if (KeyboardHandler<u8>::Set(entry->Name() + ":", true, false, 3, level, 0, 1, 100, Callback<u8>)) {
                    // Update level based on growth rates
                    for (const auto &rate : growthType) {
                        if (find(rate.begin(), rate.end(), data.species) != rate.end()) {
                            AssignExperience(&data, level, distance(growthType.begin(), find(growthType.begin(), growthType.end(), rate)));
                            return true; // Successfully processed
                        }
                    }
                }

                return false; // Failed to process
            };

            // Lambda function to generate a dynamic message
            auto getMessage = [&]() -> string {
                return "Level set to: " + to_string(level);
            };

            // Call HandlePokemonData with the lambda functions
            HandlePokemonData(pokemon, entry, processLevel, getMessage);
        }

        void Nature(MenuEntry *entry) {
            Keyboard keyboard;
            PK6 pokemon;
            int nature;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, natureList, nature) != -1) {
                    AssignNature(&data, nature); // Update Pokemon nature
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Nature updated to: " + natureList[nature];
            });
        }

        void Form(MenuEntry *entry) {
            Keyboard keyboard;
            PK6 pokemon;
            int form;

            if (VerifyPokemonData(dataPointer, &pokemon) && pokemon.species > 0) {
                if (keyboard.Setup(entry->Name() + ":", true, formList(pokemon.species), form) != -1) {
                    HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                        AssignForm(&data, form); // Update Pokemon form
                        return true;
                    }, [&]() -> string {
                        return "Form updated";
                    });
                }
            }
        }

        void HeldItem(MenuEntry *entry) {
            PK6 pokemon;
            static u16 item = 0;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                SearchForItem(entry);

                if (heldItemName > 0) {
                    item = heldItemName;
                    AssignHeldItem(&data, item);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Held item updated to: " + string(heldItemList[item - 1]);
            });
        }

        void Ability(MenuEntry *entry) {
            PK6 pokemon;
            static u8 getAbility = 0;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                SearchForAbility(entry);

                if (abilityName > 0) {
                    getAbility = abilityName;
                    AssignAbility(&data, getAbility);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Ability updated to: " + string(abilityList[getAbility - 1]);
            });
        }

        void Friendship(MenuEntry *entry) {
            PK6 pokemon;
            static u16 friendship = 0;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (KeyboardHandler<u16>::Set(entry->Name() + ":", true, false, 3, friendship, 0, 0, 255, Callback<u16>)) {
                    AdjustFriendship(&data, friendship); // Update friendship level
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Friendship level updated to: " + to_string(friendship);
            });
        }

        void Language(MenuEntry *entry) {
            static const vector<string> options = {"JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR", "CHS", "CHT"};
            Keyboard keyboard;
            PK6 pokemon;
            static int languageChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, languageChoice) != -1) {
                    SpecifyLanguage(&data, (languageChoice >= 5 ? languageChoice + 2 : languageChoice + 1));
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Language updated to: " + options[languageChoice];
            });
        }

        void IsEgg(MenuEntry *entry) {
            static const vector<string> options = {"No", "Yes"};
            Keyboard keyboard;
            PK6 pokemon;
            static int eggChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, eggChoice) != -1) {
                    MarkAsEgg(&data, eggChoice); // Set egg status
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Egg status updated to: " + options[eggChoice];
            });
        }

        void Pokerus(MenuEntry *entry) {
            static const vector<string> options = {"Cured", "Non-Cured"};
            Keyboard keyboard;
            PK6 pokemon;
            static u8 pkrsVal[2];
            static int cureChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, cureChoice) != -1) {
                    if (cureChoice == 0) { // Cured
                        if (KeyboardHandler<u8>::Set("Strain:", true, false, 1, pkrsVal[1], 0, 0, 3, Callback<u8>)) {
                            SetPokerusStatus(&data, 0, pkrsVal[1]);
                            return true;
                        }
                    }

                    else { // Non-Cured
                        if (KeyboardHandler<u8>::Set("Days:", true, false, 2, pkrsVal[0], 0, 1, 15, Callback<u8>)) {
                            if (KeyboardHandler<u8>::Set("Strain:", true, false, 1, pkrsVal[1], 0, 0, 3, Callback<u8>)) {
                                SetPokerusStatus(&data, pkrsVal[0], pkrsVal[1]);
                                return true;
                            }
                        }
                    }
                }

                return false;
            }, [&]() -> string {
                return "Pokerus status updated";
            });
        }

        void Country(MenuEntry *entry) {
            vector<string> options;
            Keyboard keyboard;
            PK6 pokemon;
            static int getPlayerCountry;
            static int playerCountry;

            // Populate country options
            for (const Nations &geo : countryList)
                options.push_back(geo.name);

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, getPlayerCountry) != -1) {
                    playerCountry = countryList[getPlayerCountry].id;
                    SetCountry(&data, playerCountry);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Country updated to: " + options[getPlayerCountry];
            });
        }

        void ConsoleRegion(MenuEntry *entry) {
            static const vector<string> options = {"Japan", "Americas", "Europe", "China", "Korea", "Taiwan"};
            Keyboard keyboard;
            PK6 pokemon;
            static int consRegion;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, consRegion) != -1) {
                    SetConsoleRegion(&data, consRegion + 1);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Console region updated to: " + options[consRegion];
            });
        }

        void OriginGame(MenuEntry *entry) {
            static vector<string> options;
            Keyboard keyboard;
            PK6 pokemon;
            static int getOrigin;
            static int originID;
            static int originsAvailable = 0;

            for (const Origins &origin : originList) {
                if (originsAvailable < 8) {
                    options.push_back(origin.name);
                    originsAvailable++;
                }

                else break;
            }

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, getOrigin) != -1) {
                    originID = originList[getOrigin].matchValue;
                    DefineOriginVersion(&data, originID);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Origin game updated to: " + string(originList[getOrigin].name);
            });
        }

        void Ball(MenuEntry *entry) {
            vector<string> options;
            Keyboard keyboard;
            PK6 pokemon;
            static int getBall;
            static int ballID;

            for (const Balls &ball : ballList)
                options.push_back(ball.name);

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, getBall) != -1) {
                    ballID = ballList[getBall].matchValue;
                    AssignPokeBall(&data, ballID);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Ball updated to: " + string(ballList[getBall].name);
            });
        }

        void MetLevel(MenuEntry *entry) {
            PK6 pokemon;
            static u8 levelMetAt;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (KeyboardHandler<u8>::Set(entry->Name() + ":", true, false, 3, levelMetAt, 0, 1, 100, Callback<u8>)) {
                    SpecifyMetLevel(&data, levelMetAt);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Met level updated to: " + to_string(levelMetAt);
            });
        }

        // Function to return date limit based on index
        int Calendar(int index) {
            switch (index) {
                case 0: return 99;
                case 1: return 12;
                case 2: return 31;
                default: return 0;
            }
        }

        void MetDate(MenuEntry *entry) {
            static const vector<string> options = {"Year", "Month", "Day"};
            Keyboard keyboard;
            PK6 pokemon;
            static u8 date[3];
            static int dateChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (keyboard.Setup(entry->Name() + ":", true, options, dateChoice) != -1) {
                    int settings[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

                    if (KeyboardHandler<u8>::Set(options[dateChoice] + ":", true, false, 2, date[dateChoice], 0, 1, Calendar(dateChoice), Callback<u8>)) {
                        SpecifyMetOrEggDate(&data, false, date, settings[dateChoice][0], settings[dateChoice][1], settings[dateChoice][2]);
                        return true;
                    }
                }

                return false;
            }, [&]() -> string {
                return "Met date updated";
            });
        }

        void IsFatefulEncounter(MenuEntry *entry) {
            static const vector<string> options = {"No", "Yes"};
            Keyboard keyboard;
            PK6 pokemon;
            static int encChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (keyboard.Setup(entry->Name() + ":", true, options, encChoice) != -1) {
                    SetFatefulEncounterFlag(&data, encChoice);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Fateful encounter status updated to: " + options[encChoice];
            });
        }

        void EggMetDate(MenuEntry *entry) {
            static const vector<string> options = {"Year", "Month", "Day"};
            Keyboard keyboard;
            PK6 pokemon;
            static u8 eggDate[3];
            static int eggDateChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, eggDateChoice) != -1) {
                        int settings[3][3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

                        if (KeyboardHandler<u8>::Set(options[eggDateChoice] + ":", true, false, 2, eggDate[eggDateChoice], 0, 1, Calendar(eggDateChoice), Callback<u8>)) {
                            SpecifyMetOrEggDate(&data, true, eggDate, settings[eggDateChoice][0], settings[eggDateChoice][1], settings[eggDateChoice][2]);
                            return true;
                        }
                    }
                }
            }, [&]() -> string {
                return "Egg date updated";
            });
        }

        void IV(MenuEntry *entry) {
            static const vector<string> options = {"HP", "Atk", "Def", "Spe", "SpA", "SpD"};
            Keyboard keyboard;
            PK6 pokemon;
            static u8 ivs[6];
            static int ivChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, ivChoice) != -1) {
                        if (KeyboardHandler<u8>::Set(options[ivChoice] + ": " + to_string((u8)(data.iv32 >> (5 * ivChoice)) & 0x1F), true, false, 2, ivs[ivChoice], 0, 0, 31, Callback<u8>)) {
                            AssignIndividualValue(&data, ivChoice, ivs[ivChoice]);
                            return true;
                        }
                    }
                }
            }, [&]() -> string {
                return options[ivChoice] + ": " + to_string(ivs[ivChoice]);
            });
        }

        void EV(MenuEntry *entry) {
            static const vector<string> options = {"HP", "Atk", "Def", "Spe", "SpA", "SpD"};
            Keyboard keyboard;
            PK6 pokemon;
            static vector<u16> evAmount(6, 0);
            static int evChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, evChoice) != -1) {
                        if (KeyboardHandler<u16>::Set(options[evChoice] + ": " + to_string(data.EV[evChoice]), true, false, 3, evAmount[evChoice], 0, 0, 252, Callback<u16>)) {
                            AssignEffortValue(&data, evChoice, evAmount[evChoice]);
                            return true;
                        }
                    }
                }
            }, [&]() -> string {
                return options[evChoice] + ": " + to_string(evAmount[evChoice]);
            });
        }

        void Contest(MenuEntry *entry) {
            static const vector<string> options = {"Cool", "Beauty", "Cute", "Smart", "Tough", "Sheen"};
            Keyboard keyboard;
            PK6 pokemon;
            static u16 contestStats[6];
            static int contestChoice;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, contestChoice) != -1) {
                        if (KeyboardHandler<u16>::Set(options[contestChoice] + ":", true, false, 3, contestStats[contestChoice], 0, 0, 255, Callback<u16>)) {
                            AssignContestStat(&data, contestChoice, contestStats[contestChoice]);
                            return true;
                        }
                    }
                }
            }, [&]() -> string {
                return options[contestChoice] + ": " + to_string(contestStats[contestChoice]);
            });
        }

        void CurrentMove(MenuEntry *entry) {
            static const vector<string> options = {"Move 1", "Move 2", "Move 3", "Move 4"};
            Keyboard keyboard;
            PK6 pokemon;
            static u16 moves;
            static int moveSlot;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, moveSlot) != -1) {
                        SearchForMove(entry);
                        moves = moveName;

                        if (moves > 0) {
                            AssignMove(&data, moveSlot, moves, false);
                            return true;
                        }
                    }
                }

                return false;
            }, [&]() -> string {
                return options[moveSlot] + ": " + string(movesList[moves - 1]);
            });
        }

        void PPUp(MenuEntry *entry) {
            static const vector<string> options = {"Move 1", "Move 2", "Move 3", "Move 4"};
            Keyboard keyboard;
            PK6 pokemon;
            static u8 ppUp[6];
            static int ppUpSlot;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, ppUpSlot) != -1) {
                        if (KeyboardHandler<u8>::Set(options[ppUpSlot] + ":", true, false, 1, ppUp[ppUpSlot], 0, 0, 3, Callback<u8>)) {
                            AssignPPUp(&data, ppUpSlot, ppUp[ppUpSlot]);
                            return true;
                        }
                    }
                }

                return false;
            }, [&]() -> string {
                return options[ppUpSlot] + ": " + to_string(ppUp[ppUpSlot]);
            });
        }

        void RelearnMove(MenuEntry *entry) {
            static const vector<string> options = {"Move 1", "Move 2", "Move 3", "Move 4"};
            Keyboard keyboard;
            PK6 pokemon;
            static u16 relearnMoves;
            static int relearnMoveSlot;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                while (true) {
                    if (keyboard.Setup(entry->Name() + ":", true, options, relearnMoveSlot) != -1) {
                        SearchForMove(entry);
                        relearnMoves = moveName;

                        if (relearnMoves > 0) {
                            AssignMove(&data, relearnMoveSlot, relearnMoves, true);
                            return true;
                        }
                    }
                }

                return false;
            }, [&]() -> string {
                return options[relearnMoveSlot] + ": " + string(movesList[relearnMoves - 1]);
            });
        }

        void SID(MenuEntry *entry) {
            PK6 pokemon;
            static u32 value;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (KeyboardHandler<u32>::Set(entry->Name() + ":", true, false, 5, value, 0, 1, 65535, Callback<u32>)) {
                    AssignSecretID(&data, value);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "SID: " + to_string(value);
            });
        }

        void TID(MenuEntry *entry) {
            PK6 pokemon;
            static u32 value;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (KeyboardHandler<u32>::Set(entry->Name() + ":", true, false, 5, value, 0, 1, 65535, Callback<u32>)) {
                    AssignTrainerID(&data, value);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "TID: " + to_string(value);
            });
        }

        void OTName(MenuEntry *entry) {
            PK6 pokemon;
            static string originalTrainerName;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (KeyboardHandler<string>::Set(entry->Name() + ":", true, 26, originalTrainerName, "", nullptr)) {
                    SetOriginalTrainerName(&data, originalTrainerName);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "OT: " + originalTrainerName;
            });
        }

        void LatestHandler(MenuEntry *entry) {
            PK6 pokemon;
            static string handler;

            HandlePokemonData(pokemon, entry, [&](PK6 &data) -> bool {
                if (KeyboardHandler<string>::Set(entry->Name() + ":", true, 26, handler, "", nullptr)) {
                    SetHiddenTrainerName(&data, handler);
                    return true;
                }

                return false;
            }, [&]() -> string {
                return "Latest OT: " + handler;
            });
        }
    }

    void BoxesUnlocked(MenuEntry *entry) {
        static const u32 address = AutoGameSet(0x8C6AC26, 0x8C7232A);

        // Lambda function to check if the boxes are unlocked
        auto isBoxesUnlocked = [](u8 value) -> bool {
            return value >= 31; // Check if the boxes are fully unlocked (value >= 31)
        };

        // Function to unlock the boxes
        auto unlockBoxes = [&]() -> bool {
            return Process::Write8(address, 31); // Write 31 to the address to unlock all boxes
        };

        u8 checkUnlockedBox; // Variable to store the current box status

        // Read the current box status and check if they are not fully unlocked
        if (Process::Read8(address, checkUnlockedBox) && !isBoxesUnlocked(checkUnlockedBox)) {
            if (unlockBoxes())
                OSD::Notify("All boxes unlocked!"); // Notify the user that all boxes are unlocked
        }

        else OSD::Notify("Already unlocked all boxes!"); // If all boxes are already unlocked
    }

    void PCAnywhere(MenuEntry *entry) {
        static const u32 address = AutoGameSet<u32>(0x3BBFA8, AutoGame(0x3D8748, 0x3D8744));
        static u32 original;
        static bool saved = false;

        if (!Process::Write32(address, 0xE3A00004, original, entry, saved))
            return;
    }

    bool ClonePokemon(u8 srcBox, u8 srcPosition, u8 destBox, u8 destPosition) {
        PK6 pokemon; // Pokemon object to hold data

        // Calculate source and destination pointers
        u32 source = ((srcPosition - 1) * 0xE8) + ((srcBox - 1) * 6960 + GetSpeciesPointer());
        u32 destination = ((destPosition - 1) * 0xE8) + ((destBox - 1) * 6960 + GetSpeciesPointer());

        // Retrieve Pokemon data from source
        if (!GetPokemon(source, &pokemon)) {
            OSD::Notify("Failed to retrieve Pokemon from source");
            return false;
        }

        // Set Pokemon data to destination
        if (!SetPokemon(destination, &pokemon)) {
            OSD::Notify("Failed to set Pokemon to destination");
            return false;
        }

        OSD::Notify("Pokemon cloned successfully");
        return true;
    }

    bool ValidateBoxPosition(u8 box, u8 position) {
        return (box >= 1 && box <= 31) && (position >= 1 && position <= 31);
    }

    bool SetBoxPosition(u8 &srcBox, u8 &srcPosition, u8 &destBox, u8 &destPosition) {
        // Get source box and position from the user
        if (!KeyboardHandler<u8>::Set("Copy from which box:", true, false, 2, srcBox, 1, 1, 31) ||
            !KeyboardHandler<u8>::Set("Which position in box " + Utils::ToString(srcBox, 0) + ":", true, false, 2, srcPosition, 1, 1, 31)) {
            OSD::Notify("Failed to get source box or position");
            return false;
        }

        // Get destination box and position from the user
        if (!KeyboardHandler<u8>::Set("Paste to box:", true, false, 2, destBox, 1, 1, 31) ||
            !KeyboardHandler<u8>::Set("Which position in box " + Utils::ToString(destBox, 0) + " to paste:", true, false, 2, destPosition, 1, 1, 31)) {
            OSD::Notify("Failed to get destination box or position");
            return false;
        }

        // Validate the box and position ranges
        if (!ValidateBoxPosition(srcBox, srcPosition) || !ValidateBoxPosition(destBox, destPosition)) {
            OSD::Notify("Invalid box or position input");
            return false;
        }

        return true;
    }

    void HandleCloneProcess(MenuEntry *entry) {
        u8 srcBox = 0, srcPosition = 0, destBox = 0, destPosition = 0;

        if (SetBoxPosition(srcBox, srcPosition, destBox, destPosition))
            ClonePokemon(srcBox, srcPosition, destBox, destPosition);

        else OSD::Notify("Invalid box or position input");
    }
}