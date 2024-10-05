#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Window.hpp>

#include <array>
#include <chrono>
#include <cstdlib>
#include <thread>
#include <vector>
#include <iostream>

using namespace sf; 

const int BOIDS_COUNT = 30;
// in pixels
const int MAX_RANGE = 40;

const float TURNFACTOR = 0;
const float COHESION_FACTOR = 0.0005;

struct Boid {
    int x = 0, y = 0; // position
    int vx = 0, vy = 0; // velocity 

    CircleShape shape;

    Boid(int x, int y, int vx, int vy): 
        x(x), y(y), vx(vx), vy(vy),
        shape(10)
    {
        shape.setFillColor(Color::Red);
        shape.setPosition(x, y);
    }
    Boid() = default;
};

void spawnBoids(std::array<Boid, BOIDS_COUNT> &boids) {
    for(int i = 0;i < BOIDS_COUNT; i++) {
        boids[i] = Boid(rand() % 450 + 50, rand() % 450 + 50, rand() % 6 + 3, rand() % 6 + 3);
    }
}

void checkBorders(Boid &boid) {
    if (boid.x < 0)
        boid.vx = boid.vx + TURNFACTOR;
    if (boid.x > 500)
        boid.vx = boid.vx - TURNFACTOR;
    if (boid.y > 500)
        boid.vy = boid.vy - TURNFACTOR;
    if (boid.y < 0)
        boid.vy = boid.vy + TURNFACTOR;
}


int main() {

    std::array<Boid, BOIDS_COUNT> boids;
    spawnBoids(boids);

    RenderWindow window(VideoMode(500, 500), "Boids C++");
    Event event;

    while(window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        for (int i = 0; i < BOIDS_COUNT; i++) {
            auto &boid = boids[i];
            
            int avg_x = 0; int avg_y = 0;
            int avg_vx = 0; int avg_vy = 0;
            int flock_boids = 0;

            for (int j = 0; j < BOIDS_COUNT; j++) {
                if (i != j) {
                    auto &otherBoid = boids[j];

                    int distance_x = boid.x - otherBoid.x;
                    int distance_y = boid.y - otherBoid.y;
                    if (distance_x < MAX_RANGE && distance_y < MAX_RANGE) {
                        // distance
                        if (distance_x != 0 && distance_y != 0) {
                            boid.vx -= (otherBoid.x - boid.x) / distance_x;
                            boid.vy -= (otherBoid.y - boid.y) / distance_y;
                        }
                        // pos
                        avg_x += otherBoid.x;
                        avg_y += otherBoid.y;
                        // vel
                        avg_vx += otherBoid.vx;
                        avg_vy += otherBoid.vy;

                        flock_boids += 1;

                    }
                }
            }
            if (flock_boids > 0) {
                boid.vx += (avg_x / flock_boids - boid.x);
                boid.vy += (avg_y / flock_boids - boid.y);
                boid.vx += (avg_vx / flock_boids - boid.vx);
                boid.vy += (avg_vy / flock_boids - boid.vy);
            }

            boid.x += boid.vx;
            boid.y += boid.vy;

            // limit vel
            // here <-----
            if (boid.vx > 6) {
                boid.vx = 6;
            }
            if (boid.vy > 6) {
                boid.vy = 6;
            }
            boid.shape.setPosition(boid.x, boid.y);
            // checking if boid within screen
            checkBorders(boid);
        }



        window.clear();

        for(auto &boid : boids) {
            window.draw(boid.shape);
            std::cout << boid.x << " " << boid.y << "\n";
        }
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        window.setVerticalSyncEnabled(true);
        
        window.display();
    }
}