#include <math.h>

#include "animation.h"
#include "raylib.h"

#define now() ((TimePoint)(GetTime() * ANIMATION_DURATION_PER_SEC))

Animation animationNew(AnimationKind kind, EaseFunction ease, Duration dur) {
    return (Animation) {
        .kind = kind,
        .ease = ease,
        .duration = dur,
        .epoch = (TimePoint)-1,
    };
}

void animationStart(Animation *this) {
    this->epoch = now();   
}

void animaitonStop(Animation *this) {
    this->epoch = (TimePoint)-1;
}

void animationScheduleFromNow(Animation *this, Duration duration) {
    this->epoch = now() + duration;
}

static double fclamp(double a, double mi, double ma) {
    return fmax(fmin(a, ma), mi);
}

double animationQuery(Animation *this) {
    Duration elapsed = now() - this->epoch;
    double normalised = ((double)elapsed / (double)ANIMATION_DURATION_PER_SEC)
                      / ((double)this->duration / (double)ANIMATION_DURATION_PER_SEC);

    double t;
    switch (this->kind) {
    case ANIMATION_ONCE:
        t = fclamp(normalised, 0.0f, 1.0f);
        break;
    case ANIMATION_LOOP:
        t = fmax(normalised, 0.0f);
        break;
    }

    return this->ease(t);
}
