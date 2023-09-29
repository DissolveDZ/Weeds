typedef enum directions
{
    RIGHT,
    LEFT,
    UP,
    DOWN
} directions;
directions direction;

typedef enum Menu
{
    NONE,
    CLOSED,
    FARM,
    SELECT,
    SKILL,
    SEEDS,
    FARMERS
} Menu;
Menu menu;

typedef enum Weed_Types
{
    PLANTED,
    SAPLING,
    GROW1,
    GROW2,
    GROW3,
    GROWN,
    DED
} Weed_Types;

typedef struct Weed
{
    Weed_Types type;
    Texture texture;
    Vector2I pos;
    bool watered;
    bool warning;
    bool planted;
    bool auto_watering;
    bool auto_harvest;
    int value;
    float time;
    float last_watered;
} Weed;

typedef struct ScreenText
{
    char *text;
    Vector2 position;
    Vector2 velocity;
    float size;
    int random;
    float opacity;
    float lifetime;
} ScreenText;

typedef struct MultiSound
{
    u32 sound;
    int ID;
    int len;
} MultiSound;

bool CheckCollisionPointRec(Vector2 point, Rectangle rec)
{
    bool collision = false;

    if ((point.x >= rec.x) && (point.x < (rec.x + rec.width)) && (point.y >= rec.y) && (point.y < (rec.y + rec.height)))
        collision = true;

    return collision;
}

static inline void DrawTextureV(Texture texture, Vector2 pos, Vector4 tint)
{
    DrawTexRect((Rectangle){pos.x, pos.y, texture.width, texture.height, texture});
}

static inline void DrawTextureVI(Texture texture, Vector2I pos, Vector4 tint)
{
    DrawTexRect((Rectangle){pos.x, pos.y, texture.width, texture.height, texture});
}

static inline void DrawTextureEx(Texture texture, Vector2 pos, float width, float height, Vector4 tint)
{
    DrawTexRect((Rectangle){pos.x, pos.y, width, height, texture});
}

static inline void DrawTexture(Texture texture, float x, float y, Vector4 tint)
{
    DrawTexRect((Rectangle){x, y, texture.width, texture.height, texture});
}

Shader plant_message_displacement;
Shader background;
// RenderTexture plant_messages_rendertexture;

int rander = 0;
char *Game_Name = "";
unsigned int plant_stage_len = 0;
unsigned int max_plant_messages = 20;
unsigned int max_plant_messages_on_screen = 20;
unsigned int cur_plant_messages_on_screen = 0;
Shader main_shader;
Shader post_processing;
unsigned int color_fbo, post_processing_fbo;
unsigned int color_buffer, post_processing_buffer;
unsigned int depth_buffer;
bool camera_lerp = true;
bool hover_ui = false;
char *plant_messages[] =
    {
        "planted!",
        "mission completed return to HQ",
        "I don't feel so good..",
        "very nice!",
        "yuy!",
        "finally..",
        "great job!?!?",
        "just a bit longer..",
        "is this what I'm supposed to do?",
        "i wonder how my familay is feeling... are they misssing me? its been so long..."};
Text *plant_message_array;
Vector4 upgrades_rec_color;
Vector4 cursor_color = (Vector4){255, 255, 255, 255};

Vector4 background1 = (Vector4){255, 255, 255, 255};
Vector4 background2 = (Vector4){255, 255, 255, 255};

Vector4 fade1 = (Vector4){0, 0, 0, 255};
Vector4 fade2 = (Vector4){0, 0, 0, 255};

Texture cursor1;
Texture cursor2;
Texture cursor_texture;
Texture plant_stages[6];
Texture dirt[3];
Texture dirt_dry[3];
Texture warning;
Texture background_texture;
Texture seed_bag;
Texture water_bucket;
Texture shovel;
Texture clock;
Texture cash;
Texture plant_glow;

float global_time;
float day_time;
float darkness = 0;
float time_mult = 1.f;
float decay_speed = 1.f;
float water_need = 0.25f;

u32 music;
int max_weeds;
int seeds = 1000;
unsigned int day = 0;
unsigned int grid_x = 3;
unsigned int grid_y = 1;
unsigned int money = 0;
unsigned int display_money = 0;

