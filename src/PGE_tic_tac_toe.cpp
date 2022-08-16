#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <string>

enum sprite
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
  sprite sprite;
};

struct winCondition
{
  // TODO: for PvP implement "Draw" state
  bool winCondition;
  turn who;
  olc::vi2d lineStart;
  olc::vi2d lineEnd;
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
  sprite player1Sprite;
  sprite player2Sprite;
  playMode mode; // Controls the game-mode
  turn turn; // Controls who's turn it is to place a sprite onto the board
  bool playerNeedsToChooseSprite; // The player needs to choose a sprite
  bool quit = false;
  winCondition winCondition;
  cell cells [9];

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
    srand(time(nullptr));

    winCondition.winCondition = false;

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
    cells[2].spritePosition = olc::vi2d{twoThirds + 1 + offset, offset};
    cells[3].spritePosition = olc::vi2d{offset, oneThird + 1 + offset};
    cells[4].spritePosition = olc::vi2d{oneThird + 1 + offset, oneThird + 1 + offset};
    cells[5].spritePosition = olc::vi2d{twoThirds + 1 + offset, oneThird + 1 + offset};
    cells[6].spritePosition = olc::vi2d{offset, twoThirds + 1 + offset};
    cells[7].spritePosition = olc::vi2d{oneThird + 1 + offset, twoThirds + 1 + offset};
    cells[8].spritePosition = olc::vi2d{twoThirds + 1 + offset, twoThirds + 1 + offset};

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
      cell.sprite = NOT_SET;
    }

    Clear(olc::VERY_DARK_CYAN);

    player1Sprite = NOT_SET;

    mode = PvE;

    playerNeedsToChooseSprite = true;

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    if (player1Sprite == NOT_SET or player2Sprite == NOT_SET)
    {
      playerNeedsToChooseSprite = true;
    }

    Clear(olc::VERY_DARK_CYAN);

    mouse = {GetMouseX(), GetMouseY()};

    if (turn == CPU)
    {
      // TODO: prevent the AI from filling half the field at once
      // TODO: paint the UI correctly when the CPU won
      std::vector<int> availablePositions;

      // Determining all available cells that the AI can place place a sprite on
      for (int i = 0; i < 9; i++)
      {
        // Ignore cells that are already occupied
        if (cells[i].sprite != NOT_SET) continue;

        availablePositions.push_back(i);
      }

      if (not availablePositions.empty())
      {
        // Placing the CPU's sprite on a randomly selected free cell
        cells[rand() % availablePositions.size()].sprite = player2Sprite;
      }

      // After the CPU has placed a sprite it's the player's turn again
      turn == PLAYER_1;
    }

    // As long as the win condition is not met, check it on each frame
    if (not winCondition.winCondition)
    {
      CheckForWinCondition();
    }

    // Only allow user input everywhere if the player does not need to choose a sprite
    if (not playerNeedsToChooseSprite)
    {
      HandleModeChange();
      HandleRestart();

      // Inputs on the playing field are only allowed as long as the game isn't won
      if (not winCondition.winCondition)
      {
        HandleGridInput();
      }

      HandleQuit();
    }

    PaintPlayingField();
    if (not playerNeedsToChooseSprite)
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
        if (mouse.x > height + 40 and mouse.x < height + 68 and mouse.y > 5 and mouse.y < 19)
        {
          FillRect(height + 40, 6, 28, 13, olc::DARK_MAGENTA);

          // Switch to the mode clicked on
          if (GetMouse(0).bPressed)
          {
            mode = PvP;

            playerNeedsToChooseSprite = true;

            for (cell& cell : cells)
            {
              cell.sprite = NOT_SET;
            }
          }
        }
      break;

      case PvP:
        // Highlight PvE field on hover
        if (mouse.x > height + 11 and mouse.x < height + 39 and mouse.y > 5 and mouse.y < 19)
        {
          FillRect(height + 11, 6, 28, 13, olc::DARK_MAGENTA);

          // Switch to the mode clicked on
          if (GetMouse(0).bPressed)
          {
            mode = PvE;

            playerNeedsToChooseSprite = true;

            for (cell& cell : cells)
            {
              cell.sprite = NOT_SET;
            }
          }
        }
      break;
    }
  }

  // User can restart the game by clicking on the restart button
  void HandleRestart()
  {
    // if (mouse.x > height + 10 and mouse.y > 30 and mouse.x < (height + 10) + 57 and mouse.y < 30 + 14)
    // {
    //   FillRect(height + 11, 31, 56, 13, olc::DARK_MAGENTA);

    //   if (GetMouse(0).bPressed)
    //   {
    //     for (cell& cell : cells)
    //     {
    //       cell.sprite = NOT_SET;
    //     }

    //     playerNeedsToChooseSprite = true;
    //   }
    // }

    // TODO: implement this style of function code everywhere
    // A different style of programming: ealry returns in functions instead of nested ifs; makes the code quite a bit cleaner
    // On mouse hover, fill the rectangle with color to indicate hover
    if (mouse.x < height + 10 or mouse.y < 30 or mouse.x > (height + 10) + 57 or mouse.y > 30 + 14) return;

    FillRect(height + 11, 31, 56, 13, olc::DARK_MAGENTA);

    // If the reset button has been clicked, reset the whole playing field and make the player choose a sprite
    if (not GetMouse(0).bPressed) return;

    // Resetting the playing field
    for (cell& cell : cells)
    {
      cell.sprite = NOT_SET;
    }

    winCondition.lineStart = {-1, -1};
    winCondition.lineEnd = {-1, -1};

    playerNeedsToChooseSprite = true;
  }

  void HandleGridInput()
  {
    for (cell& cell : cells)
    {
      // On cell hover
      if (cell.sprite == NOT_SET and mouse.x > cell.cellPosition.x and mouse.y > cell.cellPosition.y and mouse.x <= cell.cellPosition.x + oneThird and mouse.y <= cell.cellPosition.y + oneThird)
      {
        DrawRect(cell.cellPosition.x + 1, cell.cellPosition.y + 1, oneThird - 1, oneThird - 1, olc::DARK_MAGENTA);
        DrawRect(cell.cellPosition.x + 2, cell.cellPosition.y + 2, oneThird - 3, oneThird - 3, olc::DARK_MAGENTA);

        // On left mouse click
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
                  cell.sprite = player1Sprite;

                  turn = PLAYER_2;
                break;

                case PLAYER_2:
                  cell.sprite = player2Sprite;

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
                cell.sprite = player1Sprite;

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
    // Only proceed on mouse hover of the quit button
    if (not (mouse.x > height + 10 and mouse.y > height - 19 and mouse.x < (height + 11) + 30 and mouse.y < (height - 19) + 14)) return;

    FillRect(height + 11, height - 18, 30, 13, olc::DARK_MAGENTA);

    // Only quit on button click
    if (not GetMouse(0).bPressed) return;

    quit = true;
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

    // Filling in the respective button depending on the active mode
    if (mode == PvE)
    {
      FillRect(height + 11, 6, 28, 13, olc::DARK_CYAN);
    }
    else if (mode == PvP)
    {
      FillRect(height + 40, 6, 28, 13, olc::DARK_CYAN);
    }

    DrawStringProp(height + 14, 9, "PvE", (mode == PvE ? olc::WHITE : olc::GREY));
    DrawStringProp(height + 43, 9, "PvP", (mode == PvP ? olc::WHITE : olc::GREY));

    // Restart button
    DrawRect(height + 10, 30, 57, 14, olc::GREY);
    DrawStringProp(height + 14, 34, "Restart", olc::GREY);

    // All other UI elements depend on the mode selected
    // On the respective player's turn the respective UI element is highlighted
    if (not winCondition.winCondition)
    {
      if (mode == PvE)
      {
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

        // TODO: add dificlutly selector for "easy" (= random) and "hard" (= mercyless)
      }
      else if (mode == PvP)
      {
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
      }
    }
    // Win condition has been met, declare winner instead of painting UI
    else
    {
      std::string message;

      if (winCondition.who == PLAYER_1)
      {
        message = "Pl. 1 won";
      }
      else if (winCondition.who == PLAYER_2)
      {
        message = "Pl. 2 won";
      }
      else if (winCondition.who == CPU)
      {
        message = "CPU won";
      }

      DrawStringProp(height + 14, 59, message);
    }

    // Quit button
    DrawRect(height + 10, height - 19, 31, 14, olc::GREY);
    DrawStringProp(height + 14, height - 15, "Quit", olc::GREY);
  }

  // Paints the circles and crosses at their respective positions
  void PaintSprites()
  {
    // Painting the crosses and circles
    SetPixelMode(olc::Pixel::ALPHA);

    // Painting each sprite
    for (const cell& cell : cells)
    {
      if (cell.sprite == CROSS)
      {
        DrawSprite(cell.spritePosition, cross);
      }
      else if (cell.sprite == CIRCLE)
      {
        DrawSprite(cell.spritePosition, circle);
      }
    }

    SetPixelMode(olc::Pixel::NORMAL);

    // If the game has been won, paint a dark magenta line over the winning position
    if (not winCondition.winCondition) return;

    DrawLine(winCondition.lineStart.x, winCondition.lineStart.y, winCondition.lineEnd.x, winCondition.lineEnd.y, olc::MAGENTA);
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

    // TODO: refactor using ealry returns instead of if nesting
    // Hover over circle
    if (mouse.x > oneThird + spriteOffset and mouse.y > oneThird + (oneThird / 3) and mouse.x < (oneThird + spriteOffset) + 32 and mouse.y < (oneThird + (oneThird / 3)) + 32)
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

        winCondition.winCondition = false;
      }
    }

    // TODO: refactor using ealry returns instead of if nesting
    // Hover over cross
    if (mouse.x > height - spriteOffset and mouse.y > oneThird + (oneThird / 3) and mouse.x < (height - spriteOffset) + 32 and mouse.y < (oneThird + (oneThird / 3)) + 32)
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

        winCondition.winCondition = false;
      }
    }
  }

  void CheckForWinCondition()
  {
    // Vertical constellations
    if (CheckCellsForWin(0, 3, 6))
    {
      winCondition.lineStart.x = oneThird / 2;
      winCondition.lineStart.y = 1;
      winCondition.lineEnd.x = oneThird / 2;
      winCondition.lineEnd.y = height - 1;
    }
    else if (CheckCellsForWin(1, 4, 7))
    {
      winCondition.lineStart.x = (oneThird / 2) + oneThird + 1;
      winCondition.lineStart.y = 1;
      winCondition.lineEnd.x = (oneThird / 2) + oneThird + 1;
      winCondition.lineEnd.y = height - 1;
    }
    else if (CheckCellsForWin(2, 5, 8))
    {
      winCondition.lineStart.x = (oneThird / 2) + twoThirds + 1;
      winCondition.lineStart.y = 1;
      winCondition.lineEnd.x = (oneThird / 2) + twoThirds + 1;
      winCondition.lineEnd.y = height - 1;
    }
    // Horizontal constellations
    else if (CheckCellsForWin(0, 1, 2))
    {
      winCondition.lineStart.x = 1;
      winCondition.lineStart.y = oneThird / 2;
      winCondition.lineEnd.x = height - 1;
      winCondition.lineEnd.y = oneThird / 2;
    }
    else if (CheckCellsForWin(3, 4, 5))
    {
      winCondition.lineStart.x = 1;
      winCondition.lineStart.y = (oneThird / 2) + oneThird + 1;
      winCondition.lineEnd.x = height - 1;
      winCondition.lineEnd.y = (oneThird / 2) + oneThird + 1;
    }
    else if (CheckCellsForWin(6, 7, 8))
    {
      winCondition.lineStart.x = 1;
      winCondition.lineStart.y = (oneThird / 2) + twoThirds + 1;
      winCondition.lineEnd.x = height - 1;
      winCondition.lineEnd.y = (oneThird / 2) + twoThirds + 1;
    }
    // Diagonal constellations
    else if (CheckCellsForWin(0, 4, 8))
    {
      winCondition.lineStart.x = 1;
      winCondition.lineStart.y = 1;
      winCondition.lineEnd.x = height - 1;
      winCondition.lineEnd.y = height - 1;
    }
    else if (CheckCellsForWin(2, 4, 6))
    {
      winCondition.lineStart.x = height - 1;
      winCondition.lineStart.y = 1;
      winCondition.lineEnd.x = 1;
      winCondition.lineEnd.y = height - 1;
    }
  }

  bool CheckCellsForWin(int one, int two, int three)
  {
    // If any of the three given grid positions have the same sprite
    if (not ((cells[one].sprite == CROSS and cells[two].sprite == CROSS and cells[three].sprite == CROSS) or (cells[one].sprite == CIRCLE and cells[two].sprite == CIRCLE and cells[three].sprite == CIRCLE))) return false;

    winCondition.winCondition = true;

    // Deciding the winner based on who's turn it is right now
    if (mode == PvP)
    {
      if (turn == PLAYER_2 or turn == CPU)
      {
        winCondition.who = PLAYER_1;
      }
      else if (turn == PLAYER_1)
      {
        winCondition.who = PLAYER_2;
      }
    }
    else if (mode == PvE)
    {
      if (turn == PLAYER_1)
      {
        winCondition.who = CPU;
      }
      else if (turn == CPU)
      {
        winCondition.who = PLAYER_1;
      }
    }

    return true;
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
