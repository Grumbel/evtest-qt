// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_AXIS_WIDGET_HPP
#define HEADER_EVTEST_QT_AXIS_WIDGET_HPP

#include <QWidget>

#include <stdint.h>

#include "fwd.hpp"

namespace evtest_qt {

class AxisWidget : public QWidget
{
  Q_OBJECT

public:
  AxisWidget(uint16_t code, int min, int max, QWidget* parent = nullptr);
  virtual ~AxisWidget();

  QSize sizeHint() const override { return QSize(128, 16); };

  void set_verification_mode(bool value);

public slots:
  void on_change(EvdevState const& state);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  bool m_verification_mode;

  bool m_has_used_range;
  int m_used_min;
  int m_used_max;

  uint16_t m_code;
  int m_min;
  int m_max;
  int m_value;
};

} // namespace evtest_qt

#endif

/* EOF */
