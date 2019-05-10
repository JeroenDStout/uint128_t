/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

/* This file is a rehash of Jason Lee's uint128_t, with the goal of
 * making it a single-header, constexpr capable version.
 * This Source Code Form includes substantial portions of the this
 * original Software, so please find included below the original
 * licence as applicable to those portions; but please be reminded
 * this Source Code Form as a whole is released under the licence
 * as seen above.

    Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.

    With much help from Auston Sterling

    Thanks to Stefan Deigmüller for finding
    a bug in operator*.

    Thanks to François Dessenne for convincing me
    to do a general rewrite of this class.

*/

#pragma once

#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace ap_uint {
    class uint128_t;
}

// Give uint128_t type traits
namespace std {  // This is probably not a good idea
    //template <> struct is_arithmetic <ap_uint::uint128_t> : std::true_type {};
    //template <> struct is_integral   <ap_uint::uint128_t> : std::true_type {};
    //template <> struct is_unsigned   <ap_uint::uint128_t> : std::true_type {};
}

namespace ap_uint {

class uint128_t{
    private:
        uint64_t UPPER, LOWER;

    public:
        // --- Constructors

        constexpr uint128_t()
          : UPPER(0), LOWER(0)
        {}

        constexpr uint128_t(const uint128_t & rhs)
          : UPPER(rhs.UPPER), LOWER(rhs.LOWER)
        {}

