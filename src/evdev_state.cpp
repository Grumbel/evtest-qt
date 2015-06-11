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

#include "evdev_state.hpp"

#include <iostream>

EvdevState::EvdevState(const EvdevInfo& info) :
  m_info(info),
  m_abs_values(info.abss.size(), 0),
  m_rel_values(info.rels.size(), 0),
  m_key_values(info.keys.size(), 0),
  m_mt_states()
{
  if (info.has_abs(ABS_MT_SLOT))
  {
    AbsInfo absinfo = info.get_absinfo(ABS_MT_SLOT);
    assert(absinfo.minimum == 0);
    m_mt_states.resize(static_cast<size_t>(absinfo.maximum + 1));
  }
}

void
EvdevState::update(const input_event& ev)
{
  switch(ev.type)
  {
    case EV_SYN:
      sig_change(*this);

      // clear rel values
      for(auto& v : m_rel_values)
      {
        v = 0;
      }
      break;

    case EV_KEY:
      m_key_values[m_info.get_key_idx(ev.code)] = ev.value;
      break;

    case EV_ABS:
      m_abs_values[m_info.get_abs_idx(ev.code)] = ev.value;

      if (m_info.has_abs(ABS_MT_SLOT))
      {
        const int slot = m_abs_values[m_info.get_abs_idx(ABS_MT_SLOT)];

        if (ev.code == ABS_MT_POSITION_X)
        {
          m_mt_states[static_cast<size_t>(slot)].x = ev.value;
        }
        else if (ev.code == ABS_MT_POSITION_Y)
        {
          m_mt_states[static_cast<size_t>(slot)].y = ev.value;
        }
        else if (ev.code == ABS_MT_TRACKING_ID)
        {
          m_mt_states[static_cast<size_t>(slot)].tracking_id = ev.value;
        }
      }
      break;

    case EV_REL:
      // rel values are accumulated until a EV_SYN event
      m_rel_values[m_info.get_rel_idx(ev.code)] += ev.value;
      break;
  }
}

int
EvdevState::get_key_value(uint16_t code) const
{
  return m_key_values[m_info.get_key_idx(code)];
}

int
EvdevState::get_abs_value(uint16_t code) const
{
  return m_abs_values[m_info.get_abs_idx(code)];
}

int
EvdevState::get_rel_value(uint16_t code) const
{
  return m_rel_values[m_info.get_rel_idx(code)];
}

int
EvdevState::get_mt_slot_count() const
{
  return static_cast<int>(m_mt_states.size());
}

MultitouchState
EvdevState::get_mt_state(int slot) const
{
  return m_mt_states[static_cast<size_t>(slot)];
}

/* EOF */
