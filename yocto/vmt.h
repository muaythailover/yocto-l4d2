#pragma once

//----------------------------------------------------------------
// Toolkit: Virtual method table hooks
//----------------------------------------------------------------
// These can be hooked in two distinct manners.
//
// The vmt_manager class hooks by replacing the vmt pointer with a custom crafted one.
// Only calls through this vmt_pointer will be hooked.
//
// The vmt_pointer class hooks a pointer to an interface.
// Only calls through this specific variable will be hooked.
//
// The VMTHook class hooks by editing the vtable in the dll's .rdata.
// Every vmt_pointer using that vtable will be hooked.
//
// A general hint:
// Abuse the __fastcall convention to catch the this pointer.
//
// More specific information follows the class declaration.
//
// Each class has a basic version (no constructors/destructors) and a RAII version.
//
// This code is public domain but credits (to Casual_Hacker on www.unknowncheats.me) are always appreciated.

#include <cassert>

namespace toolkit
{

	// Find the number of vfuncs in a vtable
	unsigned int count_funcs(void** pVMT);

	// Find the index for a vfunc, result is negative if not found
	int find_function(void** pVMT, void* pFunc, unsigned int vfuncs = 0);

	//----------------------------------------------------------------
	// Class: vmt_manager
	//----------------------------------------------------------------
	// Hooks virtual functions by replacing the vtable pointer from an vmt_pointer.
	// vmt_manager is the RAII version of vmt_base_manager
	class vmt_base_manager
	{
	public:
		// TODO: Abstractions can never hurt...
		//typedef const void* pfn_t;
		//struct vtbl_t
		//{
		//	pfn_t data[1];
		//	inline const pfn_t& operator[] ( unsigned int i ) const { return data[i]; }
		//	inline pfn_t& operator[] ( unsigned int i ) { return data[i]; }
		//};

		// Guard identifies if an vmt_pointer is hooked.
		enum { GUARD = 0xFAC0D775 }; //4206942069 huehuehuehuehue

		// init the hook, will not hook the vmt_pointer yet.
		vmt_base_manager& init(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		// unhook and forget about this vmt_pointer.
		void kill();
		// Are we initialized.
		bool is_init() const;

		// Hooks a function by index.
		vmt_base_manager& hook_method(void* newfunc, unsigned int index);
		// unhooks a function by index.
		vmt_base_manager& unhook_method(unsigned int index);
		// unhooks all functions.
		void EraseHooks();

		// Return the original vtable. vmt_pointer will appear completely unhooked.
		vmt_base_manager& unhook();
		// Put the vtable with the hooks back.
		vmt_base_manager& rehook();
		// Test if the hooks are active.
		bool hooked() const;

		// Get the original function.
		// Use a function prototype for the template argument to make it very easy to call this function.
		// Example syntax: hook.get_method<bool (__thiscall*)( void*, int )>( 12 )( inst, arg );
		template< typename Fn >
		const Fn& get_method(unsigned int index) const;

		// Get/Set the user__data.
		void* user__data() const;
		void user__data(void* data);

		// Test if the vmt_pointer is hooked.
		static bool hook_present(void* inst, unsigned int offset = 0);
		// Get the hook object. Does not check if the vmt_pointer is actually hooked!
		static vmt_base_manager& get_hooken(void* inst, unsigned int offset = 0);

	protected:
		static void**& _getvtbl(void* inst, unsigned int offset);

	protected:
		void*** _vftable;
		void** _oldvmt;
		void** _array;
		void* _user__data;
	};
	class vmt_manager : public vmt_base_manager
	{
		// Forbid copy constructing and assignment.
		vmt_manager(const vmt_manager&);
		vmt_manager& operator= (const vmt_manager&);

