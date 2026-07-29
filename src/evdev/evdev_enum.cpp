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

#include <sstream>

#include <libevdev/libevdev.h>
#include <linux/input.h>

namespace evtest_qt {

namespace {

std::string code_name_or_fallback(unsigned int type, uint16_t code,
                                  char const* prefix)
{
  if (char const* name = libevdev_event_code_get_name(type, code)) {
    return name;
  }
  std::ostringstream out;
  out << prefix << "#" << code;
  return out.str();
}

} // namespace

std::string evdev_abs_name(uint16_t code)
{
  return code_name_or_fallback(EV_ABS, code, "ABS_");
}

std::string evdev_key_name(uint16_t code)
{
  return code_name_or_fallback(EV_KEY, code, "KEY_");
}

std::string evdev_rel_name(uint16_t code)
{
  return code_name_or_fallback(EV_REL, code, "REL_");
}

} // namespace evtest_qt

/* EOF */
