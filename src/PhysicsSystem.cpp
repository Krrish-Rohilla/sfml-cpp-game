#include "PhysicsSystem.hpp"
#include "VectorMath.hpp"
#include <algorithm> 
#include <cmath>
#include <vector>

void PhysicsSystem::resolveCollisions(std::vector<Ball> &balls, const std::vector<Line> &lines, float e, float groundFriction) noexcept
{
    const int substeps = 8;
    const float dt = 1.f / static_cast<float>(substeps);
    const float gravityPerSubstep = 0.4f * dt; 

    for (int step = 0; step < substeps; ++step)
    {
        // ─────────────────────────────────────────────────────────────────────────────
        // SUBSTEP PASS 1: INTEGRATION
        // ─────────────────────────────────────────────────────────────────────────────
        for (Ball &ball : balls)
        {
            sf::Vector2f vel = ball.getVelocity();
            sf::Vector2f pos = ball.getPosition();
            float omega = ball.getAngularVelocity();

            vel.y += gravityPerSubstep; 
            pos += vel * dt;            
            
            float angle = ball.getRotationAngle() + (omega * dt);

            ball.setVelocity(vel);
            ball.setPosition(pos);
            ball.setRotationAngle(angle);
        }

        // ─────────────────────────────────────────────────────────────────────────────
        // SUBSTEP PASS 2: NARROWPHASE WITH GHOST SEAM FILTERING
        // ─────────────────────────────────────────────────────────────────────────────
        struct Contact
        {
            Ball *ball;
            const Line *line;
            sf::Vector2f normal;
            sf::Vector2f tangent;
            float penetration;
            float t;
        };
        std::vector<Contact> contactPool;

        for (Ball &ball : balls)
        {
            sf::Vector2f ballCenter = ball.getPosition(); 
            float radius = ball.getRadius();

            struct RawOverlap
            {
                const Line *line;
                sf::Vector2f normal;
                float penetration;
                float t;
            };
            std::vector<RawOverlap> rawOverlaps;
            bool hasFaceContact = false;

            for (const Line &line : lines)
            {
                if (line.isCollisionFree())
                    continue;

                sf::Vector2f pA = line.getA();
                sf::Vector2f pB = line.getB();
                sf::Vector2f lineVec = pB - pA;
                float lenSq = Math::lengthSq(lineVec);
                if (lenSq == 0.f)
                    continue;

                float t = Math::dot(ballCenter - pA, lineVec) / lenSq;
                float clampedT = std::clamp(t, 0.f, 1.f); 
                sf::Vector2f closestPoint = pA + clampedT * lineVec;

                sf::Vector2f collisionVec = ballCenter - closestPoint;
                float distance = Math::length(collisionVec);
                float targetDistance = radius + (line.getThickness() / 2.f);

                if (distance < targetDistance)
                {
                    sf::Vector2f normal = (distance == 0.f) ? line.getNormal() : (collisionVec / distance);
                    float penetration = targetDistance - distance;

                    rawOverlaps.push_back({&line, normal, penetration, t});

                    if (t > 0.001f && t < 0.999f)
                    {
                        hasFaceContact = true;
                    }
                }
            }

            for (const auto &ro : rawOverlaps)
            {
                bool isVertex = (ro.t <= 0.001f || ro.t >= 0.999f);

                if (isVertex && hasFaceContact)
                {
                    continue; 
                }

                Contact c;
                c.ball = &ball;
                c.line = ro.line;
                c.normal = ro.normal;
                c.tangent = sf::Vector2f(-ro.normal.y, ro.normal.x); 
                c.penetration = ro.penetration;
                c.t = ro.t;
                contactPool.push_back(c);
            }
        }

        // ─────────────────────────────────────────────────────────────────────────────
        // SUBSTEP PASS 3: CONSTRAINT RESOLUTION
        // ─────────────────────────────────────────────────────────────────────────────
        for (int iter = 0; iter < 3; ++iter)
        {
            for (Contact &c : contactPool)
            {
                sf::Vector2f vel = c.ball->getVelocity();
                float mass = c.ball->getMass();
                float radius = c.ball->getRadius();
                float omega = c.ball->getAngularVelocity();

                // 1. Position Resolution (Baumgarte)
                sf::Vector2f pos = c.ball->getPosition();
                pos += c.normal * (c.penetration * 0.4f);
                c.ball->setPosition(pos);

                // 2. Normal Linear Bounce Solver
                float velAlongNormal = Math::dot(vel, c.normal);
                float normalImpulseApplied = 0.f;
                
                if (velAlongNormal < 0.f)
                {
                    float targetVel = -velAlongNormal * e;
                    if (std::abs(velAlongNormal) < 0.2f)
                    {
                        targetVel = 0.f; 
                    }

                    normalImpulseApplied = (targetVel - velAlongNormal) * mass;
                    vel += (normalImpulseApplied / mass) * c.normal;
                }

                // 3. Decoupled Continuous Surface Friction Calculation
                float gravityPressingForce = mass * gravityPerSubstep * c.normal.y;
                float continuousNormalImpulse = (gravityPressingForce < 0.f) ? -gravityPressingForce : 0.f;
                float totalNormalPressure = normalImpulseApplied + continuousNormalImpulse;

                float velAlongTangent = Math::dot(vel, c.tangent);
                
                // FIXED: Sign changed to minus (-) to properly match SFML's clockwise coordinate system.
                float vRelTangent = velAlongTangent - (omega * radius);

                float effMassTangent = mass / 3.f;
                float frictionImpulse = -vRelTangent * effMassTangent;

                float maxFriction = groundFriction * totalNormalPressure;
                
                // Minimum static grip threshold to allow fast spinning stationary objects to launch
                if (maxFriction < 0.05f) 
                {
                    maxFriction = groundFriction * mass * 0.2f;
                }

                frictionImpulse = std::clamp(frictionImpulse, -maxFriction, maxFriction);

                // Synchronize impulses into kinematic vectors
                vel += (frictionImpulse / mass) * c.tangent;
                
                // FIXED: Sign changed to minus equals (-=) to properly couple the linear reaction force with torque.
                omega -= (2.f * frictionImpulse) / (mass * radius);

                // 4. Stabilized Resting Sleep Clamp
                float linearSpeedSq = (vel.x * vel.x) + (vel.y * vel.y);
                float rotationalSpeedSq = (omega * radius) * (omega * radius);

                if (linearSpeedSq < 0.04f && rotationalSpeedSq < 0.04f) 
                {
                    if (c.normal.y < -0.5f) 
                    {
                        vel = sf::Vector2f(0.f, 0.f);
                        omega = 0.f;
                    }
                }

                c.ball->setAngularVelocity(omega);
                c.ball->setVelocity(vel);
            }
        }
    }
}