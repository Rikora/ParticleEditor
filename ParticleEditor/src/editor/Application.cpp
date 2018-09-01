////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <editor/Application.hpp>
#include <utils/Utility.hpp>
#include <SFML/Window/Event.hpp>
#include <Thor/Math.hpp>
#include <Thor/Vectors/PolarVector2.hpp>
#include <Thor/Animations/FadeAnimation.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <imgui.h>
#include <imgui-SFML.h>
#include <nfd.h>
#include <json.hpp>

#define IM_ARRAYSIZE(_ARR) ((int)(sizeof(_ARR)/sizeof(*_ARR)))

using nlohmann::json;

namespace px
{
	int Application::m_blendItem = 0;

	inline thor::Distribution<sf::Vector2f> scaleDistribution(sf::Vector2f size)
	{	
		return [=]() -> sf::Vector2f
		{
			auto res = thor::random(size.x, size.y);
			return sf::Vector2f(res, res);
		};
	}

	Application::Application() : m_window(sf::VideoMode(1200U, 800U), "Particle Editor", sf::Style::Close,
										  sf::ContextSettings(0U, 0U, 8U)), m_particlePath("particle.png"),
										  m_fullParticlePath("src/res/textures/particle.png"), m_playing(true)
	{
		m_window.setVerticalSyncEnabled(true);
		ImGui::SFML::Init(m_window);

		// Load texture
		m_playButtonTexture.loadFromFile("src/res/textures/icons/play_button.png");
		m_pauseButtonTexture.loadFromFile("src/res/textures/icons/pause_button.png");
		m_particle.texture.loadFromFile("src/res/textures/particle.png");
		m_playButton.setTexture(m_playButtonTexture);
		m_pauseButton.setTexture(m_pauseButtonTexture);
		m_textureButton.setTexture(m_particle.texture);
		m_particleSystem.setTexture(m_particle.texture);
		m_emitterConnection = m_particleSystem.addEmitter(thor::refEmitter(m_emitter), sf::seconds(m_particle.duration)); 
		m_playbackWatch.start();
	}

	Application::~Application()
	{
		outputParticleData();
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
		m_emitter.setParticleLifetime(thor::Distributions::uniform(sf::seconds(m_particle.lifetime.x), sf::seconds(m_particle.lifetime.y)));
		m_emitter.setParticleScale(scaleDistribution(m_particle.size));
		m_emitter.setParticleRotation(thor::Distributions::uniform(m_particle.rotation.x, m_particle.rotation.y));
		m_emitter.setParticleRotationSpeed(thor::Distributions::uniform(m_particle.rotationSpeed.x, m_particle.rotationSpeed.y));
		m_emitter.setParticleColor(m_particle.color);

		if (m_particle.velocityPolarVector)
		{
			if (m_particle.deflect)
				m_emitter.setParticleVelocity(thor::Distributions::deflect(
											  thor::PolarVector2f(m_particle.velocity.x, m_particle.velocity.y), m_particle.maxRotation));
			else
				m_emitter.setParticleVelocity(thor::PolarVector2f(m_particle.velocity.x, m_particle.velocity.y));
		}
		else
		{
			if(m_particle.deflect)
				m_emitter.setParticleVelocity(thor::Distributions::deflect(m_particle.velocity, m_particle.maxRotation));
			else
				m_emitter.setParticleVelocity(m_particle.velocity);
		}

		if (m_particle.shape == "Circle")
			m_emitter.setParticlePosition(thor::Distributions::circle(m_particle.position, m_particle.radius));
		else if (m_particle.shape == "Rectangle")
			m_emitter.setParticlePosition(thor::Distributions::rect(m_particle.position, m_particle.halfSize));
		else
			m_emitter.setParticlePosition(m_particle.position);

		if(m_playing)
			m_particleSystem.update(dt);
	}

