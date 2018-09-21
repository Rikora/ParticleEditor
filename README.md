# Particle Editor

Particle editor based on the [Thor](https://github.com/Bromeon/Thor) library extension to [SFML](https://github.com/SFML/SFML). 

## Features

The editor covers most of the features provided by the particle module from Thor. There is also
support for:

* Save particle data to a `json` file
* Open an existing `json` file with particle data
* Change particle texture with file browsing

## Screenshot

![screenshot](https://i.imgur.com/ovCAsrP.png)

## How-to

Setting up:

* Build the provided Visual Studio 2017 solution (`sln`) as x86 (Debug)
* Copy the contents of the `dll` folder to your `Debug` folder
* Run the program

## How-to integrate

* Add [json](https://github.com/nlohmann/json) to your project include settings
* Add `ParticleLoader.hpp` and `ParticleLoader.cpp` to your project

## Example code

```c++
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include "ParticleLoader.hpp"

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particles");
    px::ParticleLoader system("src/res/data/example.json", sf::Vector2f(400.f, 400.f));
    sf::Clock clock;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
		
        system.update(clock.restart());
        window.clear();
        window.draw(system);
        window.display();
    }

    return 0;
}
```
## Remarks

* The size of a particle is uniform and thus ranges from a particle size to another
* Trying to manipulate a range input field with invalid input will cause the range to reset

## Dependencies

* [imgui](https://github.com/ocornut/imgui)
* [imgui-sfml](https://github.com/eliasdaler/imgui-sfml)
* [nfd](https://github.com/mlabbe/nativefiledialog)
* [json](https://github.com/nlohmann/json)
* [SFML](https://github.com/SFML/SFML)
* [Thor](https://github.com/Bromeon/Thor)

## License

This library is licensed under the MIT License, see LICENSE for more information.
