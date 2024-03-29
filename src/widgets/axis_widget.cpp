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

#include <algorithm>

#include <QPainter>

#include "evdev/evdev_enum.hpp"
#include "evdev/evdev_state.hpp"

namespace evtest_qt {

AxisWidget::AxisWidget(uint16_t code, int min, int max, QWidget* parent_) :
  QWidget(parent_),
  m_verification_mode(false),
  m_used_min(0),
  m_used_max(0),
  m_code(code),
  m_min(min),
  m_max(max),
  m_value(0)
{
  setToolTip(QString::fromStdString(evdev_abs_name(m_code)));
}

AxisWidget::~AxisWidget()
{
}

void
AxisWidget::set_verification_mode(bool value)
{
  m_verification_mode = value;

  m_used_min = 0;
  m_used_max = 0;

  update();
}

void
AxisWidget::on_change(EvdevState const& state)
{
  int old_value = m_value;
  m_value = state.get_abs_value(m_code);

  m_used_min = std::min(m_used_min, m_value);
  m_used_max = std::max(m_used_max, m_value);

  if (old_value != m_value)
  {
    update();
  }
}

void
AxisWidget::paintEvent(QPaintEvent* ev)
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

  if (m_verification_mode) {
    if (m_used_min != m_used_max) {
      painter.setPen(Qt::NoPen);
      painter.fillRect(QRect(QPoint(width() * (m_used_min - m_min) / (m_max - m_min),
                                    0),
                             QPoint(width() * (m_used_max - m_min) / (m_max - m_min),
                                    height())),
                       QColor(0, 255, 0));
    }
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

} // namespace evtest_qt

/* EOF */
