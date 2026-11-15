#include "xCommon.h"
#include "zHudExperiencePointMeter.h"

#include "xEvent.h"
#include "xGlobals.h"
#include "zCommonPlayer.h"
#include "zEvent.h"
#include "zUI.h"
#include "zUICustomHUD.h"

#include <stdio.h>

namespace zHud {

ExperiencePointMeter::ExperiencePointMeter()
{
    m_nExpPoints = 0;
    m_nExpPointsToUpgrade = 0;
    m_nCharacter = 0;
}

void ExperiencePointMeter::SetExpPoints(S32 points)
{
    if (m_nExpPoints != points) {
        switch (m_nCharacter) {
        case 1:
            if (((zCommonPlayer*)CURPLAYERS.GetMrI())->GetAvailableUpgradePoints() &&
                pUICustomHUDManager->IsHUDOn() &&
                !pUICustomHUDManager->GetMRIMasterCheat()) {
                pUICustomHUDManager->PressToUpgradeMRI(true);
            }
            if (pUICustomHUDManager->IsHUDOn()) {
                pUICustomHUDManager->SetExpPointsRatioMRI((F32)points / (F32)(points + m_nExpPointsToUpgrade));
                zEntEvent(pUICustomHUDManager->GetUI(), eEventUIHUDMRIExpPointsChange);
            }
            break;
        case 2:
            if (((zCommonPlayer*)CURPLAYERS.GetFrozone())->GetAvailableUpgradePoints() &&
                pUICustomHUDManager->IsHUDOn() &&
                !pUICustomHUDManager->GetFROMasterCheat()) {
                pUICustomHUDManager->PressToUpgradeFRO(true);
            }
            if (pUICustomHUDManager->IsHUDOn()) {
                pUICustomHUDManager->SetExpPointsRatioFRO((F32)points / (F32)(points + m_nExpPointsToUpgrade));
                zEntEvent(pUICustomHUDManager->GetUI(), eEventUIHUDFROExpPointsChange);
            }
            break;
        }
    }
    m_nExpPoints = points;
}

void ExperiencePointMeter::SetExpPointsToUpgrade(S32 points)
{
    if (m_nExpPointsToUpgrade != points) {
        switch (m_nCharacter) {
        case 1:
            if (pUICustomHUDManager->IsHUDOn()) {
                pUICustomHUDManager->SetExpPointsRatioMRI((F32)m_nExpPoints / (F32)(points + m_nExpPoints));
                zEntEvent(pUICustomHUDManager->GetUI(), eEventUIHUDMRIExpPointsToUpgradeChange);
            }
            break;
        case 2:
            if (pUICustomHUDManager->IsHUDOn()) {
                pUICustomHUDManager->SetExpPointsRatioFRO((F32)m_nExpPoints / (F32)(points + m_nExpPoints));
                zEntEvent(pUICustomHUDManager->GetUI(), eEventUIHUDFROExpPointsToUpgradeChange);
            }
            break;
        }
    }
    m_nExpPointsToUpgrade = points;
}

void ExperiencePointMeter::setup()
{
}

void ExperiencePointMeter::update(F32)
{
    sprintf(text, "%d/%d", m_nExpPoints, m_nExpPoints + m_nExpPointsToUpgrade);
    if (m_nCharacter == 1) {
        if (pUICustomHUDManager->GetMRIMasterCheat()) {
            sprintf(text, "%d/%d", MAX_EXP_POINTS, MAX_EXP_POINTS);
        }
        pUICustomHUDManager->SetExpPointsTextPLY1(text);
    } else {
        if (pUICustomHUDManager->GetFROMasterCheat()) {
            sprintf(text, "%d/%d", MAX_EXP_POINTS, MAX_EXP_POINTS);
        }
        pUICustomHUDManager->SetExpPointsTextPLY2(text);
    }
}

void ExperiencePointMeter::reset()
{
}

void ExperiencePointMeter::show()
{
}

void ExperiencePointMeter::hide()
{
}

void ExperiencePointMeter::destroy()
{
}

void ExperiencePointMeter::render()
{
}

} // namespace zHud
