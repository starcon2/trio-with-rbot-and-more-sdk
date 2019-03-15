#pragma once
#include "..\Utils\GlobalVars.h"
#include "..\Settings.h"

class Misc
{
public:
    void OnCreateMove()
    {
        this->pCmd   = g::pCmd;
        this->pLocal = g::pLocalEntity;

        if (g_Settings.misc.bBhopEnabled)
            this->DoBhop();

		if (g_Settings.misc.bAutoStrafer)
			this->AutoStrafer();
        // sum future shit
    };
private:
    CUserCmd*     pCmd;
    C_BaseEntity* pLocal;

	__forceinline static float NormalizeAngle(float angle) {
		if (angle > 180.f || angle < -180.f) {
			auto epic = roundf(fabsf(angle / 360.f));

			if (angle < 0.f)
				angle = angle + 360.f * epic;
			else
				angle = angle - 360.f * epic;
		}
		return angle;
	}

	void AutoStrafer() {

		static float circle_yaw = 0.f,
			old_yaw = 0.f;

		if (!g_Settings.misc.bAutoStrafer)
			return;

		if (this->pLocal->GetMoveType() == MoveType_t::MOVETYPE_LADDER)
			return;

		if (!(this->pLocal->GetFlags() & FL_ONGROUND))
		{
			float yaw_delta = NormalizeAngle(pCmd->viewangles.y - old_yaw);

			circle_yaw = old_yaw = pCmd->viewangles.y;

			if (yaw_delta > 0.f)
				pCmd->sidemove = -450.f;
			else if (yaw_delta < 0.f)
				pCmd->sidemove = 450.f;
		}
	}

    void DoBhop() const
    {
        if (this->pLocal->GetMoveType() == MoveType_t::MOVETYPE_LADDER)
            return;

        static bool bLastJumped = false;
        static bool bShouldFake = false;

        if (!bLastJumped && bShouldFake)
        {
            bShouldFake = false;
            pCmd->buttons |= IN_JUMP;
        }
        else if (pCmd->buttons & IN_JUMP)
        {
            if (pLocal->GetFlags() & FL_ONGROUND)
                bShouldFake = bLastJumped = true;
            else 
            {
                pCmd->buttons &= ~IN_JUMP;
                bLastJumped = false;
            }
        }
        else
            bShouldFake = bLastJumped = false;
    }
};

extern Misc g_Misc;
