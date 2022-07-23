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

struct cell
{
  olc::vi2d spritePosition;
  cellContent content;

  cell()
  {
    this->spritePosition = olc::vi2d{0, 0};
    content = cellContent{NOT_SET};
  }

  cell(olc::vi2d position, cellContent content)
  {
    this->spritePosition = position;
    this->content = content;
  }
};

class PGE_tic_tac_toe : public olc::PixelGameEngine
{
private:
  olc::Sprite *cross;
  olc::Sprite *circle;
  olc::Sprite *crossHover;
  olc::Sprite *circleHover;
  int width;
  int height;
  int oneThird; // The distance for one third of the grid or the position of the second cell from the top left of the grid
  int twoThirds;
  int offset; // This makes the cross and circle centered
  cellContent player1Sprite;
  cellContent player2Sprite;
  playMode mode;
  bool playerNeedsToChooseSprite;
  olc::vi2d mouse;
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

    for (cell& cell : cells)
    {
      cell.content = NOT_SET;
    }

    cells[2].content = CROSS;
    cells[4].content = CIRCLE;
    cells[8].content = CROSS;

    Clear(olc::VERY_DARK_CYAN);

    player1Sprite = NOT_SET;

    mode = PvE;

    playerNeedsToChooseSprite = false;

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    Clear(olc::VERY_DARK_CYAN);

    mouse = {GetMouseX(), GetMouseY()};

    // Only allow user input everywhere if the player does not need to choose a sprite
    if (!playerNeedsToChooseSprite)
    {
      HandleModeChange();
    }

    HandleRestart();

    PaintPlayingField();
    PaintUI();
    PaintSprites();

    // TODO: upon switching mode, a pop-up window should appear asking the player (or player 1 to choose their sprite)
    if (playerNeedsToChooseSprite)
    {
      PaintSpriteChoice();
    }

    return true;
  }

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

  // User can restart the game by hitting the restart button
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
    switch (mode)
    {
      case PvE:
        // TODO: difficulty selector between easy (random) or hard (pre-calculated play)
      break;

      case PvP:
        // TODO: show, who's turn it is
      break;
    }
  }

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
    if (
      mouse.x > oneThird + spriteOffset &&
      mouse.y > oneThird + (oneThird / 3) &&
      mouse.x < (oneThird + spriteOffset) + 32 &&
      mouse.y < (oneThird + (oneThird / 3)) + 32)
    {
      SetPixelMode(olc::Pixel::ALPHA);
      DrawSprite(oneThird + spriteOffset, oneThird + (oneThird / 3), circleHover);
      SetPixelMode(olc::Pixel::NORMAL);

      // Player has selected cricle
      if (GetMouse(0).bPressed)
      {
        player1Sprite = CIRCLE;
        player2Sprite = CROSS;

        playerNeedsToChooseSprite = false;
      }
    }

    // Hover over cross
    if (
      mouse.x > height - spriteOffset &&
      mouse.y > oneThird + (oneThird / 3) &&
      mouse.x < (height - spriteOffset) + 32 &&
      mouse.y < (oneThird + (oneThird / 3)) + 32)
    {
      SetPixelMode(olc::Pixel::ALPHA);
      DrawSprite(height - spriteOffset, oneThird + (oneThird / 3), crossHover);
      SetPixelMode(olc::Pixel::NORMAL);

      // Player has selected cross
      if (GetMouse(0).bPressed)
      {
        player1Sprite = CROSS;
        player2Sprite = CIRCLE;

        playerNeedsToChooseSprite = false;
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
