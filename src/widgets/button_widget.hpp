// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_BUTTON_WIDGET_HPP
#define HEADER_EVTEST_QT_BUTTON_WIDGET_HPP

#include <QWidget>

#include "fwd.hpp"

namespace evtest_qt {

class ButtonWidget : public QWidget
{
  Q_OBJECT

public:
  ButtonWidget(uint16_t code, QWidget* parent = nullptr);
  virtual ~ButtonWidget();

  QSize sizeHint() const  override { return QSize(32, 16); };

  void set_verification_mode(bool mode);

public slots:
  void on_change(EvdevState const& state);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  bool m_verification_mode;
  bool m_unused;
  uint16_t m_code;
  int32_t m_value;

private:
  ButtonWidget(ButtonWidget const&) = delete;
  ButtonWidget& operator=(ButtonWidget const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
