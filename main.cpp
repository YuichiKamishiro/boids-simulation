#include <SFML/Graphics.hpp>

#include <array>
#include <cmath>

using namespace sf; 

const int BOIDS_COUNT = 200;

const int MAX_RANGE = 100;
const int SHAPE_RADIUS = 3;
const int MIN_RANGE = SHAPE_RADIUS + 1;

const float TURNFACTOR = 0.5;
const float AVOID_FACTOR = 1;
const float MATCHING_FACTOR = 0.05;
const float CENTERING_FACTOR = 0.005;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;

const int MIN_SPEED = 3;
const int MAX_SPEED = 8;


struct Boid {
    float x = 0, y = 0;
    float vx = 0, vy = 0; 

    CircleShape shape;

    Boid(int x, int y, int vx, int vy): 
        x(x), y(y), vx(vx), vy(vy),
        shape(SHAPE_RADIUS)
    {
        shape.setFillColor(Color::Red);
        shape.setPosition(x, y);
    }
    Boid() = default;
};

void spawnBoids(std::array<Boid, BOIDS_COUNT> &boids) {
    for(int i = 0;i < BOIDS_COUNT; i++) {
        int x = rand() % SCREEN_WIDTH + 0;
        int y = rand() % SCREEN_HEIGHT + 0;
        int vx = rand() % MAX_SPEED + MIN_SPEED;
        int vy = rand() % MAX_SPEED +  MIN_SPEED;
        boids[i] = Boid(x, y, vx, vy);
    }
}

void checkBorders(Boid &boid) {
    if (boid.x < 0)
        boid.vx = boid.vx + TURNFACTOR;
    if (boid.x > SCREEN_WIDTH)
        boid.vx = boid.vx - TURNFACTOR;
    if (boid.y > SCREEN_HEIGHT)
        boid.vy = boid.vy - TURNFACTOR;
    if (boid.y < 0)
        boid.vy = boid.vy + TURNFACTOR;
}


int main() {
    srand(time(NULL));

    std::array<Boid, BOIDS_COUNT> boids;
    spawnBoids(boids);

    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Boids C++");
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
                    if (std::abs(distance_x) < MAX_RANGE && std::abs(distance_y) < MAX_RANGE) {
                        if (std::abs(distance_x) <= MIN_RANGE && std::abs(distance_y) <= MIN_RANGE) {                            
                            close_dx += boid.x - otherBoid.x;
                            close_dy += boid.y - otherBoid.y;
                        } else {
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

            float speed = sqrt(boid.vx*boid.vx + boid.vy*boid.vy);
            if (speed < 1) {
                boid.vx = (boid.vx/speed)*1;
                boid.vy = (boid.vy/speed)*1;
            }

            if (speed > 6) {
                boid.vx = (boid.vx/speed)*6;
                boid.vy = (boid.vy/speed)*6;
            }

            checkBorders(boid);

            boid.x = boid.x + boid.vx;
            boid.y = boid.y + boid.vy;

            boid.shape.setPosition(boid.x, boid.y);
        }


        window.clear();

        for(auto &boid : boids) {
            window.draw(boid.shape);
        }

        window.setVerticalSyncEnabled(true);
        window.display();
    }
}