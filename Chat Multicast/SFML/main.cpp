#include "Tools.h"
#include "Window.h"
#include "SFML/Network.hpp"

struct Player
{
	inline Player()
	{
		m_color.r = static_cast<sf::Uint8>(rand() % 256);
		m_color.g = static_cast<sf::Uint8>(rand() % 256);
		m_color.b = static_cast<sf::Uint8>(rand() % 256);
		m_color.a = 255;
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

	inline void Display() const
	{
		Window::rectangle.setPosition(m_pos);
		Window::rectangle.setSize(sf::Vector2f(50.f, 50.f));
		Window::rectangle.setFillColor(m_color);

		Window::Draw();
	}

	sf::Vector2f m_pos = sf::Vector2f(960.f, 540.f);
	float m_speed = 500.f;
	sf::Color m_color = sf::Color::White;

};

sf::Packet& operator <<(sf::Packet& _packet, const sf::Vector2f& _vec2f)
{
	return _packet << _vec2f.x << _vec2f.y;
}
sf::Packet& operator >>(sf::Packet& _packet, sf::Vector2f& _vec2f)
{
	return _packet >> _vec2f.x >> _vec2f.y;
}

sf::Packet& operator <<(sf::Packet& _packet, const sf::Color& _color)
{
	return _packet << _color.r << _color.g << _color.b << _color.a;
}
sf::Packet& operator >>(sf::Packet& _packet, sf::Color& _color)
{
	return _packet >> _color.r >> _color.g >> _color.b >> _color.a;
}

sf::Packet& operator <<(sf::Packet& _packet, const Player& _player)
{
	return _packet << _player.m_pos << _player.m_color;
}
sf::Packet& operator >>(sf::Packet& _packet, Player& _player)
{
	return _packet >> _player.m_pos >> _player.m_color;
}

void ServerUpdate()
{
	Player p;
	sf::UdpSocket sSocket;
	sSocket.bind(55002);
	//sf::IpAddress sIpAddress = sf::IpAddress::getLocalAddress();
	sf::IpAddress sIpAddress(192, 168, 10, 147);
	sf::IpAddress sIpAddress2(192, 168, 10, 148);
	//sf::IpAddress sIpAddress(255, 1, 2, 3);
	sf::Packet sPacket;
	unsigned short sPort = 55002;

	while (!Window::IsDone())
	{
		//Window::Update();
		//
		//
		//sPacket.clear();
		//sSocket.receive(sPacket, sIpAddress, sPort);
		//sPacket >> p;
		//
		//p.Display();
		//
		//
		//sPacket.clear();
		//sSocket.receive(sPacket, sIpAddress2, sPort);
		//sPacket >> p;
		//
		//p.Display();
		//
		//Window::Display();
	}
}

int main()
{
	sf::UdpSocket cSocket;
	cSocket.bind(55001);
	sf::Packet cPacket;


	Window window(sf::String("SFML Network"), sf::Style::Default);

	//sf::Thread serverThread(&ServerUpdate);
	//serverThread.launch();

	Player p1;

	Player p;
	std::cout << p.m_color.r;
	sf::UdpSocket sSocket;
	sSocket.setBlocking(false);
	unsigned short sPort = 55002;
	sSocket.bind(sPort);
	//sf::IpAddress sIpAddress = sf::IpAddress::getLocalAddress();
	//sf::IpAddress sIpAddress(192, 168, 10, 147);
	//sf::IpAddress sIpAddress2(192, 168, 10, 148);
	sf::IpAddress sIpAddress(192, 168, 0, 29);
	//sf::IpAddress sIpAddress(255, 1, 2, 3);
	sf::Packet sPacket;

	while (!Window::IsDone())
	{	
		Tools::Update();
		Window::Update();

		//std::cout << "Framerate : " <<  1.f / Tools::GetDeltaTime() << std::endl;

		p1.UpdateMovement();
		
		cPacket.clear();
		cPacket << p1;
		cSocket.send(cPacket, sIpAddress, sPort);
		
		
		
		sPacket.clear();
			sSocket.receive(sPacket, sIpAddress, sPort);
		//sPacket >> p;
		if (sPacket.getDataSize() > 0)
			std::cout << p.m_pos.x << std::endl;
		
		p.Display();
		p1.Display();
		
		Window::Display();
	}

	return EXIT_SUCCESS;
}