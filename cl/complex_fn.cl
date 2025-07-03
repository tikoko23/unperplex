#define N_CHANNELS 3

typedef float2 complex;

inline void writeColorI(generic uchar (*fb)[N_CHANNELS], uint x, uint y, uint w, uchar3 color) {
    uchar (*pixel)[N_CHANNELS] = fb + (w * y) + x;
    (*pixel)[0] = color.r;
    (*pixel)[1] = color.g;
    (*pixel)[2] = color.b;
}

inline void writeColorF(generic uchar (*fb)[N_CHANNELS], uint x, uint y, uint w, float3 color) {
    float3 scaled = color * 255.0f;
    writeColorI(fb, x, y, w, (uchar3)(scaled.r, scaled.g, scaled.b));
}

inline float3 hslToRgb(float3 hsl) {
    const uint C = 0,
               X = 1,
               R = 0,
               G = 1,
               B = 2;

    const float factor_table[6][2][3] = {
        // R  G  B

        // c, x, 0
        {{ 1, 0, 0 },
         { 0, 1, 0 }},

        // x, c, 0
        {{ 0, 1, 0 },
         { 1, 0, 0 }},

        // 0, c, x
        {{ 0, 1, 0 },
         { 0, 0, 1 }},

        // 0, x, c
        {{ 0, 0, 1 },
         { 0, 1, 0 }},

        // c, 0, x
        {{ 0, 0, 1 },
         { 1, 0, 0 }},

        // x, 0, c
        {{ 1, 0, 0 },
         { 0, 0, 1 }},
    };

    float h = hsl.x;
    float s = hsl.y;
    float l = hsl.z;

    float c = (1.0f - fabs(2.0f * l - 1)) * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = l - c / 2.0f;

    uint H = h / 60;
    return m + (float3)(
        factor_table[H][C][R] * c + factor_table[H][X][R] * x,
        factor_table[H][C][G] * c + factor_table[H][X][G] * x,
        factor_table[H][C][B] * c + factor_table[H][X][B] * x
    );
}

