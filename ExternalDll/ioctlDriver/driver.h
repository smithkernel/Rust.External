#pragma once
#include "imports.h"
#include <string_view>
#include <mutex>

static std::mutex mtx;

#include <chrono>

class timer 
{
private:
	std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

public:
	void Start()
	{
		m_StartTime = std::chrono::high_resolution_clock::now();
	}
	float GetDuration()
	{
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - m_StartTime);
		return duration.count();
	}
};

namespace mem {
    // Declare constants with descriptive names instead of using macros.
    const char* GAME_ASSEMBLY_NAME = "GameAssembly.dll";
    const char* UNITY_PLAYER_NAME = "UnityPlayer.dll";
    const uintptr_t INVALID_ADDRESS = 0;

    // Use std::optional to indicate when an address cannot be read.
    template<typename T>
    std::optional<T> read(uintptr_t addr) {
        if (addr < 0xFFFFFF) {
            return std::nullopt;
        }
        T value;
        if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<LPCVOID>(addr), &value, sizeof(T), nullptr)) {
            return value;
        }
        return std::nullopt;
    }

    // Use RAII to ensure that module handles are properly closed.
    class Module {
    public:
        Module(const char* name) : handle_(GetModuleHandleA(name)) {}
        ~Module() { if (handle_) FreeLibrary(handle_); }
        operator bool() const { return handle_ != nullptr; }
        uintptr_t base_address() const { return reinterpret_cast<uintptr_t>(handle_); }
    private:
        HMODULE handle_;
    };

    // Use the Module class to get the base addresses of modules.
    const Module GAME_ASSEMBLY_MODULE(GAME_ASSEMBLY_NAME);
    const Module UNITY_PLAYER_MODULE(UNITY_PLAYER_NAME);
    const uintptr_t GAME_ASSEMBLY_BASE = GAME_ASSEMBLY_MODULE ? GAME_ASSEMBLY_MODULE.base_address() : INVALID_ADDRESS;
    const uintptr_t UNITY_PLAYER_BASE = UNITY_PLAYER_MODULE ? UNITY_PLAYER_MODULE.base_address() : INVALID_ADDRESS;
}
