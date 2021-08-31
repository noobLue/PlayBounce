#include <string>
#include <iostream>
#include <queue>
#include <map>

#include "raylib.h"
#include "Vector2d.h"

#include "GameController.h"
#include "GGUI.h"

enum class GameState { MENU, GAME };
const int main_font_size = 24;


void render_element(std::pair<std::shared_ptr<Element>, Vector2>& element);
void render_gui(GGUI& gui);
void init_gui(GGUI& main_menu, GameController& controller, GameState& game_state, Vector2d screenSize);

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    Vector2d screenSize = { 800, 450 };

    const int score_font = 30;
    InitWindow(screenSize.x, screenSize.y, "PlayBounce");

    GameController controller(Vector2d(800, 450)); // TODO: decouple game size from render size
    GameState game_state = GameState::MENU;

    Vector2 v2 = { 800, 450 };
    GGUI main_menu(v2);
    init_gui(main_menu, controller, game_state, screenSize);
    
    InitAudioDevice();

    // Load resources ----------------------------------------------------------------------
    enum class Sounds{Beep, Boop, Splash};
    std::map<Sounds, Sound> sounds;
    sounds.insert({ Sounds::Beep, LoadSound("resources/beep.wav") });
    sounds.insert({ Sounds::Boop, LoadSound("resources/boop.wav") });
    sounds.insert({ Sounds::Splash, LoadSound("resources/splash.wav") });

    for (auto& sound: sounds) {
        SetSoundVolume(sound.second, 0.5f);
        SetSoundPitch(sound.second, 0.5f);
    }
    //SetSoundPitch(splash, 0.7f);

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())
    {
        // Initializations
        //----------------------------------------------------------------------------------
        auto player1score = ("Player1: " + std::to_string(controller.player1.score));
        auto player2score = ("Player2: " + std::to_string(controller.player2.score));
        int player2_text_offset = MeasureText(player2score.c_str(), score_font);

        int ball_radius = 5;

        float relative_speed = controller.ball.speed / 3.0f;
        if (relative_speed > 1.0f) relative_speed = 1.0f;
        //----------------------------------------------------------------------------------


        // Process game events:
        //   - make sounds, visual effects
        //----------------------------------------------------------------------------------
        if (controller.counter.hit_wall > 0) {
            controller.counter.hit_wall--;
            PlaySound(sounds[Sounds::Boop]);
            ball_radius += 1 + controller.ball.speed;
        }
        if (controller.counter.hit_player > 0) {
            SetSoundPitch(sounds[Sounds::Beep], 0.5f + relative_speed / 2);
            SetSoundPitch(sounds[Sounds::Boop], 0.5f + relative_speed / 2);

            controller.counter.hit_player--;
            PlaySound(sounds[Sounds::Beep]);
            ball_radius += 1 + controller.ball.speed;
        }
        if (controller.counter.hit_goal > 0) {
            controller.counter.hit_goal--;
            PlaySound(sounds[Sounds::Splash]);
            ball_radius += 7;
        }
        //----------------------------------------------------------------------------------


        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);


        if (game_state == GameState::MENU) {
            // Register click button clicks
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                main_menu.register_click(MouseButtonGG::LEFT, GetMousePosition());
            else if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON))
                main_menu.register_click(MouseButtonGG::MIDDLE, GetMousePosition());
            else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                main_menu.register_click(MouseButtonGG::RIGHT, GetMousePosition());

            // Render GUI.
            render_gui(main_menu);
            // TODO: expand gui to work in other game states
        }
        else if (game_state == GameState::GAME) {
            // Draw scores
            DrawText(player1score.c_str(), 50, 50, score_font, RED);
            DrawText(player2score.c_str(), screenSize.x - 50 - player2_text_offset, 50, score_font, BLUE);

            // Draw ball
            DrawCircleV(controller.ball.pos, ball_radius, DARKGREEN);

            // Draw paddles
            DrawRectangle(
                controller.player1.pos.x - controller.player1.size.x / 2,
                controller.player1.pos.y - controller.player1.size.y / 2,
                controller.player1.size.x,
                controller.player1.size.y,
                MAROON
            );
            DrawRectangle(
                controller.player2.pos.x - controller.player2.size.x / 2,
                controller.player2.pos.y - controller.player2.size.y / 2,
                controller.player2.size.x,
                controller.player2.size.y,
                BLUE
            );
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    controller.stop();

    // Unload sounds
    for (auto& sound : sounds) {
        UnloadSound(sound.second);
    }

    CloseAudioDevice();
    CloseWindow();
    //--------------------------------------------------------------------------------------

    return 0;
}


