#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <Thor/Particles.hpp>
#include <Thor/Time/StopWatch.hpp>

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
		void openFile(std::string & filePath, std::string & file);

	private:
		struct Properties
		{
			bool looping = true;
			bool deflect = false;
			bool velocityPolarVector = false;
			float duration = 1.f;
			float radius = 1.f;
			float nrOfParticles = 1.f;
			float torque = 0.f;
			float maxRotation = 0.f;
			sf::Vector2f rotationSpeed = sf::Vector2f(0.f, 0.f);
			sf::Vector2f rotation = sf::Vector2f(0.f, 0.f);
			sf::Vector2f lifetime = sf::Vector2f(1.f, 1.f);
			sf::Vector2f halfSize = sf::Vector2f(1.f, 1.f);
			sf::Vector2f position = sf::Vector2f(400.f, 400.f);
			sf::Vector2f size = sf::Vector2f(0.05f, 0.05f);
			sf::Vector2f velocity = sf::Vector2f(0.f, 0.f);
			sf::Vector2f fader = sf::Vector2f(0.f, 0.f);
			sf::Vector2f force = sf::Vector2f(0.f, 0.f);
			sf::Color color = sf::Color::White;
			sf::Texture texture;
			sf::BlendMode blendMode;
			std::string shape;
		};

	private:
		sf::RenderWindow m_window;
		std::string m_fullParticlePath, m_particlePath;
		sf::Texture m_playButtonTexture, m_pauseButtonTexture;
		sf::Sprite m_textureButton, m_playButton, m_pauseButton;
		bool m_playing = true;

	private:
		Properties m_particle;
		thor::ParticleSystem m_particleSystem;
		thor::UniversalEmitter m_emitter;
		thor::Connection m_emitterConnection;
		thor::Connection m_fadeConnection;
		thor::Connection m_torqueConnection;
		thor::Connection m_forceConnection;
		thor::StopWatch m_playbackWatch;
	};
}