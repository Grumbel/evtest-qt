// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "axis_widget.hpp"

#include <algorithm>

#include <QPainter>

#include "evdev/evdev_enum.hpp"
#include "evdev/evdev_state.hpp"

namespace evtest_qt {

AxisWidget::AxisWidget(uint16_t code, int min, int max, QWidget* parent_) :
  QWidget(parent_),
  m_verification_mode(false),
  m_has_used_range(false),
  m_used_min(0),
  m_used_max(0),
  m_code(code),
  m_min(min),
  m_max(max),
  m_value(0)
{
  setToolTip(QString("%1  min:%2 max:%3")
             .arg(QString::fromStdString(evdev_abs_name(m_code)))
             .arg(m_min)
             .arg(m_max));
}

AxisWidget::~AxisWidget()
{
}

void
AxisWidget::set_verification_mode(bool value)
{
  m_verification_mode = value;

  m_has_used_range = false;
  m_used_min = 0;
  m_used_max = 0;

  update();
}

void
AxisWidget::on_change(EvdevState const& state)
{
  int old_value = m_value;
  m_value = state.get_abs_value(m_code);

  if (!m_has_used_range) {
    m_used_min = m_value;
    m_used_max = m_value;
    m_has_used_range = true;
  } else {
    m_used_min = std::min(m_used_min, m_value);
    m_used_max = std::max(m_used_max, m_value);
  }

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
    if (m_has_used_range && m_used_min != m_used_max) {
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

  // raw value text
  painter.setPen(QColor(0, 0, 0));
  QString value_text = QString::number(m_value);
  QFontMetrics fm(painter.font());
  int text_width = fm.horizontalAdvance(value_text);
  int text_x = value_pos + 4;
  if (text_x + text_width > width() - 2) {
    text_x = value_pos - text_width - 4;
  }
  if (text_x < 2) {
    text_x = 2;
  }
  painter.drawText(text_x, 0, text_width + 2, height(),
                   Qt::AlignVCenter | Qt::AlignLeft, value_text);

  // box outline
  painter.setPen(QColor(0, 0, 0));
  painter.drawRect(0, 0, width(), height());
}

} // namespace evtest_qt

/* EOF */
