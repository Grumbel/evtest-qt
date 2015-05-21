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

#include "bits.hpp"

class EvdevInfo
{
public:
  std::string name;
  std::array<unsigned long, bits::nbits(EV_MAX)> bit;
  std::array<unsigned long, bits::nbits(ABS_MAX)> abs_bit;
  std::array<unsigned long, bits::nbits(REL_MAX)> rel_bit;
  std::array<unsigned long, bits::nbits(KEY_MAX)> key_bit;

  size_t num_abs;
  size_t num_rel;
  size_t num_key;

public:
  EvdevInfo() :
    name(),
    bit(),
    abs_bit(),
    rel_bit(),
    key_bit()
  {
  }

  EvdevInfo(std::string name,
            std::array<unsigned long, bits::nbits(EV_MAX)> bit_,
            std::array<unsigned long, bits::nbits(ABS_MAX)> abs_bit_,
            std::array<unsigned long, bits::nbits(REL_MAX)> rel_bit_,
            std::array<unsigned long, bits::nbits(KEY_MAX)> key_bit_) :
    name(std::move(name)),
    bit(std::move(bit_)),
    abs_bit(std::move(abs_bit_)),
    rel_bit(std::move(rel_bit_)),
    key_bit(std::move(key_bit_)),
    num_abs(0),
    num_rel(0),
    num_key(0)
  {
    for(int i = 0; i < ABS_MAX; ++i)
    {
      if (bits::test_bit(i, abs_bit.data()))
      {
        num_abs += 1;
      }
    }

    for(int i = 0; i < REL_MAX; ++i)
    {
      if (bits::test_bit(i, rel_bit.data()))
      {
        num_rel += 1;
      }
    }

    for(int i = 0; i < KEY_MAX; ++i)
    {
      if (bits::test_bit(i, key_bit.data()))
      {
        num_key += 1;
      }
    }
  }
};

#endif

/* EOF */
