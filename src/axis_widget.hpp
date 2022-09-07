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

#ifndef HEADER_EVTEST_QT_AXIS_WIDGET_HPP
#define HEADER_EVTEST_QT_AXIS_WIDGET_HPP

#include <QWidget>

#include <stdint.h>

#include "fwd.hpp"

namespace evtest_qt {

class AxisWidget : public QWidget
{
  Q_OBJECT

private:
  uint16_t m_code;
  int m_min;
  int m_max;
  int m_value;

public:
  AxisWidget(uint16_t code, int min, int max, QWidget* parent = nullptr);
  virtual ~AxisWidget();

  QSize sizeHint() const  override { return QSize(128, 16); };

public slots:
  void set_axis_pos(int v);
  void on_change(EvdevState const& state);

protected:
  void paintEvent(QPaintEvent* event) override;
};

} // namespace evtest_qt

#endif

/* EOF */
