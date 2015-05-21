// jstest-qt - A graphical joystick tester
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

#ifndef HEADER_EVDEV_INFO_HPP
#define HEADER_EVDEV_INFO_HPP

#include <array>
#include <string>
#include <linux/input.h>

constexpr size_t bits_per_long = sizeof(unsigned long) * 8;
constexpr size_t nbits(long x) { return (((x)-1) / bits_per_long)+1; }

class EvdevInfo
{
public:
  std::string name;
  std::array<unsigned long, nbits(EV_MAX)> bit;
  std::array<unsigned long, nbits(ABS_MAX)> abs_bit;
  std::array<unsigned long, nbits(REL_MAX)> rel_bit;
  std::array<unsigned long, nbits(KEY_MAX)> key_bit;

public:
  EvdevInfo();

};

#endif

/* EOF */
