inline Vector2I GetScreenToWorld2DVectorI(Vector2 position, Camera camera)
{
    Vector2 temp = GetScreenToWorld2D(position, state->projection);
    return (Vector2I){temp.x, temp.y};
}
bool CheckOutOfBounds(Vector2I position, directions direction)
{
    switch (direction)
    {
    case RIGHT:
        if (position.x < grid_x - 1)
            return true;
        else
            return false;
    case LEFT:
        if (position.x > 0)
            return true;
        else
            return false;
    case UP:
        if (position.y > 0)
            return true;
        else
            return false;
    case DOWN:
        if (position.y < grid_y - 1)
            return true;
        else
            return false;
    }
}
void Plant(Vector2I position)
{
    Weed *cur_plant = &weed_array[position.x][position.y];
    if (!cur_plant->planted)
    {
        if (seeds > 0)
        {
            cur_plant->last_watered = global_time;
            cur_plant->planted = true;
            cur_plant->watered = true;
            cur_plant->time = global_time;
            seeds -= 1;
            UpdatePlants();
            PlaySound(plant_sound);
            if (GetRandomValue(1, 11) == 10)
            {
                /*
                cur_plant_messages_on_screen++;
                if (cur_plant_messages_on_screen >= max_plant_messages_on_screen - 1)
                {
                    max_plant_messages += 10;
                    plant_message_array = realloc(plant_message_array, sizeof(Text) * max_plant_messages);
                }
                plant_message_array[cur_plant_messages_on_screen - 1].position = ((Vector2){GetRandomValue(0, window_width), GetRandomValue(0, window_height)});
                plant_message_array[cur_plant_messages_on_screen - 1].random = GetRandomValue(0, 10 - 1);
                plant_message_array[cur_plant_messages_on_screen - 1].velocity.y = GetRandomValue(1, 50);
                plant_message_array[cur_plant_messages_on_screen - 1].size = GetRandomValue(20, 60);
                plant_message_array[cur_plant_messages_on_screen - 1].lifetime = 0;
                plant_message_array[cur_plant_messages_on_screen - 1].opacity = 255;
                */
            }
        }
    }
    else
    {
        if (cur_plant->type == GROWN)
        {
            cur_plant->type = 0;
            cur_plant->planted = false;
            cur_plant->watered = false;
            PlaySound(dig_sound);
            UpdatePlants();
            money += cur_plant->value;
        }
        else if (!cur_plant->watered)
        {
            cur_plant->last_watered = global_time;
            cur_plant->watered = true;
            PlaySound(water_sound);
        }
    }
}
int LerpI(int a, int b, float f)
{
    return (int)(a * (1024 - f) + b * f) >> 10;
}
float money_lerp = 0;
void MoneyAnimation(float time)
{
    money_lerp = glm_clamp(Lerp(money_lerp, money, 1.f * time), 0.f, money);
    // money_lerp = glm_clamp(money_lerp, 0, money);
    display_money = (int)roundf(money_lerp);
}

void ToggleMenu()
{
    if (menu == CLOSED)
        menu = FARM;
    else if (menu == FARM)
        menu = CLOSED;
}

void CameraMovement()
{
    float move_speed = 1.5f;
    float zoom_speed = 1.f;
    camera->position.x = Lerp(camera->position.x, cursor_pos.x, move_speed * frame_time);
    camera->position.y = Lerp(camera->position.y, -cursor_pos.y, move_speed * frame_time);
}

