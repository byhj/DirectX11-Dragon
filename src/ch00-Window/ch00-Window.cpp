#include "d3d/d3dApp.h"

class WindowApp: public D3DApp
{
public:
	WindowApp() 
	{
		m_AppName = L"Window";
	}
	~WindowApp() {}

private:

};

CALL_MAIN(WindowApp);