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

#ifndef HEADER_EVDEV_STATE_HPP
#define HEADER_EVDEV_STATE_HPP

#include <QObject>

#include <stdint.h>
#include <linux/input.h>
#include <vector>

#include "evdev_info.hpp"

class EvdevInfo;

class EvdevState : public QObject
{
  Q_OBJECT

private:
  EvdevInfo m_info;
  std::vector<int32_t> m_abs_values;
  std::vector<int32_t> m_key_values;

public:
  EvdevState(const EvdevInfo& info);

  void update(const input_event& ev);

  int get_key_value(uint16_t code) const;
  int get_abs_value(uint16_t code) const;

signals:
  void sig_change(const EvdevState& state);

private:
  EvdevState(const EvdevState&) = delete;
  EvdevState& operator=(const EvdevState&) = delete;
};

#endif

/* EOF */

