#include "sdk.h"
#include "hooks.h"
#include "baseapi.h"

HMODULE h_mod{nullptr};
interfaces ints;
c_offsets g_offsets;
create_interface engine_factory{NULL};
create_interface client_factory{NULL};
create_interface server_factory{NULL};
create_interface vgui2_factory{NULL};
create_interface vgui_factory{NULL};
create_interface matsys_factory{NULL};

DWORD WINAPI thread(LPVOID args)
{
	try
	{
		client_factory = (create_interface_fn)get_proc_address("client.dll");
		engine_factory = (create_interface_fn)get_proc_address("engine.dll");
		vgui2_factory = (create_interface_fn)get_proc_address("vgui2.dll");
		vgui_factory = (create_interface_fn)get_proc_address("vguimatsurface.dll");
		server_factory = (create_interface_fn)get_proc_address("server.dll");
		matsys_factory = (create_interface_fn)get_proc_address("materialsystem.dll");

		ints.chlclient = static_cast<ssdk::chl_client*>(api.get_interfaces(
			"g_pClient", "VClient", "g_pClient", client_factory));
		ints.ent_list = static_cast<ssdk::entity_list*>(api.get_interfaces("g_pClientEntityList", "VClientEntityList",
		                                                                   "g_pClientEntityList", client_factory));
		ints.engine = static_cast<ssdk::engine_client*>(api.get_interfaces(
			"g_pEngine", "VEngineClient", "g_pEngine", engine_factory));
		ints.isurface = static_cast<ssdk::surface*>(api.get_interfaces("g_pSurface", "VGUI_Surface", "g_pSurface",
		                                                               vgui_factory));
		ints.model_info = static_cast<ssdk::c_model_info*>(api.get_interfaces(
			"g_pModelInfo", "VModelInfoClient", "g_pModelInfo",
			engine_factory));
		ints.panels = static_cast<ssdk::panel*>(api.get_interfaces("g_pPanels", "VGUI_Panel", "g_pPanels",
		                                                           vgui2_factory));
		ints.eng_vgui = static_cast<ssdk::engine_vgui*>(api.get_interfaces(
				"g_pEngineVGUI", "VEngineVGui", "g_pEngineVGUI", engine_factory)
		);
		ints.pinfo_manager = static_cast<ssdk::player_info_manager*>(api.get_interfaces(
			"g_pPlayerInfoManager", "PlayerInfoManager",
			"g_pPlayerInfoManager", server_factory));
		ints.renderview = static_cast<ssdk::render_view*>(api.get_interfaces(
			"g_pRenderView", "VEngineRenderView", "g_pRenderView",
			engine_factory));
		ints.i_prediction = static_cast<ssdk::prediction*>(api.get_interfaces(
			"g_pPred", "VClientPrediction", "g_pPred", client_factory));
		ints.i_model_renderer = static_cast<ssdk::model_renderer*>(api.get_interfaces(
			"g_pModelRenderer", "VEngineModel", "g_pModelRenderer",
			engine_factory));
		ints.i_material_system = static_cast<ssdk::material_system*>(api.get_interfaces(
			"g_pMatSystem", "VMaterialSystem", "g_pMatSystem",
			matsys_factory));

		DWORD_PTR cm_ptr{(reinterpret_cast<std::uintptr_t>(api.get_module("client.dll")) + 0x223670)}; 
		auto f_get_cm{(void* (__cdecl*)())cm_ptr};
		void * clientmode{f_get_cm()};
		
		DWORD_PTR dw_client_move{api.get_engine_sig("55 8B EC 83 EC 40 A1 ? ? ? ? 33 C5 89 45 FC 56 E8 ? ? ? ?")};
		DWORD_PTR dw_move_helper{api.get_client_sig("")};
		//ints.move_helper = **reinterpret_cast<ssdk::i_move_helper***>((dw_move_helper + 0x1));

		api.log_file("console.");

		// setup VMT manager and then hook respective funcs.
		auto * vmt_clientmode = new vmt_base_manager();
		auto* vmt_panels = new vmt_base_manager();
		auto* vmt_enginevgui = new vmt_base_manager();
		auto* vmt_renderview = new vmt_base_manager();
		auto* vmt_prediction = new vmt_base_manager();

		auto do_hooks = [&](void)
		{
			vmt_clientmode->init(clientmode);
			vmt_clientmode->hook_method(&hooks::createmove, 27);
			vmt_clientmode->rehook();
			vmt_enginevgui->init(ints.eng_vgui);
			vmt_enginevgui->hook_method(&hooks::paint, 14);
			vmt_enginevgui->rehook();
			vmt_renderview->init(ints.renderview);
			vmt_renderview->hook_method(&hooks::scene_end, 9);
			vmt_renderview->rehook();
			vmt_panels->init(ints.panels);
			vmt_panels->hook_method(&hooks::painttraverse, 41);
			vmt_panels->rehook();
			vmt_prediction->init(ints.i_prediction);
			vmt_prediction->hook_method(&hooks::in_prediction, 14);
			vmt_prediction->rehook();
		};

		do_hooks();

		for (;;)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
			if (GetAsyncKeyState(VK_END))
			{
				vmt_clientmode->kill();
				vmt_enginevgui->kill();
				vmt_panels->kill();
				vmt_renderview->kill();
				vmt_prediction->kill();
				api.log_file("unhooked");
				api.clear_console();
				std::this_thread::sleep_for(std::chrono::seconds(1));
				FreeLibraryAndExitThread(h_mod, 0);
			}
		}
	}
	catch (...) { api.error_box("Failed main thread."); }
	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		h_mod = hModule;
		CloseHandle(CreateThread(NULL, 0, static_cast<LPTHREAD_START_ROUTINE>(thread), NULL, 0, NULL));
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