	public:
		// initialize & hook the vmt_pointer.
		vmt_manager(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		~vmt_manager();

		// If the vmt_pointer is somehow destroyed before you get a chance to unhook it or destruct this hook object, call this.
		// It'll prevent the destructor from crashing.
		void poof();

		// Get the hook object. Does not check if the vmt_pointer is actually hooked!
		static vmt_manager& get_hooken(void* inst, unsigned int offset = 0);
	};
	// vmt_base_manager inlines
	inline bool vmt_base_manager::is_init() const
	{
		return _vftable != nullptr;
	}
	inline vmt_base_manager& vmt_base_manager::hook_method(void* newfunc, unsigned int index)
	{
		assert(index < count_funcs(_array + 3) && is_init());
		_array[index + 3] = newfunc;
		return *this;
	}
	inline vmt_base_manager& vmt_base_manager::unhook_method(unsigned int index)
	{
		assert(index < count_funcs(_array + 3) && is_init());
		_array[index + 3] = _oldvmt[index];
		return *this;
	}
	inline vmt_base_manager& vmt_base_manager::unhook()
	{
		*_vftable = _oldvmt;
		return *this;
	}
	inline vmt_base_manager& vmt_base_manager::rehook()
	{
		*_vftable = _array + 3;
		return *this;
	}
	inline bool vmt_base_manager::hooked() const
	{
		return *_vftable != _oldvmt;
	}
	template< typename Fn >
	inline const Fn& vmt_base_manager::get_method(unsigned int index) const
	{
		assert(index < count_funcs(_array + 3));
		return *(const Fn*)&_oldvmt[index];
	}
	inline void* vmt_base_manager::user__data() const
	{
		return _user__data;
	}
	inline void vmt_base_manager::user__data(void* data)
	{
		_user__data = data;
	}
	inline bool vmt_base_manager::hook_present(void* inst, unsigned int offset)
	{
		void** vmt = _getvtbl(inst, offset);
		return vmt[-2] == (void*)GUARD;
	}
	inline vmt_base_manager& vmt_base_manager::get_hooken(void* inst, unsigned int offset)
	{
		void** vmt = _getvtbl(inst, offset);
		return *reinterpret_cast<vmt_base_manager*>(vmt[-3]);
	}
	inline void**& vmt_base_manager::_getvtbl(void* inst, unsigned int offset)
	{
		return *reinterpret_cast<void***>((char*)inst + offset);
	}
	// vmt_manager inlines
	inline vmt_manager::vmt_manager(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		init(inst, offset, vfuncs).rehook();
	}
	inline vmt_manager::~vmt_manager()
	{
		kill();
	}
	inline void vmt_manager::poof()
	{
		_vftable = nullptr;
	}
	inline vmt_manager& vmt_manager::get_hooken(void* inst, unsigned int offset)
	{
		return static_cast<vmt_manager&>(vmt_base_manager::get_hooken(inst, offset));
	}



	//----------------------------------------------------------------
	// Class: vmt_pointer
	//----------------------------------------------------------------
	// Hooks virtual functions by replacing the whole pointer to the class with a dummy interface.
	// Assumes only 1 vtable (no MI) placed at the start of the class.
	class vmt_base_pointer
	{
	public:
		enum { GUARD = 0xDE4DB3EF };

		// init the hook, will not hook the vmt_pointer yet. (the vfuncs use __thiscall convention)
		vmt_base_pointer& init_thiscall(void* inst, unsigned int vfuncs = 0);
		// init the hook, will not hook the vmt_pointer yet. (the vfuncs use __stdcall convention)
		vmt_base_pointer& init_stdcall(void* inst, unsigned int vfuncs = 0);
		// unhook and forget about this vmt_pointer.
		void kill();

		// Hooks a function by index.
		// WARNING! The this ptr in your hook points to the hook object!
		vmt_base_pointer& hook_method(void* newfunc, unsigned int index);
		// unhooks a function by index.
		vmt_base_pointer& unhook_method(unsigned int index);

		// Hook for this variable.
		vmt_base_pointer& rehook(void* var);
		// unhook for this variable.
		vmt_base_pointer& unhook(void* var);

		// Get the original function.
		// Use a function prototype for the template argument to make it very easy to call this function.
		// Example syntax: hook.get_method<bool (__thiscall*)( void*, int )>( 12 )( inst, arg );
		template< typename Fn >
		const Fn& get_method(unsigned int index) const;

		// Get/Set the user__data.
		void* user__data() const;
		void user__data(void* data);

