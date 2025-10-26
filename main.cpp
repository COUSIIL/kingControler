// main.cpp
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include <cmath>
#include <iostream>

// Helper
static float clamp(float v, float a, float b){ return std::max(a, std::min(b, v)); }
static float length(const sf::Vector2f &v){ return std::sqrt(v.x*v.x + v.y*v.y); }
static sf::Vector2f normalize(const sf::Vector2f &v){
    float L = length(v);
    return (L > 1e-6f) ? sf::Vector2f(v.x/L, v.y/L) : sf::Vector2f(0.f,0.f);
}

// Player (circle) struct
struct Player {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float radius;
    float mass;
    float restitution; // 0..1
    sf::CircleShape shape;

    Player(float x=100, float y=100, float r=16.f)
    : pos(x,y), vel(0,0), radius(r), mass(1.f), restitution(0.8f)
    {
        shape.setRadius(radius);
        shape.setOrigin(radius, radius);
        shape.setFillColor(sf::Color(200,120,40));
        shape.setPosition(pos);
    }

    void updateShape(){ shape.setPosition(pos); }
};

// Resolve collision between circle (player) and axis-aligned rectangle (obstacle)
// Returns true if collision resolved
bool resolveCircleRect(Player &p, const sf::FloatRect &rect) {
    // find closest point on rect to circle center
    float closestX = clamp(p.pos.x, rect.left, rect.left + rect.width);
    float closestY = clamp(p.pos.y, rect.top, rect.top + rect.height);
    sf::Vector2f closest(closestX, closestY);

    sf::Vector2f diff = p.pos - closest;
    float dist = length(diff);
    float penetration = p.radius - dist;
    if (penetration > 0.f) {
        // compute normal (if dist==0, pick outward normal by comparing centers)
        sf::Vector2f normal = (dist > 1e-6f) ? diff / dist : sf::Vector2f(0, -1);
        // push circle out of obstacle
        p.pos += normal * penetration;

        // reflect velocity along normal (elastic collision)
        float vn = p.vel.x*normal.x + p.vel.y*normal.y; // velocity component along normal
        if (vn < 0.f) { // only if moving into the surface
            // simple impulse: v' = v - (1+e) * vn * n
            float e = p.restitution;
            p.vel = p.vel - (1.f + e) * vn * normal;
        }
        return true;
    }
    return false;
}

