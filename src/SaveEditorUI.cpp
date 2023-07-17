#include "SaveEditorUI.h"
#include "imgui/imgui.h"
#include "main.h"

SaveEditorUI::SaveEditorUI()
{
	aboutWindow = new AboutWindow();

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
			if (ImGui::MenuItem("Load...", "CTRL+L"))
			{
				if (saveData) delete saveData;
				saveData = new SaveData();
				saveData->Load("D:\\Consolas\\PC\\Juegos\\Super Mario 64 - PC\\sm64_save_file.bin");
			}
			if (ImGui::MenuItem("Save...", "CTRL+S")) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit", "CTRL+Q"))
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
			for (int s = 0; s < NUM_SAVE_FILES; s++)
			{
				if (ImGui::BeginTabItem(tabNames[s]))
				{
					ImGui::Checkbox("File Exists", &saveData->saveSlots[s].FileExists);

					if(!saveData->saveSlots[s].FileExists) ImGui::BeginDisabled();

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
							ImGui::Text(courseNames[c]);

							ImGui::TableSetColumnIndex(2);
							ImGui::Checkbox("##Star 1", &saveData->saveSlots[s].CourseStars[c].Star1);

							ImGui::TableSetColumnIndex(3);
							ImGui::Checkbox("##Star 2", &saveData->saveSlots[s].CourseStars[c].Star2);

							ImGui::TableSetColumnIndex(4);
							ImGui::Checkbox("##Star 3", &saveData->saveSlots[s].CourseStars[c].Star3);

							ImGui::TableSetColumnIndex(5);
							ImGui::Checkbox("##Star 4", &saveData->saveSlots[s].CourseStars[c].Star4);

							ImGui::TableSetColumnIndex(6);
							ImGui::Checkbox("##Star 5", &saveData->saveSlots[s].CourseStars[c].Star5);

							ImGui::TableSetColumnIndex(7);
							ImGui::Checkbox("##Star 6", &saveData->saveSlots[s].CourseStars[c].Star6);

							ImGui::TableSetColumnIndex(8);
							ImGui::Checkbox("##100 Coin Star", &saveData->saveSlots[s].CourseStars[c].HundredCoinStar);

							ImGui::TableSetColumnIndex(9);
							ImGui::Checkbox("##Cannon Open", &saveData->saveSlots[s].CourseStars[c].CannonOpen);

							const ImU8 u8_min = 0, u8_max = 255;

							ImGui::TableSetColumnIndex(10);
							ImGui::InputScalar("##Max Coins", ImGuiDataType_U8, &saveData->saveSlots[s].CourseStars[c].MaxCoins, NULL, NULL, "%u");

							ImGui::PopID();
						}
						ImGui::EndTable();
					}

					ImGui::EndTabItem();

					if (!saveData->saveSlots[s].FileExists) ImGui::EndDisabled();
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();

	ImGui::ShowDemoWindow();

	aboutWindow->Render();
}
