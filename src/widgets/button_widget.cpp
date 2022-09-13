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

#include "button_widget.hpp"

#include <QPainter>

#include "evdev/evdev_enum.hpp"
#include "evdev/evdev_state.hpp"

namespace evtest_qt {

ButtonWidget::ButtonWidget(uint16_t code, QWidget* parent) :
  QWidget(parent),
  m_verification_mode(false),
  m_unused(true),
  m_code(code),
  m_value(0)
{
  setToolTip(QString::fromStdString(evdev_key_name(m_code)));
}

ButtonWidget::~ButtonWidget()
{
}

void
ButtonWidget::set_verification_mode(bool value)
{
  m_verification_mode = value;

  m_unused = true;

  update();
}

void
ButtonWidget::on_change(EvdevState const& state)
{
  int old_value = m_value;
  m_value = state.get_key_value(m_code);
  if (old_value != m_value)
  {
    m_unused = false;
    update();
  }
}

void
ButtonWidget::paintEvent(QPaintEvent* ev)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  if (m_verification_mode) {
    if (!m_unused) {
      painter.setPen(Qt::NoPen);
      painter.fillRect(0, 0, width(), height(), QColor(0, 255, 0));
    }
  }

  switch(m_value)
  {
    case 0: // key up
      break;

    case 1: // key down
      painter.setPen(Qt::NoPen);
      painter.fillRect(0, 0, width(), height(), QColor(255, 0, 0));
      break;

    case 2: // key repeat
      painter.setPen(Qt::NoPen);
      painter.fillRect(0, 0, width(), height(), QColor(255, 0, 255));
      break;

    default: // unknown
      painter.setPen(Qt::NoPen);
      painter.fillRect(0, 0, width(), height(), QColor(255, 255, 0));
      break;
  }

  painter.setPen(QColor(0, 0, 0));
  painter.drawText(0, 0, width(), height(), Qt::AlignVCenter | Qt::AlignCenter,
                   QString::fromStdString(evdev_key_name(m_code)));

  // box outline
  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width(), height());
}

} // namespace evtest_qt

/* EOF */
