#include <SFML/Graphics.hpp>

#include <array>
#include <cmath>

using namespace sf; 

const int kBoidsCount = 200;

const int kMaxDetectionRange = 200;
const int kShapeRadius = 3;
const int kMinDetectionRange = kShapeRadius + 1;

const float kTurnFactor = 0.5;
const float kAvoidFactor = 1;
const float kMatchingFactor = 0.05;
const float kCentringFactor = 0.005;

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 700;

const int kMinSpeed = 3;
const int kMaxSpeed = 8;


struct Boid {
    float x = 0, y = 0;
    float vx = 0, vy = 0; 

    CircleShape shape;

    Boid(int x, int y, int vx, int vy): 
        x(x), y(y), vx(vx), vy(vy),
        shape(kShapeRadius)
    {
        shape.setFillColor(Color::White);
        shape.setPosition(x, y);
    }
    Boid() = default;
};

void spawnBoids(std::array<Boid, kBoidsCount> &boids) {
    for(auto &boid : boids) {
        boid = Boid {
            rand() % SCREEN_WIDTH + 0, 
            rand() % SCREEN_HEIGHT + 0,
            rand() % kMaxSpeed + kMinSpeed,
            rand() % kMaxSpeed +  kMinSpeed,
        };
    }
}

void checkBorders(Boid& boid) {
    if (boid.x < 0) {
        boid.vx += kTurnFactor;
    } else if (boid.x > SCREEN_WIDTH) {
        boid.vx -= kTurnFactor;
    }

    if (boid.y < 0) {
        boid.vy += kTurnFactor;
    } else if (boid.y > SCREEN_HEIGHT) {
        boid.vy -= kTurnFactor;
    }
}


int main() {
    srand(time(NULL));

    std::array<Boid, kBoidsCount> boids;
    spawnBoids(boids);

    RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Boids C++");
    Event event;

    while(window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }

        for (int i = 0; i < kBoidsCount; i++) {
            auto &boid = boids[i];
            
            int avg_x = 0; int avg_y = 0;
            int avg_vx = 0; int avg_vy = 0;
            int flock_boids = 0;
            int close_dx = 0;
            int close_dy = 0;

            for (int j = 0; j < kBoidsCount; j++) {
                if (i != j) {
                    auto &otherBoid = boids[j];
                    int distance_x = boid.x - otherBoid.x;
                    int distance_y = boid.y - otherBoid.y;

                    int distance = sqrt(pow(distance_x, 2) + pow(distance_y, 2));

                    if (distance < kMaxDetectionRange) {
                        if (distance <= kMinDetectionRange) {                            
                            close_dx += boid.x - otherBoid.x;
                            close_dy += boid.y - otherBoid.y;
                        } else {
                            avg_x += otherBoid.x;
                            avg_y += otherBoid.y;
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
                   (avg_x - boid.x) * kCentringFactor + 
                   (avg_vx - boid.vx)* kMatchingFactor);

                boid.vy = (boid.vy + 
                   (avg_y - boid.y)*kCentringFactor + 
                   (avg_vy - boid.vy)*kMatchingFactor);

            }
            boid.vx = boid.vx + (close_dx*kAvoidFactor);
            boid.vy = boid.vy + (close_dy*kAvoidFactor);

            float speed = sqrt(pow(boid.vx, 2) + pow(boid.vy, 2));
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