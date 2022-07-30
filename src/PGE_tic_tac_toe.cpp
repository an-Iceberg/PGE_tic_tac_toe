#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

enum cellContent
{
  NOT_SET,
  CROSS,
  CIRCLE
};

enum playMode
{
  PvE,
  PvP
};

enum turn
{
  PLAYER_1,
  PLAYER_2,
  CPU
};

struct cell
{
  olc::vi2d cellPosition;
  olc::vi2d spritePosition;
  cellContent content;
};

class PGE_tic_tac_toe : public olc::PixelGameEngine
{
private:
  olc::Sprite *cross;
  olc::Sprite *circle;
  olc::Sprite *crossHover;
  olc::Sprite *circleHover;
  olc::vi2d mouse;
  int width; // Screen width
  int height; // Screen height
  int oneThird; // The distance for one third of the grid or the position of the second cell from the top left of the grid
  int twoThirds;
  int offset; // This makes the cross and circle centered
  int distances[3];
  cellContent player1Sprite;
  cellContent player2Sprite;
  playMode mode; // Controls the game-mode
  turn turn; // Controls who's turn it is to place a sprite onto the board
  bool playerNeedsToChooseSprite; // The player needs to choose a sprite
  bool quit = false;
  bool winCondition = false;
  cell cells [9];

  olc::Pixel orange = olc::Pixel(255, 128, 0);

  // The field width/height is the same as the window height
  // Formula for height/distance relation: y = 6x + 94

public:
  PGE_tic_tac_toe()
  {
    sAppName = "Tic Tac Toe";
  }

public:
  bool OnUserCreate() override
  {
    width = ScreenWidth();
    height = ScreenHeight() - 1;

    oneThird = height * 0.33;
    twoThirds = oneThird * 2;
    twoThirds++;

    // Calculating the offset by modifying the above formula to x = (y - 94) / 6
    offset = (height + 1 - 94) / 6;

    // The sprites are 32x32px
    cross = new olc::Sprite("./sprites/cross.png");
    circle = new olc::Sprite("./sprites/circle.png");
    crossHover = new olc::Sprite("./sprites/cross_hover.png");
    circleHover = new olc::Sprite("./sprites/circle_hover.png");

    cells[0].spritePosition = olc::vi2d{offset, offset};
    cells[1].spritePosition = olc::vi2d{oneThird + 1 + offset, offset};
    cells[2].spritePosition = olc::vi2d{twoThirds + 2 + offset, offset};
    cells[3].spritePosition = olc::vi2d{offset, oneThird + 1 + offset};
    cells[4].spritePosition = olc::vi2d{oneThird + 1 + offset, oneThird + 1 + offset};
    cells[5].spritePosition = olc::vi2d{twoThirds + 2 + offset, oneThird + 1 + offset};
    cells[6].spritePosition = olc::vi2d{offset, twoThirds + 2 + offset};
    cells[7].spritePosition = olc::vi2d{oneThird + 1 + offset, twoThirds + 2 + offset};
    cells[8].spritePosition = olc::vi2d{twoThirds + 2 + offset, twoThirds + 2 + offset};

    cells[0].cellPosition = olc::vi2d{0, 0};
    cells[1].cellPosition = olc::vi2d{oneThird + 1, 0};
    cells[2].cellPosition = olc::vi2d{twoThirds + 1, 0};
    cells[3].cellPosition = olc::vi2d{0, oneThird + 1};
    cells[4].cellPosition = olc::vi2d{oneThird + 1, oneThird + 1};
    cells[5].cellPosition = olc::vi2d{twoThirds + 1, oneThird + 1};
    cells[6].cellPosition = olc::vi2d{0, twoThirds + 1};
    cells[7].cellPosition = olc::vi2d{oneThird + 1, twoThirds + 1};
    cells[8].cellPosition = olc::vi2d{twoThirds + 1, twoThirds + 1};

    for (cell& cell : cells)
    {
      cell.content = NOT_SET;
    }

    Clear(olc::VERY_DARK_CYAN);

    player1Sprite = NOT_SET;

    mode = PvE;

    playerNeedsToChooseSprite = true;

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    if (player1Sprite == NOT_SET || player2Sprite == NOT_SET)
    {
      playerNeedsToChooseSprite = true;
    }

    Clear(olc::VERY_DARK_CYAN);

    mouse = {GetMouseX(), GetMouseY()};

    if (turn == CPU)
    {
      // TODO: implement AI here
    }

    // Only allow user input everywhere if the player does not need to choose a sprite
    if (!playerNeedsToChooseSprite)
    {
      HandleModeChange();
      HandleRestart();

      // TODO: determine when the win condition is met
      // Inputs on the playing field are only allowed as long as the game isn't won
      if (!winCondition)
      {
        HandleGridInput();
      }

      HandleQuit();
    }

    PaintPlayingField();
    if (!playerNeedsToChooseSprite)
    {
      PaintUI();
    }
    PaintSprites();

    if (playerNeedsToChooseSprite)
    {
      PaintSpriteChoice();
    }

    return !quit;
  }


