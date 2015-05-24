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

#include "axis_widget.hpp"

#include <QPainter>

#include "evdev_enum.hpp"
#include "evdev_state.hpp"

AxisWidget::AxisWidget(uint16_t code, int min, int max, QWidget* parent) :
  QWidget(parent),
  m_code(code),
  m_min(min),
  m_max(max),
  m_value(0)
{
  setToolTip(QString::fromStdString(evdev_abs_names[m_code]));
}

AxisWidget::~AxisWidget()
{
}

void
AxisWidget::set_axis_pos(int v)
{
  m_value = v;
  update();
}

void
AxisWidget::on_change(const EvdevState& state)
{
  int old_value = m_value;
  m_value = state.get_abs_value(m_code);
  if (old_value != m_value)
  {
    update();
  }
}

void
AxisWidget::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  int value_pos = 0;
  int zero_pos = 0;

  if (m_max - m_min != 0)
  {
    value_pos = width() * (m_value - m_min) / (m_max - m_min);
    zero_pos = width() * (0 - m_min) / (m_max - m_min);
  }

  // blue rect
  painter.setPen(Qt::NoPen);
  int l = std::min(value_pos, zero_pos);
  int r = std::max(value_pos, zero_pos);
  painter.fillRect(l, 0, r - l, height(), QColor(192, 192, 255));

  if (false)
  {
    // zero line
    painter.setPen(QColor(128, 128, 128));
    painter.drawLine(zero_pos, 0, zero_pos, height());
  }

  // value line
  painter.setPen(QColor(0, 0, 0));
  painter.drawLine(value_pos, 0, value_pos, height());

  // box outline
  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width(), height());
}

/* EOF */
