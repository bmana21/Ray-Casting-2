#include <SDL2/SDL.h>
#include <deque>
#include <ctime>

#define GRAVITATION false
#define NUMBER_OF_RAYS 18
#define SMOOTH_COLORS true
#define TOTAL_NUMBER_OF_BALLS 2500

using namespace std;

const int DELAY = 15;
const int CIRCLE_QUALITY = 50;

int window_width = 600, window_height = 600;
int circle_x, circle_y;
const int circle_r = 250;
SDL_Window *window;

struct Ball {
    double px = 0, py = 0;
    double x, y;
    double alpha;
    double dir_x, dir_y, speed = 8;
    int R = 0, G = 0, B = 0;

    Ball(int X, int Y) {
        x = X;
        y = Y;
        px = x;
        py = y;
        dir_x = 0;
        dir_y = 0;
        alpha = 0;
    }

    void setColor(int r, int g, int b) {
        R = r;
        G = g;
        B = b;
        R = max(0, R);
        G = max(0, G);
        B = max(0, B);
        R = min(255, R);
        G = min(255, G);
        B = min(255, B);
    }

    void get_dirs() {
        while (alpha < 0) {
            alpha += 360;
        }
        while (alpha >= 360) {
            alpha -= 360;
        }
        dir_x = speed * cos(alpha * M_PI / 180.0);
        dir_y = speed * sin(alpha * M_PI / 180.0);
    }

    void grav() {
        double al = alpha + 90;
        while (al < 0) {
            al += 360;
        }
        while (al >= 360) {
            al -= 360;
        }
        double df = 3 * (180.0 - al) / 180.0;

        alpha += df;
    }
};

void get_new_color(Ball &ball1, Ball &ball2, bool sm) {
    if (!sm) {
        ball2.setColor(rand() % 255, rand() % 255, rand() % 255);
        return;
    }
    ball2.setColor(ball1.R + rand() % 7 - 3, ball1.G + rand() % 7 - 3, ball1.B + rand() % 7 - 3);

}

void draw_circle(SDL_Renderer *renderer, int x, int y, int radius, int quality) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int prev_x = -100, prev_y = -100;
    int start_x = 0, start_y = 0;
    for (int i = 0; i <= quality; i++) {
        double angle = 2 * M_PI * i / quality;
        int cx = x + radius * std::cos(angle);
        int cy = y + radius * std::sin(angle);
        if (!(prev_x == -100 && prev_y == -100)) {
            SDL_RenderDrawLine(renderer, prev_x, prev_y, cx, cy);
        } else {
            start_x = cx;
            start_y = cy;
        }
        prev_x = cx;
        prev_y = cy;
    }
    SDL_RenderDrawLine(renderer, prev_x, prev_y, start_x, start_y);
    SDL_RenderPresent(renderer);
}


void render(SDL_Renderer *renderer, Ball &ball) {
    SDL_SetRenderDrawColor(renderer, ball.R, ball.G, ball.B, 255);
    SDL_RenderDrawLine(renderer, ball.x, ball.y, ball.px, ball.py);
    ball.px = ball.x;
    ball.py = ball.y;
}

double get_dist(double x1, double y1, double x2, double y2) {
    double distance = sqrt((double) pow(x2 - x1, 2) + (double) pow(y2 - y1, 2));
    return distance;
}

void logic(Ball &ball) {
    ball.get_dirs();
    double dist = get_dist(ball.x + ball.dir_x, ball.y + ball.dir_y, circle_x, circle_y);
    if (dist >= circle_r) {
        dist = get_dist(ball.x, ball.y, circle_x, circle_y);
        double nx = (double) (ball.x - circle_x) / dist;
        double ny = (double) (ball.y - circle_y) / dist;
        double dot = -cos(ball.alpha * M_PI / 180.0) * nx - sin(ball.alpha * M_PI / 180.0) * ny;
        double rx = cos(ball.alpha * M_PI / 180.0) + 2 * dot * nx;
        double ry = sin(ball.alpha * M_PI / 180.0) + 2 * dot * ny;
        ball.alpha = (double) (atan2(ry, rx) * 180.0 / M_PI);
        ball.get_dirs();
    }
    ball.x += ball.dir_x;
    ball.y += ball.dir_y;
    if (GRAVITATION)
        ball.grav();
}

void loop() {
    bool quit = false;
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    deque<Ball> balls;
    bool down = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    balls.clear();
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderClear(renderer);
                    SDL_RenderPresent(renderer);
                } else
                    down = true;
            }
            if (event.type == SDL_MOUSEBUTTONUP)
                down = false;
            if (event.type == SDL_MOUSEMOTION) {
                if (down) {
                    for (int angle = 0; angle < 360; angle += 360 / NUMBER_OF_RAYS) {
                        Ball ball(event.motion.x, event.motion.y);
                        ball.alpha = angle;
                        if (balls.empty())
                            get_new_color(ball, ball, false);
                        else get_new_color(balls[balls.size() - 1], ball, SMOOTH_COLORS);
                        balls.push_back(ball);
                    }
                    while (balls.size() > TOTAL_NUMBER_OF_BALLS)
                        balls.pop_front();
                }
            }
        }
        draw_circle(renderer, circle_x, circle_y, circle_r, CIRCLE_QUALITY);
        for (auto &ball: balls) {
            render(renderer, ball);
            logic(ball);
        }
        SDL_Delay(DELAY);
    }
    SDL_DestroyRenderer(renderer);
}

void init_circle() {
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    window_width = current.w;
    window_height = current.h;
    circle_x = window_width / 2;
    circle_y = window_height / 2;
}

int main() {
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Ray Casting #2", 0, 0, window_width, window_height, SDL_WINDOW_MAXIMIZED);
    init_circle();
    loop();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
