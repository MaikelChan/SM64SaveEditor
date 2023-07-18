#include "SaveEditorUI.h"
#include <format>
#include "imgui/imgui.h"
#include "main.h"

SaveEditorUI::SaveEditorUI()
{
	aboutWindow = new AboutWindow();
	popupDialog = new PopupDialog();

	currentFileName = nullptr;
	saveData = nullptr;
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
					ImGui::Checkbox("File Exists", &saveData->saveSlots[s].FileExists);

					if (!saveData->saveSlots[s].FileExists) ImGui::BeginDisabled();

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
							ImGui::Checkbox("Basement Door", &saveData->saveSlots[s].BasementDoorUnlocked);
							ImGui::Checkbox("WF Door", &saveData->saveSlots[s].WFDoorUnlocked);
							ImGui::Checkbox("CCM Door", &saveData->saveSlots[s].CCMDoorUnlocked);
							ImGui::Checkbox("BITDW Door", &saveData->saveSlots[s].BITDWDoorUnlocked);
							ImGui::Checkbox("50 Star Door", &saveData->saveSlots[s].FiftyStarDoorUnlocked);

							ImGui::TableSetColumnIndex(1);
							ImGui::Checkbox("Upstairs Door", &saveData->saveSlots[s].UpstairsDoorUnlocked);
							ImGui::Checkbox("JRB Door", &saveData->saveSlots[s].JRBDoorUnlocked);
							ImGui::Checkbox("PSS Door", &saveData->saveSlots[s].PSSDoorUnlocked);
							ImGui::Checkbox("BITFS Door", &saveData->saveSlots[s].BITSDoorUnlocked);

							ImGui::EndTable();
						}

						ImGui::TableSetColumnIndex(1);
						ImGui::SeparatorText("Mario's Cap");
						if (ImGui::BeginTable("MarioCapTable", 2))
						{
							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							ImGui::Checkbox("Have Wing Cap", &saveData->saveSlots[s].HaveWingCap);
							ImGui::Checkbox("Have Metal Cap", &saveData->saveSlots[s].HaveMetalCap);
							ImGui::Checkbox("Have Vanish Cap", &saveData->saveSlots[s].HaveVanishCap);

							ImGui::TableSetColumnIndex(1);
							ImGui::Checkbox("Cap on Ground", &saveData->saveSlots[s].CapOnGround);
							ImGui::Checkbox("Cap on Klepto", &saveData->saveSlots[s].CapOnKlepto);
							ImGui::Checkbox("Cap on Ukiki", &saveData->saveSlots[s].CapOnUkiki);
							ImGui::Checkbox("Cap on Mr Blizzard", &saveData->saveSlots[s].CapOnMrBlizzard);

							ImGui::EndTable();
						}

						ImGui::TableSetColumnIndex(2);
						ImGui::SeparatorText("Miscellaneous");
						ImGui::Checkbox("Basement Key", &saveData->saveSlots[s].HaveKey1);
						ImGui::Checkbox("Upstairs Key", &saveData->saveSlots[s].HaveKey2);
						ImGui::Checkbox("Moat Drained", &saveData->saveSlots[s].MoatDrained);
						ImGui::Checkbox("DDD Moved Back", &saveData->saveSlots[s].DDDMovedBack);

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

							ImGui::TableSetColumnIndex(2);
							ImGui::Checkbox("##Star 1", &saveData->saveSlots[s].Courses[c].Star1);

							ImGui::TableSetColumnIndex(3);
							ImGui::Checkbox("##Star 2", &saveData->saveSlots[s].Courses[c].Star2);

							ImGui::TableSetColumnIndex(4);
							ImGui::Checkbox("##Star 3", &saveData->saveSlots[s].Courses[c].Star3);

							ImGui::TableSetColumnIndex(5);
							ImGui::Checkbox("##Star 4", &saveData->saveSlots[s].Courses[c].Star4);

							ImGui::TableSetColumnIndex(6);
							ImGui::Checkbox("##Star 5", &saveData->saveSlots[s].Courses[c].Star5);

							ImGui::TableSetColumnIndex(7);
							ImGui::Checkbox("##Star 6", &saveData->saveSlots[s].Courses[c].Star6);

							ImGui::TableSetColumnIndex(8);
							//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth(8) / 2) - (ImGui::GetItemRectSize().x / 2));
							ImGui::Checkbox("##100 Coin Star", &saveData->saveSlots[s].Courses[c].HundredCoinStar);

							ImGui::TableSetColumnIndex(9);
							//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth(9) / 2) - (ImGui::GetItemRectSize().x / 2));
							ImGui::Checkbox("##Cannon Open", &saveData->saveSlots[s].Courses[c].CannonOpen);

							const ImU8 u8_min = 0, u8_max = 255;

							ImGui::TableSetColumnIndex(10);
							//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth(10) / 2) - (ImGui::CalcItemWidth() / 2));
							ImGui::InputScalar("##Max Coins", ImGuiDataType_U8, &saveData->saveSlots[s].Courses[c].MaxCoins, NULL, NULL, "%u");

							ImGui::PopID();
						}
						ImGui::EndTable();
					}

					ImGui::EndTabItem();

					if (!saveData->saveSlots[s].FileExists) ImGui::EndDisabled();
				}
			}

			if (ImGui::BeginTabItem("Settings"))
			{
				ImGui::SeparatorText("Sound Mode");

				int value = saveData->settings.soundMode;
				ImGui::RadioButton("Stereo", &value, 0);
				ImGui::SameLine();
				ImGui::RadioButton("Mono", &value, 1);
				ImGui::SameLine();
				ImGui::RadioButton("Headset", &value, 2);
				saveData->settings.soundMode = static_cast<uint16_t>(value);

				ImGui::SeparatorText("Language");

				value = saveData->settings.language;
				ImGui::RadioButton("English", &value, 0);
				ImGui::SameLine();
				ImGui::RadioButton("French", &value, 1);
				ImGui::SameLine();
				ImGui::RadioButton("German", &value, 2);
				saveData->settings.language = static_cast<uint16_t>(value);

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

							uint8_t value = (saveData->settings.CoinScoreAges[s] >> shift) & 0x3;

							ImGui::TableSetColumnIndex(2 + s);

							ImGui::SetNextItemWidth(80);
							if (ImGui::InputScalar("##CoinAgeCell", ImGuiDataType_U8, &value, &step, NULL, "%u"))
							{
								if (value > 3) value = 3;
								saveData->settings.CoinScoreAges[s] &= ~(0x3 << shift);
								saveData->settings.CoinScoreAges[s] |= value << shift;
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

	ImGui::ShowDemoWindow();

	popupDialog->Render();
	aboutWindow->Render();
}
