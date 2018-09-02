#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics.hpp>
#include <loader/ParticleLoader.hpp>
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
		void openTextureFile(std::string & filePath, std::string & file);
		void openParticleFile();
		void saveParticleFile();
		void loadParticleData(const std::string & filePath);
		void outputParticleData(const std::string & filePath);

	private:
		sf::RenderWindow m_window;
		std::string m_particlePath;
		sf::Texture m_playButtonTexture, m_pauseButtonTexture;
		sf::Sprite m_textureButton, m_playButton, m_pauseButton;
		bool m_playing;
		static int m_shapeItem;
		static int m_blendItem;
		static float m_color[3];

	private:
		ParticleLoader::Properties m_particle;
		thor::ParticleSystem m_particleSystem;
		thor::UniversalEmitter m_emitter;
		thor::Connection m_emitterConnection;
		thor::Connection m_fadeConnection;
		thor::Connection m_torqueConnection;
		thor::Connection m_forceConnection;
		thor::StopWatch m_playbackWatch;
	};
}