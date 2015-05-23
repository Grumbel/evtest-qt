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

#ifndef HEADER_BUTTON_WIDGET_HPP
#define HEADER_BUTTON_WIDGET_HPP

#include <QWidget>

class EvdevState;

class ButtonWidget : public QWidget
{
  Q_OBJECT

private:
  uint16_t m_code;
  int32_t m_value;

public:
  ButtonWidget(uint16_t code, QWidget* parent = 0);
  ~ButtonWidget();

  QSize sizeHint() const  override { return QSize(48, 24); };

public slots:
  void on_change(const EvdevState& state);

protected:
  void paintEvent(QPaintEvent* event) override;

private:
  ButtonWidget(const ButtonWidget&) = delete;
  ButtonWidget& operator=(const ButtonWidget&) = delete;
};

#endif

/* EOF */
