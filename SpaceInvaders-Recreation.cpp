#include <array>
#include <chrono>
#include <random>
#include <SFML/Graphics.hpp>
#include <iostream>

#include "Headers/Animation.hpp"
#include "Headers/DrawText.hpp"
#include "Headers/Global.hpp"
#include "Headers/Enemy.hpp"
#include "Headers/EnemyManager.hpp"
#include "Headers/Ufo.hpp"
#include "Headers/Player.hpp"

int main()
{
	float controls_timer = 0;
	bool game_over = 0;
	bool next_level = 0;
	unsigned short level = 0;
	unsigned short next_level_timer = NEXT_LEVEL_TRANSITION;
	std::chrono::microseconds lag(0);
	std::chrono::steady_clock::time_point previous_time;
	std::mt19937_64 random_engine(std::chrono::system_clock::now().time_since_epoch().count());
	sf::Event event;
	sf::RenderWindow window(sf::VideoMode(SCREEN_RESIZE * SCREEN_WIDTH, SCREEN_RESIZE * SCREEN_HEIGHT), "Space Invaders", sf::Style::Close);
	window.setView(sf::View(sf::FloatRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)));
	sf::Sprite background_sprite;
	sf::Sprite powerup_bar_sprite;
	sf::Texture background_texture;
	background_texture.loadFromFile("Resources/Images/Background.png");
	sf::Texture font_texture;
	font_texture.loadFromFile("Resources/Images/Font.png");
	sf::Texture powerup_bar_texture;
	powerup_bar_texture.loadFromFile("Resources/Images/PowerupBar.png");
	EnemyManager enemy_manager;
	Player player;
	Ufo ufo(random_engine);
	background_sprite.setTexture(background_texture);
	powerup_bar_sprite.setTexture(powerup_bar_texture);
	previous_time = std::chrono::steady_clock::now();

	while (1 == window.isOpen())
	{
		std::chrono::microseconds delta_time = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - previous_time);
		lag += delta_time;
		previous_time += delta_time;
		while (FRAME_DURATION <= lag)
		{
			lag -= FRAME_DURATION;
			while (1 == window.pollEvent(event))
			{
				switch (event.type)
				{
				case sf::Event::Closed:
				{
					window.close();
				}
				}
			}
			if (1 == player.get_dead_animation_over())
			{
				game_over = 1;
			}

			if (1 == enemy_manager.reached_player(player.get_y()))
			{
				player.die();
			}

			if (0 == game_over)
			{
				if (0 == enemy_manager.get_enemies().size())
				{
					if (0 == next_level_timer)
					{
						next_level = 0;

						level++;
						next_level_timer = NEXT_LEVEL_TRANSITION;

						player.reset();

						enemy_manager.reset(level);

						ufo.reset(1, random_engine);
					}
					else
					{
						next_level = 1;

						next_level_timer--;
					}
				}
				else
				{
					player.update(random_engine, enemy_manager.get_enemy_bullets(), enemy_manager.get_enemies(), ufo);

					enemy_manager.update(random_engine);

					ufo.update(random_engine);
				}
			}
			else if (1 == sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
			{
				game_over = 0;

				level = 0;

				player.reset();

				enemy_manager.reset(level);

				ufo.reset(1, random_engine);
			}

			if (FRAME_DURATION > lag)
			{
				window.clear();

				window.draw(background_sprite);
				if (0 == player.get_dead())
				{
					enemy_manager.draw(window);

					ufo.draw(window);
					if (0 < player.get_current_power())
					{
						powerup_bar_sprite.setColor(sf::Color(255, 255, 255));
						powerup_bar_sprite.setPosition(SCREEN_WIDTH - powerup_bar_texture.getSize().x - 0.25f * BASE_SIZE, 0.25f * BASE_SIZE);
						powerup_bar_sprite.setTextureRect(sf::IntRect(0, 0, powerup_bar_texture.getSize().x, BASE_SIZE));

						window.draw(powerup_bar_sprite);

						powerup_bar_sprite.setPosition(SCREEN_WIDTH - powerup_bar_texture.getSize().x - 0.125f * BASE_SIZE, 0.25f * BASE_SIZE);
						//Calculating the length of the bar.
						powerup_bar_sprite.setTextureRect(sf::IntRect(0.125f * BASE_SIZE, BASE_SIZE, ceil(player.get_power_timer() * static_cast<float>(powerup_bar_texture.getSize().x - 0.25f * BASE_SIZE) / POWERUP_DURATION), BASE_SIZE));

						switch (player.get_current_power())
						{
						case 1:
						{
							powerup_bar_sprite.setColor(sf::Color(0, 146, 255));

							break;
						}
						case 2:
						{
							powerup_bar_sprite.setColor(sf::Color(255, 0, 0));

							break;
						}
						case 3:
						{
							powerup_bar_sprite.setColor(sf::Color(255, 219, 0));

							break;
						}
						case 4:
						{
							powerup_bar_sprite.setColor(sf::Color(219, 0, 255));
						}
						}

						window.draw(powerup_bar_sprite);
					}
				}

				player.draw(window);

				draw_text(0.25f * BASE_SIZE, 0.25f * BASE_SIZE, "Level: " + std::to_string(level), window, font_texture, 1.0f);

				controls_timer += 0.01f;

				if (controls_timer <= 3.0f)
					draw_text(0.25f * BASE_SIZE, 0.86f * (SCREEN_HEIGHT - BASE_SIZE), "Controls: Enter - Restart\nLeft/Right - Move\nSpace - Shoot", window, font_texture, 0.5f);

				draw_text(0.25f * BASE_SIZE, 1.03f * (SCREEN_HEIGHT - BASE_SIZE), "Space Invaders Recreation | Developed by Jan Enri Arquero", window, font_texture, 0.6f);

				if (1 == game_over)
				{
					draw_text(0.36f * (SCREEN_WIDTH - 5 * BASE_SIZE), 0.4f * (SCREEN_HEIGHT - BASE_SIZE), "Game over!", window, font_texture, 2.0f);
				}
				else if (1 == next_level)
				{
					draw_text(0.36f * (SCREEN_WIDTH - 5 * BASE_SIZE), 0.4f * (SCREEN_HEIGHT - BASE_SIZE), "Next level!", window, font_texture, 2.0f);
				}
				window.display();
			}
		}
	}
}