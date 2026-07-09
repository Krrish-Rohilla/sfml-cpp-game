#include "PhysicsSystem.hpp"
#include "VectorMath.hpp"
#include <algorithm>    // For std::clamp

void PhysicsSystem::resolveCollisions(std::vector<Ball>& balls, const std::vector<Line>& lines, float e = 1.f, float groundFriction = 0.f) noexcept {
    // Loop through every ball in the game
    for (Ball& ball : balls) {
        // Check that ball against every single line segment in the map
        for (const Line& line : lines) {
            
            // NEW: If the line is marked as collision-free, completely skip it!
            if (line.isCollisionFree()) {
                continue;
            }
            checkBallLineCollision(ball, line, e, groundFriction);

        }
    }
}

void PhysicsSystem::checkBallLineCollision(Ball& ball, const Line& line, float e, float groundFriction) noexcept {
    // 1. Grab foundational positions and directions
    sf::Vector2f pA = line.getA();
    sf::Vector2f pB = line.getB();
    sf::Vector2f ballPos = ball.getPosition();

    // To adapt to SFML's top-left origin drawing for sf::CircleShape,
    // we calculate using the absolute center point of the ball
    float ballRadius = ball.getRadius();
    sf::Vector2f ballCenter = ballPos + sf::Vector2f(ballRadius, ballRadius);

    // 2. Vector Projection Math (Finding the Closest Point)
    sf::Vector2f lineVec = pB - pA;
    sf::Vector2f ballToA = ballCenter - pA;

    // Project ballToA onto lineVec using the Dot Product
    float numerator = Math::dot(ballToA, lineVec);
    float denominator = Math::lengthSq(lineVec); // Squared length is much faster!

    if (denominator == 0.f) return; // Guard against an invalid line with zero length

    // 't' represents the percentage slider position along the segment (0.0 to 1.0)
    float t = numerator / denominator;
    t = std::clamp(t, 0.f, 1.f); // Keep the point locked onto the segment bounds

    // Calculate the exact closest coordinate on the track to the ball center
    sf::Vector2f closestPoint = pA + t * lineVec;

    // 3. Distance Check (Are they overlapping?)
    sf::Vector2f collisionVec = ballCenter - closestPoint;
    float distance = Math::length(collisionVec);
    
    // Total boundary distance = Ball Radius + Half of the track thickness
    float targetDistance = ballRadius + (line.getThickness() / 2.f);

    if (distance < targetDistance) {
        // ─────────────────────────────────────────────────────────
        // A COLLISION HAS OCCURRED!
        // ─────────────────────────────────────────────────────────
        
        // Find the safe direction to push the ball away
        sf::Vector2f collisionNormal;
        if (distance == 0.f) {
            // Edge case: If ball center is exactly perfectly centered on the line vector,
            // fall back onto the precalculated surface normal of the line segment
            collisionNormal = line.getNormal();
        } else {
            collisionNormal = collisionVec / distance; // Clean normalized direction
        }

        // A. POSITION RESOLUTION (Keeps ball from embedding)
        float penetrationDepth = targetDistance - distance;
        sf::Vector2f newCenter = ballCenter + collisionNormal * penetrationDepth;
        ball.setPosition(newCenter - sf::Vector2f(ballRadius, ballRadius));


        // B. VELOCITY RESOLUTION (The Bounce)
        sf::Vector2f vel = ball.getVelocity();
        float velAlongNormal = Math::dot(vel, collisionNormal);

        // Only apply impulse if moving TOWARDS the plane surface
        // This stops back-to-back segment checks from double-boosting energy!
        // Only reflect if the ball is moving into the surface
        if (velAlongNormal < 0.f) {
            // Subtract the frame's gravity component along the normal to replicate your state split!
            // This prevents the gravity energy leak entirely.
            float bounceVel = -velAlongNormal * e;
            
            // Reconstruct velocity along the normal plane
            vel -= velAlongNormal * collisionNormal; // Remove old incoming normal velocity
            vel += bounceVel * collisionNormal;     // Apply perfect dampened bounce velocity

            // C. SEPARATE GROUND FRICTION (Applied to the tangent sliding surface)
            sf::Vector2f tangent = {-collisionNormal.y, collisionNormal.x};
            float velAlongTangent = Math::dot(vel, tangent);
            vel -= velAlongTangent * groundFriction * tangent;

            ball.setVelocity(vel);

        }
    }
}