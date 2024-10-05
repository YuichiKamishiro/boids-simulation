#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Window/Window.hpp>

#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <cmath>

using namespace sf; 

const int BOIDS_COUNT = 60;
// in pixels
const int MAX_RANGE = 100;

const float TURNFACTOR = 0.5;
const float AVOID_FACTOR = 0.2;
const float MATCHING_FACTOR = 0.05;
const float CENTERING_FACTOR = 0.009; // centering in 1 floak

struct Boid {
    float x = 0, y = 0; // position
    float vx = 0, vy = 0; // velocity 

    CircleShape shape;

    Boid(int x, int y, int vx, int vy): 
        x(x), y(y), vx(vx), vy(vy),
        shape(5)
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
    if (boid.x < 20)
        boid.vx = boid.vx + TURNFACTOR;
    if (boid.x > 480)
        boid.vx = boid.vx - TURNFACTOR;
    if (boid.y > 480)
        boid.vy = boid.vy - TURNFACTOR;
    if (boid.y < 20)
        boid.vy = boid.vy + TURNFACTOR;
}


int main() {
    srand(time(NULL));

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
            int close_dx = 0;
            int close_dy = 0;

            for (int j = 0; j < BOIDS_COUNT; j++) {
                if (i != j) {
                    auto &otherBoid = boids[j];
                    int distance_x = boid.x - otherBoid.x;
                    int distance_y = boid.y - otherBoid.y;

                    if (std::abs(distance_x) < 40 && abs(distance_y) < 40) {
                        int squared_distance = distance_x*distance_x + distance_y*distance_y;

                        if (squared_distance <= 10*10) {                            
                            // distance
                            close_dx += boid.x - otherBoid.x;
                            close_dy += boid.y - otherBoid.y;
                        } else if(squared_distance < 40*40){
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
            }
            if (flock_boids > 0) {
                avg_x /= flock_boids;
                avg_y /= flock_boids;
                avg_vx /= flock_boids;
                avg_vy /= flock_boids;

                boid.vx = (boid.vx + 
                   (avg_x - boid.x) * CENTERING_FACTOR + 
                   (avg_vx - boid.vx)* MATCHING_FACTOR);

                boid.vy = (boid.vy + 
                   (avg_y - boid.y)*CENTERING_FACTOR + 
                   (avg_vy - boid.vy)*MATCHING_FACTOR);

            }
            boid.vx = boid.vx + (close_dx*AVOID_FACTOR);
            boid.vy = boid.vy + (close_dy*AVOID_FACTOR);

            checkBorders(boid);

            float speed = sqrt(boid.vx*boid.vx + boid.vy*boid.vy);
            if (speed < 1) {
                boid.vx = (boid.vx/speed)*1;
                boid.vy = (boid.vy/speed)*1;
            }

            if (speed > 6) {
                boid.vx = (boid.vx/speed)*6;
                boid.vy = (boid.vy/speed)*6;
            }

            boid.x = boid.x + boid.vx;
            boid.y = boid.y + boid.vy;

            boid.shape.setPosition(boid.x, boid.y);
        }


        window.clear();

        for(auto &boid : boids) {
            window.draw(boid.shape);
            std::cout << boid.vx << " " << boid.vy << "\n";
        }
        window.setVerticalSyncEnabled(true);
        //std::this_thread::sleep_for(std::chrono::seconds(1));
        window.display();
    }
}