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

#include "evdev_enum.hpp"

#include <linux/input.h>

namespace evtest_qt {

class EvDevRelEnum : public EnumBox<uint16_t>
{
public:
  EvDevRelEnum() :
    EnumBox<uint16_t>("EV_REL")
  {
#  include "rel_list.x"
  }
};

class EvDevAbsEnum : public EnumBox<uint16_t>
{
public:
  EvDevAbsEnum() :
    EnumBox<uint16_t>("EV_ABS")
  {
#  include "abs_list.x"
  }
};

class EvDevKeyEnum : public EnumBox<uint16_t>
{
public:
  EvDevKeyEnum() :
    EnumBox<uint16_t>("EV_KEY")
  {
#  include "key_list.x"
  }
};

std::string evdev_abs_name(uint16_t code)
{
  static EvDevAbsEnum evdev_abs_names;
  auto it = evdev_abs_names.find(code);
  if (it == evdev_abs_names.end())
  {
    std::ostringstream out;
    out << "ABS_#" << code;
    return out.str();
  }
  else
  {
    return it->second;
  }
}

std::string evdev_key_name(uint16_t code)
{
  static EvDevKeyEnum evdev_key_names;
  auto it = evdev_key_names.find(code);
  if (it == evdev_key_names.end())
  {
    std::ostringstream out;
    out << "KEY_#" << code;
    return out.str();
  }
  else
  {
    return it->second;
  }
}

std::string evdev_rel_name(uint16_t code)
{
  static EvDevRelEnum evdev_rel_names;
  auto it = evdev_rel_names.find(code);
  if (it == evdev_rel_names.end())
  {
    std::ostringstream out;
    out << "REL_#" << code;
    return out.str();
  }
  else
  {
    return it->second;
  }
}

} // namespace evtest_qt

/* EOF */
