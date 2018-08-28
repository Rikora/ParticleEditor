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
		void updateParticles(sf::Time dt);
		void updateGUI();
		void render();

	private:
		struct Properties
		{
			float nrOfParticles = 1.f;
			float rotation = 0.f;
			sf::Vector2f position = sf::Vector2f(600.f, 400.f);
			sf::Vector2f scale = sf::Vector2f(1.f, 1.f);
			float lifetime = 1.f;
		};

	private:
		sf::RenderWindow m_window;
		sf::Texture m_particleTexture;
		thor::ParticleSystem m_particleSystem;
		thor::UniversalEmitter m_emitter;
		Properties m_particle;
	};
}