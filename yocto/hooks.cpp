#include "sdk.h"
#include "hooks.h"
#include "baseapi.h"

#define player(x) handle_structs::player_related::x

// This function hooks into the game's CreateMove function to manipulate player actions
bool __fastcall hooks::createmove(PVOID client_mode, int edx, float input_sample_frametime, handle_structs::player_related::user_cmd * cmd) {
    try {
        // Get the hook for the client mode
        vmt_manager & hook = vmt_manager::get_hooken(client_mode);
        // Call the original CreateMove function and store its return value
        bool b_return{ hook.get_method<bool(__thiscall*)(PVOID, float, handle_structs::player_related::user_cmd *)>(27)( client_mode, input_sample_frametime, cmd) };
        // Get the local player entity
        ssdk::c_base_entity * local = get_base_entity(ints.engine->get_local_player());
        // If the local player entity is null, return the original CreateMove return value
        if (local == NULL) return b_return;
        // Get the player's flags
        static auto flags = g_offsets.get_flags(local);
        // Manipulate the player's jump action
        cmd->buttons &= cmd->buttons & player(IN_JUMP) && !(flags & player(FL_ONGROUND)) ? ~player(IN_JUMP) : UINT_MAX;
        // Define a lambda function for auto pistol shooting
        auto do_autopistol = [&](void) {
            static bool shootshoot{false};
            if (cmd->buttons & player(IN_ATTACK)) {
                if (shootshoot) cmd->buttons &= ~player(IN_ATTACK);
            }
            shootshoot = cmd->buttons & player(IN_ATTACK) ? true : false;
        };
        // Execute the auto pistol shooting function
        do_autopistol();
        // Return the original CreateMove return value
        return b_return;
    } catch (const std::exception & e) {
        // Log any exceptions
        api.log_file("failed createmove");
    }
}

// This function hooks into the game's PaintTraverse function to draw on the game's panels
void __fastcall hooks::painttraverse(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce) {
    try {
        // Get the hook for the panels
        vmt_manager & hook = vmt_manager::get_hooken(pPanels);
        // Call the original PaintTraverse function
        hook.get_method<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(41)( pPanels, vguiPanel, forceRepaint, allowForce);
        // Define a static variable for the panel
        static unsigned int panel;
        // If the panel is null, get the panel's name and check if it matches 'M', 'S', 'y', 's', 't' - the reason for this is because it's faster than just using strcmp which is hilariously slow. 
	// That's why I check for the panel via the individual characters.
	    
        if (panel == NULL) {
            const char * n = ints.panels->get_name(vguiPanel);
            if (n == 'M' && n == 'S' && n == 'y' && n == 's' && n == 't') {
                panel = vguiPanel;
                pt::intro();  // Call the intro function if the panel's name matches
            }
        }
        // If the panel does not match the vguiPanel, return
        if (!(panel == vguiPanel)) return;
    } catch (...) {
        // Log any exceptions
        api.log_file("failed painttraverse");
    }
}

