#include "Macros.h"

Macros::Macros(Ui::PluginWindow *parent)
{
	ui = parent;
	blog(LOG_DEBUG, "Macros startup");

	setup_actions();
	hide_all_pairs();
	setup_connections();
}
Macros::~Macros() {}
void Macros::setup_connections()
{
	connect(ui->cb_macro_action, &QComboBox::currentTextChanged, this, &Macros::slot_action_changed);
}
void Macros::setup_actions()
{
	ui->cb_macro_action->clear();
	ui->cb_macro_action->addItems(Utils::TranslateActions());
}
void Macros::show_pair(Pairs Pair)
{
	switch (Pair) {
	case Pairs::Scene:
		ui->label_obs_output_scene_2->show();
		ui->cb_obs_output_scene_2->show();
		ui->cb_obs_output_scene_2->addItems(Utils::get_scene_names());
		ui->w_scene_2->show();
		break;
	case Pairs::Source:
		ui->label_obs_output_source_2->show();
		ui->cb_obs_output_source_2->show();
		ui->cb_obs_output_source_2->addItems(Utils::get_source_names(ui->cb_obs_output_scene_2->currentText()));
		ui->w_source_2->show();
		break;
	case Pairs::Filter:
		ui->label_obs_output_filter_2->show();
		ui->cb_obs_output_filter_2->show();
		ui->cb_obs_output_filter_2->addItems(Utils::get_filter_names(ui->cb_obs_output_source_2->currentText()));
		ui->w_filter_2->show();
		break;
	case Pairs::Transition:
		ui->label_obs_output_transition_2->show();
		ui->cb_obs_output_transition_2->show();
		ui->w_transition_2->show();
		ui->cb_obs_output_transition_2->addItems(Utils::GetTransitionsList());
		break;
	case Pairs::Item:
		ui->label_obs_output_item_2->show();
		ui->cb_obs_output_item_2->show();
		ui->cb_obs_output_item_2->addItems(Utils::GetSceneItemsList(ui->cb_obs_output_scene_2->currentText()));
		ui->w_item_2->show();
		break;
	case Pairs::Audio:
		ui->cb_obs_output_audio_source_2->clear();
		ui->cb_obs_output_audio_source_2->addItems(Utils::GetAudioSourceNames());
		ui->label_obs_output_audio_source_2->show();
		ui->cb_obs_output_audio_source_2->show();
		ui->w_audio_2->show();
		break;
	case Pairs::Media:
		ui->cb_obs_output_media_source_2->clear();
		ui->cb_obs_output_media_source_2->addItems(Utils::GetMediaSourceNames());
		ui->label_obs_output_media_source_2->show();
		ui->cb_obs_output_media_source_2->show();
		ui->w_media_2->show();
		break;
	case Pairs::String:
		break;
	case Pairs::Boolean:
		break;
	case Pairs::Integer:
		ui->spinBox->show();
		ui->label_7->show();
		break;
	}
}
void Macros::hide_pair(Pairs Pair)
{
	switch (Pair) {
	case Pairs::Scene:
		ui->label_obs_output_scene_2->hide();
		ui->cb_obs_output_scene_2->hide();
		ui->cb_obs_output_scene_2->clear();
		ui->w_scene_2->hide();
		break;
	case Pairs::Source:
		ui->label_obs_output_source_2->hide();
		ui->cb_obs_output_source_2->hide();
		ui->cb_obs_output_source_2->clear();
		ui->w_source_2->hide();
		break;
	case Pairs::Filter:
		ui->label_obs_output_filter_2->hide();
		ui->cb_obs_output_filter_2->hide();
		ui->cb_obs_output_filter_2->clear();
		ui->w_filter_2->hide();
		break;
	case Pairs::Transition:
		ui->label_obs_output_transition_2->hide();
		ui->cb_obs_output_transition_2->hide();
		ui->cb_obs_output_transition_2->clear();
		ui->w_transition_2->hide();
		break;
	case Pairs::Item:
		ui->label_obs_output_item_2->hide();
		ui->cb_obs_output_item_2->hide();
		ui->cb_obs_output_item_2->clear();
		ui->w_item_2->hide();
		break;
	case Pairs::Audio:
		ui->label_obs_output_audio_source_2->hide();
		ui->cb_obs_output_audio_source_2->hide();
		ui->cb_obs_output_audio_source_2->clear();
		ui->w_audio_2->hide();
		break;
	case Pairs::Media:
		ui->label_obs_output_media_source_2->hide();
		ui->cb_obs_output_media_source_2->hide();
		ui->cb_obs_output_media_source_2->clear();
		ui->w_media_2->hide();
		break;
	case Pairs::String:
		break;
	case Pairs::Boolean:
		break;
	case Pairs::Integer:
		ui->spinBox->hide();
		ui->label_7->hide();
		break;
	}
}
void Macros::hide_all_pairs()
{
	hide_pair(Pairs::Transition);
	hide_pair(Pairs::Audio);
	hide_pair(Pairs::Media);
	hide_pair(Pairs::Filter);
	hide_pair(Pairs::Scene);
	hide_pair(Pairs::Source);
	hide_pair(Pairs::Item);
	hide_pair(Pairs::String);
	hide_pair(Pairs::Integer);
	hide_pair(Pairs::Boolean);
}
void Macros::slot_action_changed(const QString &action)
{
	hide_all_pairs();
	switch (ActionsClass::string_to_action(Utils::untranslate(action))) {
	case ActionsClass::Actions::Set_Current_Scene:
		show_pair(Pairs::Scene);
		break;
	case ActionsClass::Actions::Set_Preview_Scene:
		show_pair(Pairs::Scene);
		break;
	case ActionsClass::Actions::Enable_Source_Filter:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		show_pair(Pairs::Filter);
		break;
	case ActionsClass::Actions::Disable_Source_Filter:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		show_pair(Pairs::Filter);
		break;
	case ActionsClass::Actions::Set_Gain_Filter:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		show_pair(Pairs::Filter);
		break;
	case ActionsClass::Actions::Toggle_Source_Filter:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		show_pair(Pairs::Filter);
		break;
	case ActionsClass::Actions::Reset_Scene_Item:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		break;
	case ActionsClass::Actions::Set_Scene_Item_Render:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		show_pair(Pairs::Item);
		break;
	case ActionsClass::Actions::Set_Scene_Item_Position:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Item);
		break;
	case ActionsClass::Actions::Set_Scene_Item_Transform:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Item);
		break;
	case ActionsClass::Actions::Set_Scene_Item_Crop:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Item);
		break;
	case ActionsClass::Actions::Set_Scene_Transition_Override:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Transition);
		break;
	case ActionsClass::Actions::Set_Current_Transition:
		show_pair(Pairs::Transition);
		break;
	case ActionsClass::Actions::Set_Volume:
		show_pair(Pairs::Audio);
		break;
	case ActionsClass::Actions::Set_Mute:
		show_pair(Pairs::Audio);
		break;
	case ActionsClass::Actions::Toggle_Mute:
		show_pair(Pairs::Audio);
		break;
	case ActionsClass::Actions::Set_Source_Filter_Visibility:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		show_pair(Pairs::Filter);
		break;
	case ActionsClass::Actions::Take_Source_Screenshot:
		show_pair(Pairs::Source);
		show_pair(Pairs::Scene);
		break;
	case ActionsClass::Actions::Play_Pause_Media:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Restart_Media:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Stop_Media:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Next_Media:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Previous_Media:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Set_Media_Time:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Scrub_Media:
		show_pair(Pairs::Media);
		break;
	case ActionsClass::Actions::Toggle_Source_Visibility:
		show_pair(Pairs::Scene);
		show_pair(Pairs::Source);
		break;
	default:
		hide_all_pairs();
		break;
	}
}