  // ---------------------------


private:
  void HandleModeChange()
  {
    // If a play mode is active only the other one is selectable
    switch (mode)
    {
      case PvE:
        // Highlight PvP field on hover
        if (mouse.x > height + 40 && mouse.x < height + 68 && mouse.y > 5 && mouse.y < 19)
        {
          FillRect(height + 40, 6, 28, 13, olc::DARK_MAGENTA);

          // Switch to the mode clicked on
          if (GetMouse(0).bPressed)
          {
            mode = PvP;

            playerNeedsToChooseSprite = true;

            for (cell& cell : cells)
            {
              cell.content = NOT_SET;
            }
          }
        }
      break;

      case PvP:
        // Highlight PvE field on hover
        if (mouse.x > height + 11 && mouse.x < height + 39 && mouse.y > 5 && mouse.y < 19)
        {
          FillRect(height + 11, 6, 28, 13, olc::DARK_MAGENTA);

          // Switch to the mode clicked on
          if (GetMouse(0).bPressed)
          {
            mode = PvE;

            playerNeedsToChooseSprite = true;

            for (cell& cell : cells)
            {
              cell.content = NOT_SET;
            }
          }
        }
      break;
    }
  }

  // User can restart the game by clicking on the restart button
  void HandleRestart()
  {
    if (mouse.x > height + 10 && mouse.y > 30 && mouse.x < (height + 10) + 57 && mouse.y < 30 + 14)
    {
      FillRect(height + 11, 31, 56, 13, olc::DARK_MAGENTA);

      if (GetMouse(0).bPressed)
      {
        for (cell& cell : cells)
        {
          cell.content = NOT_SET;
        }

        playerNeedsToChooseSprite = true;
      }
    }
  }

  // TODO: rename this function
  void HandleGridInput()
  {
    for (cell& cell : cells)
    {
      // On cell hover
      if (cell.content == NOT_SET && mouse.x > cell.cellPosition.x && mouse.y > cell.cellPosition.y && mouse.x <= cell.cellPosition.x + oneThird && mouse.y <= cell.cellPosition.y + oneThird)
      {
        DrawRect(cell.cellPosition.x + 1, cell.cellPosition.y + 1, oneThird - 1, oneThird - 1, olc::DARK_MAGENTA);
        DrawRect(cell.cellPosition.x + 2, cell.cellPosition.y + 2, oneThird - 3, oneThird - 3, olc::DARK_MAGENTA);

        // FIX: input does not have any action
        if (GetMouse(0).bPressed)
        {
          // In PvP change player turn after a sprite has been placed
          switch (mode)
          {
            case PvP:
              // Alternating between the two players
              switch (turn)
              {
                case PLAYER_1:
                  cell.content = player1Sprite;

                  turn = PLAYER_2;
                break;

                case PLAYER_2:
                  cell.content = player2Sprite;

                  turn = PLAYER_1;
                break;

                case CPU:
                  turn = PLAYER_1;
                break;
              }
            break;

            case PvE:
              // Alternating between the player and CPU
              if (turn != CPU)
              {
                cell.content = player1Sprite;

                turn = CPU;
              }
              // It's the CPU's turn
              else
              {
                turn = PLAYER_1;
              }
            break;
          }
        }
      }
    }
  }

