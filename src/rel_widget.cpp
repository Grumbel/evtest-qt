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

#include "rel_widget.hpp"

#include <QPainter>

#include "evdev_enum.hpp"
#include "evdev_state.hpp"

namespace evtest_qt {

RelWidget::RelWidget(uint16_t code, QWidget* parent_) :
  QWidget(parent_),
  m_code(code),
  m_offset_x()
{
  setToolTip(QString::fromStdString(evdev_rel_name(m_code)));
}

RelWidget::~RelWidget()
{
}

void
RelWidget::paintEvent(QPaintEvent* ev)
{
  QPainter painter(this);

  int x_pos = ((m_offset_x % width()) + width()) % width();
  int b = 8;
  painter.fillRect(x_pos - b - width(), 0, 2*b, height(), QColor(255, 0, 0));
  painter.fillRect(x_pos - b, 0, 2*b, height(), QColor(255, 0, 0));
  painter.fillRect(x_pos - b + width(), 0, 2*b, height(), QColor(255, 0, 0));

  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width()-1, height()-1);
}

void
RelWidget::on_change(EvdevState const& state)
{
  m_offset_x += state.get_rel_value(m_code);
  //m_offset_y += state.get_rel_value(m_code);
  update();
}

} // namespace evtest_qt

/* EOF */
