#include "GOLScene.h"
#include "Random.h"

const color_t white{ 255, 255, 255, 255 };
const color_t black{ 0, 0, 0, 0 };

bool GOLScene::Initialize()
{
	m_renderer.Initialize();
	m_renderer.CreateWindow("Game Of Life", 800, 600);

	m_input.Initialize();
	m_input.Update();

	m_framebuffer = std::make_unique<Framebuffer>(m_renderer, m_renderer.m_width / 4, m_renderer.m_height / 4);

	m_cellsA = std::make_unique<Cells<uint8_t>>(m_framebuffer->m_width, m_framebuffer->m_height);
	m_cellsB = std::make_unique<Cells<uint8_t>>(m_framebuffer->m_width, m_framebuffer->m_height);

	return true;
}

void GOLScene::Update()
{
	Scene::Update();

	m_frame++;

	Cells<uint8_t>* currentCells = (m_frame % 2) ? m_cellsA.get() : m_cellsB.get();
	Cells<uint8_t>* nextCells = (m_frame % 2) ? m_cellsB.get() : m_cellsA.get();

	std::fill(nextCells->m_data.begin(), nextCells->m_data.end(), 0);

	if (m_input.GetKeyDown(SDL_SCANCODE_SPACE) && !m_input.GetPreviousKeyDown(SDL_SCANCODE_SPACE))
	{
		for (int i = 0; i < currentCells->m_data.size(); i++)
		{
			currentCells->m_data[i] = (random(40) == 0) ? 1 : 0;
		}
	}

	for (int y = 1; y < nextCells->m_height - 1; y++)
	{
		for (int x = 1; x < nextCells->m_width - 1; x++)
		{
			int numNeighbors = 0;

			numNeighbors += currentCells->Read(x + 0, y - 1);
			numNeighbors += currentCells->Read(x + 1, y - 1);
			numNeighbors += currentCells->Read(x - 1, y + 0);
			numNeighbors += currentCells->Read(x + 1, y + 0);
			numNeighbors += currentCells->Read(x - 1, y - 1);
			numNeighbors += currentCells->Read(x - 1, y + 1);
			numNeighbors += currentCells->Read(x + 0, y + 1);
			numNeighbors += currentCells->Read(x + 1, y + 1);

			// Do the game of life rules
			
			uint8_t currentState = currentCells->Read(x, y);
			if (currentState) 
			{
				// Cells stay alive if there are 2 or 3 neighbors, otherwise they will die

				uint8_t nextState = (numNeighbors == 2 || numNeighbors == 3) ? 1 : 0;

				nextCells->Write(x, y, nextState);
			}
			else
			{
				// if dead and there are 3 neighbors the cell comes back
				
				uint8_t deadState = 0;

				if (numNeighbors == 3)
				{
					deadState = 1;
				}
				
				nextCells->Write(x, y, deadState);
			}

			// nextCells->Write(x, y, 0/1);
		}
	}

	// Write cells to the framebuffer

	m_framebuffer->Clear(color_t{ 255, 0, 0, 255 });

	for (int i = 0; i < nextCells->m_data.size(); i++)
	{
		m_framebuffer->m_buffer[i] = (nextCells->m_data[i]) ? white : black;
	}
}

void GOLScene::Draw()
{
	/*
	for (int i = 0; i < 10; i++)
	{
		m_framebuffer->DrawPoint(random(m_renderer.m_width / 2), random(m_renderer.m_height), color_t{ 0, 0, 0, 255 });
	}
	*/


	// update framebuffer
	m_framebuffer->Update();

	// show screen
	m_renderer = *m_framebuffer;
	SDL_RenderPresent(m_renderer.m_renderer);
}
