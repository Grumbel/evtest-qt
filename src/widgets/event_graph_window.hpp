// SPDX-FileCopyrightText: 2015-2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_EVTEST_QT_EVENT_GRAPH_WINDOW_HPP
#define HEADER_EVTEST_QT_EVENT_GRAPH_WINDOW_HPP

#include <QCheckBox>
#include <QComboBox>
#include <QMainWindow>
#include <QToolBar>
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
  void set_state_source(EvdevState const* state);
  void push_state(EvdevState const& state);

  EventGraphWidget* graph() { return m_graph; }

protected:
  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;

private slots:
  void on_sample_timeout();
  void on_window_changed(int index);
  void on_pause_toggled(bool checked);
  void on_clear_history();
  void on_only_used_toggled(bool checked);

private:
  void update_timer();

  EventGraphWidget* m_graph;
  QTimer m_sample_timer;
  EvdevState const* m_state_source;

  QComboBox* m_window_combo;
  QCheckBox* m_pause_check;
  QCheckBox* m_only_used_check;
};

} // namespace evtest_qt

#endif

/* EOF */
