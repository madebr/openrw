#ifndef _RWENGINE_ANIMATOR_HPP_
#define _RWENGINE_ANIMATOR_HPP_
#include <map>
#include <vector>

#include <rw/debug.hpp>
#include <rw/forward.hpp>

struct AnimationBone;
class ModelFrame;

/**
 * @brief calculates animation frame matrices, as well as procedural frame
 * animation.
 *
 * Animations are played using the playAnimation() interface, this will add
 * the animation to the animator. This sets the configuration to use for the
 * animation, such as it's speed and time.
 *
 * The Animator will blend all active animations together.
 */
class Animator {
    /**
     * @brief The AnimationState struct stores information about playing
     * animations
     */
    struct AnimationState {
        AnimationPtr animation;
        /// Timestamp of the last frame
        float time;
        /// Speed multiplier
        float speed;
        /// Automatically restart
        bool repeat;
        std::map<AnimationBone*, ModelFrame*> boneInstances;
    };

    /**
     * @brief model The model being animated.
     */
    ClumpPtr _model;

    /**
     * @brief Currently playing animations
     */
    std::vector<AnimationState> _animations;

public:
    Animator(ClumpPtr _model);

    AnimationPtr getAnimation(unsigned int slot) {
        if (slot < _animations.size()) {
            return _animations[slot].animation;
        }
        return nullptr;
    }

    void playAnimation(unsigned int slot, const AnimationPtr& anim, float speed,
                       bool repeat) {
        if (slot >= _animations.size()) {
            _animations.resize(slot + 1);
        }
        _animations[slot] = {anim, 0.f, speed, repeat, {}};
    }

    void setAnimationSpeed(unsigned int slot, float speed) {
        RW_CHECK(slot < _animations.size(), "Slot out of range");
        if (slot < _animations.size()) {
            _animations[slot].speed = speed;
        }
    }

    /**
     * @brief tick Update animation paramters for server-side data.
     * @param dt
     */
    void tick(float dt);

    /**
     * Returns true if the animation has finished playing.
     */
    bool isCompleted(unsigned int slot) const;
    float getAnimationTime(unsigned int slot) const;
    void setAnimationTime(unsigned int slot, float time);
};

#endif
