#pragma once

#ifndef __EMUCORE_H__
#define __EMUCORE_H__

#include <d3d11.h>
#include <thread>
#include <mutex>
#include <cstdint>

#include "Unity/IUnityInterface.h"
#include "Unity/IUnityGraphics.h"
#include "Unity/IUnityGraphicsD3D11.h"

#define TEX_WIDTH	640
#define TEX_HEIGHT	200
#define EMUTEX_W	(TEX_WIDTH)
#define EMUTEX_H	(TEX_HEIGHT)

#ifdef __cplusplus
extern "C" {
#endif
#ifdef __cplusplus
}
#endif



class EmuCore
{
public:
	EmuCore(IUnityInterfaces* unity)
		: m_width(TEX_WIDTH)
		, m_height(TEX_HEIGHT)
		, m_unity(unity)
	{
		m_offscreen = new uint32_t[TEX_WIDTH * TEX_HEIGHT];
		StartEmulation();
	}

	~EmuCore()
	{
		StopEmulation();
		delete[] m_offscreen;
		m_offscreen = nullptr;
	}

private:
	void StartEmulation();
	void StopEmulation();

public:
	void Update();

	int GetWidth() const
	{
		return m_width;
	}

	int GetHeight() const
	{
		return m_height;
	}

	void SetTexturePtr(void* ptr)
	{
		m_texture = static_cast<ID3D11Texture2D*>(ptr);
	}

private:
	int m_width, m_height;

	uint32_t* m_offscreen = nullptr;
	IUnityInterfaces* m_unity;
	ID3D11Texture2D* m_texture = nullptr;

	std::thread m_thread;
	std::mutex m_mutex;
	bool m_isRunning = false;
};


namespace
{
	IUnityInterfaces* g_unity = nullptr;
	EmuCore*       g_emucore = nullptr;
}


extern "C"
{
	// Low-Level Native Plugin Interface で Unity 側から呼ばれる
	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces* unityInterfaces)
	{
		g_unity = unityInterfaces;
	}

	// Low-Level Native Plugin Interface で Unity 側から呼ばれる
	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API UnityPluginUnload()
	{
		// 特に終了処理は必要なし
	}

	// Unity 側で GL.IssuePlugin(この関数のポインタ, eventId) を呼ぶとレンダリングスレッドから呼ばれる
	void UNITY_INTERFACE_API OnRenderEvent(int eventId)
	{
		if (g_emucore) g_emucore->Update();
	}

	// GL.IssuePlugin で登録するコールバック関数のポインタを返す
	UNITY_INTERFACE_EXPORT UnityRenderingEvent UNITY_INTERFACE_API GetRenderEventFunc()
	{
		return OnRenderEvent;
	}

	UNITY_INTERFACE_EXPORT void* UNITY_INTERFACE_API GetEmul()
	{
		g_emucore = new EmuCore(g_unity);
		return g_emucore;
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API ReleaseEmul(void* ptr)
	{
		auto emucore = reinterpret_cast<EmuCore*>(ptr);
		if (g_emucore == emucore) g_emucore = nullptr;
		delete emucore;
	}

	UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API GetEmulWidth(void* ptr)
	{
		auto emucore = reinterpret_cast<EmuCore*>(ptr);
		return emucore->GetWidth();
	}

	UNITY_INTERFACE_EXPORT int UNITY_INTERFACE_API GetEmulHeight(void* ptr)
	{
		auto emucore = reinterpret_cast<EmuCore*>(ptr);
		return emucore->GetHeight();
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API SetEmulTexturePtr(void* ptr, void* texture)
	{
		auto emucore = reinterpret_cast<EmuCore*>(ptr);
		emucore->SetTexturePtr(texture);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API EmulKeyUp(int keyCode)
	{
//		keyup(keyCode);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API EmulKeyDown(int keyCode)
	{
//		keydown(keyCode);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API EmulSetQDFile(int fno)
	{
//		set_file_no(fno);
	}

	UNITY_INTERFACE_EXPORT void UNITY_INTERFACE_API EmulReset(void)
	{
//		reset();
	}

};

#endif // ! __EMUCORE_H__
