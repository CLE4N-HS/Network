#pragma once
#include "Tools.h"

class Window
{
public:
	Window();
	Window(const sf::String& title, sf::Uint32 style = sf::Style::Default);
	~Window() = default;

	/// <summary>
	/// Call this at the start of every frame
	/// </summary>
	static void Update();

	/// <summary>
	/// Call this at the end of every frame in order to draw everything on the RenderWindow
	/// </summary>
	static void Display();

	static inline bool IsDone() { return m_isDone; }
	static inline bool HasFocus() { return m_hasFocus; }
	static inline sf::Vector2f GetMousePos() { return m_mousePos; }
	static void Draw(const sf::Drawable& drawable = rectangle, const sf::RenderStates& states = sf::RenderStates::Default);

	static sf::RectangleShape rectangle;
	static sf::Text text;

private:
	static sf::RenderWindow m_renderWindow;
	static sf::RenderTexture m_renderTexture;
	static sf::Font m_font;

	static sf::VideoMode m_videoMode;
	static sf::String m_title;
	static sf::Uint32 m_style;
	static bool m_isFullscreen;
	static unsigned int m_framerateLimit;
	static bool m_isDone;
	static bool m_hasFocus;
	static float m_fullscreenTimer;

	static sf::Event m_event;
	static sf::Vector2f m_mousePos;

	static sf::Sprite m_sprite;
	static sf::Texture m_texture;


	static void ToggleFullscreen();
	static void CreateWindow();

};
