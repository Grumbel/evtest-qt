// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "rel_widget.hpp"

#include <QPainter>

#include "evdev/evdev_enum.hpp"
#include "evdev/evdev_state.hpp"

namespace evtest_qt {

RelWidget::RelWidget(uint16_t code, QWidget* parent_) :
  QWidget(parent_),
  m_verification_mode(false),
  m_unused(true),
  m_code(code),
  m_offset_x(),
  m_last_value(0)
{
  setToolTip(QString::fromStdString(evdev_rel_name(m_code)));
}

RelWidget::~RelWidget()
{
}

void
RelWidget::set_verification_mode(bool value)
{
  m_verification_mode = value;

  m_unused = true;

  update();
}

void
RelWidget::paintEvent(QPaintEvent* ev)
{
  QPainter painter(this);

  if (m_verification_mode) {
    if (!m_unused) {
      painter.setPen(Qt::NoPen);
      painter.fillRect(0, 0, width(), height(), QColor(0, 255, 0));
    }
  }

  int x_pos = ((m_offset_x % width()) + width()) % width();
  int b = 8;
  painter.fillRect(x_pos - b - width(), 0, 2*b, height(), QColor(255, 0, 0));
  painter.fillRect(x_pos - b, 0, 2*b, height(), QColor(255, 0, 0));
  painter.fillRect(x_pos - b + width(), 0, 2*b, height(), QColor(255, 0, 0));

  // raw value text
  painter.setPen(QColor(0, 0, 0));
  QString value_text = QString::number(m_last_value);
  painter.drawText(0, 0, width(), height(),
                   Qt::AlignVCenter | Qt::AlignCenter, value_text);

  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width()-1, height()-1);
}

void
RelWidget::on_change(EvdevState const& state)
{
  int const value = state.get_rel_value(m_code);

  if (value != 0) {
    m_unused = false;
  }

  m_last_value = value;
  m_offset_x += value;

  update();
}

} // namespace evtest_qt

/* EOF */
