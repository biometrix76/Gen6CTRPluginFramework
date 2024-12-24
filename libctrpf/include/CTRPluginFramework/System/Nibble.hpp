#ifndef CTRPLUGINFRAMEWORK_NIBBLE_HPP
#define CTRPLUGINFRAMEWORK_NIBBLE_HPP

#include <Headers.h>

namespace CTRPluginFramework {
    class Nibble {
        public:
            /**
            * \brief Safely read 8 bits
            * \param address The address to read from
            * \param rightSide The side of the 8 bits to read from
            */
            static bool Read8(u32 address, u8 &value, bool rightSide);

            /**
            * \brief Safely read 8 bits
            * \param address The addressses to read from
            * \param rightSide The side of the 8 bits to read from
            */
            static bool Read8(vector<u32> address, vector<u8> &value, vector<bool> rightSide);

            /**
            * \brief Safely write 8 bits
            * \param address The addresses to write to
            * \param rightSide The side of the 8 bits to write to
            */
            static bool Write8(vector<u32> address, vector<u8> value, vector<bool> rightSide);

            /**
            * \brief Safely write 8 bits
            * \param address The address to write to
            * \param rightSide The side of the 8 bits to write to
            */
            static bool Write8(u32 address, u8 value, bool rightSide);

        private:
    };
}

#endif