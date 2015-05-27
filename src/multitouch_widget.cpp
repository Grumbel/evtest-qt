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

MultitouchWidget::MultitouchWidget(QWidget* parent_) :
  QWidget(parent_),
  m_max_x(),
  m_max_y(),
  m_mt_x(),
  m_mt_y()
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

  m_mt_x.resize(state.get_mt_slot_count());
  m_mt_y.resize(state.get_mt_slot_count());

  for(int slot = 0; slot < state.get_mt_slot_count(); ++slot)
  {
    m_mt_x[slot] = state.get_mt_x(slot);
    m_mt_y[slot] = state.get_mt_y(slot);
  }

  update();
}

void
MultitouchWidget::paintEvent(QPaintEvent* ev)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  int b = 8;
  for(size_t slot = 0; slot < m_mt_x.size(); ++slot)
  {
    int x_pos = m_mt_x[slot] * width() / m_max_x;
    int y_pos = m_mt_y[slot] * height() / m_max_y;
    painter.fillRect(x_pos - b, y_pos - b, 2*b, 2*b, QColor(255, 0, 0));
  }
  painter.drawRect(0, 0, width(), height());
}

/* EOF */
