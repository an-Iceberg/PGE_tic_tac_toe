#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

enum cellContent
{
  NOT_SET,
  CROSS,
  CIRCLE
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
  int oneThird;
  int twoThirds;
  olc::vi2d mouse;
  cell cells [9];

  // The field width/height is the same as the window height

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

    // The sprites are 32x32px
    cross = new olc::Sprite("./sprites/cross.png");
    circle = new olc::Sprite("./sprites/circle.png");

    // TODO: find a position that centers the sprites
    cells[0].spritePosition = olc::vi2d{1, 1};
    cells[1].spritePosition = olc::vi2d{oneThird + 2, 1};
    cells[2].spritePosition = olc::vi2d{twoThirds + 2, 1};

    cells[3].spritePosition = olc::vi2d{1, oneThird + 2};
    cells[4].spritePosition = olc::vi2d{oneThird + 2, oneThird + 2};
    cells[5].spritePosition = olc::vi2d{twoThirds + 2, oneThird + 2};

    cells[6].spritePosition = olc::vi2d{1, twoThirds + 2};
    cells[7].spritePosition = olc::vi2d{oneThird + 2, twoThirds + 2};
    cells[8].spritePosition = olc::vi2d{twoThirds + 2, twoThirds + 2};

    for (cell& cell : cells)
    {
      cell.content = cellContent{NOT_SET};
    }

    Clear(olc::VERY_DARK_CYAN);

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override
  {
    Clear(olc::VERY_DARK_CYAN);

    mouse = {GetMouseX(), GetMouseY()};

    // Dawing the playing field
    DrawRect(0, 0, height, height);
    DrawLine(oneThird + 1, 1, oneThird + 1, height - 1);
    DrawLine(twoThirds + 1, 1, twoThirds + 1, height - 1);
    DrawLine(1, oneThird + 1, height - 1, oneThird + 1);
    DrawLine(1, twoThirds + 1, height - 1, twoThirds + 1);

    DrawStringProp(height + 10, 5, "Hello World");

    // Drawing the crosses and circles
    SetPixelMode(olc::Pixel::ALPHA);
    for (int i = 0; i < 9; i++)
    {
      DrawSprite(cells[i].spritePosition, cross);
    }
    SetPixelMode(olc::Pixel::NORMAL);

    return true;
  }

private:
};

int main()
{
  PGE_tic_tac_toe demo;

  if (demo.Construct(300, 200, 2, 2))
  {
    demo.Start();
  }

  return 0;
}
