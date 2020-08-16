#include "EMUCore.h"

void EmuCore::StartEmulation()
{
	// メインループ
	// メインスレッド
	m_thread = std::thread([this] {
		bool bFirst = true;
		m_isRunning = true;
		while (m_isRunning) {
			auto start = std::chrono::system_clock::now();      // 計測スタート時刻を保存
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (bFirst) {
					bFirst = false;
					// いったんオフスクリーンをクリア
					memset(m_offscreen, 0, sizeof(uint32_t) * EMUTEX_W * EMUTEX_H);
				}

				// 画面書き換え
				// 砂嵐
				uint32_t* ptr = m_offscreen;
				uint32_t data = 0;
				int i;
				int j;
				for (i = 0; i < TEX_HEIGHT; i++) {
					for (j = 0; j < TEX_WIDTH; j++) {
						data = 0xFF000000 | ((rand() & 0xFF) << 16) | ((rand() & 0xFF) << 9) | (rand() & 0xFF);
						//						*(ptr++) = 0xFFFF0000;		//AABBGGRR
						*(ptr++) = data;
					}
				}
			} // !lock_guard
			auto end = std::chrono::system_clock::now();       // 計測終了時刻を保存
			auto dur = end - start;							   // 要した時間を計算
			auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
			if (msec < 17) {
				std::chrono::milliseconds dura(17 - msec);	// 17ミリ秒
				std::this_thread::sleep_for(dura);
			}
			else {
				std::this_thread::yield();
			}
		}
	});
}

void EmuCore::StopEmulation()
{
	// スレッド停止
	m_isRunning = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
	// 後始末

}

void EmuCore::Update()
{
	if (m_unity == nullptr || m_texture == nullptr || m_offscreen == nullptr) return;

	std::lock_guard<std::mutex> lock(m_mutex);
	auto device = m_unity->Get<IUnityGraphicsD3D11>()->GetDevice();
	ID3D11DeviceContext* context;
	device->GetImmediateContext(&context);
	context->UpdateSubresource(m_texture, 0, nullptr, m_offscreen, TEX_WIDTH*4, 0);
}
