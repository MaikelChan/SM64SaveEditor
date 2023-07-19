#include "SaveEditorUI.h"
#include <format>

SaveEditorUI::SaveEditorUI()
{
	aboutWindow = new AboutWindow();
	popupDialog = new PopupDialog();

	currentFileName = nullptr;
	saveData = nullptr;

	showBackup = false;
}

SaveEditorUI::~SaveEditorUI()
{
	if (saveData)
	{
		delete saveData;
		saveData = nullptr;
	}

	delete popupDialog;
	popupDialog = nullptr;

	delete aboutWindow;
	aboutWindow = nullptr;
}

void SaveEditorUI::VisibilityChanged(const bool isVisible)
{
	BaseUI::VisibilityChanged(isVisible);
}

void SaveEditorUI::DoRender()
{
	BaseUI::DoRender();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load..."))
			{
				if (saveData)
				{
					delete saveData;
					saveData = nullptr;
				}

				try
				{
					saveData = SaveData::Load("D:\\Consolas\\PC\\Juegos\\Super Mario 64 - PC\\sm64_save_file.bin");
				}
				catch (const std::runtime_error& error)
				{
					popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
					popupDialog->SetIsVisible(true);
				}
			}

			if (ImGui::MenuItem("Save..."))
			{
				if (saveData)
				{
					try
					{
						SaveData::Save("D:\\Consolas\\PC\\Juegos\\Super Mario 64 - PC\\sm64_save_file.bin", saveData);
					}
					catch (const std::runtime_error& error)
					{
						popupDialog->SetMessage(MessageTypes::Error, "Error", error.what());
						popupDialog->SetIsVisible(true);
					}
				}
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Quit"))
			{
				CloseMainWindow();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("About...", NULL, aboutWindow->GetIsVisible()))
			{
				aboutWindow->ToggleIsVisible();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	if (ImGui::Begin("Save Editor", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground))
	{
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		if (saveData && ImGui::BeginTabBar("Save Slots", tab_bar_flags))
		{
			for (int s = 0; s < NUM_SAVE_SLOTS; s++)
			{
				if (ImGui::BeginTabItem(tabNames[s]))
				{
					ImGui::Checkbox("Show backup", &showBackup);
					ImGui::SameLine();

					bool fileExists = CheckboxSaveFlags("File Exists", s, showBackup, SAVE_FLAG_FILE_EXISTS);
					if (!fileExists) ImGui::BeginDisabled();

					ImGui::SameLine();
					PrintChecksum(saveData->saveSlots[s][showBackup].Checksum);

					if (ImGui::BeginTable("FlagsTable", 3))
					{
						ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("three", ImGuiTableColumnFlags_WidthFixed, WINDOW_WIDTH * 0.18f);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::SeparatorText("Unlocked Doors");
						if (ImGui::BeginTable("DoorsUnlockedTable", 2))
						{
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							CheckboxSaveFlags("Basement Door", s, showBackup, SAVE_FLAG_UNLOCKED_BASEMENT_DOOR);
							CheckboxSaveFlags("WF Door", s, showBackup, SAVE_FLAG_UNLOCKED_WF_DOOR);
							CheckboxSaveFlags("CCM Door", s, showBackup, SAVE_FLAG_UNLOCKED_CCM_DOOR);
							CheckboxSaveFlags("BITDW Door", s, showBackup, SAVE_FLAG_UNLOCKED_BITDW_DOOR);
							CheckboxSaveFlags("50 Star Door", s, showBackup, SAVE_FLAG_UNLOCKED_50_STAR_DOOR);

							ImGui::TableSetColumnIndex(1);
							CheckboxSaveFlags("Upstairs Door", s, showBackup, SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR);
							CheckboxSaveFlags("JRB Door", s, showBackup, SAVE_FLAG_UNLOCKED_JRB_DOOR);
							CheckboxSaveFlags("PSS Door", s, showBackup, SAVE_FLAG_UNLOCKED_PSS_DOOR);
							CheckboxSaveFlags("BITFS Door", s, showBackup, SAVE_FLAG_UNLOCKED_BITFS_DOOR);

							ImGui::EndTable();
						}

						ImGui::TableSetColumnIndex(1);
						ImGui::SeparatorText("Mario's Cap");
						if (ImGui::BeginTable("MarioCapTable", 2))
						{
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							CheckboxSaveFlags("Have Wing Cap", s, showBackup, SAVE_FLAG_HAVE_WING_CAP);
							CheckboxSaveFlags("Have Metal Cap", s, showBackup, SAVE_FLAG_HAVE_METAL_CAP);
							CheckboxSaveFlags("Have Vanish Cap", s, showBackup, SAVE_FLAG_HAVE_VANISH_CAP);

							ImGui::TableSetColumnIndex(1);
							CheckboxSaveFlags("Cap on Ground", s, showBackup, SAVE_FLAG_CAP_ON_GROUND);
							CheckboxSaveFlags("Cap on Klepto", s, showBackup, SAVE_FLAG_CAP_ON_KLEPTO);
							CheckboxSaveFlags("Cap on Ukiki", s, showBackup, SAVE_FLAG_CAP_ON_UKIKI);
							CheckboxSaveFlags("Cap on Mr Blizzard", s, showBackup, SAVE_FLAG_CAP_ON_MR_BLIZZARD);

							ImGui::EndTable();
						}

						ImGui::TableSetColumnIndex(2);
						ImGui::SeparatorText("Miscellaneous");
						CheckboxSaveFlags("Basement Key", s, showBackup, SAVE_FLAG_HAVE_KEY_1);
						CheckboxSaveFlags("Upstairs Key", s, showBackup, SAVE_FLAG_HAVE_KEY_2);
						CheckboxSaveFlags("Moat Drained", s, showBackup, SAVE_FLAG_MOAT_DRAINED);
						CheckboxSaveFlags("DDD Moved Back", s, showBackup, SAVE_FLAG_DDD_MOVED_BACK);

						ImGui::EndTable();
					}

					ImGui::SeparatorText("Castle Stars");

					if (ImGui::BeginTable("CastleStarsTable", 2))
					{
						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						CheckboxSaveFlags("Toad Star 1", s, showBackup, SAVE_FLAG_COLLECTED_TOAD_STAR_1);
						ImGui::SameLine();
						CheckboxSaveFlags("Toad Star 2", s, showBackup, SAVE_FLAG_COLLECTED_TOAD_STAR_2);
						ImGui::SameLine();
						CheckboxSaveFlags("Toad Star 3", s, showBackup, SAVE_FLAG_COLLECTED_TOAD_STAR_3);

						ImGui::TableSetColumnIndex(1);
						CheckboxSaveFlags("Mips Star 1", s, showBackup, SAVE_FLAG_COLLECTED_MIPS_STAR_1);
						ImGui::SameLine();
						CheckboxSaveFlags("Mips Star 2", s, showBackup, SAVE_FLAG_COLLECTED_MIPS_STAR_2);

						ImGui::EndTable();
					}

					ImGui::SeparatorText("Courses");

					static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuter;

					if (ImGui::BeginTable("CoursesTable", 11, flags))
					{
						ImGui::TableSetupScrollFreeze(0, 1);
						ImGui::TableSetupColumn("#");
						ImGui::TableSetupColumn("Name");
						ImGui::TableSetupColumn("*");
						ImGui::TableSetupColumn("*");
						ImGui::TableSetupColumn("*");
						ImGui::TableSetupColumn("*");
						ImGui::TableSetupColumn("*");
						ImGui::TableSetupColumn("*");
						ImGui::TableSetupColumn("100 Coin *");
						ImGui::TableSetupColumn("Cannon Open");
						ImGui::TableSetupColumn("Max Coins");

						ImGui::TableHeadersRow();

						for (int c = 0; c < COURSE_COUNT; c++)
						{
							ImGui::PushID(c);

							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							ImGui::Text("%i", c + 1);

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%s", courseNames[c]);

							for (int st = 0; st < MAX_STARS_PER_LEVEL; st++)
							{
								if (st >= courseStarCount[c]) continue;

								ImGui::PushID(st);

								ImGui::TableSetColumnIndex(2 + st);
								CheckboxCourseData("##Star", s, showBackup, c, 1 << st);

								ImGui::PopID();
							}

							if (courseHasCannon[c])
							{
								ImGui::TableSetColumnIndex(9);
								CheckboxCourseData("##Cannon Open", s, showBackup, c + 1, SAVE_COURSE_FLAG_STAR_CANNON_OPEN);
							}

							ImGui::TableSetColumnIndex(10);

							if (c < COURSE_STAGES_COUNT)
							{
								if (ImGui::InputScalar("##Max Coins", ImGuiDataType_U8, &saveData->saveSlots[s][showBackup].CourseCoinScores[c], NULL, NULL, "%u"))
								{
									saveData->saveSlots[s][showBackup].CalculateChecksum();
								}
							}

							ImGui::PopID();
						}
						ImGui::EndTable();
					}

					ImGui::EndTabItem();

					if (!fileExists) ImGui::EndDisabled();
				}
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				ImGui::Checkbox("Show backup", &showBackup);

				ImGui::SameLine();
				PrintChecksum(saveData->settings[showBackup].Checksum);

				ImGui::SeparatorText("Sound Mode");

				int value = saveData->settings[showBackup].soundMode;
				ImGui::RadioButton("Stereo", &value, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Mono", &value, 1);
				ImGui::SameLine();
				ImGui::RadioButton("Headset", &value, 2);
				if (saveData->settings[showBackup].soundMode != value)
				{
					saveData->settings[showBackup].soundMode = static_cast<uint16_t>(value);
					saveData->settings[showBackup].CalculateChecksum();
				}

				ImGui::SeparatorText("Language");

				value = saveData->settings[showBackup].language;
				ImGui::RadioButton("English", &value, 0);
				ImGui::SameLine();
				ImGui::RadioButton("French", &value, 1);
				ImGui::SameLine();
				ImGui::RadioButton("German", &value, 2);
				if (saveData->settings[showBackup].language != value)
				{
					saveData->settings[showBackup].language = static_cast<uint16_t>(value);
					saveData->settings[showBackup].CalculateChecksum();
				}

				ImGui::SeparatorText("Coin Ages");

				static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_BordersOuter;

				if (ImGui::BeginTable("CoinAges", 6, flags))
				{
					ImGui::TableSetupScrollFreeze(0, 1);
					ImGui::TableSetupColumn("#");
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Mario A");
					ImGui::TableSetupColumn("Mario B");
					ImGui::TableSetupColumn("Mario C");
					ImGui::TableSetupColumn("Mario D");

					ImGui::TableHeadersRow();

					for (int c = 0; c < COURSE_STAGES_COUNT; c++)
					{
						ImGui::PushID(c);

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::Text("%i", c + 1);

						ImGui::TableSetColumnIndex(1);
						ImGui::Text("%s", courseNames[c]);

						const ImU8 minRange = 0, maxRange = 4, step = 1;
						int shift = c * 2;

						for (int s = 0; s < NUM_SAVE_SLOTS; s++)
						{
							ImGui::PushID(s);

							uint8_t value = (saveData->settings[showBackup].CoinScoreAges[s] >> shift) & 0x3;

							ImGui::TableSetColumnIndex(2 + s);

							ImGui::SetNextItemWidth(80);
							if (ImGui::InputScalar("##CoinAgeCell", ImGuiDataType_U8, &value, &step, NULL, "%u"))
							{
								if (value > 3) value = 3;
								saveData->settings[showBackup].CoinScoreAges[s] &= ~(0x3 << shift);
								saveData->settings[showBackup].CoinScoreAges[s] |= value << shift;
								saveData->settings[showBackup].CalculateChecksum();
							}

							ImGui::PopID();
						}

						ImGui::PopID();
					}
					ImGui::EndTable();
				}

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	//ImGui::ShowDemoWindow();

	popupDialog->Render();
	aboutWindow->Render();
}
