void draw()
{
    /*
    if (cur_plant_messages_on_screen > 0)
    {
        BeginTextureMode(plant_messages_rendertexture);
        ClearBackground((Vector4){0, 0, 0, 0});
        for (size_t i = 0; i < cur_plant_messages_on_screen; i++)
        {
            plant_message_array[i].lifetime += frame_time * 6;
            DrawText(TextSubtext(plant_messages[plant_message_array[i].random], 0, (int)plant_message_array[i].lifetime), plant_message_array[i].position.x, plant_message_array[i].position.y, plant_message_array[i].size, (Color){255, 33, 0, plant_message_array[i].opacity});
            plant_message_array[i].position.y += plant_message_array[i].velocity.y * frame_time;
            plant_message_array[i].opacity = Lerp(plant_message_array[i].opacity, 0, .2 * frame_time);
            if (plant_message_array[i].opacity < 10)
            {
                for (int o = i; o < cur_plant_messages_on_screen; o++)
                {
                    plant_message_array[o] = plant_message_array[o + 1];
                }
                cur_plant_messages_on_screen -= 1;
            }
        }
        DrawRect((Rectangle){0, 0, (float)plant_messages_rendertexture.texture.width, (float)-plant_messages_rendertexture.texture.height}, WHITE);
        EndTextureMode();
    }
    */
    float time_adjusted = day_time * 24 / 120 + 1;
    int oclock = (int)time_adjusted;
    if (oclock > 24)
    {
        day_time = 24 / 120;
        day++;
    }
    if (menu == CLOSED)
    {
        if (oclock < 12)
        {
            darkness = Lerp(darkness, (float)time_adjusted / 12, 1.f * frame_time * time_mult);
        }
        else
            darkness = Lerp(darkness, 1 - 1 * ((float)time_adjusted - 12) / 12, 1.f * frame_time * time_mult);
        darkness = glm_clamp(darkness, 0, 1);
    }
    unsigned int day_times = 4;
    for (int steps = 0; steps < day_times; steps++)
    {
        int cur_step = darkness / day_times * steps;
        switch (cur_step)
        {
        }
    }
    Vector3 day_color_mult = {glm_clamp(darkness + (1 - darkness) * 0.2f, 0, 1), glm_clamp(darkness + (1 - darkness) * 0.2f, 0, 1), glm_clamp(darkness + (1 - darkness) * 0.4f, 0, 1)};
    background1 = (Vector4){0, (121 - 50) * day_color_mult.y, (241 - 100) * day_color_mult.z, 255};
    background2 = (Vector4){150 * day_color_mult.x, 200 * day_color_mult.y, 255 * day_color_mult.z, 255};
    glClearColor(1, 1, 1, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, color_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DrawGradientV(background1, background2, state->camera.position.y / 100);
    for (int o = 0; o < grid_x; o++)
    {
        bool left_plant = o != 0;
        bool right_plant = o + 1 < grid_x;
        for (int i = 0; i < grid_y; i++)
        {
            Weed *cur_plant = &weed_array[o][i];
            int sides = 0;
            if (left_plant && right_plant)
                sides = 0;
            else if (left_plant)
                sides = 2;
            else if (right_plant)
                sides = 1;
            if (cur_plant->watered)
                DrawTexRect((Rectangle){cur_plant->pos.x, -cur_plant->pos.y, dirt[sides].width, dirt[sides].height, dirt[sides]});
            else
                DrawTexRect((Rectangle){cur_plant->pos.x, -cur_plant->pos.y, dirt_dry[sides].width, dirt_dry[sides].height, dirt_dry[sides]});
        }
    }

    for (int o = 0; o < grid_x; o++)
    {
        for (int i = 0; i < grid_y; i++)
        {
            Weed *cur_plant = &weed_array[o][i];
            if (cur_plant->type == GROWN)
                DrawTexRectTint((Rectangle){cur_plant->pos.x, -cur_plant->pos.y, plant_glow.width, plant_glow.height, plant_glow}, (Vector4){255 * 2, 255 * 2, 255 * 2, 255});
            if (cur_plant->planted)
                DrawTextureV(cur_plant->texture, (Vector2){cur_plant->pos.x, -cur_plant->pos.y}, (Vector4){255, 255, 255, 255});
            if (cur_plant->auto_watering)
            {
                float width = 0.5f;
                DrawTexRect((Rectangle){(float)cur_plant->pos.x - width / 2 - width / 4, -(float)cur_plant->pos.y + width - width / 4, width, width, water_bucket});
            }
            if (cur_plant->auto_harvest)
            {
                float width = 0.5f;
                DrawTexRect((Rectangle){(float)cur_plant->pos.x + 1 - width / 2 - width + width / 4, -(float)cur_plant->pos.y + width - width / 4, width, width, shovel});
            }
            if (cur_plant->warning && cur_plant->planted)
            {
                float width = 0.5f;
                DrawTexRect((Rectangle){(float)cur_plant->pos.x + 1 - width, -(float)cur_plant->pos.y - width, width, width, warning});
            }
        }
    }

    if (active_selected == MOREVALUE)
    {
        for (int o = 0; o < grid_x; o++)
        {
            for (int i = 0; i < grid_y; i++)
            {
                Weed *cur_plant = &weed_array[o][i];
                const char *text = TextFormat("%i", cur_plant->value);
                float text_size = 1.5f;
                Vector3 text_offset = MeasureWorldText(text, pixelfont, text_size);
                while (text_offset.x > 1)
                {
                    text_offset = MeasureWorldText(text, pixelfont, text_size);
                    text_size *= 0.95f;
                }
                printf("text size: %f\n", text_size);
                DrawWorldText(text, pixelfont, (float)cur_plant->pos.x - text_offset.x / 2, -(float)cur_plant->pos.y - text_offset.y / 2, text_size, (Vector4){255*2, 0, 0, 255});
            }
        }
    }

    DrawTexRectTint((Rectangle){cursor_pos.x, -cursor_pos.y, cursor_texture.width, cursor_texture.height, cursor_texture}, cursor_color);
    // EndMode2D();
    // draw world recs
    /*
    if (cur_plant_messages_on_screen > 0)
    {
        BeginShaderMode(plant_message_displacement);
        DrawTextureRec(plant_messages_rendertexture.texture, (Rectangle){0, 0, (float)plant_messages_rendertexture.texture.width, (float)-plant_messages_rendertexture.texture.height}, (Vector2){0, 0}, WHITE);
        EndShaderMode();
    }
    */

    DrawUITexRect((Rectangle){seed_pos.x + 100, seed_pos.y + seed.height / 2, seed.width, seed.height, seed});
    if (menu != CLOSED && menu != SELECT)
    {
        DrawUIRect((Rectangle){state->screen_width / 2, state->screen_height / 2, state->screen_width, state->screen_height}, (Vector4){0, 0, 0, 125});
        DrawUIRect((Rectangle){400 + (state->screen_width - 800) / 2, 125 + (state->screen_height - 250) / 2, state->screen_width - 800, state->screen_height - 250}, (Vector4){65, 65, 65, 125});
    }
    if (seeds == 0)
    {
        DrawText("Out of seeds!", 0, state->screen_width / 2 - MeasureText("Out of seeds!", 0, 0.0825f).x / 2, 120, 0.0825f, (Vector4){255*2, 0, 0, 255});
    }
    else if (menu == SELECT)
    {
        DrawText("Press TAB to exit select mode", 0, state->screen_width / 2 - MeasureText("Press TAB to exit select mode", 0, 0.0825f).x / 2, 120, 0.0825f, (Vector4){255*2, 0, 0, 255});
    }

    RenderUI();
    DrawButtonInfo();
    DrawText(TextFormat("money: %i$ ", display_money), pixelfont, 15, state->screen_height * .06f, 0.1f, (Vector4){0, 0, 0, 255});
    DrawText(TextFormat("%i: ", seeds), pixelfont, state->screen_width - upgrade_button->rec.width - 125 - MeasureText(TextFormat("%i: ", seeds), pixelfont, 0.0825f).x, state->screen_height * .035 + upgrade_button->rec.height / 2, 0.0825f, (Vector4){153, 101, 21, 255});

    DrawText(TextFormat("day: %i time: %i", day, oclock), pixelfont, state->screen_width / 2 - GetTextMiddle(TextFormat("day: %i time: %i", day, oclock), 0.0925f, pixelfont, 0), 70, 0.0925f, (Vector4){212, 140, 55, 125});

    Rectangle toolrec = {0};
    int ID = GetTooltipIndex();
    if (ID != -1)
    {
        char *tooltip = buy_buttons[ID].tooltip;
        float padding = 20.0f;
        float offset = padding / 2;
        Vector2 pos = (Vector2){state->mouse_pos.x + offset, state->mouse_pos.y + offset};
        float text_size = 0.0625f;
        Vector3 size = MeasureText(tooltip, pixelfont, text_size);
        toolrec.x = pos.x - offset;
        toolrec.y = pos.y - offset;
        toolrec.width = size.x + padding;
        toolrec.height = size.y + padding;
        DrawUIRect((Rectangle){toolrec.x + toolrec.width / 2, toolrec.y + toolrec.height / 2 + size.z, toolrec.width, toolrec.height}, (Vector4){125, 125, 125, 255});
        DrawText(tooltip, pixelfont, pos.x, pos.y + toolrec.height / 2, text_size, (Vector4){0, 0, 0, 255});
    }
    // DrawGradientV(fade1, fade2, 0.f);
    fade1.a = Lerp(fade1.a, 0, 2.0f * frame_time);
    fade2.a = Lerp(fade2.a, 0, 1.0f * frame_time);
    // DrawUIRect((Rectangle){state->mouse_pos.x, state->mouse_pos.y, 50, 50}, (Vector4){255, 255, 255, 255});
    glDisable(GL_BLEND);
    RenderBloom(color_buffer, 0.004f, 1.5f, 1.0f);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    UseShader(post_processing);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_buffer); // state->bloom.mip_chain[0].texture.ID);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, state->bloom.mip_chain[0].texture.ID);
    SetShaderInt(post_processing.ID, "lighting", 0);
    SetShaderInt(post_processing.ID, "bloom", 1);
    SetShaderFloat(post_processing.ID, "exposure", 1.0);         // 1.0f - (scene_exposure-4));
    SetShaderFloat(post_processing.ID, "bloom_strength", 0.5f); // scene_exposure/2 * 0.05f);
    DrawQuad();
    SDL_GL_SwapWindow(state->main_window);
}