Weed **weed_array;

u32 plant_sound;
u32 dig_sound;
u32 water_sound;
u32 cash_sound;
Vector2 src_resolution = {1920, 1080};
Vector2 shop_slide_offset = {0};
Vector2 seed_pos = {0};
Vector2 camera_pos = {0};
Vector2I cursor_pos = {0};
Vector2I last_cursor_pos = {0};
Camera *camera;
float frame_time;
float general_time;
float time_passed;
float camera_zoom = 10;
bool toggle_cursor = false;
bool fullscreen = false;
bool clicked_ui = false;
Texture seed;

Font *pixelfont;
typedef struct UI
{
    Rectangle rec;
    Vector4 rec_color;
    Vector4 text_color;
    Menu menu;
    char *text;
    char *display_text;
    bool mouse_over;
    bool pressed;
    bool format;
} UI;

typedef struct Buy_Button
{
    UI **ui;
    float cost;
    unsigned int amount;
    char *tooltip;
    bool select_plant;
    Texture *tex;
} Buy_Button;

UI *UI_array;
unsigned int UI_len = 0;
unsigned int UI_max = 50;

UI *upgrade_button;

Buy_Button *buy_land;
Buy_Button *buy_auto_water;
Buy_Button *buy_auto_harvest;
Buy_Button *buy_value;
Buy_Button *buy_seeds;
Buy_Button *buy_time;
Buy_Button *buy_fertilizer;

int AddUIElement(Rectangle rec, char *text, Vector4 rec_color, Vector4 text_color, Menu menu)
{
    if (UI_len >= UI_max - 1)
    {
        UI_max += 10;
        UI_array = realloc(UI_array, UI_max * sizeof(UI));
    }
    UI_array[UI_len].rec = rec;
    UI_array[UI_len].text = text;
    UI_array[UI_len].display_text = malloc(strlen(text) + 1);
    strcpy(UI_array[UI_len].display_text, text);
    UI_array[UI_len].rec_color = rec_color;
    UI_array[UI_len].text_color = text_color;
    UI_array[UI_len].menu = menu;
    UI_len++;
    return UI_len - 1;
}

// Create a UI element which will be returned as a pointer
UI *CreateUIElement(float x, float y, float width, float height, char *text, Vector4 rec_color, Vector4 text_color, Menu menu)
{
    if (UI_len == 0)
        UI_array = malloc(UI_max * sizeof(UI));
    return &UI_array[AddUIElement((Rectangle){x, y, width, height}, text, rec_color, text_color, menu)];
}

void UpdateUI()
{
    for (int i = 0; i < UI_len; i++)
    {
        UI_array[i].pressed = false;
        UI_array[i].mouse_over = false;
        if (UI_array[i].menu != NONE && UI_array[i].menu != menu)
            continue;
        if (CheckCollisionPointRec((Vector2){state->mouse_pos.x, state->mouse_pos.y}, UI_array[i].rec))
        {
            UI_array[i].mouse_over = true;
            hover_ui = true;
            if (clicked_ui)
            {
                UI_array[i].pressed = true;
            }
        }
        else
            hover_ui = false;
    }
}
void RenderUI()
{
    for (int i = 0; i < UI_len; i++)
    {
        UI *cur_ui = &UI_array[i];
        if (cur_ui->menu != NONE && cur_ui->menu != menu)
            continue;
        Vector4 color = cur_ui->rec_color;
        float darken = 0.5f;
        if (cur_ui->mouse_over)
        {
            color = (Vector4){color.r * darken, color.g * darken, color.b * darken, color.a};
        }
        DrawUIRect((Rectangle){cur_ui->rec.x + cur_ui->rec.width / 2, cur_ui->rec.y + cur_ui->rec.height / 2, cur_ui->rec.width, cur_ui->rec.height}, color);
        float text_size = 0.06f;
        Vector3 text_offset = MeasureText(cur_ui->display_text, pixelfont, text_size);
        while (text_offset.x > cur_ui->rec.width)
        {
            text_offset = MeasureText(cur_ui->display_text, pixelfont, text_size);
            text_size *= 0.95f;
        }
        DrawText(cur_ui->display_text, pixelfont, cur_ui->rec.x + cur_ui->rec.width / 2 - text_offset.x / 2, cur_ui->rec.y + text_offset.y / 2 + cur_ui->rec.height / 2, text_size, cur_ui->text_color);
    }
}

