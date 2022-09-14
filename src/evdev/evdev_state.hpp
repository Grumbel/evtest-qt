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

#ifndef HEADER_EVTEST_QT_EVDEV_STATE_HPP
#define HEADER_EVTEST_QT_EVDEV_STATE_HPP

#include <stdint.h>
#include <linux/input.h>
#include <vector>

#include "evdev_info.hpp"
#include "fwd.hpp"

namespace evtest_qt {

class MultitouchState
{
public:
  MultitouchState() :
    x(0),
    y(0),
    tracking_id(0)
  {}

  MultitouchState(int x_, int y_, int tracking_id_) :
    x(x_),
    y(y_),
    tracking_id(tracking_id_)
  {}

  int x;
  int y;
  int tracking_id;
};

class EvdevState
{
public:
  EvdevState(EvdevInfo const& info);

  /** Return true if a sync event was received and the state is
      complete, false otherwise */
  bool update(input_event const& ev);

  int get_key_value(uint16_t code) const;
  int get_abs_value(uint16_t code) const;
  int get_rel_value(uint16_t code) const;

  int get_mt_slot_count() const;
  MultitouchState get_mt_state(int slot) const;

  EvdevInfo const& get_info() const { return m_info; }

private:
  EvdevInfo m_info;
  std::vector<int32_t> m_abs_values;
  std::vector<int32_t> m_rel_values;
  std::vector<int32_t> m_key_values;
  std::vector<MultitouchState> m_mt_states;
  bool m_complete;
};

} // namespace evtest_qt

#endif

/* EOF */