  // If the player clicks the quit button, the program exist
  void HandleQuit()
  {
    if (mouse.x > height + 10 && mouse.y > height - 19 && mouse.x < (height + 11) + 30 && mouse.y < (height - 19) + 14)
    {
      FillRect(height + 11, height - 18, 30, 13, olc::DARK_MAGENTA);

      if (GetMouse(0).bPressed)
      {
        quit = true;
      }
    }
  }

  void PaintPlayingField()
  {
    // Dawing the playing field
    DrawRect(0, 0, height, height, olc::GREY);
    DrawLine(oneThird + 1, 1, oneThird + 1, height - 1, olc::GREY);
    DrawLine(twoThirds + 1, 1, twoThirds + 1, height - 1, olc::GREY);
    DrawLine(1, oneThird + 1, height - 1, oneThird + 1, olc::GREY);
    DrawLine(1, twoThirds + 1, height - 1, twoThirds + 1, olc::GREY);
  }

  void PaintUI()
  {
    // Drawing mode switcher (mode switcher is always present)
    DrawRect(height + 10, 5, width - (height + 20), 14, olc::GREY);
    DrawLine(width - ((width - height) / 2), 6, width - ((width - height) / 2), 18, olc::GREY);

    switch (mode)
    {
      case PvE:
        FillRect(height + 11, 6, 28, 13, olc::DARK_CYAN);
      break;

      case PvP:
        FillRect(height + 40, 6, 28, 13, olc::DARK_CYAN);
      break;
    }

    DrawStringProp(height + 14, 9, "PvE", (mode == PvE ? olc::WHITE : olc::GREY));
    DrawStringProp(height + 43, 9, "PvP", (mode == PvP ? olc::WHITE : olc::GREY));

    // Restart button
    DrawRect(height + 10, 30, 57, 14, olc::GREY);
    DrawStringProp(height + 14, 34, "Restart", olc::GREY);

    // All other UI elements depend on the mode selected
    // On the respective player's turn the respective UI element is highlighted
    switch (mode)
    {
      case PvE:
        // Player 1's turn
        if (turn == PLAYER_1)
        {
          DrawRect(height + 10, 55, 56, 14);
          FillRect(height + 11, 56, 55, 13, olc::DARK_CYAN);
        }
        DrawStringProp(height + 14, 59, "Pl. 1", (turn == PLAYER_1 ? olc::WHITE : olc::GREY));
        DrawStringProp(height + 56, 59, (player1Sprite == CROSS ? "X" : "O"), (turn == PLAYER_1 ? olc::WHITE : olc::GREY));

        // CPU's turn
        if (turn == CPU)
        {
          DrawRect(height + 10, 80, 56, 14);
          FillRect(height + 11, 81, 55, 13, olc::DARK_CYAN);
        }
        DrawStringProp(height + 14, 84, "CPU", (turn == CPU ? olc::WHITE : olc::GREY));
        DrawStringProp(height + 56, 84, (player1Sprite == CROSS ? "O" : "X"), (turn == CPU ? olc::WHITE : olc::GREY));
      break;

      case PvP:
        // Player 1's turn
        if (turn == PLAYER_1)
        {
          DrawRect(height + 10, 55, 56, 14);
          FillRect(height + 11, 56, 55, 13, olc::DARK_CYAN);
        }
        DrawStringProp(height + 14, 59, "Pl. 1", (turn == PLAYER_1 ? olc::WHITE : olc::GREY));
        DrawStringProp(height + 56, 59, (player1Sprite == CROSS ? "X" : "O"), (turn == PLAYER_1 ? olc::WHITE : olc::GREY));

        // Player 2's turn
        if (turn == PLAYER_2)
        {
          DrawRect(height + 10, 80, 56, 14);
          FillRect(height + 11, 81, 55, 13, olc::DARK_CYAN);
        }
        DrawStringProp(height + 14, 84, "Pl. 2", (turn == PLAYER_2 ? olc::WHITE : olc::GREY));
        DrawStringProp(height + 56, 84, (player2Sprite == CROSS ? "X" : "O"), (turn == PLAYER_2 ? olc::WHITE : olc::GREY));
      break;
    }

    // TODO: adjust padding to be 2px
    // Quit button
    DrawRect(height + 10, height - 19, 31, 14, olc::GREY);
    DrawStringProp(height + 14, height - 15, "Quit", olc::GREY);
  }

