// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#ifndef HEADER_EVTEST_QT_EVENT_GRAPH_WINDOW_HPP
#define HEADER_EVTEST_QT_EVENT_GRAPH_WINDOW_HPP

#include <QMainWindow>
#include <QScrollArea>
#include <QTimer>

#include "event_graph_widget.hpp"
#include "fwd.hpp"

namespace evtest_qt {

class EventGraphWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit EventGraphWindow(QWidget* parent = nullptr);

  void set_device(EvdevInfo const& info);
  void clear_device();

  /** Point at the live EvdevState owned by EvtestApp (or nullptr). */
  void set_state_source(EvdevState const* state);

  /** Push one sample immediately (e.g. on EV_SYN for sharper edges). */
  void push_state(EvdevState const& state);

  EventGraphWidget* graph() { return m_graph; }

protected:
  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;

private slots:
  void on_sample_timeout();

private:
  void update_timer();

  QScrollArea* m_scroll;
  EventGraphWidget* m_graph;
  QTimer m_sample_timer;
  EvdevState const* m_state_source;
};

} // namespace evtest_qt

#endif

/* EOF */
