////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <editor/Application.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <Thor/Math.hpp>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace px
{
	Application::Application() : m_window(sf::VideoMode(1200U, 800U), "Particle Editor", sf::Style::Close,
										  sf::ContextSettings(0U, 0U, 8U))
	{
		m_window.setVerticalSyncEnabled(true);
		ImGui::SFML::Init(m_window);

		// Load texture
		m_particleTexture.loadFromFile("src/res/textures/particle.png");
		m_textureButton.setTexture(m_particleTexture);
		m_particleSystem.setTexture(m_particleTexture);
		m_particleSystem.addEmitter(thor::refEmitter(m_emitter));
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
		updateParticles(dt);
	}

	void Application::updateParticles(sf::Time dt)
	{
		// TODO: Set texture on choice here?

		// Update emitter
		// TODO: Add conditional expressions when circular position is marked for instance
		m_emitter.setEmissionRate(m_particle.nrOfParticles);
		m_emitter.setParticleLifetime(sf::seconds(m_particle.lifetime));
		m_emitter.setParticleScale(m_particle.scale);
		m_emitter.setParticleRotation(m_particle.rotation);
		m_emitter.setParticlePosition(thor::Distributions::circle(m_particle.position, 50.f)); // Emit as circle
		m_particleSystem.update(dt);
	}

	void Application::updateGUI()
	{
		static int floatPrecision = 3;
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Particle System", NULL, ImVec2(0, 0), 1.0f, flags);
		
		ImGui::Spacing();
		ImGui::InputFloat("Particles", &m_particle.nrOfParticles, 1.f);
		ImGui::Spacing();
		ImGui::InputFloat2("Position", &m_particle.position.x, floatPrecision);
		ImGui::Spacing();
		ImGui::InputFloat2("Scale", &m_particle.scale.x, floatPrecision);
		ImGui::Spacing();
		ImGui::InputFloat("Rotation", &m_particle.rotation, 1.f, 0.f, floatPrecision);
		ImGui::Spacing();
		ImGui::InputFloat("Lifetime", &m_particle.lifetime, 0.1f, 0.f, floatPrecision);
		ImGui::Spacing();

		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Emission"))
		{
		}
		ImGui::Spacing();

		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Shape"))
		{
		}
		ImGui::Spacing();

		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Renderer"))
		{
			ImGui::Spacing();
			static int m_blendItem = 0;
			const char* itemList[] = { "None", "BlendAdd", "BlendAlpha", "BlendMultiply" };
			ImGui::Combo("Blend mode", &m_blendItem, itemList, IM_ARRAYSIZE(itemList));

			switch (m_blendItem)
			{
			case 0:
				m_blendMode = sf::BlendNone;
				break;
			case 1:
				m_blendMode = sf::BlendAdd;
				break;
			case 2:
				m_blendMode = sf::BlendAlpha;
				break;
			case 3:
				m_blendMode = sf::BlendMultiply;
				break;
			default:
				break;
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();
			ImGui::ImageButton(m_textureButton, sf::Vector2f(100.f, 100.f));
			ImGui::Text("src/res/textures/particle.png"); // TODO: fix real path with windows file browser?
			ImGui::Spacing();
			ImGui::Separator();
		}
		ImGui::Spacing();

		ImGui::End();
	}

	void Application::render()
	{
		m_window.clear();
		m_blendMode == sf::BlendNone ? m_window.draw(m_particleSystem) : m_window.draw(m_particleSystem, m_blendMode);
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