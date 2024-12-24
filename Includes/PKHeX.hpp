  #ifndef PKHEX_HPP
  #define PKHEX_HPP

  namespace CTRPluginFramework {
   void Identity(MenuEntry *entry);
   void InGameName(MenuEntry *entry);
   void PlayTime(MenuEntry *entry);
   void UpdatePlayTime(MenuEntry *entry);
   void GameLanguage(MenuEntry *entry);
   void UpdateLanguage(MenuEntry *entry);
   void Money(MenuEntry *entry);
   void ApplyMoney(MenuEntry *entry);
   void BattlePoints(MenuEntry *entry);
   void ApplyBattlePoints(MenuEntry *entry);
   void PokeMiles(MenuEntry *entry);
   void ApplyPokeMiles(MenuEntry *entry);

   void Items(MenuEntry *entry);
   void UpdateItems(MenuEntry *entry);
   void Medicines(MenuEntry *entry);
   void UpdateMedicines(MenuEntry *entry);
   void Berries(MenuEntry *entry);
   void UpdateBerries(MenuEntry *entry);
   void UnlockTMsAndHMs(MenuEntry *entry);
   void UnlockKeyItems(MenuEntry *entry);
   void KeyItems(MenuEntry *entry);

   struct PK6 {
      u32 encryptionConstant;
      u16 sanity;
      u16 checksum;

      // Block A
      u16 species;
      u16 heldItem;
      u16 TID;
      u16 SID;
      u32 exp;
      u8 ability;
      u8 abilityNumber;
      u8 markValue;
      u32 PID;
      u8 nature;
      u8 fatefulEncounterGenderForm;
      u8 EV[6];
      u8 contest[6];
      u8 resortEventStatus;
      u8 infected;
      u8 st1;
      u8 st2;
      u8 st3;
      u8 st4;
      u8 ribbons[6];
      u8 unused1[2];
      u8 ribbonCountMemoryContest;
      u8 ribbonCountMemoryBattle;
      u8 superTrainingFlags;
      u8 unused2[5];

      // Block B
      u8 nickname[26];
      u16 move[4];
      u8 movePP[4];
      u8 movePPUp[4];
      u16 relearn[4];
      u8 secretSuperTrainingUnlockled;
      u8 unused3;
      u32 iv32;

      // Block C
      u8 hiddenTrainerName[26];
      u8 hiddenTrainerGender;
      u8 currentHandler;
      u8 geo1Region;
      u8 geo1Country;
      u8 geo2Region;
      u8 geo2Country;
      u8 geo3Region;
      u8 geo3Country;
      u8 geo4Region;
      u8 geo4Country;
      u8 geo5Region;
      u8 geo5Country;
      u8 unused4[4];
      u8 hiddenTrainerFriendship;
      u8 hiddenTrainerAffection;
      u8 hiddenTrainerIntensity;
      u8 hiddenTrainerMemory;
      u8 hiddenTrainerFeeling;
      u8 unused5;
      u16 hiddenTrainerTextVar;
      u8 unused6[4];
      u8 fullness;
      u8 enjoyment;

      // Block D
      u8 originalTrainerName[26];
      u8 originalTrainerFriendship;
      u8 originalTrainerAffection;
      u8 originalTrainerIntensity;
      u8 originalTrainerMemory;
      u16 originalTrainerTextVar;
      u8 originalTrainerFeeling;
      u8 eggDate[3];
      u8 metDate[3];
      u8 unused7;
      u16 eggLocation;
      u16 metLocation;
      u8 ball;
      u8 metLevel;
      u8 hyperTrainFlags;
      u8 version;
      u8 country;
      u8 region;
      u8 consoleRegion;
      u8 language;
      u8 unused8[4];
   };

   /* Template function to retrieve Pokemon data using a given pointer.
   The function takes a pointer to the Pokemon data and a reference to a data object where the retrieved data will be stored.
   It returns true if the data retrieval is successful, otherwise false. */
   template <class Pokemon>
   bool GetPokemon(u32 pokePointer, Pokemon data);

   /* External variable declaration for the pointer to the species data.
   This pointer is used to access the Pokemon species information in memory. */
   extern u32 speciesPointer;

   /* Function to retrieve the pointer to the species data in memory.
   This function returns the value of the speciesPointer, which points to the Pokemon species information. */
   u32 GetSpeciesPointer(void);

   namespace PKHeX {
      void Position(MenuEntry *entry);
      void Shiny(MenuEntry *entry);
      void Species(MenuEntry *entry);
      void IsNicknamed(MenuEntry *entry);
      void Nickname(MenuEntry *entry);
      void Level(MenuEntry *entry);
      void Nature(MenuEntry *entry);
      void Gender(MenuEntry *entry);
      void Form(MenuEntry *entry);
      void HeldItem(MenuEntry *entry);
      void Ability(MenuEntry *entry);
      void Friendship(MenuEntry *entry);
      void Language(MenuEntry *entry);
      void IsEgg(MenuEntry *entry);
      void Pokerus(MenuEntry *entry);
      void Country(MenuEntry *entry);
      void ConsoleRegion(MenuEntry *entry);
      void OriginGame(MenuEntry *entry);
      void Ball(MenuEntry *entry);
      void MetLevel(MenuEntry *entry);
      void MetDate(MenuEntry *entry);
      void IsFatefulEncounter(MenuEntry *entry);
      void EggMetDate(MenuEntry *entry);
      void IV(MenuEntry *entry);
      void EV(MenuEntry *entry);
      void Contest(MenuEntry *entry);
      void CurrentMove(MenuEntry *entry);
      void PPUp(MenuEntry *entry);
      void RelearnMove(MenuEntry *entry);
      void SID(MenuEntry *entry);
      void TID(MenuEntry *entry);
      void OTName(MenuEntry *entry);
      void LatestHandler(MenuEntry *entry);
   }

   void ExportImport(MenuEntry *entry);
   void PCAnywhere(MenuEntry *entry);
   void BoxesUnlocked(MenuEntry *entry);
}

#endif // PKHEX_HPP