void ProcessButtons()
{
    if (upgrade_button->pressed)
    {
        ToggleMenu();
    }
    if (buy_seeds->ui[0]->pressed && Buy(buy_seeds))
    {
        seeds += buy_seeds->amount;
    }
    if (buy_auto_water->ui[0]->pressed)
    {
        active_selected = AUTOWATER;
        menu = SELECT;
        clicked_ui = false;
    }
    if (buy_auto_harvest->ui[0]->pressed)
    {
        active_selected = AUTOFARM;
        menu = SELECT;
        clicked_ui = false;
    }
    if (buy_value->ui[0]->pressed)
    {
        active_selected = MOREVALUE;
        menu = SELECT;
        clicked_ui = false;
    }
    if (buy_time->ui[0]->pressed && Buy(buy_time))
    {
        time_mult += 0.5f * buy_time->amount;
        printf("time mult: %f\n", time_mult);
    }
    if (buy_land->ui[0]->pressed && Buy(buy_land))
    {
        for (int p = 0; p < buy_land->amount; p++)
        {
            if (grid_x <= grid_y)
            {
                grid_x += 1;
                weed_array = realloc(weed_array, grid_x * sizeof(Weed *));
                weed_array[grid_x - 1] = malloc(grid_y * sizeof(Weed));
                for (int i = 0; i < grid_y; i++)
                {
                    InitPlant(&weed_array[grid_x - 1][i]);
                }
            }
            else
            {
                grid_y += 1;
                for (int i = 0; i < grid_x; i++)
                {
                    weed_array[i] = realloc(weed_array[i], grid_y * sizeof(Weed));
                    InitPlant(&weed_array[i][grid_y - 1]);
                }
            }
        }
        UpdatePlants();
    }
}

