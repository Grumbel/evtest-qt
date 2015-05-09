// jstest-qt - A graphical joystick tester
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
#include <iostream>

AxisWidget::AxisWidget(QWidget* parent) :
  QWidget(parent),
  m_pos(0)
{
}

AxisWidget::~AxisWidget()
{
}

void
AxisWidget::set_axis_pos(int v)
{
  m_pos = v;
  update();
}

void
AxisWidget::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.drawLine(0, 0, 100, 100);
  painter.drawLine(0, 0, 50, 100);
  painter.drawLine(50 + m_pos/300, 30, 50 + m_pos/300, 70);
}

/* EOF */
