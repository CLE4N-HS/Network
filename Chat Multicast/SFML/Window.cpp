#include "Window.h"
//#include "MouseManager.h"

sf::RectangleShape Window::rectangle = sf::RectangleShape();
sf::Text Window::text = sf::Text();

sf::RenderWindow Window::m_renderWindow;/* = sf::RenderWindow();*/
sf::RenderTexture Window::m_renderTexture;// = sf::RenderTexture();
sf::Font Window::m_font = sf::Font();
sf::VideoMode Window::m_videoMode = sf::VideoMode::getDesktopMode();
sf::String Window::m_title = sf::String("Echoes of Runterra");
sf::Uint32 Window::m_style = sf::Style::Default;
bool Window::m_isFullscreen = false;
unsigned int Window::m_framerateLimit = 60;
bool Window::m_isDone = false;
bool Window::m_hasFocus = false;
float Window::m_fullscreenTimer = 0.f;
sf::Event Window::m_event = sf::Event();
sf::Vector2f Window::m_mousePos = sf::Vector2f();
sf::Sprite Window::m_sprite = sf::Sprite();
sf::Texture Window::m_texture = sf::Texture();

Window::Window() : Window(m_title, m_style)
{
}

Window::Window(const sf::String& title, sf::Uint32 style)
{
	CreateWindow();
	m_renderTexture.create(m_videoMode.width, m_videoMode.height);
	//m_font.loadFromFile("../Resources/NeoTech.ttf"); // default font for now
	text.setFont(m_font);
}

void Window::Update()
{
	while (m_renderWindow.pollEvent(m_event))
	{
		if (m_event.type == sf::Event::Closed
			|| sf::Mouse::isButtonPressed(sf::Mouse::Middle)) // TODO remove this
			m_isDone = true;
	}

	m_hasFocus = m_renderWindow.hasFocus();

	m_mousePos = sf::Vector2f(sf::Mouse::getPosition(m_renderWindow));

	m_fullscreenTimer += ((m_fullscreenTimer > 0.5f) ? 0.f : Tools::GetDeltaTime());
	if (m_hasFocus && m_fullscreenTimer >= 0.5f && sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
		ToggleFullscreen();

	//MouseManager::Update();
}

void Window::Display()
{
	m_renderWindow.clear();
	m_renderTexture.display();

	m_texture = m_renderTexture.getTexture();
	m_renderTexture.clear();
	m_sprite.setTexture(m_texture, true);
	m_renderWindow.draw(m_sprite);
	m_renderWindow.display();
}

void Window::Draw(const sf::Drawable& drawable, const sf::RenderStates& states)
{
	m_renderTexture.draw(drawable, states);
}

void Window::ToggleFullscreen()
{
	m_fullscreenTimer = 0.f;
	m_style = (m_style == sf::Style::Fullscreen ? sf::Style::Default : sf::Style::Fullscreen);
	CreateWindow();
}

void Window::CreateWindow()
{
	m_renderWindow.create(m_videoMode, m_title, m_style);
	m_isFullscreen = (m_style == sf::Style::Fullscreen);
	m_renderWindow.setFramerateLimit(m_framerateLimit);
}