		// Get the original vmt_pointer.
		void* vmt_pointer() const;
		// Get the dummy vmt_pointer.
		void* dummy() const;

		// Test if the vmt_pointer is hooked.
		static bool hook_present(void* inst);
		// Get the hook object. Does not check if the vmt_pointer is actually hooked!
		static vmt_base_pointer& get_hooken(void* inst);

	protected:
		// Internal constructor
		typedef void(__fastcall* CallGateFn)();
		vmt_base_pointer& init(void* inst, CallGateFn gate, unsigned int vfuncs = 0);

		static void* __fastcall FindCallOffset(vmt_base_pointer* _this, unsigned char* addr);
		static void __fastcall CallGate_thiscall(); // This passed in ecx
		static void __fastcall CallGate_stdcall(); // This passed on stack
		static void** _getvtbl(void* inst, unsigned int offset = 0);

	protected:
		// Note! Do not move these members around!
		// dummy object has to come first, original vmt_pointer ptr has to be at this+4!

		// Fake 'this' points to our hook object
		struct dummy_t
		{
			void** vtable;
		} _dummy;
		// Original vmt_pointer
		void* _inst;
		// Call gate (__thiscall/__stdcall version, needed for unhook_method)
		CallGateFn _gate;
		// User data context
		void* _user__data;
	};
	class vmt_pointer : public vmt_base_pointer
	{
		// Forbid copy constructing and assignment
		vmt_pointer(const vmt_pointer&);
		vmt_pointer& operator= (const vmt_pointer&);
	public:
		vmt_pointer(void** var, bool thiscall = true, unsigned int vfuncs = 0);
		~vmt_pointer();

		// Return the original vtable. vmt_pointer will appear completely unhooked.
		vmt_pointer& unhook();
		// Put the vtable with the hooks back.
		vmt_pointer& rehook();
		// Test if the hooks are active.
		bool hooked() const;

		static vmt_pointer& get_hooken(void* inst);
	private:
		void** _var;
	};
	// Inlines for vmt_base_pointer
	inline vmt_base_pointer& vmt_base_pointer::init_thiscall(void* inst, unsigned int vfuncs)
	{
		return init(inst, &CallGate_thiscall, vfuncs);
	}
	inline vmt_base_pointer& vmt_base_pointer::init_stdcall(void* inst, unsigned int vfuncs)
	{
		return init(inst, &CallGate_stdcall, vfuncs);
	}
	inline vmt_base_pointer& vmt_base_pointer::hook_method(void* newfunc, unsigned int index)
	{
		assert(index < count_funcs(_dummy.vtable) && newfunc);
		_dummy.vtable[index] = newfunc;
		return *this;
	}
	inline vmt_base_pointer& vmt_base_pointer::unhook_method(unsigned int index)
	{
		assert(index < count_funcs(_dummy.vtable));
		_dummy.vtable[index] = _gate;
		return *this;
	}
	inline vmt_base_pointer& vmt_base_pointer::rehook(void* var)
	{
		void*& x = *(void**)var;
		assert(!x || x == _inst || x == &_dummy);
		x = &_dummy;
		return *this;
	}
	inline vmt_base_pointer& vmt_base_pointer::unhook(void* var)
	{
		void*& x = *(void**)var;
		assert(!x || x == _inst || x == &_dummy);
		x = _inst;
		return *this;
	}
	template< typename Fn >
	inline const Fn& vmt_base_pointer::get_method(unsigned int index) const
	{
		assert(index < count_funcs(_dummy.vtable));
		return *(const Fn*)(&_getvtbl(_inst)[index]);
	}
	inline void* vmt_base_pointer::user__data() const
	{
		return _user__data;
	}
	inline void vmt_base_pointer::user__data(void* data)
	{
		_user__data = data;
	}
	inline void* vmt_base_pointer::vmt_pointer() const
	{
		return _inst;
	}
	inline void* vmt_base_pointer::dummy() const
	{
		return const_cast<dummy_t*>(&_dummy);
	}
	inline bool vmt_base_pointer::hook_present(void* inst)
	{
		void** vmt = *(void***)inst;
		return vmt[-2] == (void*)GUARD;
	}
	inline vmt_base_pointer& vmt_base_pointer::get_hooken(void* inst)
	{
		void** vmt = *(void***)inst;
		return *reinterpret_cast<vmt_base_pointer*>(vmt[-3]);
	}
	inline void** vmt_base_pointer::_getvtbl(void* inst, unsigned int offset)
	{
		return *reinterpret_cast<void***>((char*)inst + offset);
	}
	// Inlines for vmt_pointer
	inline vmt_pointer::vmt_pointer(void** var, bool thiscall, unsigned int vfuncs) : _var(var)
	{
		if (thiscall)
			init_thiscall(*var, vfuncs);
		else
			init_stdcall(*var, vfuncs);
	}
	inline vmt_pointer::~vmt_pointer()
	{
		*_var = _inst;
		kill();
	}
	inline vmt_pointer& vmt_pointer::unhook()
	{
		*_var = _inst;
		return *this;
	}
	inline vmt_pointer& vmt_pointer::rehook()
	{
		*_var = &_dummy;
		return *this;
	}
	inline bool vmt_pointer::hooked() const
	{
		return *_var == &_dummy;
	}
	inline vmt_pointer& vmt_pointer::get_hooken(void* inst)
	{
		return static_cast<vmt_pointer&>(vmt_base_pointer::get_hooken(inst));
	}







