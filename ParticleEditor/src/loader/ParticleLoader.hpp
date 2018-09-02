#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <Thor/Particles.hpp>

namespace sf
{
	class Color;
	class RenderTarget;
}

namespace px
{
	class ParticleLoader : public sf::Drawable
	{
	public:
		ParticleLoader(const std::string & filePath, const sf::Vector2f & position);
		~ParticleLoader() = default;

	public:
		struct Properties
		{
			bool looping = true;
			bool deflect = false;
			bool velocityPolarVector = false;
			bool enableTorqueAff = false;
			bool enableFadeAff = false;
			bool enableForceAff = false;
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
			sf::Texture texture = sf::Texture();
			sf::BlendMode blendMode = sf::BlendNone;
			std::string shape = "None";
			std::string fullParticlePath;
		};

	public:
		void update(sf::Time dt);
		virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

	private:
		void loadParticleData(const std::string & filePath, const sf::Vector2f & position);

	private:
		Properties m_particle;
		thor::ParticleSystem m_particleSystem;
		thor::UniversalEmitter m_emitter;
	};
}