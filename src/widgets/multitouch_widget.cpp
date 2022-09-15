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

#include "multitouch_widget.hpp"

#include <QPainter>

#include "evdev/evdev_state.hpp"

namespace evtest_qt {

MultitouchWidget::MultitouchWidget(QWidget* parent_) :
  QWidget(parent_),
  m_verification_mode(false),
  m_max_x(),
  m_max_y(),
  m_mt_states(),
  m_history()
{
}

MultitouchWidget::~MultitouchWidget()
{
}

void
MultitouchWidget::set_verification_mode(bool value)
{
  m_verification_mode = value;

  if (!m_verification_mode) {
    m_history.clear();
  }
}

void
MultitouchWidget::on_change(EvdevState const& state)
{
  m_max_x = state.get_info().get_absinfo(ABS_MT_POSITION_X).maximum;
  m_max_y = state.get_info().get_absinfo(ABS_MT_POSITION_Y).maximum;

  m_mt_states.resize(static_cast<size_t>(state.get_mt_slot_count()));

  for(int slot = 0; slot < state.get_mt_slot_count(); ++slot)
  {
    m_mt_states[static_cast<size_t>(slot)] = state.get_mt_state(slot);

    if (m_verification_mode) {
      m_history.emplace_back(m_mt_states[static_cast<size_t>(slot)].x,
                             m_mt_states[static_cast<size_t>(slot)].y);
    }
  }

  update();
}

void
MultitouchWidget::paintEvent(QPaintEvent* ev)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int const b = 3;

  if (m_verification_mode)
  {
    for (QPoint const& p : m_history) {
      painter.fillRect(p.x() * width() / m_max_x - b,
                       p.y() * width() / m_max_x - b,
                       2 * b, 2 * b,
                       QColor(0, 255, 0));
    }
  }

  for(size_t slot = 0; slot < m_mt_states.size(); ++slot)
  {
    int const x_pos = m_mt_states[slot].x * width() / m_max_x;
    int const y_pos = m_mt_states[slot].y * height() / m_max_y;

    if (m_mt_states[slot].tracking_id == -1)
    {
      painter.setPen(QColor(255, 0, 0));
      painter.drawRect(x_pos - b, y_pos - b, 2*b, 2*b);
    }
    else
    {
      painter.fillRect(x_pos - b, y_pos - b, 2*b, 2*b, QColor(255, 0, 0));
    }
    painter.setPen(QColor(0, 0, 0));
    // fudging with b to create a larger clipping rectangle
    painter.drawText(x_pos - 10*b, y_pos - 8*b, 20*b, 16*b, Qt::AlignHCenter  | Qt::AlignBottom,
                     QString::number(m_mt_states[slot].tracking_id));
  }

  painter.drawRect(0, 0, width(), height());
}

} // namespace evtest_qt

/* EOF */