void update()
{
    clicked_ui = false;
    state->wheel = 0;
    camera->type = DEFAULT_CAMERA;
    bool panning = false;
    if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON_RMASK)
    {
        state->camera_pan_start = GetScreenToWorld2D((Vector2){state->mouse_pos.x, state->mouse_pos.y}, state->projection);
        camera_lerp = false;
        panning = true;
    }
    if (!camera_lerp && !hover_ui)
        camera->type = PANNING_CAMERA;
    while (SDL_PollEvent(&state->window_event))
    {
        UpdateKeys();
        switch (state->window_event.type)
        {
        case SDL_KEYDOWN:
            if (state->window_event.key.keysym.scancode == SDL_SCANCODE_TAB)
            {
                if (menu == SELECT)
                {
                    menu = CLOSED;
                    active_selected = DEFAULT;
                }
                else
                {
                    ToggleMenu();
                }
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (state->window_event.button.button == SDL_BUTTON_LEFT)
                clicked_ui = true;
        }
        if (menu == CLOSED)
        {
            switch (state->window_event.type)
            {
            case SDL_KEYDOWN:
                switch (state->window_event.key.keysym.scancode)
                {
                case SDL_SCANCODE_D:
                    direction = RIGHT;
                    if (!panning)
                        camera_lerp = true;
                    if (CheckOutOfBounds(cursor_pos, direction))
                    {
                        cursor_pos.x += 1;
                    }
                    break;
                case SDL_SCANCODE_A:
                    direction = LEFT;
                    if (!panning)
                        camera_lerp = true;
                    if (CheckOutOfBounds(cursor_pos, direction))
                    {
                        cursor_pos.x -= 1;
                    }
                    break;
                case SDL_SCANCODE_W:
                    direction = UP;
                    if (!panning)
                        camera_lerp = true;
                    if (CheckOutOfBounds(cursor_pos, direction))
                    {
                        cursor_pos.y -= 1;
                    }
                    break;
                case SDL_SCANCODE_S:
                    direction = DOWN;
                    if (!panning)
                        camera_lerp = true;
                    if (CheckOutOfBounds(cursor_pos, direction))
                    {
                        cursor_pos.y += 1;
                    }
                    break;
                case SDL_SCANCODE_RETURN:
                    Plant(cursor_pos);
                    break;
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                switch (state->window_event.button.button)
                {
                case SDL_BUTTON_LEFT:
                    if (!hover_ui)
                    {
                        Vector2 temp = GetScreenToWorld2D((Vector2){state->mouse_pos.x, state->mouse_pos.y}, state->projection);
                        Vector2I rounded_click = (Vector2I){roundf(temp.x), -roundf(temp.y)};
                        cursor_pos.x = rounded_click.x;
                        cursor_pos.y = rounded_click.y;
                        if (rounded_click.x > grid_x - 1)
                            cursor_pos.x = grid_x - 1;
                        if (rounded_click.x <= 0)
                            cursor_pos.x = 0;
                        if (rounded_click.y > grid_y - 1)
                            cursor_pos.y = grid_y - 1;
                        if (rounded_click.y <= 0)
                            cursor_pos.y = 0;
                        if (last_cursor_pos.x == cursor_pos.x && last_cursor_pos.y == cursor_pos.y)
                            Plant(cursor_pos);
                        last_cursor_pos = cursor_pos;
                    }
                    break;
                }
                break;
            }
        }
    }
    EngineUpdate();
    if (camera_lerp)
    {
        // camera->target = camera->position;
        camera->position.z += -1.f * state->wheel;
        if (camera->position.z < 1)
            camera->position.z = 1;
        else if (camera->position.z > 100)
            camera->position.z = 100;
        // if (state->wheel != 0)
        // CameraZoom(camera, -1.f * state->wheel, 1.f, 100.f);
        CameraMovement();
    }
    UpdateUI();
    UpdateBuyButtons();
    ProcessButtons();
    if (menu != CLOSED)
    {
        general_time = state->frame_time;
        MoneyAnimation(general_time);
    }
    switch (menu)
    {
    case CLOSED:
        frame_time = state->frame_time;
        MoneyAnimation(frame_time);
        global_time += frame_time * time_mult;
        day_time += frame_time * time_mult;
        time_passed += frame_time * 7;
        UpdatePlants();
        UpdateCamera();
        cursor_color = (Vector4){255 * 2, 255 * 2, 255 * 2, 255};
        if ((int)(time_passed) % 2)
        {
            toggle_cursor = !toggle_cursor;
            time_passed = 0;
        }
        if (toggle_cursor)
            cursor_texture = cursor1;
        else
            cursor_texture = cursor2;
        break;
    case SELECT:
        UpdateCamera();
        Vector2 temp = GetScreenToWorld2D((Vector2){state->mouse_pos.x, state->mouse_pos.y}, state->projection);
        Vector2I rounded_click = (Vector2I){roundf(temp.x), -roundf(temp.y)};
        cursor_pos.x = rounded_click.x;
        cursor_pos.y = rounded_click.y;
        if (rounded_click.x > grid_x - 1)
            cursor_pos.x = grid_x - 1;
        if (rounded_click.x <= 0)
            cursor_pos.x = 0;
        if (rounded_click.y > grid_y - 1)
            cursor_pos.y = grid_y - 1;
        if (rounded_click.y <= 0)
            cursor_pos.y = 0;
        last_cursor_pos = cursor_pos;
        Weed *cur_weed = &weed_array[cursor_pos.x][cursor_pos.y];
        switch (active_selected)
        {
        case AUTOWATER:
            if (!cur_weed->auto_watering)
                cursor_color = (Vector4){255 * 2, 255 * 2, 255 * 2, 255};
            else
                cursor_color = (Vector4){255 * 2, 0, 0, 255};
            if (!cur_weed->auto_watering && clicked_ui && !hover_ui && Buy(buy_auto_water))
            {
                cur_weed->auto_watering = true;
                menu = CLOSED;
                active_selected = DEFAULT;
            }
            break;
        case AUTOFARM:
            if (!cur_weed->auto_harvest)
                cursor_color = (Vector4){255 * 2, 255 * 2, 255 * 2, 255};
            else
                cursor_color = (Vector4){255 * 2, 0, 0, 255};
            if (!cur_weed->auto_harvest && clicked_ui && !hover_ui && Buy(buy_auto_harvest))
            {
                cur_weed->auto_harvest = true;
                menu = CLOSED;
                active_selected = DEFAULT;
            }
            break;
        case MOREVALUE:
            if (clicked_ui && !hover_ui && Buy(buy_value))
            {
                cur_weed->value += 1 * buy_value->amount;
                menu = CLOSED;
                active_selected = DEFAULT;
            }
            break;
        }
        break;
    }
}