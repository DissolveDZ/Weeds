void InitBuffers()
{
    glGenFramebuffers(1, &color_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, color_fbo);
    glGenTextures(1, &color_buffer);
    glBindTexture(GL_TEXTURE_2D, color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, state->screen_width, state->screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, state->screen_width, state->screen_height);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_buffer, 0);
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, state->screen_width, state->screen_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

    glGenFramebuffers(1, &post_processing_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, post_processing_fbo);
    glGenTextures(1, &post_processing_buffer);
    glBindTexture(GL_TEXTURE_2D, post_processing_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, state->screen_width, state->screen_height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, post_processing_buffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init()
{
    state = EngineInit(Game_Name, "resources/textures/cube.png", 1920, 1080, 12);
    InitBuffers();
    // main_shader = LoadShader("engine/quad.vert", "color_buffer.frag");
    post_processing = LoadShader("engine/quad.vert", "post_process.frag");
    state->resize_callback = &GameResize;
    max_weeds = grid_x * grid_y;
    rander = GetRandomValue(0, 6);
    switch (rander)
    {
    case 0:
        Game_Name = "Weeds";
        break;
    case 1:
        Game_Name = "generic weed game";
        break;
    case 2:
        Game_Name = "do NOT touch the grass";
        break;
    case 3:
        Game_Name = "i think they're onto us!!";
        break;
    case 4:
        Game_Name = "0)_(o";
        break;
    case 5:
        Game_Name = "who knows how long he has been sitting there";
        break;
    case 6:
        Game_Name = "i dont know if this is legal";
        break;
    }
    printf("%i\n", rander);
    printf("%s\n", Game_Name);
    SDL_SetWindowTitle(state->main_window, Game_Name);
    camera_pos = (Vector2){grid_x / 2, grid_y / 2};
    camera = CreateCamera2D(45.f, (Vector3){camera_pos.x, camera_pos.y, camera_zoom}, PANNING_CAMERA);
    cursor1 = LoadTexture2D("resources/textures/cursor1.png", 0, true);
    cursor2 = LoadTexture2D("resources/textures/cursor2.png", 0, true);
    seed_bag = LoadTexture2D("resources/textures/seed_bag.png", 0, true);
    char *temp_text = malloc(strlen("resources/textures/weed.png") + 1);
    for (int i = 0; i < 6; i++)
    {
        memcpy(temp_text, TextFormat("resources/textures/weed%i.png", i + 1), strlen("resources/textures/weed.png") + 1);
        temp_text[strlen("resources/textures/weed.png") + 1] = '\0';
        printf("loading %s\n", temp_text);
        plant_stages[i] = LoadTexture2D(temp_text, 0, true);
        plant_stages[i].width = 1;
        plant_stages[i].height = 1;
        plant_stage_len++;
    }
    free(temp_text);
    for (int i = 0; i < 3; i++)
    {
        char *temp_chars;
        switch (i)
        {
        case 0:
            temp_chars = "resources/textures/dirt.png";
            break;
        case 1:
            temp_chars = "resources/textures/dirt_left.png";
            break;
        case 2:
            temp_chars = "resources/textures/dirt_right.png";
        }
        dirt[i] = LoadTexture2D(temp_chars, 0, true);
        dirt[i].width = 1;
        dirt[i].height = 1;
        printf("%s\n", temp_chars);
    }
    for (int i = 0; i < 3; i++)
    {
        char *temp_chars;
        switch (i)
        {
        case 0:
            temp_chars = "resources/textures/dirt_dry.png";
            break;
        case 1:
            temp_chars = "resources/textures/dirt_dry_left.png";
            break;
        case 2:
            temp_chars = "resources/textures/dirt_dry_right.png";
        }
        dirt_dry[i] = LoadTexture2D(temp_chars, 0, true);
        dirt_dry[i].width = 1;
        dirt_dry[i].height = 1;
    }
    water_bucket = LoadTexture2D("resources/textures/water_bucket.png", 0, true);
    shovel = LoadTexture2D("resources/textures/shovel.png", 0, true);
    warning = LoadTexture2D("resources/textures/warning.png", 0, true);
    clock = LoadTexture2D("resources/textures/clock.png", 0, true);
    cash = LoadTexture2D("resources/textures/money.png", 0, true);
    plant_glow = LoadTexture2D("resources/textures/glow.png", 0, true);
    plant_glow.width = 4;
    plant_glow.height = 4;
    warning.width = 1;
    warning.height = 1;
    water_bucket.width = 1;
    water_bucket.height = 1;
    shovel.width = 1;
    shovel.height = 1;
    cursor1.width = 1;
    cursor1.height = 1;
    cursor2.width = 1;
    cursor2.height = 1;
    cursor_texture.width = 1;
    cursor_texture.height = 1;
    seed_bag.width = 128;
    seed_bag.height = 128;
    plant_sound = LoadSound("resources/sounds/plant.mp3");
    dig_sound = LoadSound("resources/sounds/dig.mp3");
    water_sound = LoadSound("resources/sounds/water.mp3");
    cash_sound = LoadSound("resources/sounds/cash.mp3");
    pixelfont = LoadFont("resources/fonts/Pixel_Square.ttf", 512);
    SetVolume(plant_sound, 1000.f);
    SetVolume(cash_sound, 200.f);
    SetVolume(dig_sound, 1200.f);
    // plant_message_array = malloc(max_plant_messages_on_screen * sizeof(Text));
    // plant_message_displacement = LoadShader(0, "warp.fs");
    music = LoadAudioStream("resources/sounds/35_Return_Trip.mp3");
    SetAudioStreamVolume(music, 25.f);
    PlayAudioStream(music);
    seed = LoadTexture2D("resources/textures/seed.png", 0, true);
    seed.width = 128;
    seed.height = 64;
    upgrades_rec_color = (Vector4){127, 106, 79, 125};

    // start with 20 seeds
    seeds = 20;
    // start with 0$
    money = 10000;

    day_time = 45;

    menu = CLOSED;
    upgrade_button = CreateUIElement(1575, 20, 325, 64, "upgrades", upgrades_rec_color, (Vector4){0, 0, 0, 255}, NONE);
    buy_seeds = CreateBuyButton(state->screen_width / 2 - 200, state->screen_height / 2 - 375, 400, 100, "Buy seeds: ", &seed_bag, upgrades_rec_color, (Vector4){255, 0, 0, 255}, FARM, 0.25f, 5, "You need to buy seeds\nelse you might go broke", false);
    buy_land = CreateBuyButton(state->screen_width / 2 - 200, state->screen_height / 2 - 250, 400, 100, "Buy more land: ", &dirt_dry[0], upgrades_rec_color, (Vector4){255, 0, 0, 255}, FARM, 10, 1, "Buy more land for your farm!", false);
    buy_auto_water = CreateBuyButton(state->screen_width / 2 - 200, state->screen_height / 2 - 125, 400, 100, "Buy automatic watering: ", &water_bucket, upgrades_rec_color, (Vector4){255, 0, 0, 255}, FARM, 30, 0, "Buy to automate watering, select and\nclick a plant to attach auto-watering", true);
    buy_auto_harvest = CreateBuyButton(state->screen_width / 2 - 200, state->screen_height / 2, 400, 100, "Buy automatic harvesting: ", &shovel, upgrades_rec_color, (Vector4){255, 0, 0, 255}, FARM, 30, 0, "Buy a harvester to automatically plant\nand harvest your plants, select and\nclick a plant to attach a harvester", true);
    buy_value = CreateBuyButton(state->screen_width / 2 - 200, state->screen_height / 2 + 125, 400, 100, "Buy more value: ", &cash, upgrades_rec_color, (Vector4){255, 0, 0, 255}, FARM, 30, 1, "Higher quality plants will result in\nhigher profit, select and click a plant\nto upgrade it's value!", true);
    buy_time = CreateBuyButton(state->screen_width / 2 - 200, state->screen_height / 2 + 250, 400, 100, "Speed up time: ", &clock, upgrades_rec_color, (Vector4){255, 0, 0, 255}, FARM, 100, 1, "Speed up time so your plants grow faster!\nIf sped up too much you won't be\nable to water in time so be careful!", false);

    weed_array = malloc(grid_x * sizeof(Weed *));
    for (int o = 0; o < grid_x; o++)
    {
        weed_array[o] = malloc(grid_y * sizeof(Weed));
        for (int i = 0; i < grid_y; i++)
        {
            InitPlant(&weed_array[o][i]);
            weed_array[o][i].pos = (Vector2I){o, i};
        }
    }
    OnResize(state->screen_width, state->screen_height);
    UpdatePlants();
}