void render_element(std::pair<std::shared_ptr<Element>, Vector2>& element)
{
    auto color = BLACK;

    auto cummulative_position = element.second + element.first->position;

    Vector2 delta = {
        element.first->size.x - MeasureText(element.first->text.c_str(), main_font_size),
        element.first->size.y - main_font_size
    };

    if (element.first->render_style == RenderStyleGG::ROOT)
    {
        // For debugging:
        //DrawRectangleLines(cummulative_position.x, cummulative_position.y, element.first->size.x, element.first->size.y, BLACK);
    }
    else if (element.first->render_style == RenderStyleGG::TEXT)
    {
        DrawText(
            element.first->text.c_str(),
            cummulative_position.x + delta.x / 2,
            cummulative_position.y + delta.y / 2,
            main_font_size,
            element.first->is_focus ? RED : BLACK
        );
    }
    else if (element.first->render_style == RenderStyleGG::TEXT_BORDERS)
    {
        DrawText(
            element.first->text.c_str(),
            cummulative_position.x + delta.x / 2,
            cummulative_position.y + delta.y / 2,
            main_font_size,
            element.first->is_focus ? RED : BLACK
        );
        DrawRectangleLines(cummulative_position.x, cummulative_position.y, element.first->size.x, element.first->size.y, BLACK);
    }
    else if (element.first->render_style == RenderStyleGG::BUTTON)
    {
        DrawRectangle(
            cummulative_position.x, cummulative_position.y,
            element.first->size.x, element.first->size.y,
            LIGHTGRAY
        );

        DrawText(
            element.first->text.c_str(),
            cummulative_position.x + delta.x / 2,
            cummulative_position.y + delta.y / 2,
            main_font_size,
            BLACK
        );
        DrawRectangleLines(cummulative_position.x, cummulative_position.y, element.first->size.x, element.first->size.y, BLACK);
    }
    // .... Rest of the element types
    else
    {
        std::cout << "This specific render style not implemented yet" << std::endl;
    }
}

void render_gui(GGUI& gui)
{
    std::queue<std::pair<std::shared_ptr<Element>, Vector2>> queue;
    std::pair<std::shared_ptr<Element>, Vector2> node =
        std::pair<std::shared_ptr<Element>, Vector2>(gui.root, { 0,0 });

    queue.push(node);

    while (!queue.empty())
    {
        auto next = queue.front();
        queue.pop();

        render_element(next);

        for (auto nn : next.first->children)
        {
            queue.push(std::pair<std::shared_ptr<Element>, Vector2>(nn, next.second + next.first->position));
        }
    }
}


void init_gui(GGUI& main_menu, GameController& controller, GameState& game_state, Vector2d screenSize)
{
    std::string play_bounce_text("PlayBounce!");
    Vector2 hello_size = { MeasureText(play_bounce_text.c_str(), main_font_size) + 20, 60 };

    main_menu.push_child_element(
        main_menu.root,
        std::make_shared<Element>(Element(
            { 50.0f, 50.0f },
            hello_size,
            play_bounce_text,
            RenderStyleGG::TEXT
        ))
    );

    auto start_game_callback = [&controller, &game_state](MouseButtonGG button, Vector2 position)
    {
        if (button == MouseButtonGG::LEFT)
        {
            game_state = GameState::GAME;
            controller.start();
        }
    };

    std::string start_game_text("Start game!");
    auto start_text_width = MeasureText(start_game_text.c_str(), main_font_size);
    Vector2 button_size = { start_text_width + 20, 50 };

    main_menu.push_child_element(
        main_menu.root,
        std::make_shared<Button>(Button(
            { screenSize.x / 2 - button_size.x * 0.5f, screenSize.y / 2 - button_size.y * 0.5f },
            button_size,
            start_game_text,
            start_game_callback
        ))
    );
}