	void Application::updateGUI()
	{
		// Simulation overlay
		static std::string status = "Playing";

		if (!m_emitterConnection.isConnected() && !m_particle.looping)
		{
			status = "Stopped";
			m_playbackWatch.reset();
		}

		ImGui::SetNextWindowPos(ImVec2(650, 25));
		ImGui::Begin("Overlay", NULL, ImVec2(165, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
		ImGui::SetCursorPos(ImVec2(52, 10)); // Center buttons
		if (ImGui::ImageButton(m_playButton, sf::Vector2f(20.f, 25.f), 1, sf::Color::Black))
		{
			m_playing = true;
			if (m_playing)
			{
				status = "Playing";
				if (!m_playbackWatch.isRunning())
					m_playbackWatch.start();
			}

			if (!m_particle.looping && !m_emitterConnection.isConnected()) // Play the emitter once when looping is disabled
				m_emitterConnection = m_particleSystem.addEmitter(thor::refEmitter(m_emitter), sf::seconds(m_particle.duration));
		}
		ImGui::SameLine();
		if (ImGui::ImageButton(m_pauseButton, sf::Vector2f(20.f, 25.f), 1, sf::Color::Black))
		{
			m_playing = !m_playing;

			if (m_playing)
			{
				status = "Playing";
				if (!m_playbackWatch.isRunning())
					m_playbackWatch.start();
			}
			else
			{
				status = "Paused";
				if (m_playbackWatch.isRunning())
					m_playbackWatch.stop();
			}
		}
		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Text("Playback Time: %.2f", m_playbackWatch.getElapsedTime().asSeconds());
		ImGui::Text("Status: %s", status.c_str());
		ImGui::End();

		// General properties
		static int floatPrecision = 3;
		const ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open..", "CTRL+O")) 
				{
				}

				if (ImGui::MenuItem("Save as..", "CTRL+S"))
				{
				}
				ImGui::EndMenu();
			}

			ImGui::Begin("Particle System", NULL, ImVec2(0, 0), 1.0f, flags);
			ImGui::Spacing();
			if (ImGui::Checkbox("Looping", &m_particle.looping))
			{
				if (!m_particle.looping)
					m_emitterConnection.disconnect();
				else
				{
					status = "Playing";
					if (!m_playbackWatch.isRunning())
						m_playbackWatch.start();
				}
			}
			ImGui::Spacing();
			ImGui::InputFloat("Duration", &m_particle.duration, 0.1f);
			ImGui::Spacing();
			ImGui::InputFloat("Particles", &m_particle.nrOfParticles, 1.f);
			ImGui::Spacing();
			ImGui::InputFloat2("Position", &m_particle.position.x, floatPrecision);
			ImGui::Spacing();
			ImGui::InputFloat2("Size", &m_particle.size.x, floatPrecision);
			ImGui::Spacing();
			ImGui::InputFloat2("Rotation", &m_particle.rotation.x, floatPrecision);
			ImGui::Spacing();
			ImGui::InputFloat2("Rotation Speed", &m_particle.rotationSpeed.x, floatPrecision);
			ImGui::Spacing();
			ImGui::InputFloat2("Lifetime", &m_particle.lifetime.x, floatPrecision);
			ImGui::Spacing();

			static float color[3] = { 1.f, 1.f, 1.f };
			if (ImGui::ColorEdit3("Color", color))
			{
				m_particle.color.r = static_cast<sf::Uint8>(color[0] * 255.f);
				m_particle.color.g = static_cast<sf::Uint8>(color[1] * 255.f);
				m_particle.color.b = static_cast<sf::Uint8>(color[2] * 255.f);
			}
			ImGui::Spacing();

			// Alpha over lifetime
			if (ImGui::CollapsingHeader("Alpha over Lifetime"))
			{
				ImGui::Spacing();
				if (ImGui::InputFloat2("Alpha", &m_particle.fader.x, floatPrecision))
				{
					utils::clampVec(m_particle.fader, 0.f, 1.f);

					// Time interval between [0, 1]
					if (m_particle.fader.x + m_particle.fader.y > 1.f)
						m_particle.fader = sf::Vector2f(0.f, 0.f);

					// Make sure we don't add more than one animation affector
					if (m_fadeConnection.isConnected())
						m_fadeConnection.disconnect();

					thor::FadeAnimation fader(m_particle.fader.x, m_particle.fader.y);
					m_fadeConnection = m_particleSystem.addAffector(thor::AnimationAffector(fader));
				}
				ImGui::Spacing();
			}
			ImGui::Spacing();

			// Velocity over lifetime
			if (ImGui::CollapsingHeader("Velocity over Lifetime"))
			{
				ImGui::Spacing();
				ImGui::Checkbox("Deflect", &m_particle.deflect);
				ImGui::Spacing();
				if (ImGui::Checkbox("Use Polar Vector", &m_particle.velocityPolarVector))
					m_particle.velocity = sf::Vector2f(0.f, 0.f);
				ImGui::Spacing();
				ImGui::InputFloat2("Velocity", &m_particle.velocity.x, floatPrecision);
				ImGui::Spacing();

				if (m_particle.deflect)
				{
					ImGui::InputFloat("Max rotation", &m_particle.maxRotation, 1.f);
					ImGui::Spacing();
				}
			}
			ImGui::Spacing();

			// Force over lifetime
			if (ImGui::CollapsingHeader("Force over Lifetime"))
			{
				ImGui::Spacing();
				if (ImGui::InputFloat2("Force", &m_particle.force.x, floatPrecision))
				{
					// Make sure we don't add more than one force affector
					if (m_forceConnection.isConnected())
						m_forceConnection.disconnect();

					m_forceConnection = m_particleSystem.addAffector(thor::ForceAffector(m_particle.force));
				}
				ImGui::Spacing();
			}
			ImGui::Spacing();

			// Torque over lifetime
			if (ImGui::CollapsingHeader("Torque over Lifetime"))
			{
				ImGui::Spacing();
				if (ImGui::InputFloat("Torque", &m_particle.torque, 1.f))
				{
					// Make sure we don't add more than one torque affector
					if (m_torqueConnection.isConnected())
						m_torqueConnection.disconnect();

					m_torqueConnection = m_particleSystem.addAffector(thor::TorqueAffector(m_particle.torque));
				}
				ImGui::Spacing();
			}
			ImGui::Spacing();

			// Shape
			if (ImGui::CollapsingHeader("Shape"))
			{
				ImGui::Spacing();
				static int m_shapeItem = 0;
				const char* itemList[] = { "None", "Circle", "Rectangle" };
				ImGui::Combo("Shape##1", &m_shapeItem, itemList, IM_ARRAYSIZE(itemList));

				if (m_shapeItem == 1)
				{
					m_particle.shape = "Circle";
					ImGui::Spacing();
					ImGui::InputFloat("Radius", &m_particle.radius, 1.f);
					ImGui::Spacing();
				}
				else if (m_shapeItem == 2)
				{
					m_particle.shape = "Rectangle";
					ImGui::Spacing();
					ImGui::InputFloat2("Half size", &m_particle.halfSize.x, floatPrecision);
					ImGui::Spacing();
				}
				else
					m_particle.shape = "None";
			}
			ImGui::Spacing();

			// Renderer
			ImGui::SetNextTreeNodeOpen(true, 2);
			if (ImGui::CollapsingHeader("Renderer"))
			{
				ImGui::Spacing();
				const char* itemList[] = { "None", "BlendAdd", "BlendAlpha", "BlendMultiply" };
				ImGui::Combo("Blend mode", &m_blendItem, itemList, IM_ARRAYSIZE(itemList));

				switch (m_blendItem)
				{
				case 0:
					m_particle.blendMode = sf::BlendNone;
					break;
				case 1:
					m_particle.blendMode = sf::BlendAdd;
					break;
				case 2:
					m_particle.blendMode = sf::BlendAlpha;
					break;
				case 3:
					m_particle.blendMode = sf::BlendMultiply;
					break;
				default:
					break;
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Spacing();
				if (ImGui::ImageButton(m_textureButton, sf::Vector2f(100.f, 100.f), -1, sf::Color::Black, m_particle.color))
				{
					openTextureFile(m_fullParticlePath, m_particlePath);
					m_particle.texture.loadFromFile(m_fullParticlePath);
					m_particleSystem.setTexture(m_particle.texture);
					m_textureButton.setTexture(m_particle.texture);
				}
				ImGui::Text(m_particlePath.c_str());
				ImGui::Spacing();
				ImGui::Separator();
			}
			ImGui::Spacing();
			ImGui::End();
			ImGui::EndMainMenuBar();
		}
		
		// Prevent the editor from crashing on undefined behavior
		utils::constrainNegatives(m_particle.maxRotation);
		utils::constrainNegatives(m_particle.duration);
		utils::constrainNegatives(m_particle.nrOfParticles);
		utils::constrainNegatives(m_particle.radius);
		utils::constrainNegativesVec(m_particle.size);
		utils::constrainNegativesVec(m_particle.lifetime);
		utils::constrainNegativesVec(m_particle.halfSize);
		utils::constrainNegativesVec(m_particle.rotationSpeed);
		utils::constrainDistrVec(m_particle.rotation);
		utils::constrainDistrVec(m_particle.lifetime);
		utils::constrainDistrVec(m_particle.rotationSpeed);
		utils::constrainDistrVec(m_particle.size);

		if (!m_emitterConnection.isConnected() && m_particle.looping)
			m_emitterConnection = m_particleSystem.addEmitter(thor::refEmitter(m_emitter), sf::seconds(m_particle.duration));
	}

	void Application::render()
	{
		m_window.clear();
		m_particle.blendMode == sf::BlendNone ? m_window.draw(m_particleSystem) : m_window.draw(m_particleSystem, m_particle.blendMode);
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

	// File browser for selecting a texture
	void Application::openTextureFile(std::string & filePath, std::string & file)
	{
		nfdchar_t* outPath = NULL;
		nfdresult_t result = NFD_OpenDialog("png,jpg", NULL, &outPath);

		if (result == NFD_OKAY)
		{
			filePath = outPath;
			std::replace(filePath.begin(), filePath.end(), '\\', '/');
			auto found = filePath.find_last_of("/");
			file = filePath.substr(found + 1);
			free(outPath);
		}
		else if (result == NFD_CANCEL)
			printf("User pressed cancel.\n");
		else
			printf("Error: %s\n", NFD_GetError());
	}

	// Write particle data to json file
	void Application::outputParticleData()
	{
		std::string directory = "src/res/data/test.json";
		auto enableFloat = [](const float & value) -> bool { return value == 0.f ? false : true; };
		auto enableVec = [](const sf::Vector2f & vec) -> bool { return vec == sf::Vector2f(0.f, 0.f) ? false : true; };

		m_particle.enableTorqueAff = enableFloat(m_particle.torque);
		m_particle.enableFadeAff = enableVec(m_particle.fader);
		m_particle.enableForceAff = enableVec(m_particle.force);

		// Data
		json data = {
			{ "texture", m_fullParticlePath },
			{ "looping", m_particle.looping },
			{ "deflect", m_particle.deflect },
			{ "velPolarVector", m_particle.velocityPolarVector },
			{ "enableTorqueAff", m_particle.enableTorqueAff },
			{ "enableFadeAff", m_particle.enableFadeAff },
			{ "enableForceAff", m_particle.enableForceAff },
			{ "duration", m_particle.duration },
			{ "circleRadius",  m_particle.radius},
			{ "particles", m_particle.nrOfParticles },
			{ "torque",  m_particle.torque},
			{ "maxRotation", m_particle.maxRotation },
			{ "rotation", m_particle.rotation.x, m_particle.rotation.y },
			{ "lifetime", m_particle.lifetime.x, m_particle.lifetime.y },
			{ "rectHalfSize", m_particle.halfSize.x, m_particle.halfSize.y },
			{ "position", m_particle.position.x, m_particle.position.y },
			{ "size", m_particle.size.x, m_particle.size.y },
			{ "velocity", m_particle.velocity.x, m_particle.velocity.y },
			{ "fader", m_particle.fader.x, m_particle.fader.y },
			{ "force", m_particle.force.x, m_particle.force.y },
			{ "color", m_particle.color.r, m_particle.color.g, m_particle.color.b, m_particle.color.a },
			{ "force", m_particle.force.x, m_particle.force.y },
			{ "blendMode", m_blendItem },
			{ "shape", m_particle.shape },
		};

		std::ofstream o(directory);
		o << std::setw(4) << data << std::endl;
	}
}