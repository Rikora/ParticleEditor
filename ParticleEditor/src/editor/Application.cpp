////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <editor/Application.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <Thor/Math.hpp>

namespace px
{
	Application::Application() : m_window(sf::VideoMode(1200U, 800U), "Particle Editor", sf::Style::Close,
										  sf::ContextSettings(0U, 0U, 8U))
	{
		m_window.setVerticalSyncEnabled(true);
		ImGui::SFML::Init(m_window);

		// Load texture
		m_particleTexture.loadFromFile("src/res/textures/particle.png");
		m_particleSystem.setTexture(m_particleTexture);

		// Prepare the particle emitter
		m_emitter.setEmissionRate(10);
		m_emitter.setParticleLifetime(sf::seconds(2.f));
		m_emitter.setParticlePosition(thor::Distributions::circle(sf::Vector2f(300.f, 200.f), 50.f));
		m_emitter.setParticleScale(sf::Vector2f(0.05f, 0.05f));
		m_particleSystem.addEmitter(m_emitter);
	}

	Application::~Application()
	{
		ImGui::SFML::Shutdown();
	}

	void Application::pollEvents()
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				m_window.close();
		}
	}

	void Application::update(sf::Time dt)
	{
		ImGui::SFML::Update(m_window, dt);
		m_particleSystem.update(dt);
	}

	void Application::updateGUI()
	{
		ImGui::Begin("Hello, world!");
		if (ImGui::Button("Look at this pretty button"))
		{
			printf("Hello\n");
		}
		ImGui::End();
	}

	void Application::render()
	{
		m_window.clear();
		m_window.draw(m_particleSystem);
		ImGui::SFML::Render(m_window);
		m_window.display();
	}

	void Application::run()
	{
		sf::Clock clock;

		while (m_window.isOpen())
		{
			pollEvents();
			update(clock.restart());
			//updateGUI();
			render();
		}
	}
}