void ResizeUI(int new_width, int new_height)
{
    printf("screen width: %i\n", state->screen_width);
    Rectangle fraction;
    for (int i = 0; i < UI_len; i++)
    {
        UI *cur_ui = &UI_array[i];
        fraction = (Rectangle){cur_ui->rec.x / (float)state->screen_width, cur_ui->rec.y / (float)state->screen_height, cur_ui->rec.width / (float)state->screen_width, cur_ui->rec.height / (float)state->screen_height};
        cur_ui->rec = (Rectangle){fraction.x * new_width, fraction.y * new_height, fraction.width * new_width, fraction.height * new_height};
    }
}

void GameResize(int new_width, int new_height)
{
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_width, new_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, new_width, new_height);

    ResizeUI(new_width, new_height);
    // plant_messages_rendertexture = LoadRenderTexture(state->screen_width, state->screen_height);
    seed_pos = (Vector2){upgrade_button->rec.x - 150, new_height * .035 - seed.height / 4};
    float screenSize[2] = {(float)new_width, (float)new_height};
    if (new_width && new_height)
        SetShaderVec2(plant_message_displacement.ID, "size", screenSize);
}

void FormatUI(UI *ui, char *type, const char *text)
{
    int len = (strlen(ui->text) + strlen(text)) * sizeof(char) + 1;
    if (!ui->format)
        ui->display_text = realloc(ui->display_text, len);
    else if (strlen(ui->display_text) + 1 < len)
        ui->display_text = realloc(ui->display_text, len);
    strcpy(ui->display_text, ui->text);
    ui->format = true;
}

void FormatUIInt(UI *ui, char *type, int val)
{
    const char *text = TextFormat(type, val);
    FormatUI(ui, type, text);
    strcat(ui->display_text, text);
}

void FormatUIFloat(UI *ui, char *type, float val)
{
    const char *text = TextFormat(type, val);
    FormatUI(ui, type, text);
    strcat(ui->display_text, text);
}

void InitPlant(Weed *weed)
{
    weed->pos = (Vector2I){0, 0};
    weed->time = 0;
    weed->last_watered = 0;
    weed->planted = false;
    weed->auto_watering = false;
    weed->auto_harvest = false;
    weed->watered = false;
    weed->warning = false;
    weed->value = 1;
    weed->type = 0;
}

Buy_Button *buy_buttons;
unsigned int buy_buttons_len = 0;
unsigned int buy_buttons_max = 10;

void FormatBuyButtons()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        Buy_Button *cur_button = &buy_buttons[i];
        int amount = cur_button->amount ? cur_button->amount : 1;
        FormatUIInt(cur_button->ui[0], "%i$", cur_button->cost * amount);
    }
}

int AmountLookup(int amount, bool dir)
{
    int new_amount = 1;
    if (dir)
    {
        switch (amount)
        {
        case 1:
            new_amount = 2;
            break;
        case 2:
            new_amount = 5;
            break;
        case 5:
            new_amount = 10;
            break;
        case 10:
            new_amount = 20;
            break;
        case 20:
            new_amount = 50;
            break;
        case 50:
            new_amount = 100;
            break;
        case 100:
            new_amount = 250;
            break;
        case 250:
            new_amount = 500;
            break;
        case 500:
            new_amount = 1000;
            break;

        default:
            new_amount = 1;
            break;
        }
    }
    else
    {
        switch (amount)
        {
        case 2:
            new_amount = 1;
            break;
        case 5:
            new_amount = 2;
            break;
        case 10:
            new_amount = 5;
            break;
        case 20:
            new_amount = 10;
            break;
        case 50:
            new_amount = 20;
            break;
        case 100:
            new_amount = 50;
            break;
        case 250:
            new_amount = 100;
            break;
        case 500:
            new_amount = 250;
            break;
        case 1000:
            new_amount = 500;
            break;

        default:
            new_amount = 1000;
            break;
        }
    }
    return new_amount;
}

