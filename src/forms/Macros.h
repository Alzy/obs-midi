#pragma once

#include <qobject.h>
#include "ui_settings-dialog.h"
#include "../utils.h"
class Macros : public QObject {
	Q_OBJECT
public:
	Macros(Ui::PluginWindow *pw);
	~Macros();
	void setup_connections();
	void setup_actions();
	void show_pair(Pairs pair);
	void hide_pair(Pairs pair);
	void hide_all_pairs();
public slots:
	void slot_action_changed(const QString &action);

private:
	Ui::PluginWindow *ui;
};
