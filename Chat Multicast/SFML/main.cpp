#include "Tools.h"
#include "Window.h"
#include "SFML/Network.hpp"

class Player
{
public:
	inline Player()
	{

	}

	~Player() = default;

	inline void UpdateMovement()
	{
		float dt = Tools::GetDeltaTime();

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
			m_pos.y -= m_speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
			m_pos.x -= m_speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
			m_pos.y += m_speed * dt;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
			m_pos.x += m_speed * dt;
	}

	inline void Display()
	{
		Window::rectangle.setPosition(m_pos);
		Window::rectangle.setFillColor(m_color);
	}

private:
	sf::Vector2f m_pos = sf::Vector2f(960.f, 540.f);
	float m_speed = 500.f;
	sf::Color m_color = sf::Color::White;

};

int main()
{
	Tools tools;
	Window window(sf::String("SFML Network"), sf::Style::Default);

	while (!window.IsDone())
	{
		Tools::Update();
		window.Update();



		window.Display();
	}

	return EXIT_SUCCESS;
}