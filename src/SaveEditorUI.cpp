#include "SaveEditorUI.h"
#include "MainUI.h"
#include "SaveData.h"

SaveEditorUI::SaveEditorUI(const MainUI* mainUI) : BaseUI(mainUI)
{
	SaveEditorUI::mainUI = mainUI;
	showBackup = false;
}

SaveEditorUI::~SaveEditorUI()
{

}

void SaveEditorUI::VisibilityChanged(const bool isVisible)
{
	BaseUI::VisibilityChanged(isVisible);
}

void SaveEditorUI::DoRender()
{
	BaseUI::DoRender();

	SaveData* saveData = mainUI->GetSaveData();

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
				// Mario A (255 \u0101)###Mario A\0   (27 characters)
				// "###tabNames[s] is used as tab id, to prevent regenerating
				// the whole window when updating the number of stars
				char tabName[27];
				uint8_t stars = saveData->saveSlots[s][showBackup].GetTotalStars();
				snprintf(tabName, 27, u8"%s (%u \u0101)###%s", tabNames[s], stars, tabNames[s]);

				if (ImGui::BeginTabItem(tabName))
				{
					if (showBackup) ImGui::BeginDisabled();
					bool fileExists = CheckboxSaveFlags("File Exists", s, showBackup, SAVE_FLAG_FILE_EXISTS);
					if (!fileExists) ImGui::BeginDisabled();

					ImGui::SameLine();
					PrintChecksum(saveData->saveSlots[s][showBackup].Checksum);

					if (ImGui::BeginTable("FlagsTable", 3))
					{
						ImGui::TableSetupColumn("DoorColumn", ImGuiTableColumnFlags_WidthStretch, 0.8f);
						ImGui::TableSetupColumn("CapColumn", ImGuiTableColumnFlags_WidthStretch);
						ImGui::TableSetupColumn("MiscColumn", ImGuiTableColumnFlags_WidthStretch, 0.4f);

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

							ImGui::SetNextItemWidth(24);
							if (ImGui::InputScalar("##Cap Level", ImGuiDataType_U8, &saveData->saveSlots[s][showBackup].CapLevel, NULL, NULL, "%u"))
							{
								if (saveData->saveSlots[s][showBackup].CapLevel > COURSE_STAGES_COUNT_INTERNAL)
									saveData->saveSlots[s][showBackup].CapLevel = COURSE_STAGES_COUNT_INTERNAL;
								saveData->saveSlots[s][showBackup].UpdateChecksum();
							}

							ImGui::SameLine();

							ImGui::SetNextItemWidth(24);
							if (ImGui::InputScalar("Cap Level/Area", ImGuiDataType_U8, &saveData->saveSlots[s][showBackup].CapArea, NULL, NULL, "%u"))
							{
								saveData->saveSlots[s][showBackup].UpdateChecksum();
							}

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
						ImGui::TableSetupColumn(u8"\u0101");
						ImGui::TableSetupColumn(u8"\u0101");
						ImGui::TableSetupColumn(u8"\u0101");
						ImGui::TableSetupColumn(u8"\u0101");
						ImGui::TableSetupColumn(u8"\u0101");
						ImGui::TableSetupColumn(u8"\u0101");
						ImGui::TableSetupColumn(u8"100 Coin \u0101");
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

							for (int st = 0; st < courseStarCount[c]; st++)
							{
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
									saveData->saveSlots[s][showBackup].UpdateChecksum();
								}
							}

							ImGui::PopID();
						}
						ImGui::EndTable();
					}

					if (!fileExists) ImGui::EndDisabled();
					if (showBackup) ImGui::EndDisabled();

					ImGui::EndTabItem();
				}
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				if (showBackup) ImGui::BeginDisabled();

				ImGui::Dummy(ImVec2(21, 21));
				ImGui::SameLine();
				ImGui::AlignTextToFramePadding();
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
					saveData->settings[showBackup].UpdateChecksum();
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
					saveData->settings[showBackup].UpdateChecksum();
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

						const ImU8 step = 1;
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
								saveData->settings[showBackup].UpdateChecksum();
							}

							ImGui::PopID();
						}

						ImGui::PopID();
					}
					ImGui::EndTable();
				}

				if (showBackup) ImGui::EndDisabled();

				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

bool SaveEditorUI::CheckboxSaveFlags(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint32_t flag) const
{
	SaveData* saveData = mainUI->GetSaveData();

	bool value = saveData->saveSlots[saveSlot][copyIndex].GetFlag(flag);

	if (ImGui::Checkbox(label, &value))
	{
		saveData->saveSlots[saveSlot][copyIndex].SetFlag(flag, value);
		saveData->saveSlots[saveSlot][copyIndex].UpdateChecksum();
	}

	return value;
}

bool SaveEditorUI::CheckboxCourseData(const char* label, const uint8_t saveSlot, const uint8_t copyIndex, const uint8_t courseIndex, const uint8_t flag) const
{
	SaveData* saveData = mainUI->GetSaveData();

	bool value = saveData->saveSlots[saveSlot][copyIndex].GetCourseDataFlag(courseIndex, flag);

	if (ImGui::Checkbox(label, &value))
	{
		saveData->saveSlots[saveSlot][copyIndex].SetCourseDataFlag(courseIndex, flag, value);
		saveData->saveSlots[saveSlot][copyIndex].UpdateChecksum();
	}

	return value;
}

void SaveEditorUI::PrintChecksum(const uint16_t checksum) const
{
	ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("Checksum: 0xFFFF").x - 32);
	ImGui::TextColored(ImVec4(0.7f, 0.5f, 0.6f, 1.0f), "Checksum: 0x%x", checksum);
}
