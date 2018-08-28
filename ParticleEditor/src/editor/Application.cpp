////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <editor/Application.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>

namespace px
{
	Application::Application() : m_window(sf::VideoMode(800U, 600U), "Particle Editor", sf::Style::Close,
										  sf::ContextSettings(0U, 0U, 8U)), m_shape(50.f)
	{
		m_window.setVerticalSyncEnabled(true);
		ImGui::SFML::Init(m_window);
		m_shape.setFillColor(sf::Color::Green);
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
		m_window.draw(m_shape);
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
			updateGUI();
			render();
		}
	}
}