int main(){
    sf::ContextSettings settings;
    settings.antialiasingLevel = 4;
    sf::RenderWindow window(sf::VideoMode(1024, 720), "SFML 2D physics prototype", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // world bounds (used to bounce off window edges)
    sf::FloatRect worldBounds(0,0, window.getSize().x, window.getSize().y);

    // player
    Player player(200, 200, 18.f);

    // obstacles: rectangles
    std::vector<sf::RectangleShape> obstacles;
    auto addRect = [&](float x, float y, float w, float h, sf::Color c){
        sf::RectangleShape r({w,h});
        r.setPosition(x,y);
        r.setFillColor(c);
        obstacles.push_back(r);
    };

    addRect(400, 150, 300, 24, sf::Color(120,120,200));
    addRect(150, 420, 200, 28, sf::Color(100,200,100));
    addRect(700, 360, 24, 240, sf::Color(200,100,120));
    addRect(50, 50, 120, 24, sf::Color(180,180,80)); // example

    // joystick virtual (on-screen) — circle that the user drags/touches
    bool useVirtualJoystick = true;
    sf::Vector2f vjoyCenter(100, window.getSize().y - 100);
    float vjoyRadius = 48.f;
    sf::CircleShape vjoyOuter(vjoyRadius); vjoyOuter.setOrigin(vjoyRadius, vjoyRadius);
    vjoyOuter.setPosition(vjoyCenter);
    vjoyOuter.setFillColor(sf::Color(20,20,20,80));
    sf::CircleShape vjoyInner(18.f); vjoyInner.setOrigin(18.f,18.f);
    vjoyInner.setPosition(vjoyCenter);
    vjoyInner.setFillColor(sf::Color(220,220,220,200));
    bool vjoyActive = false;
    sf::Vector2f vjoyDir(0,0);

    // settings
    float speedMax = 520.f; // clamp speed
    float accel = 1200.f;   // acceleration when input
    float friction = 600.f; // damping
    sf::Clock clock;

    // initial instruction text
    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        // it's OK if font not found; text optional
    }
    sf::Text info;
    info.setFont(font);
    info.setCharacterSize(14);
    info.setFillColor(sf::Color::White);
    info.setPosition(8,8);
    info.setString("Move: mouse / left-click joystick / gamepad left stick\nEsc to quit");

    while (window.isOpen()){
        sf::Time dtTime = clock.restart();
        float dt = dtTime.asSeconds();
        if (dt <= 0.f) dt = 1.f/60.f;

        // ----- INPUT -----
        sf::Event ev;
        sf::Vector2f inputDir(0.f, 0.f);
        bool gotInput = false;
        while (window.pollEvent(ev)){
            if (ev.type == sf::Event::Closed) window.close();
            if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Escape) window.close();

            // mouse joystick (simulate touch)
            if (ev.type == sf::Event::MouseButtonPressed && ev.mouseButton.button == sf::Mouse::Left){
                sf::Vector2f mpos((float)ev.mouseButton.x, (float)ev.mouseButton.y);
                // if click near virtual joystick center, activate it
                if (useVirtualJoystick && length(mpos - vjoyCenter) <= vjoyRadius + 20.f){
                    vjoyActive = true;
                    vjoyInner.setPosition(mpos);
                }
            }
            if (ev.type == sf::Event::MouseButtonReleased && ev.mouseButton.button == sf::Mouse::Left){
                vjoyActive = false;
                vjoyInner.setPosition(vjoyCenter);
                vjoyDir = {0,0};
            }
            if (ev.type == sf::Event::MouseMoved){
                if (vjoyActive){
                    sf::Vector2f mp((float)ev.mouseMove.x, (float)ev.mouseMove.y);
                    sf::Vector2f diff = mp - vjoyCenter;
                    float dist = length(diff);
                    if (dist > vjoyRadius) diff = normalize(diff) * vjoyRadius;
                    vjoyInner.setPosition(vjoyCenter + diff);
                    vjoyDir = (dist > 1e-4f) ? diff / vjoyRadius : sf::Vector2f(0,0);
                }
            }
        }

        // --- mouse direct control mode (if right click held -> move toward cursor)
        if (sf::Mouse::isButtonPressed(sf::Mouse::Right)){
            sf::Vector2i m = sf::Mouse::getPosition(window);
            sf::Vector2f target((float)m.x, (float)m.y);
            sf::Vector2f dir = target - player.pos;
            float d = length(dir);
            if (d > 4.f){
                inputDir = normalize(dir);
                gotInput = true;
            }
        }

        // --- virtual joystick input
        if (!gotInput && useVirtualJoystick && length(vjoyDir) > 0.001f){
            inputDir = vjoyDir;
            gotInput = true;
        }

        // --- real gamepad (sf::Joystick)
        // We'll take joystick id 0 if connected
        if (!gotInput && sf::Joystick::isConnected(0)) {
            float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X); // -100..100
            float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
            // deadzone
            const float dead = 15.f;
            if (std::abs(x) > dead || std::abs(y) > dead){
                // invert Y to get usual up=-1
                inputDir = sf::Vector2f(x/100.f, y/100.f);
                inputDir.y = inputDir.y; // use as-is; tune as needed
                gotInput = true;
            }
        }

        // ----- PHYSICS -----
        // apply input acceleration
        if (gotInput){
            sf::Vector2f desiredAcc = normalize(inputDir) * accel;
            // semi-implicit euler
            player.vel += desiredAcc * dt;
        } else {
            // simple friction (damp)
            if (length(player.vel) > 1e-3f){
                sf::Vector2f fric = -normalize(player.vel) * friction * dt;
                // don't invert sign
                if (length(fric) > length(player.vel)) player.vel = sf::Vector2f(0,0);
                else player.vel += fric;
            }
        }

        // clamp speed
        float spd = length(player.vel);
        if (spd > speedMax){
            player.vel = normalize(player.vel) * speedMax;
        }

        // integrate position
        player.pos += player.vel * dt;

        // window bounds collision (bounce)
        // left
        if (player.pos.x - player.radius < worldBounds.left){
            player.pos.x = worldBounds.left + player.radius;
            if (player.vel.x < 0) player.vel.x = -player.vel.x * player.restitution;
        }
        // right
        if (player.pos.x + player.radius > worldBounds.left + worldBounds.width){
            player.pos.x = worldBounds.left + worldBounds.width - player.radius;
            if (player.vel.x > 0) player.vel.x = -player.vel.x * player.restitution;
        }
        // top
        if (player.pos.y - player.radius < worldBounds.top){
            player.pos.y = worldBounds.top + player.radius;
            if (player.vel.y < 0) player.vel.y = -player.vel.y * player.restitution;
        }
        // bottom
        if (player.pos.y + player.radius > worldBounds.top + worldBounds.height){
            player.pos.y = worldBounds.top + worldBounds.height - player.radius;
            if (player.vel.y > 0) player.vel.y = -player.vel.y * player.restitution;
        }

        // resolve collisions with obstacles
        for (auto &rs : obstacles){
            resolveCircleRect(player, rs.getGlobalBounds());
        }

        player.updateShape();

        // --- RENDER ---
        window.clear(sf::Color(32,34,40));
        // obstacles
        for (auto &r : obstacles) window.draw(r);
        // player
        window.draw(player.shape);
        // draw virtual joystick
        if (useVirtualJoystick){
            window.draw(vjoyOuter);
            window.draw(vjoyInner);
        }
        // info
        if (font.getInfo().family != ""){
            info.setString(
                "Move: mouse (right-click to move) / left-click for virtual joystick\nGamepad: left stick\nPosition: (" + 
                std::to_string((int)player.pos.x) + "," + std::to_string((int)player.pos.y) + ")  Speed: " + std::to_string((int)length(player.vel))
            );
            window.draw(info);
        }

        window.display();
    }

    return 0;
}
