// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_REL_WIDGET_HPP
#define HEADER_EVTEST_QT_REL_WIDGET_HPP

#include <QWidget>
#include <stdint.h>

#include "fwd.hpp"

namespace evtest_qt {

class RelWidget : public QWidget
{
  Q_OBJECT

public:
  RelWidget(uint16_t code, QWidget* parent = nullptr);
  virtual ~RelWidget();

  QSize sizeHint() const  override { return QSize(128, 16); };

  void set_verification_mode(bool value);

public slots:
  void on_change(EvdevState const& state);

protected:
  void paintEvent(QPaintEvent* ev) override;

private:
  bool m_verification_mode;
  bool m_unused;

  uint16_t m_code;
  int m_offset_x;
  int m_last_value;

private:
  RelWidget(RelWidget const&) = delete;
  RelWidget& operator=(RelWidget const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
