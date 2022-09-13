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

#ifndef HEADER_EVTEST_QT_EVDEV_ENUM_HPP
#define HEADER_EVTEST_QT_EVDEV_ENUM_HPP

#include "util/enum_box.hpp"

namespace evtest_qt {

std::string evdev_abs_name(uint16_t code);
std::string evdev_key_name(uint16_t code);
std::string evdev_rel_name(uint16_t code);

} // namespace evtest_qt

#endif

/* EOF */
