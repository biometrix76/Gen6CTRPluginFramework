#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

#include <Headers.h>

namespace CTRPluginFramework {
    class MemoryManager {
    public:
        // Constructor that initializes with the target address
        MemoryManager(u32 address);

        // Destructor that restores the original value
        ~MemoryManager();

        // Write a new value to the address
        bool Write(u32 newValue);

        // Check if the original value was successfully set
        bool HasOriginalValue() const;

        // Get the original value
        u32 GetOriginalValue() const;

        // Restore the original value manually
        bool RestoreOriginalValue();

        // Save the current value as a new "original" value
        bool SaveCurrentAsOriginal();

    private:
        u32 address_; // Address to manage
        u32 originalValue_; // Stored original value
        bool isValueSet_; // Flag to check if value is set
    };
}

#endif // MEMORY_MANAGER_HPP