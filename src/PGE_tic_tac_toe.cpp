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
  int width;
  int height;
  int oneThird; // The distance for one third of the grid or the position of the second cell from the top left of the grid
  int twoThirds;
  int offset; // This makes the cross and circle centered
  cellContent player1Sprite;
  playMode mode;
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

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    Clear(olc::VERY_DARK_CYAN);

    mouse = {GetMouseX(), GetMouseY()};

    PaintPlayingField();
    PaintUI();
    PaintSprites();

    return true;
  }

private:
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
    DrawRect(height + 10, 5, width - (height + 20), 15, olc::GREY);
    DrawLine(width - ((width - height) / 2), 6, width - ((width - height) / 2), 19, olc::GREY);

    switch (mode)
    {
      case PvE:
        FillRect(height + 11, 6, 28, 14, olc::DARK_CYAN);
      break;

      case PvP:
        FillRect(height + 11 + 28 + 1, 6, 28, 14, olc::DARK_CYAN);
      break;
    }

    DrawStringProp(height + 14, 9, "PvE", (mode == PvE ? olc::WHITE : olc::GREY));
    DrawStringProp(height + 43, 9, "PvP", (mode == PvP ? olc::WHITE : olc::GREY));
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
