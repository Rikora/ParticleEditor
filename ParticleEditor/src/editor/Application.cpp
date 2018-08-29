////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <editor/Application.hpp>
#include <iostream>
#include <Windows.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Window/Event.hpp>
#include <Thor/Math.hpp>

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

namespace px
{
	Application::Application() : m_window(sf::VideoMode(1200U, 800U), "Particle Editor", sf::Style::Close,
										  sf::ContextSettings(0U, 0U, 8U)), m_particlePath("particle.png")
	{
		m_window.setVerticalSyncEnabled(true);
		ImGui::SFML::Init(m_window);

		// Load texture
		m_particleTexture.loadFromFile("src/res/textures/particle.png");
		m_textureButton.setTexture(m_particleTexture);
		m_particleSystem.setTexture(m_particleTexture);
		m_emitterConnection = m_particleSystem.addEmitter(thor::refEmitter(m_emitter));
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
		// Update emitter
		m_emitter.setEmissionRate(m_particle.nrOfParticles);
		m_emitter.setParticleLifetime(sf::seconds(m_particle.lifetime));
		m_emitter.setParticleScale(m_particle.scale);
		m_emitter.setParticleRotation(m_particle.rotation);
		m_emitter.setParticleColor(m_particle.color);

		if (m_shape == "Circle")
			m_emitter.setParticlePosition(thor::Distributions::circle(m_particle.position, m_particle.radius));
		else if (m_shape == "Rectangle")
			m_emitter.setParticlePosition(thor::Distributions::rect(m_particle.position, m_particle.halfSize));
		else
			m_emitter.setParticlePosition(m_particle.position);

		m_particleSystem.update(dt);
	}

	void Application::updateGUI()
	{
		auto constrainNegatives = [](float & value)
		{
			if (value < 0.f)
				value = 0.f;
		};

		auto constrainNegativesVec = [](sf::Vector2f & value)
		{
			if (value.x < 0.f)
				value.x = 0.f;
			if (value.y < 0.f)
				value.y = 0.f;
		};

		// General properties
		static int floatPrecision = 3;
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		ImGui::Begin("Particle System", NULL, ImVec2(0, 0), 1.0f, flags);	
		ImGui::Spacing();

		// Note: The emitter/affector can be added for a duration of time also!
		if (ImGui::Checkbox("Looping", &m_particle.looping))
			if(!m_emitterConnection.isConnected())
				m_emitterConnection = m_particleSystem.addEmitter(thor::refEmitter(m_emitter));
		else
			m_emitterConnection.disconnect();

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

		static float color[3] = { 0.f, 0.f, 0.f };
		if (ImGui::ColorEdit3("Color", color))
		{
			m_particle.color.r = static_cast<sf::Uint8>(color[0] * 255.f);
			m_particle.color.g = static_cast<sf::Uint8>(color[1] * 255.f);
			m_particle.color.b = static_cast<sf::Uint8>(color[2] * 255.f);
		}
		ImGui::Spacing();

		// Emission
		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Emission"))
		{
		}
		ImGui::Spacing();

		// Shape
		ImGui::SetNextTreeNodeOpen(true, 2);
		if (ImGui::CollapsingHeader("Shape"))
		{
			ImGui::Spacing();
			static int m_shapeItem = 0;
			const char* itemList[] = { "None", "Circle", "Rectangle" };
			ImGui::Combo("Shape##1", &m_shapeItem, itemList, IM_ARRAYSIZE(itemList));

			if (m_shapeItem == 1)
			{
				m_shape = "Circle";
				ImGui::Spacing();
				ImGui::InputFloat("Radius", &m_particle.radius, 1.f);
				ImGui::Spacing();
			}
			else if (m_shapeItem == 2)
			{
				m_shape = "Rectangle";
				ImGui::Spacing();
				ImGui::InputFloat2("Half size", &m_particle.halfSize.x, floatPrecision);
				ImGui::Spacing();
			}
			else
				m_shape = "None";
		}
		ImGui::Spacing();

		// Renderer
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
			if (ImGui::ImageButton(m_textureButton, sf::Vector2f(100.f, 100.f), -1, sf::Color::Black, m_particle.color))
			{
				openFile(m_fullParticlePath, m_particlePath);
				m_particleTexture.loadFromFile(m_fullParticlePath);
				m_particleSystem.setTexture(m_particleTexture);
				m_textureButton.setTexture(m_particleTexture);
			}
			ImGui::Text(m_particlePath.c_str());
			ImGui::Spacing();
			ImGui::Separator();
		}
		ImGui::Spacing();
		ImGui::End();

		// Prevent the editor from crashing on negative values
		constrainNegatives(m_particle.nrOfParticles);
		constrainNegatives(m_particle.lifetime);
		constrainNegatives(m_particle.radius);
		constrainNegativesVec(m_particle.halfSize);
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

	// File browser with the Windows API
	void Application::openFile(std::string & filePath, std::string & file)
	{
		char filename[MAX_PATH] = "\0";
		OPENFILENAME ofn;
		ZeroMemory(&filename, sizeof(filename));
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = m_window.getSystemHandle();
		const char *filter =
			"Images (.jpg;.png)\0*.jpg;*.png\0"
			"All Files (*.*)\0*.*\0\0";
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filename;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_HIDEREADONLY;

		if (GetOpenFileNameA(&ofn))
		{
			filePath = filename;
			std::replace(filePath.begin(), filePath.end(), '\\', '/');
			auto found = filePath.find_last_of("/");
			file = filePath.substr(found + 1);
		}
	}
}