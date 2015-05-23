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

EvDevRelEnum evdev_rel_names;
EvDevKeyEnum evdev_key_names;
EvDevAbsEnum evdev_abs_names;

EvDevRelEnum::EvDevRelEnum() :
  EnumBox<int>("EV_REL")
{
#  include "rel_list.x"
}

EvDevAbsEnum::EvDevAbsEnum() :
    EnumBox<int>("EV_ABS")
{
#  include "abs_list.x"
}

EvDevKeyEnum::EvDevKeyEnum() :
  EnumBox<int>("EV_KEY")
{
#  include "key_list.x"
}

/* EOF */
