#ifndef UNPERPLEX_UI_ANIMATION_H
#define UNPERPLEX_UI_ANIMATION_H

#include <stdint.h>

#define ANIMATION_DURATION_PER_SEC (1000 * 1000) 

typedef int64_t Duration;
typedef int64_t TimePoint;
typedef double (*EaseFunction)(double t);

typedef enum {
    ANIMATION_LOOP,
    ANIMATION_ONCE,
} AnimationKind;

typedef struct {
    TimePoint epoch;
    Duration duration;
    EaseFunction ease;
    AnimationKind kind;
} Animation;

Animation animationNew(AnimationKind kind, EaseFunction ease, Duration dur);

void animationStart(Animation *this);
void animaitonStop(Animation *this);
void animationScheduleFromNow(Animation *this, Duration duration);
void animationSchedule(Animation *this, TimePoint when);

double animationQuery(Animation *this);

#endif

#if !defined(ANIM_TIME_CONVERSIONS) && defined(WANT_ANIM_TIME_CONVERSIONS)
#define ANIM_TIME_CONVERSIONS

#define DUR_US(us) (us * (ANIMATION_DURATION_PER_SEC / (1000 * 1000))) 
#define DUR_MS(ms) (ms * (ANIMATION_DURATION_PER_SEC / 1000)) 
#define DUR_S(s) (s * ANIMATION_DURATION_PER_SEC) 

#endif
