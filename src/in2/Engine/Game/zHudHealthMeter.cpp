#include "xCommon.h"
#include "zHudHealthMeter.h"

#include "zUICustomHUD.h"

namespace zHud {

HealthMeter::HealthMeter()
{
    m_fCurValue = 0.0f;
    m_fMaxValue = 0.0f;
    m_nCharacter = 0;
}

void HealthMeter::SetCurValue(F32 value, bool defeated)
{
    if (m_fCurValue != value && m_fCurValue <= m_fMaxValue) {
        S32 character = m_nCharacter;
        if (m_fMaxValue) {
            switch (character) {
            case 1:
                pUICustomHUDManager->SetHealthRatioMRI(value / m_fMaxValue, defeated, value < m_fCurValue);
                break;
            case 2:
                pUICustomHUDManager->SetHealthRatioFRO(value / m_fMaxValue, defeated, value < m_fCurValue);
                break;
            }
        }
    }
    m_fCurValue = value;
}

void HealthMeter::SetMaxValue(F32 value)
{
    if (m_fMaxValue != value) {
        S32 character = m_nCharacter;
        if (value) {
            switch (character) {
            case 1:
                pUICustomHUDManager->SetHealthRatioMRI(m_fCurValue / value, false, true);
                break;
            case 2:
                pUICustomHUDManager->SetHealthRatioFRO(m_fCurValue / value, false, true);
                break;
            }
        }
    }
    m_fMaxValue = value;
}

void HealthMeter::reset()
{
}

void HealthMeter::setup()
{
}

void HealthMeter::destroy()
{
}

void HealthMeter::update(F32)
{
}

void HealthMeter::render()
{
}

void HealthMeter::show()
{
}

void HealthMeter::hide()
{
}

} // namespace zHud
