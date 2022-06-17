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
#include <iostream>

#include "evdev_state.hpp"

namespace evtest_qt {

MultitouchWidget::MultitouchWidget(QWidget* parent_) :
  QWidget(parent_),
  m_max_x(),
  m_max_y(),
  m_mt_states()
{
}

MultitouchWidget::~MultitouchWidget()
{
}

void
MultitouchWidget::on_change(const EvdevState& state)
{
  m_max_x = state.get_info().get_absinfo(ABS_MT_POSITION_X).maximum;
  m_max_y = state.get_info().get_absinfo(ABS_MT_POSITION_Y).maximum;

  m_mt_states.resize(static_cast<size_t>(state.get_mt_slot_count()));

  for(int slot = 0; slot < state.get_mt_slot_count(); ++slot)
  {
    m_mt_states[static_cast<size_t>(slot)] = state.get_mt_state(slot);
  }

  update();
}

void
MultitouchWidget::paintEvent(QPaintEvent* ev)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int b = 3;
  for(size_t slot = 0; slot < m_mt_states.size(); ++slot)
  {
    int x_pos = m_mt_states[slot].x * width() / m_max_x;
    int y_pos = m_mt_states[slot].y * height() / m_max_y;
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