void UpdateBuyButtons()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        Buy_Button *cur_button = &buy_buttons[i];
        int amount = cur_button->amount ? cur_button->amount : 1;
        if (cur_button->cost * amount <= money && 1 <= floorf(cur_button->cost * amount))
        {
            cur_button->ui[0]->text_color = (Vector4){0, 0, 0, 255};
            if (cur_button->amount)
            {
                cur_button->ui[1]->text_color = (Vector4){0, 0, 0, 255};
                cur_button->ui[2]->text_color = (Vector4){0, 0, 0, 255};
            }
        }
        else
        {
            cur_button->ui[0]->text_color = (Vector4){255*2, 0, 0, 255};
            if (cur_button->amount)
            {
                cur_button->ui[1]->text_color = (Vector4){255*2, 0, 0, 255};
                cur_button->ui[2]->text_color = (Vector4){255*2, 0, 0, 255};
            }
        }
        if (cur_button->amount && cur_button->ui[1]->pressed)
        {
            cur_button->amount = AmountLookup(cur_button->amount, 1);
            FormatBuyButtons();
        }
        if (cur_button->amount && cur_button->ui[2]->pressed)
        {
            cur_button->amount = AmountLookup(cur_button->amount, 0);
            FormatBuyButtons();
        }
    }
}

bool Buy(Buy_Button *button)
{
    int amount = button->amount ? button->amount : 1;
    if (money >= button->cost * amount && 1 <= floorf(button->cost * amount))
    {
        PlaySound(cash_sound);
        money -= button->cost * amount;
        // button->cost += button->cost / 2;
        // button->cost *= 1.25f;
        FormatBuyButtons();
        return true;
    }
    return false;
}
typedef enum Selectables
{
    DEFAULT,
    AUTOWATER,
    AUTOFARM,
    MOREVALUE
} Selectables;
Selectables active_selected;

// Create a Buy button which will be returned as a pointer
Buy_Button *CreateBuyButton(float x, float y, float width, float height, char *text, Texture *texture, Vector4 rec_color, Vector4 text_color, Menu menu, float cost, int amount, char *tooltip, bool select_plant)
{
    Buy_Button *button = {0};
    if (buy_buttons_len == 0)
    {
        buy_buttons = malloc(buy_buttons_max * sizeof(Buy_Button));
    }
    if (buy_buttons_len >= buy_buttons_max)
    {
        buy_buttons_max += 10;
        buy_buttons = realloc(buy_buttons, buy_buttons_max * sizeof(Buy_Button));
    }
    if (UI_len == 0)
        UI_array = malloc(UI_max * sizeof(UI));
    button = &buy_buttons[buy_buttons_len];
    float padding = 120.f;
    int size = 120;
    if (amount)
    {
        button->ui = malloc(3 * sizeof(UI *));
        button->ui[1] = &UI_array[AddUIElement((Rectangle){x + width + padding, y, size, height / 2}, "up", rec_color, text_color, menu)];
        button->ui[2] = &UI_array[AddUIElement((Rectangle){x + width + padding, y + height / 2, size, height / 2}, "down", rec_color, text_color, menu)];
    }
    else
        button->ui = malloc(1 * sizeof(UI *));
    button->ui[0] = &UI_array[AddUIElement((Rectangle){x, y, width, height}, text, rec_color, text_color, menu)];
    button->amount = amount;
    button->cost = cost;
    button->tooltip = tooltip;
    button->tex = texture;
    buy_buttons_len++;
    FormatBuyButtons();
    return button;
}

