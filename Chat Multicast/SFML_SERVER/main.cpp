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
	//sf::IpAddress sIpAddress(192, 168, 10, 147);
	//sf::IpAddress sIpAddress2(192, 168, 10, 148);
	sf::IpAddress sIpAddress(192, 168, 0, 29);
	sf::IpAddress sIpAddress2(192, 168, 0, 29);
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

	Window window(sf::String("SFML Network SERVER"), sf::Style::Default);

	//sf::Thread serverThread(&ServerUpdate);
	//serverThread.launch();


	Player p1;

	// server
	Player p;
	sf::UdpSocket sSocket;
	sSocket.setBlocking(false);
	sSocket.bind(55002);
	//sf::IpAddress sIpAddress = sf::IpAddress::getLocalAddress();
	//sf::IpAddress cIpAddress(192, 168, 10, 148);
	//sf::IpAddress cIpAddress2(192, 168, 10, 147);
	sf::IpAddress cIpAddress(192, 168, 0, 29);
	sf::IpAddress cIpAddress2(192, 168, 0, 29);
	//sf::IpAddress sIpAddress(255, 1, 2, 3);
	sf::Packet sPacket;
	unsigned short sPort = 55002;

	int nbPlayer = 1;

	while (!Window::IsDone())
	{	
		Tools::Update();
		Window::Update();

		//std::cout << "SERVER Framerate : " << 1.f / Tools::GetDeltaTime() << std::endl;

		//window.Update();

		//p1.UpdateMovement();

		cPacket.clear();

		// recieve
		if (nbPlayer >= 1)
			sSocket.receive(cPacket, cIpAddress, sPort);
		if (nbPlayer >= 2)
			sSocket.receive(cPacket, cIpAddress2, sPort);


		// send
		if (nbPlayer >= 1 && cPacket.getDataSize() > 0)
			sSocket.send(cPacket, cIpAddress, sPort);
		if (nbPlayer >= 2)
			sSocket.send(cPacket, cIpAddress2, sPort);

		//cPacket >> p;
		//std::cout << "SEND : " << p.m_pos.x << std::endl;
		
		//cPacket.clear();
		//cPacket << p1;
		////cSocket.send(cPacket, sf::IpAddress::getLocalAddress().toString(), 55002);
		////cSocket.send(cPacket, sf::IpAddress(255, 1, 2, 3), 55002);
		//cSocket.send(cPacket, sf::IpAddress(192, 168, 10, 147), 55002);
		//cSocket.send(cPacket, sf::IpAddress(192, 168, 10, 148), 55002);
		//
		//
		//
		//sPacket.clear();
		//if (cPacket.getDataSize() > 0)
		//	sSocket.receive(sPacket, sIpAddress, sPort);
		//sPacket >> p;
		//
		////p.Display();
		//
		//
		////sPacket.clear();
		////sSocket.receive(sPacket, sIpAddress2, sPort);
		//sPacket >> p;
		//
		//p.Display();

		Window::Display();
		//p1.Display();
		
		//window.Display();
	}

//unsigned short sPort = 55002;
//cPacket << p1;
////cPacket.append(reinterpret_cast<char*>(&p1.m_pos.x))
//cSocket.send(cPacket, sf::IpAddress::getLocalAddress().toString(), 55002);

//cPacket << sPort;


//p1.m_pos = sf::Vector2f(-1.f, -2.f);
//sSocket.receive(sPacket, sIpAddress, sPort);
//Player cPlayer;
//sPacket >> cPlayer;
//std::cout << sPacket << std::endl;

//// Send a message to 192.168.1.50 on port 55002
//std::string cMessage = "Hi, I am " + sf::IpAddress::getLocalAddress().toString();
////cSocket.send(cMessage.c_str(), cMessage.size() + 1, "192.168.10.147", 55002);
//
//// Receive an answer (most likely from 192.168.1.50, but could be anyone else)
//char cBuffer[1024];
//std::size_t cReceived = 0;
//sf::IpAddress cSender;
//sSocket.receive(cBuffer, sizeof(cBuffer), cReceived, cSender, sPort);
//std::cout << cSender.toString() << " said: " << cBuffer << std::endl;

	return EXIT_SUCCESS;
}