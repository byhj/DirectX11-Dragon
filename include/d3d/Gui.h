#ifndef __GUI_H_
#define __GUI_H_

#include "AntTweakBar.h"
#include <d3d11.h>

namespace byhj
{
	namespace d3d
	{
		class Gui
		{
		public:
			Gui() = default;
			~Gui() = default;
			void Init(ID3D11Device *pD3D11Device, int sw, int sh);
			void Update();
			void Render();
			void Shutdown();

		private:

		};

	}
}
#endif
