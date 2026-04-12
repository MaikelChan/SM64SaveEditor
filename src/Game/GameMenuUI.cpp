#include "GameMenuUI.h"

#include <imgui/imgui.h>

#include "../MainUI.h"
#include "SaveData.h"
#include "SaveFile.h"
#include "SaveEditorUI.h"

GameMenuUI::GameMenuUI(Window* window, BaseUI* parentUi, SaveEditorUI* saveEditorUi) : BaseUI(window, parentUi)
{
	this->mainUi = (MainUI*)parentUi;
	this->saveEditorUi = saveEditorUi;
}

GameMenuUI::~GameMenuUI()
{
}

void GameMenuUI::VisibilityChanged(const bool _isVisible)
{
}

void GameMenuUI::DoRender()
{
	SaveData* saveData = mainUi->GetSaveFile()->GetSaveData();

	if (saveData && ImGui::BeginMenu("Tools"))
	{
		for (uint8_t s = 0; s < NUM_SAVE_SLOTS; s++)
		{
			if (ImGui::BeginMenu(tabNames[s]))
			{
				if (ImGui::MenuItem("100% Complete"))
				{
					CompleteSlot(s);
				}

				ImGui::Separator();

				if (ImGui::BeginMenu("Copy"))
				{
					for (uint8_t ds = 0; ds < NUM_SAVE_SLOTS; ds++)
					{
						if (s == ds) continue;

						char menuName[27];
						snprintf(menuName, 27, "To %s", tabNames[ds]);

						if (ImGui::MenuItem(menuName))
						{
							CopySlot(s, ds);
						}
					}

					ImGui::EndMenu();
				}

				if (ImGui::MenuItem("Delete"))
				{
					DeleteSlot(s);
				}

				ImGui::EndMenu();
			}
		}

		ImGui::Separator();

		if (ImGui::MenuItem("Show backup data", NULL, saveEditorUi->showBackup))
		{
			saveEditorUi->showBackup = !saveEditorUi->showBackup;
		}

		ImGui::EndMenu();
	}
}

void GameMenuUI::CompleteSlot(const uint8_t slotIndex) const
{
	SaveData* saveData = mainUi->GetSaveFile()->GetSaveData();

	memset(&saveData->saveSlots[slotIndex][0], 0, SAVE_SLOT_SIZE);

	saveData->saveSlots[slotIndex][0].Flags = 0x1f10ffcf;

	for (int c = 0; c < COURSE_COUNT; c++)
	{
		for (int st = 0; st < courseStarCount[c]; st++)
		{
			saveData->saveSlots[slotIndex][0].CourseData[c] |= (1 << st);
		}

		if (courseHasCannon[c])
		{
			saveData->saveSlots[slotIndex][0].CourseData[c + 1] |= SAVE_COURSE_FLAG_STAR_CANNON_OPEN;
		}
	}

	for (int c = 0; c < COURSE_STAGES_COUNT; c++)
	{
		saveData->saveSlots[slotIndex][0].CourseCoinScores[c] = 100;
	}

	saveData->saveSlots[slotIndex][0].Magic = SAVE_SLOT_MAGIC_LE;
	saveData->saveSlots[slotIndex][0].UpdateChecksum();
}

void GameMenuUI::CopySlot(const uint8_t originSlotIndex, const uint8_t destinationSlotIndex) const
{
	SaveData* saveData = mainUi->GetSaveFile()->GetSaveData();

	std::copy(&saveData->saveSlots[originSlotIndex][0], &saveData->saveSlots[originSlotIndex][0] + 1, &saveData->saveSlots[destinationSlotIndex][0]);
}

void GameMenuUI::DeleteSlot(const uint8_t slotIndex) const
{
	SaveData* saveData = mainUi->GetSaveFile()->GetSaveData();

	memset(&saveData->saveSlots[slotIndex][0], 0, SAVE_SLOT_SIZE);
	saveData->saveSlots[slotIndex][0].Magic = SAVE_SLOT_MAGIC_LE;
	saveData->saveSlots[slotIndex][0].UpdateChecksum();
}
