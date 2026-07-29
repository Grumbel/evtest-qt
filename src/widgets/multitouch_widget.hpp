// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_MULTITOUCH_WIDGET_HPP
#define HEADER_EVTEST_QT_MULTITOUCH_WIDGET_HPP

#include <QWidget>

#include "evdev/evdev_state.hpp"

namespace evtest_qt {

class MultitouchWidget : public QWidget
{
  Q_OBJECT

public:
  MultitouchWidget(QWidget* parent_ = nullptr);
  virtual ~MultitouchWidget();

  QSize sizeHint() const  override { return QSize(400, 225); };

  void set_verification_mode(bool value);

public slots:
  void on_change(EvdevState const& state);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  bool m_verification_mode;
  int m_max_x;
  int m_max_y;
  std::vector<MultitouchState> m_mt_states;
  std::vector<QPoint> m_history;

private:
  MultitouchWidget(MultitouchWidget const&) = delete;
  MultitouchWidget& operator=(MultitouchWidget const&) = delete;
};

} // namespace evtest_qt

#endif

/* EOF */