	//----------------------------------------------------------------
	// Class: VMTHook
	//----------------------------------------------------------------
	// Hooks virtual functions by overwriting the function pointers in .rdata.
	// Currently no support for unhooking...
	class VMTBaseHook
	{
	public:
		// init the hook.
		VMTBaseHook& init(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		VMTBaseHook& init(void** vmt, unsigned int vfuncs = 0);
		// unhook and forget about this vmt_pointer.
		void kill();

		// Hooks a function by index.
		VMTBaseHook& hook_method(void* new_func, unsigned int index);
		// unhooks a function by index.
		VMTBaseHook& unhook_method(unsigned int index);
		// Erase the hooks.
		void EraseHooks();

		// Get the original function.
		// Use a function prototype for the template argument to make it very easy to call this function.
		// Example syntax: hook.get_method<bool (__thiscall*)( void*, int )>( 0x5 )( inst, arg );
		template< typename Fn >
		const Fn& get_method(unsigned int index) const;

	protected:
		static bool WriteHook(void* dest, const void* src, unsigned int bytes);

	private:
		void** _vftable;
		void** _backup;
		unsigned int _vcount;
	};
	class VMTHook : public VMTBaseHook
	{
		// Forbid copy constructing and assignment
		VMTHook(const VMTHook&);
		VMTHook& operator= (const VMTHook&);

	public:
		VMTHook(void* inst, unsigned int offset = 0, unsigned int vfuncs = 0);
		VMTHook(void** vmt, unsigned int vfuncs = 0);
		~VMTHook();
	};
	// Inlines for VMTBaseHook
	inline VMTBaseHook& VMTBaseHook::init(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		return init(*reinterpret_cast<void***>((char*)inst + offset), vfuncs);
	}
	template< typename Fn >
	inline const Fn& VMTBaseHook::get_method(unsigned int index) const
	{
		return *(Fn*)&_backup[index];
	}
	inline VMTBaseHook& VMTBaseHook::hook_method(void* func, unsigned int index)
	{
		WriteHook(&_vftable[index], &func, sizeof(void*));
		return *this;
	}
	inline VMTBaseHook& VMTBaseHook::unhook_method(unsigned int index)
	{
		WriteHook(&_vftable[index], &_backup[index], sizeof(void*));
		return *this;
	}
	inline void VMTBaseHook::EraseHooks()
	{
		WriteHook(_vftable, _backup, _vcount * sizeof(void*));
	}
	// Inlines for VMTHook
	inline VMTHook::VMTHook(void* inst, unsigned int offset, unsigned int vfuncs)
	{
		init(inst, offset, vfuncs);
	}
	inline VMTHook::VMTHook(void** vmt, unsigned int vfuncs)
	{
		init(vmt, vfuncs);
	}
	inline VMTHook::~VMTHook()
	{
		kill();
	}


}