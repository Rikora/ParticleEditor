//////////////////////////////////////////////////////////////
//// Headers
//////////////////////////////////////////////////////////////
#include <editor/Application.hpp>

int main()
{
	px::Application app;
	app.run();
	return 0;
}

//#include <SFML/Graphics.hpp>
//#include <loader/ParticleLoader.hpp>
//
//int main()
//{
//	sf::RenderWindow window(sf::VideoMode(800, 600), "Particles");
//	px::ParticleLoader system("src/res/data/example.json", sf::Vector2f(400.f, 400.f));
//	sf::Clock clock;
//
//	while (window.isOpen())
//	{
//		sf::Event event;
//
//		while (window.pollEvent(event))
//		{
//			if (event.type == sf::Event::Closed)
//				window.close();
//		}
//		
//		system.update(clock.restart());
//		window.clear();
//		window.draw(system);
//		window.display();
//	}
//
//	return 0;
//}