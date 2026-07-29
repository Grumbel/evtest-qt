// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "event_graph_window.hpp"

#include <QAction>
#include <QHideEvent>
#include <QLabel>
#include <QShowEvent>
#include <QToolBar>

#include "evdev/evdev_state.hpp"

namespace evtest_qt {

EventGraphWindow::EventGraphWindow(QWidget* parent) :
  QMainWindow(parent),
  m_graph(new EventGraphWidget),
  m_sample_timer(this),
  m_state_source(nullptr),
  m_window_combo(new QComboBox),
  m_pause_check(new QCheckBox(QStringLiteral("Pause"))),
  m_only_used_check(new QCheckBox(QStringLiteral("Used buttons only")))
{
  setWindowTitle(QStringLiteral("Event Graph"));
  setAttribute(Qt::WA_DeleteOnClose, false);

  // Graph fills the window (no fixed lane sizes / scroll needed)
  setCentralWidget(m_graph);
  resize(800, 480);

  auto* tb = addToolBar(QStringLiteral("Graph"));
  tb->setMovable(false);
  tb->addWidget(new QLabel(QStringLiteral(" History: ")));

  struct Opt { char const* label; int ms; };
  Opt opts[] = {
    { "1 s", 1000 },
    { "2 s", 2000 },
    { "5 s", 5000 },
    { "8 s", 8000 },
    { "15 s", 15000 },
    { "30 s", 30000 },
  };
  for (auto const& o : opts) {
    m_window_combo->addItem(QString::fromUtf8(o.label), o.ms);
  }
  m_window_combo->setCurrentIndex(3); // 8 s default
  tb->addWidget(m_window_combo);
  connect(m_window_combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
          this, &EventGraphWindow::on_window_changed);

  tb->addSeparator();
  tb->addWidget(m_pause_check);
  connect(m_pause_check, &QCheckBox::toggled, this, &EventGraphWindow::on_pause_toggled);

  auto* clear_act = tb->addAction(QStringLiteral("Clear"));
  connect(clear_act, &QAction::triggered, this, &EventGraphWindow::on_clear_history);

  tb->addSeparator();
  m_only_used_check->setToolTip(
    QStringLiteral("Hide button lanes that have never been pressed"));
  tb->addWidget(m_only_used_check);
  connect(m_only_used_check, &QCheckBox::toggled, this, &EventGraphWindow::on_only_used_toggled);

  m_sample_timer.setInterval(16);
  connect(&m_sample_timer, &QTimer::timeout, this, &EventGraphWindow::on_sample_timeout);
}

void
EventGraphWindow::set_device(EvdevInfo const& info)
{
  m_graph->set_device(info);
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
  if (m_state_source && isVisible() && !m_graph->is_paused()) {
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
  if (m_state_source && !m_graph->is_paused()) {
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
  if (m_state_source && !m_graph->is_paused()) {
    m_graph->push_state(*m_state_source);
  }
}

void
EventGraphWindow::on_window_changed(int index)
{
  int ms = m_window_combo->itemData(index).toInt();
  if (ms > 0) {
    m_graph->set_time_window_ms(ms);
  }
}

void
EventGraphWindow::on_pause_toggled(bool checked)
{
  m_graph->set_paused(checked);
  update_timer();
}

void
EventGraphWindow::on_clear_history()
{
  m_graph->clear_history();
}

void
EventGraphWindow::on_only_used_toggled(bool checked)
{
  m_graph->set_only_used_buttons(checked);
}

void
EventGraphWindow::update_timer()
{
  if (isVisible() && m_state_source && !m_graph->is_paused()) {
    if (!m_sample_timer.isActive()) {
      m_sample_timer.start();
    }
  } else {
    m_sample_timer.stop();
  }
}

} // namespace evtest_qt

/* EOF */
