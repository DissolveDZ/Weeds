# 0 "SpriteLight/engine_include/SpriteLight.h"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "SpriteLight/engine_include/SpriteLight.h"
# 1 "SpriteLight/engine_include/main.h" 1



# 1 "SpriteLight/engine_include/shader.h" 1
       

char *ReadTextFile(char *path);



static char* FormatShaderUniform(const char* uniform_name, int index);

typedef struct Shader
{
    unsigned int ID;
} Shader;

Shader LoadShader(char *vertex_path, char *fragment_path);

void UseShader(Shader shader);

void SetShaderBool(int Shader_ID, const char *name, bool value);

void SetShaderInt(int Shader_ID, const char *name, int value);

void SetShaderFloat(int Shader_ID, const char *name, float value);

void SetShaderMat4(int Shader_ID, const char *name, mat4 matrix);

void SetShaderVec2(int Shader_ID, const char *name, vec2 vector);

void SetShaderVec3(int Shader_ID, const char *name, vec3 vector);

void SetShaderVec4(int Shader_ID, const char *name, vec4 value);

void SetShaderVec3v(int Shader_ID, const char *name, vec3 *vector, int amount);
# 5 "SpriteLight/engine_include/main.h" 2
# 1 "SpriteLight/engine_include/texture.h" 1
       

typedef struct Texture
{
    unsigned int ID;
    char *path;
    char *type;
    int width;
    int height;
} Texture;

Texture LoadTexture2D(const char *path, float anisotropy, bool gamma);
SDL_Surface *LoadSDLImage(char *path);
# 6 "SpriteLight/engine_include/main.h" 2
# 1 "SpriteLight/engine_include/lights.h" 1
       


static int MAX_POINT_LIGHTS = 10;

typedef struct PointLight
{
    vec3 position;
    float padding1;
    vec3 color;
    float padding2;
    vec3 ambient;
    float intensity;
    float radius;
    vec3 padding3;
} PointLight;

typedef struct DirectionalLight
{
    vec3 color;
    vec3 ambient;
    vec3 direction;
} DirectionalLight;

typedef struct SpotLight
{
    vec3 pos;
    vec3 color;
    vec3 ambient;
    vec3 direction;
    float cutoff;
    float outer_cutoff;
    float falloff_mulitplier;
} SpotLight;

static int point_light_count;

static PointLight *point_lights;
static GLuint lights_ubo;
static GLuint lights_index = 0;
static GLuint lights_binding_point = 0;

void InitLights();

PointLight *CreatePointLight(vec3 position, vec3 color, vec3 ambient, float intensity, float radius);

void UpdateLights();

void FreeLights();
# 7 "SpriteLight/engine_include/main.h" 2
       

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef float f32;
typedef double f64;

typedef vec2s Vector2;
typedef ivec2s Vector2I;
typedef vec3s Vector3;
typedef ivec3s Vector3I;
typedef vec4s Vector4;
typedef ivec4s Vector4I;

typedef enum CameraType
{
    DEFAULT_CAMERA,
    PANNING_CAMERA
} CameraType;

typedef struct Camera
{
    Vector3 position;
    Vector3 target;
    CameraType type;
    float angle;
    float fov;
    float zoom;
} Camera;

typedef struct Material
{

} Material;

typedef struct Line
{
    Vector2 start;
    Vector2 end;
} Line;

typedef struct Rectangle
{
    float x;
    float y;
    float width;
    float height;
    Texture texture;

} Rectangle;

typedef struct Collider
{
    float x;
    float y;
    float width;
    float height;
    Line *vertices;
    bool dynamic;
    bool rotating;
} Collider;

typedef struct Entity
{
    float accel;
    float decel;
    Vector2 velocity;
    Vector2 pelocity;
    bool is_floor;
    float speed;
    float max_speed;
    float jump_height;
    float health;
    bool flip;
    Texture tex;
    Collider col;
    Rectangle floor_check;
} Entity;

typedef struct BloomMip
{
    vec2 size;
    ivec2 int_size;
    Texture texture;
} BloomMip;
typedef struct Bloom
{
    unsigned int mip_chain_len;
    BloomMip *mip_chain;
    unsigned int FBO;
    bool karis_average;
    bool enabled;
} Bloom;
typedef struct PointIntersect
{
    Vector2 dist;
    bool hit;
} PointIntersect;

typedef enum Player_State
{
    IDLE,
    WALKING
} Player_State;

typedef struct Player
{
    float turn_mult;
    float vertical_to_horizontal;
    Vector2 last_velocity;
    bool was_floor;
    Entity entity;
    Player_State state;
} Player;

typedef struct TextCharacter
{
    unsigned int texture_id;
    ivec2 size;
    ivec2 bearing;
    unsigned int advance;
} TextCharacter;
static TextCharacter default_chars[128];

typedef struct Font
{
    char *path;
    TextCharacter loaded_chars[128];
} Font;
static Font **loaded_fonts;
static unsigned int loaded_fonts_len;
static unsigned int loaded_fonts_max = 5;

typedef struct Text
{
    char *text;
    float x;
    float y;
    float scale;
    Vector4 color;
} Text;

static unsigned int text_characters_max = 100;

typedef struct Sound
{
    Mix_Chunk *chunk;
} Sound;

typedef struct Music
{
    Mix_Music *music;
    u32 volume;
} Music;

typedef struct Audio
{
    Music **music;
    u32 music_len;
    u32 music_max;
    Sound **sounds;
    u32 parallel_sounds;
    u32 max_parallel_sounds;
    u32 max_parallel_musics;
    u32 sounds_playing;
    u32 sounds_len;
    u32 sounds_max;
    u32 volume;
} Audio;

