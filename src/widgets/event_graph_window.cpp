// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "event_graph_window.hpp"

#include <QScrollArea>

namespace evtest_qt {

EventGraphWindow::EventGraphWindow(QWidget* parent) :
  QMainWindow(parent),
  m_scroll(new QScrollArea(this)),
  m_graph(new EventGraphWidget)
{
  setWindowTitle(QStringLiteral("Event Graph"));
  setAttribute(Qt::WA_DeleteOnClose, false);

  m_scroll->setWidget(m_graph);
  m_scroll->setWidgetResizable(true);
  setCentralWidget(m_scroll);
  resize(720, 420);
}

void
EventGraphWindow::set_device(EvdevInfo const& info)
{
  m_graph->set_device(info);
  m_graph->setMinimumHeight(m_graph->sizeHint().height());
}

void
EventGraphWindow::clear_device()
{
  m_graph->clear_device();
}

void
EventGraphWindow::push_state(EvdevState const& state)
{
  m_graph->push_state(state);
}

} // namespace evtest_qt

/* EOF */
