#pragma once
#ifndef _EVENT_MANAGER_
#define _EVENT_MANAGER_

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <vector>

namespace sfev {

	using EventCallback = std::function<void(const sf::Event& event)>;
	template<typename T>
	using EventCallbackMap = std::unordered_map<T, EventCallback>;

	using CstEv = const sf::Event&;

	template<typename T>
	class SubTypeManager {
	public:
		SubTypeManager(std::function<T(const sf::Event&)> unpack): m_unpack(unpack){}
		~SubTypeManager() = default;

		void processEvent(const sf::Event& event) const {
			T sub_value = m_unpack(event);
			auto it(m_callmap.find(sub_value));
			if (it != m_callmap.end())
			{
				(it->second)(event);
			}
		}

		void addCallback(const T& sub_value, EventCallback callback) {
			m_callmap[sub_value] = callback;
		}

	private:
		EventCallbackMap<T> m_callmap;
		std::function<T(const sf::Event&)> m_unpack;
	};

	class EventManager {
	public:
		EventManager(sf::Window& window) :
			m_window(window),
			m_key_pressed_manager([](const sf::Event& event) {return event.key.code; }),
			m_key_released_manager([](const sf::Event& event) {return event.key.code; }),
			m_mouse_pressed_manager([](const sf::Event& event) {return event.mouseButton.button; }),
			m_mouse_released_manager([](const sf::Event& event) {return event.mouseButton.button; })
		{
			this->addEventCallback(sf::Event::EventType::KeyPressed, [&](const sf::Event& event) {m_key_pressed_manager.processEvent(event); });
			this->addEventCallback(sf::Event::EventType::KeyReleased, [&](const sf::Event& event) {m_key_released_manager.processEvent(event); });
			this->addEventCallback(sf::Event::EventType::MouseButtonPressed, [&](const sf::Event& event) {m_mouse_pressed_manager.processEvent(event); });
			this->addEventCallback(sf::Event::EventType::MouseButtonReleased, [&](const sf::Event& event) {m_mouse_released_manager.processEvent(event); });

		}

		void addEventCallback(sf::Event::EventType type, EventCallback callback) {
			m_event_callmap[type] = callback;
		}

		std::vector<sf::Event> processEvents() const {
			std::vector<sf::Event> non_processed;
			sf::Event event;

			while (m_window.pollEvent(event)) {
				sf::Event::EventType type = event.type;
				auto it(m_event_callmap.find(type));
				if (it != m_event_callmap.end()) {
					(it->second)(event);
				}
				else {
					non_processed.push_back(event);
				}
			}

			return non_processed;
		}

		void removeCallback(sf::Event::EventType type) {
			auto it(m_event_callmap.find(type));
			if (it != m_event_callmap.end()) {
				m_event_callmap.erase(it);
			}
		}

		void addKeyPressedCallback(sf::Keyboard::Key key_code, EventCallback callback) {
			m_key_pressed_manager.addCallback(key_code, callback);
		}

		void addKeyReleasedCallback(sf::Keyboard::Key key_code, EventCallback callback) {
			m_key_released_manager.addCallback(key_code, callback);
		}

		void addMousePressedCallback(sf::Mouse::Button button, EventCallback callback) {
			m_mouse_pressed_manager.addCallback(button, callback);
		}

		void addMouseReleasedCallback(sf::Mouse::Button button, EventCallback callback) {
			m_mouse_released_manager.addCallback(button, callback);
		}

	private:
		sf::Window& m_window;
		SubTypeManager<sf::Keyboard::Key> m_key_pressed_manager;
		SubTypeManager<sf::Keyboard::Key> m_key_released_manager;
		SubTypeManager<sf::Mouse::Button> m_mouse_pressed_manager;
		SubTypeManager<sf::Mouse::Button> m_mouse_released_manager;

		EventCallbackMap<sf::Event::EventType> m_event_callmap;
	};
}
#endif // !_EVENT_MANAGER_
