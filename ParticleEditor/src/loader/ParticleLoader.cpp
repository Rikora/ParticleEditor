////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <loader/ParticleLoader.hpp>
#include <SFML/Graphics/Color.hpp>
#include <Thor/Animations/FadeAnimation.hpp>
#include <fstream>
#include <iomanip>
#include <json.hpp>

using nlohmann::json;

namespace px
{
	ParticleLoader::ParticleLoader(const std::string & filePath, const sf::Vector2f & position)
	{
		loadParticleData(filePath, position);
	}

	void ParticleLoader::loadParticleData(const std::string & filePath, const sf::Vector2f & position)
	{
		std::ifstream i(filePath);
		json data;
		i >> data;

		// Data
		m_particle.position = position;
		m_particle.fullParticlePath = data["texture"].get<std::string>();
		m_particle.looping = data["looping"].get<bool>();
		m_particle.deflect = data["deflect"].get<bool>();
		m_particle.enableTorqueAff = data["enableTorqueAff"].get<bool>();
		m_particle.enableFadeAff = data["enableFadeAff"].get<bool>();
		m_particle.enableForceAff = data["enableForceAff"].get<bool>();
		m_particle.velocityPolarVector = data["velPolarVector"].get<bool>();
		m_particle.duration = data["duration"].get<float>();
		m_particle.radius = data["circleRadius"].get<float>();
		m_particle.nrOfParticles = data["particles"].get<float>();
		m_particle.torque = data["torque"].get<float>();
		m_particle.maxRotation = data["maxRotation"].get<float>();
		m_particle.rotationSpeed = sf::Vector2f(data["rotationSpeed"][0].get<float>(), data["rotationSpeed"][1].get<float>());
		m_particle.rotation = sf::Vector2f(data["rotation"][0].get<float>(), data["rotation"][1].get<float>());
		m_particle.lifetime = sf::Vector2f(data["lifetime"][0].get<float>(), data["lifetime"][1].get<float>());
		m_particle.halfSize = sf::Vector2f(data["rectHalfSize"][0].get<float>(), data["rectHalfSize"][1].get<float>());
		m_particle.size = sf::Vector2f(data["size"][0].get<float>(), data["size"][1].get<float>());
		m_particle.velocity = sf::Vector2f(data["velocity"][0].get<float>(), data["velocity"][1].get<float>());
		m_particle.fader = sf::Vector2f(data["fader"][0].get<float>(), data["fader"][1].get<float>());
		m_particle.force = sf::Vector2f(data["force"][0].get<float>(), data["force"][1].get<float>());
		m_particle.color = sf::Color(data["color"][0].get<sf::Uint8>(), data["color"][1].get<sf::Uint8>(),
			data["color"][2].get<sf::Uint8>(), data["color"][3].get<sf::Uint8>());
		m_particle.shape = data["shape"].get<std::string>();

		// Set blend mode
		switch (data["blendMode"].get<int>())
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

		// Affectors
		if (m_particle.enableTorqueAff)
			m_particleSystem.addAffector(thor::TorqueAffector(m_particle.torque));
		if (m_particle.enableForceAff)
			m_particleSystem.addAffector(thor::ForceAffector(m_particle.force));
		if (m_particle.enableFadeAff)
		{
			thor::FadeAnimation fader(m_particle.fader.x, m_particle.fader.y);
			m_particleSystem.addAffector(thor::AnimationAffector(fader));
		}

		// Set texture and prepare emitter
		m_particle.texture.loadFromFile(m_particle.fullParticlePath);
		m_particleSystem.setTexture(m_particle.texture);
		m_particle.looping ? m_particleSystem.addEmitter(m_emitter) : m_particleSystem.addEmitter(m_emitter, sf::seconds(m_particle.duration));
	}
}