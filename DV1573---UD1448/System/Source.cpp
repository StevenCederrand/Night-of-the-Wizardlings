#include <iostream>
#include <System/Log.h>
#include <System/Application.h>

int main() {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Log::initialize();
	
	Application* m_app = new Application();
	bool m_init = m_app->init();

	if (m_init) {
		m_app->update();
	}
	else {
		LOG_ERROR("Failed to init application");
		system("pause");
	}

	delete m_app;

	return 0;
}
