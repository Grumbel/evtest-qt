// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

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

