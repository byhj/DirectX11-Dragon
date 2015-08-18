#include "RenderSystem.h"

namespace byhj
{

RenderSystem::RenderSystem()
{

}

RenderSystem::~RenderSystem()
{

}

void RenderSystem::v_Init()
{
	m_Window.Init();

}

void RenderSystem::v_Render()
{
	m_Window.Render();
}

void RenderSystem::v_Shutdown()
{

	m_Window.Shutdown();
}


}