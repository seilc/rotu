#ifndef ZUICUSTOMHUD_H
#define ZUICUSTOMHUD_H

#include "zUICustom.h"

class zUICustomHUDManager : public zUICustom
{
public:
    bool GetMRIMasterCheat();
    bool GetFROMasterCheat();
    void SetMRIMasterCheat(bool);
    void SetFROMasterCheat(bool);
    void PressToUpgradeMRI(bool);
    void PressToUpgradeFRO(bool);
    void SetExpPointsRatioMRI(F32);
    void SetExpPointsRatioFRO(F32);
    void SetHealthRatioMRI(F32, bool, bool);
    void SetHealthRatioFRO(F32, bool, bool);
    void SetExpPointsTextPLY1(const char*);
    void SetExpPointsTextPLY2(const char*);

    bool IsHUDOn() const
    {
        return hudOn;
    }

private:
    U8 unk8[0x50];
    bool hudOn;
};

extern zUICustomHUDManager* pUICustomHUDManager;

#endif
