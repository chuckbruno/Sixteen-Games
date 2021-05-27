// Asteroids.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <SFML/Graphics.hpp>
#include <time.h>
#include <list>

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

class Animation {
public:
	float Frame, speed;
	sf::Sprite sprite;
	std::vector<sf::IntRect> frames;

	Animation(){}
	Animation(sf::Texture &t, int x, int y, int w, int h, int count, float Speed) {
		Frame = 0;
		speed = Speed;

		for (int i = 0; i < count; i++) {
			frames.push_back(sf::IntRect(x + i * w, y, w, h));
		}

		sprite.setTexture(t);
		sprite.setOrigin(w / 2, h / 2);
		sprite.setTextureRect(frames[0]);
	}

	void update() {
		Frame += speed;
		int n = frames.size();
		if (Frame >= n) Frame -= n;
		if (n > 0) sprite.setTextureRect(frames[int(Frame)]);
	}

	bool isEnd()
	{
		return Frame + speed >= frames.size();
	}
};


class Entity
{
public:
	float x, y, dx, dy, R, angle;
	bool life;
	std::string name;
	Animation anim;

	Entity()
	{
		life = 1;
	}

	void settints(Animation &a, int X, int Y, float Angle = 0, int radius = 1)
	{
		anim = a;
		x = X;
		y = Y;
		angle = Angle;
		R = radius;
	}

	virtual void update(){}

	void draw(sf::RenderWindow& app)
	{
		anim.sprite.setPosition(x, y);
		anim.sprite.setRotation(angle + 90);
		app.draw(anim.sprite);

		sf::CircleShape circle(R);
		circle.setFillColor(sf::Color(255, 0, 0, 170));
		circle.setPosition(x, y);
		circle.setOrigin(R, R);
	}

	virtual ~Entity() {};
};


class asteroid : public Entity
{
public:
	asteroid()
	{
		dx = rand() % 8 - 4;
		dy = rand() % 8 - 4;
		name = "asteroid";
	}

	void update() {
		x += dx;
		y += dy;

		if (x > W) x = 0; if (x < 0)x = W;
		if (y > H) y = 0; if (y < 0)y = H;

	}
};


class bullet : public Entity
{
public:
	bullet()
	{
		name = "bullet";
	}

	void update()
	{
		dx = std::cos(angle*DEGTORAD) * 6;
		dy = std::sin(angle*DEGTORAD) * 6;

		x += dx;
		y += dy;

		if (x > W || x <0 || y>H || y < 0) life = 0;
	}
};


class player : public Entity
{
public:
	bool thrust;
	player()
	{
		name = "player";
	}

	void update()
	{
		if (thrust) {
			dx += cos(angle*DEGTORAD) * 0.2;
			dy += sin(angle*DEGTORAD) * 0.2;
		}
		else
		{
			dx *= 0.99;
			dy *= 0.99;

		}

		int maxSpeed = 15;
		float speed = std::sqrt(dx * dx + dy * dy);
		if (speed > maxSpeed)
		{
			dx *= maxSpeed / speed;
			dy *= maxSpeed / speed;
		}

		x += dx;
		y += dy;

		if (x > W) x = 0; if (x < 0)x = W;
		if (y > H) y = 0; if (y < 0)y = H;
	}
};

bool isCollide(Entity* a, Entity* b)
{
	return (b->x - a->x)*(b->x - a->x) +
		(b->y - a->y)*(b->y - a->y) <
		(a->R + b->R)*(a->R + b->R);
}


int main()
{
	srand(time(0));
	sf::RenderWindow app(sf::VideoMode(W, H), "Asteroids");
	app.setFramerateLimit(60);

	sf::Texture t1, t2, t3, t4, t5, t6, t7;
	t1.loadFromFile("images/spaceship.png");
	t2.loadFromFile("images/background.jpg");
	t3.loadFromFile("images/explosions/type_C.png");
	t4.loadFromFile("images/rock.png");
	t5.loadFromFile("images/fire_blue.png");
	t6.loadFromFile("images/rock_small.png");
	t7.loadFromFile("images/explosions/type_B.png");

	t1.setSmooth(true);
	t2.setSmooth(true);

	sf::Sprite background(t2);

	Animation sExplosion(t3, 0, 0, 256, 256, 48, 0.5);
	Animation sRock(t4, 0, 0, 64, 64, 16, 0.2);
	Animation sRock_small(t6, 0, 0, 64, 64, 16, 0.2);
	Animation sBullet(t5, 0, 0, 32, 64, 16, 0.8);
	Animation sPlayer(t1, 40, 0, 40, 40, 1, 0);
	Animation sPlayer_go(t1, 40, 40, 40, 40, 1, 0);
	Animation sExplosion_ship(t7, 0, 0, 192, 192, 64, 0.5);

	std::list<Entity*> entities;

	for (int i = 0; i < 15; i++) {
		asteroid* aptr = new asteroid();
		aptr->settints(sRock, rand() % W, rand() % H, rand() % 360, 25);
		entities.push_back(aptr);
	}

	player *p = new player();
	p->settints(sPlayer, 200, 200, 0, 20);

	entities.push_back(p);

	while (app.isOpen())
	{
		sf::Event event;
		while (app.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				app.close();
			if(event.type == sf::Event::KeyPressed)
				if (event.key.code == sf::Keyboard::Space) {
					bullet* b = new bullet();
					b->settints(sBullet, p->x, p->y, p->angle, 10);
					entities.push_back(b);
				}
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) p->angle += 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))p->angle -= 3;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))p->thrust = true;
		else p->thrust = false;

		for(auto a:entities)
			for (auto b : entities) {
				if (a->name == "asteroid" && b->name == "bullet")
					if (isCollide(a, b))
					{
						a->life = false;
						b->life = false;

						Entity *e = new Entity();
						e->settints(sExplosion, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						for (int i = 0; i < 2; i++)
						{
							if (a->R == 15) continue;
							Entity *e = new asteroid();
							e->settints(sRock_small, a->x, a->y, rand() % 360, 15);
							entities.push_back(e);
						}
					}

				if (a->name == "player" && b->name == "asteroid")
					if (isCollide(a, b))
					{
						b->life = false;
						Entity *e = new Entity();
						e->settints(sExplosion_ship, a->x, a->y);
						e->name = "explosion";
						entities.push_back(e);

						p->settints(sPlayer, W / 2, H / 2, 0, 20);
						p->dx = 0;
						p->dx = 0;
					}
			}

		if (p->thrust) p->anim = sPlayer_go;
		else p->anim = sPlayer;

		for (auto e : entities)
			if (e->name == "explosion")
				if (e->anim.isEnd()) e->life = 0;
		if (rand() % 150 == 0)
		{
			asteroid* a = new asteroid();
			a->settints(sRock, 0, rand() % W, rand() % 360, 25);
			entities.push_back(a);
		}

		for (auto i = entities.begin(); i != entities.end();)
		{
			Entity* e = *i;
			e->update();
			e->anim.update();

			if (e->life == false)
			{
				i = entities.erase(i);
				delete e;
			}
			else
				i++;
		}

		app.draw(background);
		for (auto i : entities) i->draw(app);
		app.display();

	}

    std::cout << "Hello World!\n";
	return 0;
}