  // Paints the circles and crosses at their respective positions
  void PaintSprites()
  {
    // Drawing the crosses and circles
    SetPixelMode(olc::Pixel::ALPHA);

    for (const cell& cell : cells)
    {
      switch (cell.content)
      {
        case CROSS:
          DrawSprite(cell.spritePosition, cross);
        break;

        case CIRCLE:
          DrawSprite(cell.spritePosition, circle);
        break;

        default:
        break;
      }
    }

    SetPixelMode(olc::Pixel::NORMAL);
  }

  // Paints a pop-up dialogue, allowing the player to choose their preferred sprite
  void PaintSpriteChoice()
  {
    int shadowOffset = 3;
    int spriteOffset = 15;

    // Shadow
    SetPixelMode(olc::Pixel::ALPHA);
    FillRect((oneThird / 2) + shadowOffset, (oneThird / 2) + shadowOffset, (width - oneThird) + shadowOffset, (twoThirds) + shadowOffset, olc::Pixel(0, 0, 0, 128));
    SetPixelMode(olc::Pixel::NORMAL);

    FillRect(oneThird / 2, oneThird / 2, width - oneThird, twoThirds, olc::VERY_DARK_CYAN);

    // Border
    DrawRect(oneThird / 2, oneThird / 2, width - oneThird, twoThirds, olc::DARK_CYAN);

    // Text
    DrawStringProp(oneThird - 4, oneThird - 10, "Please choose a sprite:");

    // Sprites
    SetPixelMode(olc::Pixel::ALPHA);
    DrawSprite(oneThird + spriteOffset, oneThird + (oneThird / 3), circle);
    DrawSprite(height - spriteOffset, oneThird + (oneThird / 3), cross);
    SetPixelMode(olc::Pixel::NORMAL);

    // Hover over circle
    if (mouse.x > oneThird + spriteOffset && mouse.y > oneThird + (oneThird / 3) && mouse.x < (oneThird + spriteOffset) + 32 && mouse.y < (oneThird + (oneThird / 3)) + 32)
    {
      SetPixelMode(olc::Pixel::ALPHA);
      DrawSprite(oneThird + spriteOffset, oneThird + (oneThird / 3), circleHover);
      SetPixelMode(olc::Pixel::NORMAL);

      // Player has selected cricle
      if (GetMouse(0).bPressed)
      {
        player1Sprite = CIRCLE;
        player2Sprite = CROSS;

        turn = PLAYER_1;

        playerNeedsToChooseSprite = false;

        winCondition = false;
      }
    }

    // Hover over cross
    if (mouse.x > height - spriteOffset && mouse.y > oneThird + (oneThird / 3) && mouse.x < (height - spriteOffset) + 32 && mouse.y < (oneThird + (oneThird / 3)) + 32)
    {
      SetPixelMode(olc::Pixel::ALPHA);
      DrawSprite(height - spriteOffset, oneThird + (oneThird / 3), crossHover);
      SetPixelMode(olc::Pixel::NORMAL);

      // Player has selected cross
      if (GetMouse(0).bPressed)
      {
        player1Sprite = CROSS;
        player2Sprite = CIRCLE;

        turn = PLAYER_1;

        playerNeedsToChooseSprite = false;

        winCondition = false;
      }
    }
  }
};

int main()
{
  PGE_tic_tac_toe demo;

  // Suitable y heights are 94 + (6 * n) where n is any whole number
  int n = 10;
  int y = 94 + (6 * n);
  int x = y / 2 * 3;

  if (demo.Construct(x, y, 2, 2))
  {
    demo.Start();
  }

  return 0;
}