inline float3 hsvToRgb(float3 hsv) {
    const uint C = 0,
               X = 1,
               R = 0,
               G = 1,
               B = 2;

    const float factor_table[6][2][3] = {
        // R  G  B

        // c, x, 0
        {{ 1, 0, 0 },
         { 0, 1, 0 }},

        // x, c, 0
        {{ 0, 1, 0 },
         { 1, 0, 0 }},

        // 0, c, x
        {{ 0, 1, 0 },
         { 0, 0, 1 }},

        // 0, x, c
        {{ 0, 0, 1 },
         { 0, 1, 0 }},

        // c, 0, x
        {{ 0, 0, 1 },
         { 1, 0, 0 }},

        // x, 0, c
        {{ 1, 0, 0 },
         { 0, 0, 1 }},
    };

    float h = hsv.x;
    float s = hsv.y;
    float v = hsv.z;

    float c = v * s;
    float x = c * (1.0f - fabs(fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    uint H = h / 60;
    return m + (float3)(
        factor_table[H][C][R] * c + factor_table[H][X][R] * x,
        factor_table[H][C][G] * c + factor_table[H][X][G] * x,
        factor_table[H][C][B] * c + factor_table[H][X][B] * x
    );
}

inline float Re(complex z) {
    return z.x;
}

inline float Im(complex z) {
    return z.y;
}

inline complex C(float re, float im) {
    return (complex)(re, im);
}

inline float carg(complex z) {
    return atan2(Im(z), Re(z));
}

inline float cabs(complex z) {
    return hypot(Re(z), Im(z));
}

inline complex cexp(complex z) {
    float mod = exp(Re(z));
    complex dir = C(cos(Im(z)), sin(Im(z)));

    return dir * mod;
}

inline complex conj(complex z) {
    return C(Re(z), -Im(z));
}

inline complex cmul(complex z, complex w) {
    float a = Re(z);
    float b = Im(z);
    float c = Re(w);
    float d = Im(w);

    return C(a * c - b * d, a * d + b * c);
}

inline complex cdiv(complex z, complex w) {
    float a = Re(z);
    float b = Im(z);
    float c = Re(w);
    float d = Im(w);

    float denom = c * c + d * d;

    return C(
        (a * c + b * d) / denom,
        (b * c - a * d) / denom
    );
}

inline complex csin(complex z) {
    complex p = cexp(cmul(z, C(0,  1)));
    complex q = cexp(cmul(z, C(0, -1)));

    return cdiv(p - q, C(0, 2));
}

inline complex ccos(complex z) {
    complex p = cexp(cmul(z, C(0, 1)));
    complex q = cexp(cmul(z, C(0, -1)));

    return cdiv(p + q, C(2, 0));
}

inline complex cln(complex z) {
    return C(
        log(cabs(z)),
        carg(z)
    );
}

inline complex cpow(complex z, complex w) {
    float c = Re(w);
    float d = Im(w);

    float mod_log = log(cabs(z));
    float arg = carg(z);

    return cexp(C(
        c * mod_log - d * arg,
        d * mod_log + c * arg
    ));
}

inline float discreteCompress(float r) {
    float pow = trunc(log2(r));
    float pow_range = exp2(pow);
    float factor = r / pow_range - 1.0f;  

    return factor * 0.7f + 0.3f;
}

inline float continuousCompress(float r) {
    float min_light = 0.05f;
    float max_light = 0.90f;

    float p = 1.4f;
    float q = 10.0f;
    float c = 0.5f;

    /*
    const float max_l = 0.95f;
    const float threshold = 0.01f;
    const float steepness = 0.1f;

    return max_l - exp((r + (1.0f - max_l) - threshold) * -steepness);
    */

    float s = tanh(r);
    float f = c * sqrt(q * r);
    float g = 1.0f - exp(-p * r);
    return (f * (1.0f - s) + g * s) * (max_light - min_light) + min_light;
}

inline float3 discreteColorFn(complex z) {
    float arg = carg(z);
    float mod = cabs(z);

    float hue = fmod(fmod(degrees(arg), 360.0f) + 360.0f, 360.0f);
    float lig = discreteCompress(mod);

    return hsvToRgb((float3)(hue, 1, lig));
}

inline float3 continuousColorFn(complex z) {
    float arg = carg(z);
    float mod = cabs(z);

    float hue = fmod(fmod(degrees(arg), 360.0f) + 360.0f, 360.0f);
    float lig = continuousCompress(mod);

    return hslToRgb((float3)(hue, 1, lig));
}

inline complex f(complex z) {
    complex zsq = cmul(z, z);
    complex a = (z - C(2, 1));

    return cdiv(cmul((zsq - C(1, 0)), cmul(a, a)), (zsq + C(2, 2)));
}

inline complex _f(complex z) {
    return z;
}

typedef enum {
    COLOR_CONTINUOUS = 0,
    COLOR_DISCRETE,
    COLOR_N_MODES,
} ColorMode;

kernel void funcRender(global uchar (*fb)[N_CHANNELS], uint w, uint h, float2 offset, float scale, uint mode) {
    uint x = get_global_id(0);
    uint y = get_global_id(1);

    float real = ((int)x - (int)w / 2 + offset.x) * scale;
    float imag = ((int)y - (int)h / 2 - offset.y) * scale * -1.0f;

    complex val = f(C(real, imag));
    float3 col;

    switch (mode) {
    case COLOR_DISCRETE:
        col = discreteColorFn(val);
        break;
    case COLOR_CONTINUOUS:
        col = continuousColorFn(val);
        break;
    default:
        col = 0;
        break;
    }

    writeColorF(fb, x, y, w, col);
}

kernel void funcCalc(global const float2 *query, global float2 *result) {
    uint i = get_global_id(0);

    result[i] = f(query[i]);
}
