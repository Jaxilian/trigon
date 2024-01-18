#include "trigon.h"
#include "core/core.h"




#ifdef _DEBUG
int main() {
	trigon_setup_events();
	trigon_core_start();
	return 0;
}
#else
#ifdef _WIN32
#include <Windows.h>
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	trigon_core_start();
	return 0;
}
#endif
#endif
