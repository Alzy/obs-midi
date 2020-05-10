/*
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <obs-frontend-api.h>

#include "settings-midi-map.h"
#include <QtWidgets/qdialogbuttonbox.h>

#define CHANGE_ME "changeme"

SettingsMidiMap::SettingsMidiMap(QWidget *parent)
	:
	QDialog(parent, Qt::Dialog), ui(new Ui::SettingsMidiMap)
{
	ui->setupUi(this);
}


void SettingsMidiMap::ToggleShowHide()
{
	if (!isVisible())
		setVisible(true);
	else
		setVisible(false);
}



void SettingsMidiMap::FormAccepted()
{
	return;
}

SettingsMidiMap::~SettingsMidiMap()
{
	delete ui;
}
