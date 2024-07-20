#include "utils.h"

inline float dot(const vector & a, const vector & b) { return (a.x * b.x + a.y * b.y + a.z * b.z); }

inline void vec_trans(const vector in1, float in2[3][4], vector & out)
{
	out[0] = dot(in1, vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3];
	out[1] = dot(in1, vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3];
	out[2] = dot(in1, vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3];
}

vector c_utils::get_hbox(ssdk::c_base_entity * ent, INT8 hbox)
{
	matrix3x4 matrix[128];
	auto setup_and_get = [&](void)
	{
		if (ent->setup_bones(matrix, 128, 0x100, 0.f))
		{
			handle_structs::studiohdr_t * hdr = ints.model_info->get_studio_model(ent->get_mdl());
			handle_structs::mstudiohitboxset_t * set = hdr->get_hitbox_set(0);

			handle_structs::mstudiobbox_t * hitbox = set->get_hitbox(hbox);
			if (hitbox)
			{
				vector min, max, cent;
				vec_trans(hitbox->bbmin, matrix[hitbox->bone], min);
				vec_trans(hitbox->bbmax, matrix[hitbox->bone], max);
				cent = ((min + max) * 0.5f);
				return cent;
			}
		}
	};
	setup_and_get();
	return vector{0, 0, 0};
}
