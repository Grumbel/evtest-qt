// evtest-qt - A graphical joystick tester
// Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

#include "event_graph_window.hpp"

#include <QHideEvent>
#include <QShowEvent>

#include "evdev/evdev_state.hpp"

namespace evtest_qt {

EventGraphWindow::EventGraphWindow(QWidget* parent) :
  QMainWindow(parent),
  m_scroll(new QScrollArea(this)),
  m_graph(new EventGraphWidget),
  m_sample_timer(this),
  m_state_source(nullptr)
{
  setWindowTitle(QStringLiteral("Event Graph"));
  setAttribute(Qt::WA_DeleteOnClose, false);

  m_scroll->setWidget(m_graph);
  m_scroll->setWidgetResizable(true);
  setCentralWidget(m_scroll);
  resize(720, 420);

  // ~60 Hz continuous sampling so held axes still advance the timeline
  m_sample_timer.setInterval(16);
  connect(&m_sample_timer, &QTimer::timeout, this, &EventGraphWindow::on_sample_timeout);
}

void
EventGraphWindow::set_device(EvdevInfo const& info)
{
  m_graph->set_device(info);
  m_graph->setMinimumHeight(m_graph->sizeHint().height());
  m_graph->updateGeometry();
  update_timer();
}

void
EventGraphWindow::clear_device()
{
  m_state_source = nullptr;
  m_graph->clear_device();
  update_timer();
}

void
EventGraphWindow::set_state_source(EvdevState const* state)
{
  m_state_source = state;
  update_timer();
  if (m_state_source && isVisible()) {
    m_graph->push_state(*m_state_source);
  }
}

void
EventGraphWindow::push_state(EvdevState const& state)
{
  m_graph->push_state(state);
}

void
EventGraphWindow::showEvent(QShowEvent* event)
{
  QMainWindow::showEvent(event);
  update_timer();
  if (m_state_source) {
    m_graph->push_state(*m_state_source);
  }
}

void
EventGraphWindow::hideEvent(QHideEvent* event)
{
  QMainWindow::hideEvent(event);
  m_sample_timer.stop();
}

void
EventGraphWindow::on_sample_timeout()
{
  if (m_state_source) {
    m_graph->push_state(*m_state_source);
  }
}

void
EventGraphWindow::update_timer()
{
  if (isVisible() && m_state_source) {
    if (!m_sample_timer.isActive()) {
      m_sample_timer.start();
    }
  } else {
    m_sample_timer.stop();
  }
}

} // namespace evtest_qt

/* EOF */