void __fastcall hooks::paint(PVOID engine, int edx, int mode) 
{ 
    try 
    { 
        // Get the hook manager for the engine
        vmt_manager & hook = vmt_manager::get_hooken(engine); 
        // Call the original paint method
        hook.get_method<void(__thiscall*)(PVOID, int)>(14)(engine, mode); 
        // Get the start and finish drawing functions
        auto const start_drawing{ reinterpret_cast<void(__thiscall*)(void *)>(api.get_sig("vguimatsurface.dll", "33 C5 50 8D 45 F4 64 A3 ? ? ? ? 8B F9 80 3D") - 0x1b) }; 
        auto const finish_drawing{ reinterpret_cast<void(__thiscall*)(void *)>(api.get_sig("vguimatsurface.dll", "51 56 A1 ? ? ? ? 33 C5 50 8D 45 F4 64 A3 ? ? ? ? 6A") - 0x11) }; 
        // Function to draw entities
        auto draw_entities = [&](void) 
        { 
            // Get the local player
            ssdk::c_base_entity * local = get_base_entity(ints.engine->get_local_player()); 
            if (local == NULL) return; 
            vector screen, worldpos; 
            // Loop through all entities
            for (auto i = 1; i <= ints.ent_list->get_highest_entity_index(); i++) 
            { 
                // Get the entity
                ssdk::c_base_entity * entity = ints.ent_list->get_client_entity(i); 
                if (entity == NULL) continue; 
                // Get the entity's world position
                worldpos = g_offsets.get_infected_origin(entity); 
                // If the entity is not on screen, skip it
                if (!draw.w2s(worldpos, screen)) continue; 
                auto class_id{entity->get_client_class()->classid}; 
                char * name{entity->get_client_class()->name}; 
                // If the entity is valid, draw it
                if (entity->is_entity_valid()) 
                { 
                    if (((class_id == handle_structs::class_ids::Infected) || (class_id == handle_structs::class_ids:: CInsectSwarm))) 
                        draw.draw_string(screen.x, screen.y, COLORCODE(0, 255, 0, 255), "infected"); 
                    screen.y += ESP_HEIGHT; 
                } 
            } 
        }; 
        // If the mode is 2, start drawing
        if (mode & 2) 
        { 
            start_drawing(ints.isurface); 
            vector s(5, 10, 0); 
            draw.draw_string(s.x, s.y, COLORCODE(255, 255, 255, 255), "yocto-base"); 
            // Draw entities
            draw_entities(); 
            // Finish drawing
            finish_drawing(ints.isurface); 
        } 
    } 
    catch (...) 
    { 
        // If an error occurs, log it
        api.error_box("fug, enginevgui"); 
    } 
}
void __fastcall hooks::scene_end(PVOID renderview, int edx)
{
	try
	{
		vmt_manager & hook = vmt_manager::get_hooken(renderview);
		hook.get_method<void(__thiscall*)(PVOID)>(9)(renderview);

		ssdk::c_base_entity * local = get_base_entity(ints.engine->get_local_player());
		if (local == NULL)
			return;

		auto remove_boomer_vomit = [&](void)
		{
			static ssdk::material * mat{nullptr}; //nullptr > null owned

			if (!mat)
			{
				mat = ints.i_material_system->find_mat("particle/screenspaceboomervomit", "Particle textures");
				mat->set_mat_var_flag(handle_structs::mat::mat_var_flags::MATERIAL_VAR_NO_DRAW, true);
			}

			ints.i_model_renderer->forced_mat_override(mat);
			ints.i_model_renderer->forced_mat_override(nullptr);
		};

		remove_boomer_vomit();
	}
	catch (...) { api.log_file("dead"); }
}

vector backup_punch;

bool __fastcall hooks::in_prediction(PVOID iprediction, int edx)
{
	try
	{
		vmt_manager & hook = vmt_manager::get_hooken(iprediction);
		static auto ret = hook.get_method<bool(__thiscall*)(PVOID)>(14)(iprediction);

		void * vesi;
		__asm mov vesi, esi;

		ssdk::c_base_entity * local = get_base_entity(ints.engine->get_local_player());
		if (local == NULL)
			return ret;

		if (!ints.ent_list->get_client_entity(ints.engine->get_local_player()) || ints .ent_list->get_client_entity( ints.engine->get_local_player()) != vesi)
			return ret;

		/*
		You can do nospread here. Take a look at InPrediction.
		*/
		
		auto * c_return = _ReturnAddress();

		return ret;
	}
	catch (...) { api.log_file("fok"); }
}

void hooks::cl_move::client_move(float a, bool b)
{
	o_client_move(a, b);
	if (GetAsyncKeyState(VK_XBUTTON1) & 0x8000)
	{
		for (auto var = 0; var < 2; var++)
			o_client_move(a, b);
	}
}

void hooks::pt::intro()
{
	auto do_init = [&](void)
	{
		try
		{
			g_offsets.Initialize();
			draw.initialize_font();
		}
		catch (const std::exception & e) { api.log_file("couldn't init"); }
	};
	const bool dump_vars{false};
	dump_vars == true ? g_offsets.dump_netvars() : do_init();
}
