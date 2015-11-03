#include "Gui.h"

namespace byhj
{
	namespace d3d
	{
		void Gui::Init(ID3D11Device *pD3D11Device, int sw, int sh)
		{
			TwInit(TW_DIRECT3D11, pD3D11Device);
			TwWindowSize(sw, sh);
			TwBar *bar = TwNewBar("Test");

		}

		void Gui::Update()
		{

		}

		void  Gui::Render()
		{
			// Draw tweak bars
			TwDraw();
		}

		void Gui::Shutdown()
		{
			TwTerminate();
		}
	}
}