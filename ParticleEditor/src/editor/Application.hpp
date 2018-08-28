#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <Thor/Particles.hpp>

namespace px
{
	class Application
	{
	public:
		Application();
		~Application();

	public:
		void run();

	private:
		void pollEvents();
		void update(sf::Time dt);
		void updateGUI();
		void render();

	private:
		sf::RenderWindow m_window;
		sf::Texture m_particleTexture;
		thor::ParticleSystem m_particleSystem;
		thor::UniversalEmitter m_emitter;
	};
}