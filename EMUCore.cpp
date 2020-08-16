#include "EMUCore.h"

void EmuCore::StartEmulation()
{
	// ���C�����[�v
	// ���C���X���b�h
	m_thread = std::thread([this] {
		bool bFirst = true;
		m_isRunning = true;
		while (m_isRunning) {
			auto start = std::chrono::system_clock::now();      // �v���X�^�[�g������ۑ�
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				if (bFirst) {
					bFirst = false;
					// ��������I�t�X�N���[�����N���A
					memset(m_offscreen, 0, sizeof(uint32_t) * EMUTEX_W * EMUTEX_H);
				}

				// ��ʏ�������
				// ����
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
			auto end = std::chrono::system_clock::now();       // �v���I��������ۑ�
			auto dur = end - start;							   // �v�������Ԃ��v�Z
			auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
			if (msec < 17) {
				std::chrono::milliseconds dura(17 - msec);	// 17�~���b
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
	// �X���b�h��~
	m_isRunning = false;
	if (m_thread.joinable()) {
		m_thread.join();
	}
	// ��n��

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
