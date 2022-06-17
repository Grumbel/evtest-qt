// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_BITS_HPP
#define HEADER_BITS_HPP

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