void DrawButtonInfo()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        Buy_Button *cur_button = &buy_buttons[i];
        if (menu != cur_button->ui[0]->menu)
            continue;
        if (cur_button->tex != NULL)
        {
            cur_button->tex->width = cur_button->ui[0]->rec.height;
            cur_button->tex->height = cur_button->ui[0]->rec.height;
            Texture tex = *cur_button->tex;
            DrawUITexRect((Rectangle){cur_button->ui[0]->rec.x - tex.width / 2 - 25, cur_button->ui[0]->rec.y + cur_button->ui[0]->rec.height / 2, tex.width, tex.height, tex});
            cur_button->tex->width = 1;
            cur_button->tex->height = 1;
        }
        if (!cur_button->amount)
            continue;
        const char *text = TextFormat("%i", cur_button->amount);
        float text_size = 0.0825f;
        Vector3 text_len = MeasureText(text, pixelfont, text_size);
        Vector2 dist = (Vector2){(cur_button->ui[1]->rec.x - cur_button->ui[0]->rec.x) + cur_button->ui[0]->rec.x - cur_button->ui[1]->rec.width / 2, cur_button->ui[0]->rec.y + cur_button->ui[0]->rec.height / 2}; //(Vector2){(cur_button->ui[1]->rec.x - cur_button->ui[0]->rec.x)/2 + cur_button->ui[0]->rec.x - cur_button->ui[1]->rec.width/2, cur_button->ui[1]->rec.y + cur_button->ui[1]->rec.height};
        DrawText(text, pixelfont, dist.x - text_len.x / 2, dist.y - text_len.y / 2 + text_len.y, text_size, cur_button->ui[0]->text_color);
    }
}

int GetTooltipIndex()
{
    for (int i = 0; i < buy_buttons_len; i++)
    {
        if (buy_buttons[i].ui[0]->mouse_over)
            return i;
    }
    return -1;
}

void UpdatePlants()
{
    int t = 0;
    for (size_t i = 0; i < grid_x; i++)
    {
        for (size_t o = 0; o < grid_y; o++)
        {
            Weed *cur_plant = &weed_array[i][o];
            if (cur_plant->planted)
            {
                cur_plant->pos = (Vector2I){i, o};
                cur_plant->warning = false;
                if ((global_time - cur_plant->time) * decay_speed <= 1)
                    cur_plant->type = PLANTED;
                else if ((global_time - cur_plant->time) * decay_speed <= 2)
                    cur_plant->type = SAPLING;
                else if ((global_time - cur_plant->time) * decay_speed <= 3)
                    cur_plant->type = GROW1;
                else if ((global_time - cur_plant->time) * decay_speed <= 4)
                    cur_plant->type = GROW2;
                else if ((global_time - cur_plant->time) * decay_speed <= 5)
                    cur_plant->type = GROW3;
                else if ((global_time - cur_plant->time) * decay_speed >= 6)
                    cur_plant->type = GROWN;
                if ((global_time - cur_plant->last_watered) * water_need > 0.5f)
                    cur_plant->watered = false;
                if ((global_time - cur_plant->last_watered) * water_need > 0.75f && !cur_plant->auto_watering)
                    cur_plant->warning = true;
                if ((global_time - cur_plant->last_watered) * water_need > 1.f && !cur_plant->auto_watering)
                    cur_plant->type = DED;
            }
            if (!cur_plant->watered && cur_plant->auto_watering && seeds)
            {
                PlaySound(water_sound);
                cur_plant->last_watered = global_time;
                cur_plant->watered = true;
            }
            if (!cur_plant->planted && cur_plant->auto_harvest && seeds)
            {
                PlaySound(plant_sound);
                seeds -= 1;
                cur_plant->planted = true;
                cur_plant->time = global_time;
                cur_plant->last_watered = global_time;
                cur_plant->watered = false;
                cur_plant->type = PLANTED;
            }
            if (cur_plant->type == GROWN && cur_plant->auto_harvest)
            {
                PlaySound(dig_sound);
                cur_plant->type = PLANTED;
                cur_plant->planted = false;
                cur_plant->watered = false;
                money += cur_plant->value;
            }
            if (!cur_plant->planted)
            {
                cur_plant->pos = (Vector2I){i, o};
                cur_plant->time = 0;
            }
            if (cur_plant->type == DED)
            {
                cur_plant->planted = false;
            }
            else if (cur_plant->type < DED)
                cur_plant->texture = plant_stages[(int)cur_plant->type];
        }
    }
}

float GetTextMiddle(char *text, float size, Font *font, float spacing)
{
    return MeasureText(text, font, size).x / 2;
}