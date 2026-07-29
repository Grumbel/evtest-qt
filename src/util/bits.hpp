// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_BITS_HPP
#define HEADER_EVTEST_QT_BITS_HPP

namespace evtest_qt {

namespace bits {

constexpr size_t bits_per_long = sizeof(unsigned long) * 8;
constexpr size_t nbits(size_t x) { return (((x)-1) / bits_per_long)+1; }
constexpr size_t off(size_t x) { return (x) % bits_per_long; }
constexpr unsigned long bit(size_t x) { return 1UL << off(x); }
constexpr size_t long_idx(size_t x) { return x / bits_per_long; }
constexpr bool test_bit(size_t bit, const unsigned long* array) { return (array[long_idx(bit)] >> off(bit)) & 1; }

} // namespace bits

} // namespace evtest_qt

#endif

/* EOF */