        constexpr uint128_t(uint128_t && rhs)
          : UPPER(std::move(rhs.UPPER)), LOWER(std::move(rhs.LOWER))
        {
            if (this != &rhs){
                rhs.UPPER = 0;
                rhs.LOWER = 0;
            }
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t(const T & rhs)
            : UPPER(0), LOWER(rhs)
        {}

        template <typename S, typename T, typename = typename std::enable_if <std::is_integral<S>::value && std::is_integral<T>::value, void>::type>
        constexpr uint128_t(const S & upper_rhs, const T & lower_rhs)
            : UPPER(upper_rhs), LOWER(lower_rhs)
        {}

        //  RHS input args only
        
        // --- Assignment

        constexpr uint128_t & operator=(const uint128_t & rhs){
            UPPER = rhs.UPPER;
            LOWER = rhs.LOWER;
            return *this;
        }

        constexpr uint128_t & operator=(uint128_t && rhs) {
            if (this != &rhs) {
                UPPER = std::move(rhs.UPPER);
                LOWER = std::move(rhs.LOWER);
                rhs.UPPER = 0;
                rhs.LOWER = 0;
            }
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator=(const T & rhs){
            UPPER = 0;
            LOWER = rhs;
            return *this;
        }

        // --- Casting

        constexpr operator bool() const{
            return (bool) (UPPER | LOWER);
        }

        constexpr operator uint8_t() const{
            return (uint8_t) LOWER;
        }

        constexpr operator uint16_t() const{
            return (uint16_t) LOWER;
        }

        constexpr operator uint32_t() const{
            return (uint32_t) LOWER;
        }

        constexpr operator uint64_t() const{
            return (uint64_t) LOWER;
        }

        // --- Bitwise Operators

        constexpr uint128_t operator&(const uint128_t & rhs) const{
            return uint128_t(UPPER & rhs.UPPER, LOWER & rhs.LOWER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator&(const T & rhs) const{
            return uint128_t(0, LOWER & (uint64_t) rhs);
        }

        constexpr uint128_t & operator&=(const uint128_t & rhs) {
            UPPER &= rhs.UPPER;
            LOWER &= rhs.LOWER;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator&=(const T & rhs){
            UPPER = 0;
            LOWER &= rhs;
            return *this;
        }

        constexpr uint128_t operator|(const uint128_t & rhs)const {
            return uint128_t(UPPER | rhs.UPPER, LOWER | rhs.LOWER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator|(const T & rhs) const{
            return uint128_t(UPPER, LOWER | (uint64_t) rhs);
        }

        constexpr uint128_t & operator|=(const uint128_t & rhs) {
            UPPER |= rhs.UPPER;
            LOWER |= rhs.LOWER;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator|=(const T & rhs){
            LOWER |= (uint64_t) rhs;
            return *this;
        }

        constexpr uint128_t operator^(const uint128_t & rhs) const {
            return uint128_t(UPPER ^ rhs.UPPER, LOWER ^ rhs.LOWER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator^(const T & rhs) const{
            return uint128_t(UPPER, LOWER ^ (uint64_t) rhs);
        }

        constexpr uint128_t & operator^=(const uint128_t & rhs){
            UPPER ^= rhs.UPPER;
            LOWER ^= rhs.LOWER;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator^=(const T & rhs){
            LOWER ^= (uint64_t) rhs;
            return *this;
        }

        constexpr uint128_t operator~() const{
            return uint128_t(~UPPER, ~LOWER);
        }

        // --- Bit shift Operators

        constexpr uint128_t operator<<(const uint128_t & rhs) const{
            const uint64_t shift = rhs.LOWER;
            if (((bool) rhs.UPPER) || (shift >= 128)){
                return uint128_0();
            }
            else if (shift == 64){
                return uint128_t(LOWER, 0);
            }
            else if (shift == 0){
                return *this;
            }
            else if (shift < 64){
                return uint128_t((UPPER << shift) + (LOWER >> (64 - shift)), LOWER << shift);
            }
            else if ((128 > shift) && (shift > 64)){
                return uint128_t(LOWER << (shift - 64), 0);
            }
            else{
                return uint128_0();
            }
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator<<(const T & rhs) const{
            return *this << uint128_t(rhs);
        }

        constexpr uint128_t & operator<<=(const uint128_t & rhs) {
            *this = *this << rhs;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator<<=(const T & rhs){
            *this = *this << uint128_t(rhs);
            return *this;
        }

        constexpr uint128_t operator>>(const uint128_t & rhs) const{
            const uint64_t shift = rhs.LOWER;
            if (((bool) rhs.UPPER) || (shift >= 128)){
                return uint128_0();
            }
            else if (shift == 64){
                return uint128_t(0, UPPER);
            }
            else if (shift == 0){
                return *this;
            }
            else if (shift < 64){
                return uint128_t(UPPER >> shift, (UPPER << (64 - shift)) + (LOWER >> shift));
            }
            else if ((128 > shift) && (shift > 64)){
                return uint128_t(0, (UPPER >> (shift - 64)));
            }
            else{
                return uint128_0();
            }
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator>>(const T & rhs) const{
            return *this >> uint128_t(rhs);
        }

        constexpr uint128_t & operator>>=(const uint128_t & rhs) {
            *this = *this >> rhs;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator>>=(const T & rhs){
            *this = *this >> uint128_t(rhs);
            return *this;
        }

        // --- Logical Operators

        constexpr bool operator!() const {
            return !(bool) (UPPER | LOWER);
        }

        constexpr bool operator&&(const uint128_t & rhs) const {
            return ((bool) *this && rhs);
        }

        constexpr bool operator||(const uint128_t & rhs) const {
            return ((bool) *this || rhs);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator&&(const T & rhs){
            return static_cast <bool> (*this && rhs);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator||(const T & rhs){
            return static_cast <bool> (*this || rhs);
        }

        // --- Comparison Operators

        constexpr bool operator==(const uint128_t & rhs) const {
            return ((UPPER == rhs.UPPER) && (LOWER == rhs.LOWER));
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator==(const T & rhs) const{
            return (!UPPER && (LOWER == (uint64_t) rhs));
        }

        constexpr bool operator!=(const uint128_t & rhs) const {
            return ((UPPER != rhs.UPPER) | (LOWER != rhs.LOWER));
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator!=(const T & rhs) const{
            return (UPPER | (LOWER != (uint64_t) rhs));
        }

        constexpr bool operator>(const uint128_t & rhs) const {
            if (UPPER == rhs.UPPER){
                return (LOWER > rhs.LOWER);
            }
            return (UPPER > rhs.UPPER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator>(const T & rhs) const{
            return (UPPER || (LOWER > (uint64_t) rhs));
        }

        constexpr bool operator<(const uint128_t & rhs) const {
            if (UPPER == rhs.UPPER){
                return (LOWER < rhs.LOWER);
            }
            return (UPPER < rhs.UPPER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator<(const T & rhs) const{
            return (!UPPER)?(LOWER < (uint64_t) rhs):false;
        }

        constexpr bool operator>=(const uint128_t & rhs) const {
            return ((*this > rhs) | (*this == rhs));
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator>=(const T & rhs) const{
            return ((*this > rhs) | (*this == rhs));
        }

        constexpr bool operator<=(const uint128_t & rhs) const {
            return ((*this < rhs) | (*this == rhs));
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr bool operator<=(const T & rhs) const{
            return ((*this < rhs) | (*this == rhs));
        }

        // --- Arithmetic Operators

        constexpr uint128_t operator+(const uint128_t & rhs) const {
            return uint128_t(UPPER + rhs.UPPER + ((LOWER + rhs.LOWER) < LOWER), LOWER + rhs.LOWER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator+(const T & rhs) const {
            return uint128_t(UPPER + ((LOWER + (uint64_t) rhs) < LOWER), LOWER + (uint64_t) rhs);
        }

        constexpr uint128_t & operator+=(const uint128_t & rhs) {
            UPPER += rhs.UPPER + ((LOWER + rhs.LOWER) < LOWER);
            LOWER += rhs.LOWER;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator+=(const T & rhs){
            UPPER = UPPER + ((LOWER + rhs) < LOWER);
            LOWER = LOWER + rhs;
            return *this;
        }

        constexpr uint128_t operator-(const uint128_t & rhs) const {
            return uint128_t(UPPER - rhs.UPPER - ((LOWER - rhs.LOWER) > LOWER), LOWER - rhs.LOWER);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator-(const T & rhs) const {
            return uint128_t((uint64_t) (UPPER - ((LOWER - rhs) > LOWER)), (uint64_t) (LOWER - rhs));
        }

        constexpr uint128_t & operator-=(const uint128_t & rhs) {
            *this = *this - rhs;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator-=(const T & rhs){
            *this = *this - rhs;
            return *this;
        }

        constexpr uint128_t operator*(const uint128_t & rhs) const {
            // split values into 4 32-bit parts
            const uint64_t top[4] = {UPPER >> 32, UPPER & 0xffffffff, LOWER >> 32, LOWER & 0xffffffff};
            const uint64_t bottom[4] = {rhs.UPPER >> 32, rhs.UPPER & 0xffffffff, rhs.LOWER >> 32, rhs.LOWER & 0xffffffff};

            const auto ref = [](auto x, auto y) constexpr {
                return y*3+x;
            };

            uint64_t products[16] = { top[3] * bottom[3], top[3] * bottom[2], top[3] * bottom[1], top[3] * bottom[0],
                                      top[2] * bottom[3], top[2] * bottom[2], top[2] * bottom[1], top[2] * bottom[0],
                                      top[1] * bottom[3], top[1] * bottom[2], top[1] * bottom[1], top[1] * bottom[0],
                                      top[0] * bottom[3], top[0] * bottom[2], top[0] * bottom[1], top[0] * bottom[0] };

            // first row
            uint64_t fourth32 = (products[ref(0, 3)] & 0xffffffff);
            uint64_t third32  = (products[ref(0, 2)] & 0xffffffff) + (products[ref(0, 3)] >> 32);
            uint64_t second32 = (products[ref(0, 1)] & 0xffffffff) + (products[ref(0, 2)] >> 32);
            uint64_t first32  = (products[ref(0, 0)] & 0xffffffff) + (products[ref(0, 1)] >> 32);

            // second row
            third32  += (products[ref(1, 3)] & 0xffffffff);
            second32 += (products[ref(1, 2)] & 0xffffffff) + (products[ref(1, 3)] >> 32);
            first32  += (products[ref(1, 1)] & 0xffffffff) + (products[ref(1, 2)] >> 32);

            // third row
            second32 += (products[ref(2, 3)] & 0xffffffff);
            first32  += (products[ref(2, 2)] & 0xffffffff) + (products[ref(2, 3)] >> 32);

            // fourth row
            first32  += (products[ref(3, 3)] & 0xffffffff);

            // move carry to next digit
            third32  += fourth32 >> 32;
            second32 += third32  >> 32;
            first32  += second32 >> 32;

            // remove carry from current digit
            fourth32 &= 0xffffffff;
            third32  &= 0xffffffff;
            second32 &= 0xffffffff;
            first32  &= 0xffffffff;

            // combine components
            return uint128_t((first32 << 32) | second32, (third32 << 32) | fourth32);
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator*(const T & rhs) const{
            return *this * uint128_t(rhs);
        }

        constexpr uint128_t & operator*=(const uint128_t & rhs) {
            *this = *this * rhs;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator*=(const T & rhs) {
            *this = *this * uint128_t(rhs);
            return *this;
        }

    private:
        constexpr auto divmod(const uint128_t & lhs, const uint128_t & rhs) const
          -> std::pair<uint128_t, uint128_t> {
            if (rhs == uint128_0()){
                throw std::domain_error("Error: division or modulus by 0");
            }
            else if (rhs == uint128_1()){
                return std::pair <uint128_t, uint128_t> (lhs, uint128_0());
            }
            else if (lhs == rhs){
                return std::pair <uint128_t, uint128_t> (uint128_1(), uint128_0());
            }
            else if ((lhs == uint128_0()) || (lhs < rhs)){
                return std::pair <uint128_t, uint128_t> (uint128_0(), lhs);
            }

            std::pair <uint128_t, uint128_t> qr (uint128_0(), uint128_0());
            for(uint8_t x = lhs.bits(); x > 0; x--){
                qr.first  <<= uint128_1();
                qr.second <<= uint128_1();

                if ((lhs >> (x - 1U)) & 1){
                    ++qr.second;
                }

                if (qr.second >= rhs){
                    qr.second -= rhs;
                    ++qr.first;
                }
            }
            return qr;
        }

    public:
        constexpr uint128_t operator/(const uint128_t & rhs) const {
            return divmod(*this, rhs).first;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator/(const T & rhs) const{
            return *this / uint128_t(rhs);
        }

        constexpr uint128_t & operator/=(const uint128_t & rhs) {
            *this = *this / rhs;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator/=(const T & rhs){
            *this = *this / uint128_t(rhs);
            return *this;
        }

        constexpr uint128_t operator%(const uint128_t & rhs) const {
            return divmod(*this, rhs).second;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t operator%(const T & rhs) const{
            return *this % uint128_t(rhs);
        }

        constexpr uint128_t & operator%=(const uint128_t & rhs) {
            *this = *this % rhs;
            return *this;
        }

        template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
        constexpr uint128_t & operator%=(const T & rhs){
            *this = *this % uint128_t(rhs);
            return *this;
        }

        // --- Increment/Decrement Operators

        constexpr uint128_t & operator++() {
            return *this += uint128_1();
        }

        constexpr uint128_t operator++(int) {
            uint128_t temp(*this);
            ++*this;
            return temp;
        }

        constexpr uint128_t & operator--() {
            return *this -= uint128_1();
        }

        constexpr uint128_t operator--(int) {
            uint128_t temp(*this);
            --*this;
            return temp;
        }

        // Nothing done since promotion doesn't work here
        constexpr uint128_t operator+() const {
            return *this;
        }

        // two's complement
        constexpr uint128_t operator-() const {
            return ~*this + uint128_1();
        }

        // --- Access

        constexpr const uint64_t & upper() const {
            return UPPER;
        }

        constexpr const uint64_t & lower() const {
            return LOWER;
        }

        // Get bitsize of value
        uint8_t bits() const{
            uint8_t out = 0;
            if (UPPER){
                out = 64;
                uint64_t up = UPPER;
                while (up){
                    up >>= 1;
                    out++;
                }
            }
            else{
                uint64_t low = LOWER;
                while (low){
                    low >>= 1;
                    out++;
                }
            }
            return out;
        }

        // Get string representation of value
        std::string str(uint8_t base = 10, const unsigned int & len = 0) const {
            if ((base < 2) || (base > 16)){
                throw std::invalid_argument("Base must be in the range [2, 16]");
            }
            std::string out = "";
            if (!(*this)){
                out = "0";
            }
            else{
                std::pair<uint128_t, uint128_t> qr(*this, uint128_0());
                do{
                    qr = divmod(qr.first, base);
                    out = "0123456789abcdef"[(uint8_t) qr.second] + out;
                } while (qr.first);
            }
            if (out.size() < len){
                out = std::string(len - out.size(), '0') + out;
            }
            return out;
        };

        static constexpr uint128_t uint128_0() {
            return uint128_t(0, 0);
        }

        static constexpr uint128_t uint128_1() {
            return uint128_t(0, 1);
        }

};

// lhs type T as first arguemnt
// If the output is not a bool, casts to type T

// Bitwise Operators
template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator&(const T & lhs, const uint128_t & rhs){
    return rhs & lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator&=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs & lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator|(const T & lhs, const uint128_t & rhs){
    return rhs | lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator|=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs | lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator^(const T & lhs, const uint128_t & rhs){
    return rhs ^ lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator^=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (rhs ^ lhs);
}

    // --- Bitshift operators

constexpr uint128_t operator<<(const bool     & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const uint8_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const uint16_t & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const uint32_t & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const uint64_t & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const int8_t   & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const int16_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const int32_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

constexpr uint128_t operator<<(const int64_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) << rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator<<=(T & lhs, const uint128_t & rhs){
    return lhs = static_cast <T> (uint128_t(lhs) << rhs);
}

constexpr uint128_t operator>>(const bool     & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const uint8_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const uint16_t & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const uint32_t & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const uint64_t & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const int8_t   & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const int16_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const int32_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

constexpr uint128_t operator>>(const int64_t  & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) >> rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator>>=(T & lhs, const uint128_t & rhs) {
    return lhs = static_cast <T> (uint128_t(lhs) >> rhs);
}

    // --- Comparison operators

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr bool operator==(const T & lhs, const uint128_t & rhs) {
    return (!rhs.upper() && ((uint64_t) lhs == rhs.lower()));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr bool operator!=(const T & lhs, const uint128_t & rhs) {
    return (rhs.upper() | ((uint64_t) lhs != rhs.lower()));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr bool operator>(const T & lhs, const uint128_t & rhs) {
    return (!rhs.upper()) && ((uint64_t) lhs > rhs.lower());
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr bool operator<(const T & lhs, const uint128_t & rhs) {
    if (rhs.upper()) {
        return true;
    }
    return ((uint64_t) lhs < rhs.lower());
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr bool operator>=(const T & lhs, const uint128_t & rhs) {
    if (rhs.upper()) {
        return false;
    }
    return ((uint64_t) lhs >= rhs.lower());
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
bool operator<=(const T & lhs, const uint128_t & rhs) {
    if (rhs.upper()){
        return true;
    }
    return ((uint64_t) lhs <= rhs.lower());
}

    // --- Arithmetic operators

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator+(const T & lhs, const uint128_t & rhs) {
    return rhs + lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator+=(T & lhs, const uint128_t & rhs) {
    return lhs = static_cast <T> (rhs + lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator-(const T & lhs, const uint128_t & rhs) {
    return -(rhs - lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator-=(T & lhs, const uint128_t & rhs) {
    return lhs = static_cast <T> (-(rhs - lhs));
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator*(const T & lhs, const uint128_t & rhs) {
    return rhs * lhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator*=(T & lhs, const uint128_t & rhs) {
    return lhs = static_cast <T> (rhs * lhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator/(const T & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) / rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator/=(T & lhs, const uint128_t & rhs) {
    return lhs = static_cast <T> (uint128_t(lhs) / rhs);
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr uint128_t operator%(const T & lhs, const uint128_t & rhs) {
    return uint128_t(lhs) % rhs;
}

template <typename T, typename = typename std::enable_if<std::is_integral<T>::value, T>::type >
constexpr T & operator%=(T & lhs, const uint128_t & rhs) {
    return lhs = static_cast <T> (uint128_t(lhs) % rhs);
}

    // --- IO operators

std::ostream & operator<<(std::ostream & stream, const uint128_t & rhs) {
    if (stream.flags() & stream.oct){
        stream << rhs.str(8);
    }
    else if (stream.flags() & stream.dec){
        stream << rhs.str(10);
    }
    else if (stream.flags() & stream.hex){
        stream << rhs.str(16);
    }
    return stream;
}

}