typedef struct State
{
    int screen_width;
    int screen_height;
    float near_z;
    float far_z;
    SDL_Event window_event;
    SDL_Window *main_window;
    SDL_GLContext main_context;

    Audio audio;

    u8 *key_state;
    u32 mouse_state;
    Vector2 mouse_world;
    Vector2 camera_pan_start;
    Vector2 camera_pan_end;
    Vector2I mouse_pos;
    Vector2I mouse_delta;
    int wheel;
    bool deferred;
    bool sdf_font;
    void (*resize_callback)(int, int);
    Bloom bloom;
    Player player;
    Camera camera;
    float frame_time;
    float time;
    int active_camera;
    float gravity;
    bool quit;
    mat4 model, view, projection, ortho_projection;
    unsigned int max_colliders;
    unsigned int cur_colliders;
    bool fullscreen;
} State;

static Shader downsample_shader, upsample_shader, basic_shader, basic_screen_space_shader, circle_shader, text_shader_world, text_shader, gradient_shader;

static float quad_vertices[] = {

    -1.f,
    1.f,
    0.0f,
    0.0f,
    1.0f,
    -1.f,
    -1.f,
    0.0f,
    0.0f,
    0.0f,
    1.f,
    1.f,
    0.0f,
    1.0f,
    1.0f,
    1.f,
    -1.f,
    0.0f,
    1.0f,
    0.0f,
};

static float plane_vertices[] = {

    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    1.0f,
    1.0f,
    0.5f,
    -0.5f,
    0.0f,
    1.0f,
    0.0f,
};

static State *state;

static int MAX_BLOOM_MIP = 10;
static int last_frame;
static int current_frame;

static unsigned int quad_vbo, quad_vao;
static unsigned int plane_vbo, plane_vao;
static unsigned int text_vbo, text_vao;

int GetRandomValue(int min, int max);
char* TextFormat(const char* format, ...);
float Lerp(float start, float end, float amount);

void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
char *ReadTextFile(char *path);

Vector2 Vector2Scale(Vector2 vector, float scalar);

Vector2 Vector2Add(Vector2 first, Vector2 second);
Vector2 Vector2Zero(void);
Vector3 Vector3Scale(Vector3 vector, float scalar);
Vector3 Vector3Add(Vector3 first, Vector3 second);
Vector3 Vector3Subtract(Vector3 first, Vector3 second);
Vector3 Vector3Zero(void);
float Vector3Distance(Vector3 v1, Vector3 v2);

bool Vector2Comp(Vector2 first, Vector2 second);

Vector2 GetMouseDelta(void);
Vector2 GetMouseWorldDelta(void);
Vector3 Vector3Transform(vec3 v, mat4 mat);
Vector2 GetScreenToWorld2D(Vector2 position, mat4 projection);
Camera *CreateCamera2D(float fov, Vector3 position, CameraType type);
void GBufferSetup(unsigned int *g_buffer, unsigned int *g_position, unsigned int *g_normal, unsigned int *g_albedo, unsigned int *depth, int screen_width, int screen_height);
void PostProcessBuffer(unsigned int *post_process_fbo, unsigned int *post_process_color, unsigned int *depth, int screen_width, int screen_height);
void BufferSetup(unsigned int *VAO, unsigned int *VBO, float vertices[], int size, bool textured, bool normals);
void OnResize(int new_width, int new_height);
Font *LoadFont(char *path, unsigned int resolution);
void InitDefaultFont(unsigned int resolution);
Vector3 MeasureText(char *text, Font *font, float scale);
Vector3 MeasureTextText(Text *text, Font *font);
Vector3 MeasureWorldText(char *text, Font *font, float scale);
Vector3 MeasureWorldTextText(Text *text, Font *font);
void CameraZoom(Camera *camera, float amount, float min, float max);
void LightingPass();


State *EngineInit(char *window_name, char *icon_path, int width, int height, int bloom_mip_level);
void EngineUpdate();
void UpdateKeys();
void UpdateCamera();
void EngineQuit(void);
void BloomInit(int mip_amount);
void UpsampleBloom(float filter_radius);
void DownSampleBloom(unsigned int src_texture, float threshold, float knee);
void RenderBloom(unsigned int src_texture, float filter_radius, float threshold, float knee);
# 2 "SpriteLight/engine_include/SpriteLight.h" 2

# 1 "SpriteLight/engine_include/audio.h" 1
u32 LoadAudioStream(const char *file_name);
u32 LoadSound(const char *file_name);
u32 PlaySound(u32 sound);
u32 PlayAudioStream(u32 stream);
void SetGlobalVolume(u32 volume);
void SetVolume(u32 sound, u8 volume);
void SetAudioStreamVolume(u32 stream, u8 volume);
u32 InitAudio();
u32 QuitAudio();
void ToggleAudio();
# 4 "SpriteLight/engine_include/SpriteLight.h" 2


# 1 "SpriteLight/engine_include/draw.h" 1
void DrawQuad();
void DrawRect(Rectangle rec, Vector4 color);
void DrawUIRect(Rectangle rec, Vector4 color);
void DrawUITexRect(Rectangle rec);
void DrawTexRect(Rectangle rec);
void DrawTexRectTint(Rectangle rec, Vector4 tint);
void DrawWorldText(char *text, Font *font, float x, float y, float scale, Vector4 color);
void DrawWorldTextText(Text text, Font *font);
void DrawText(char *text, Font *font, float x, float y, float scale, Vector4 color);
void DrawTextText(Text text, Font *font);
void DrawSubText(char *text, Font *font, int count, float x, float y, float scale, Vector4 color);
void DrawSubTextText(Text *text, Font *font, int count);
void DrawGradientV(Vector4 start, Vector4 end, float offset);
# 7 "SpriteLight/engine_include/SpriteLight.h" 2
