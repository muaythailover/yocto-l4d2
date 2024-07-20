#pragma once

#include "sdk.h"

namespace hooks
{
	bool __fastcall createmove(PVOID ClientMode, int edx, float input_sample_frametime,
	                           handle_structs::player_related::user_cmd * cmd);
	void __fastcall painttraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);

	namespace pt
	{
		void intro();
	};

	void __fastcall paint(PVOID engine, int edx, int mode);
	void __fastcall scene_end(PVOID renderview, int edx);
	bool __fastcall in_prediction(PVOID iprediction, int edx);

	class cl_move
	{
	public:
		typedef void (*client_move_fn)(float a, bool b);
		client_move_fn o_client_move{NULL};
		void client_move(float a, bool b);
	};
}
