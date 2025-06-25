#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <sstream>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

enum ShapeType { SQUARE, CIRCLE, TRIANGLE, GROUND, NONE };
ShapeType currentShape = NONE;

enum BuildType { SQUARETYPE, CIRCLETYPE, TRIANGLETYPE, AUTOSTC, AUTOCST, NONET };
BuildType currentType = NONET;

char whichFigure[10] = { 'N','N','N','N','N','N','N','N','N','N' };

struct AnimationState {
    int xPos = 230;
    int yPos = 200;
    int width = 2;
    int height = 60;
    int speed = 5;
    int widthPoint = 3;
    int heightPoint = 3;
    Color colorPoint = Color(255, 255, 0, 0);
    Color color = Color(255, 0, 0, 0);
    bool moveUp = false;
    bool moveDown = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool moveElement = false;
    int pickedSquareIndex = -1;
    int pickedCircleIndex = -1;
    int pickedTriangleIndex = -1;
} animState;

struct MainPart {
    int xPos1 = 200;
    int yPos1 = 200;
    int width1 = 3;
    int height1 = 400;

    int xPos2 = 220;
    int yPos2 = 200;
    int width2 = 3;
    int height2 = 400;

    int xPos3 = 50;
    int yPos3 = 200;
    int width3 = 550;
    int height3 = 3;

    int xPos4 = 200;
    int yPos4 = 600;
    int width4 = 20;
    int height4 = 3;

    int xPos5 = 0;
    int yPos5 = 600;
    int width5 = 1000;
    int height5 = 50;

    Color color1 = Color(255, 255, 165, 0);
    Color color2 = Color(255, 255, 165, 0);
    Color color3 = Color(255, 255, 165, 0);
    Color color4 = Color(255, 255, 165, 0);
    Color color5 = Color(255, 0, 0, 0);
} MainPartState;

struct Square {
    int xPos[10] = { 240, 270, 300, 330, 360, 390, 420, 450, 480, 510 };
    int yPos[10] = { 580, 580, 580, 580, 580, 580, 580, 580, 580, 580 };
    int width = 20;
    int height = 20;
    Color color = Color(255, 0, 255, 0);
    float mass[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
} SquareState;

struct Circle {
    int xPos[10] = { 240, 270, 300, 330, 360, 390, 420, 450, 480, 510 };
    int yPos[10] = { 580, 580, 580, 580, 580, 580, 580, 580, 580, 580 };
    int width = 20;
    int height = 20;
    Color color = Color(255, 255, 0, 0);
    float mass[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
} CircleState;

struct Triangle {
    int xPos[10] = { 240, 270, 300, 330, 360, 390, 420, 450, 480, 510 };
    int yPos[10] = { 580, 580, 580, 580, 580, 580, 580, 580, 580, 580 };
    int width = 20;
    int height = 20;
    Color color = Color(255, 0, 0, 255);
    float mass[10] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f };
} TriangleState;

bool bulion = true;
int figureCounter = 0;
int figurePos[10] = { 0 };
bool isPickedUp = false;
bool showMassInput = false;
float newMass = 1.0f;
WCHAR massInput[32] = L"";

bool CheckCollision(int x, int y, int width, int height, int ignoreIndex = -1, ShapeType shapeType = SQUARE) {
    for (int i = 0; i < figureCounter; ++i) {
        if (i == ignoreIndex) continue;

        if (whichFigure[i] == 'S') {
            if (x < SquareState.xPos[i] + SquareState.width &&
                x + width > SquareState.xPos[i] &&
                y < SquareState.yPos[i] + SquareState.height &&
                y + height > SquareState.yPos[i]) {
                return true;
            }
        }
        else if (whichFigure[i] == 'C') {
            // Simple circle collision using bounding box for simplicity
            if (x < CircleState.xPos[i] + CircleState.width &&
                x + width > CircleState.xPos[i] &&
                y < CircleState.yPos[i] + CircleState.height &&
                y + height > CircleState.yPos[i]) {
                return true;
            }
        }
        else if (whichFigure[i] == 'T') {
            if (x < TriangleState.xPos[i] + TriangleState.width &&
                x + width > TriangleState.xPos[i] &&
                y < TriangleState.yPos[i] + TriangleState.height &&
                y + height > TriangleState.yPos[i]) {
                return true;
            }
        }
    }
    return false;
}

bool CheckLineCollision(int lineX, int lineY, int lineHeight) {
    for (int i = 0; i < figureCounter; ++i) {
        if (whichFigure[i] == 'S') {
            if (lineX < SquareState.xPos[i] + SquareState.width &&
                lineX + animState.width > SquareState.xPos[i] &&
                lineY < SquareState.yPos[i] + SquareState.height &&
                lineY + lineHeight > SquareState.yPos[i]) {
                return true;
            }
        }
        else if (whichFigure[i] == 'C') {
            if (lineX < CircleState.xPos[i] + CircleState.width &&
                lineX + animState.width > CircleState.xPos[i] &&
                lineY < CircleState.yPos[i] + CircleState.height &&
                lineY + lineHeight > CircleState.yPos[i]) {
                return true;
            }
        }
        else if (whichFigure[i] == 'T') {
            if (lineX < TriangleState.xPos[i] + TriangleState.width &&
                lineX + animState.width > TriangleState.xPos[i] &&
                lineY < TriangleState.yPos[i] + TriangleState.height &&
                lineY + lineHeight > TriangleState.yPos[i]) {
                return true;
            }
        }
    }
    return false;
}

void DrawTriangle(Graphics& graphics, int x, int y, int width, int height, const Color& color) {
    Point points[3] = {
        Point(x, y + height),
        Point(x + width / 2, y),
        Point(x + width, y + height)
    };
    SolidBrush brush(color);
    graphics.FillPolygon(&brush, points, 3);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void DrawButtons(HWND hWnd, Graphics& graphics) {
    FontFamily fontFamily(L"Arial");
    Font font(&fontFamily, 12, FontStyleBold, UnitPixel);
    SolidBrush textBrush(Color(255, 255, 255, 255));
    Pen borderPen(Color(255, 0, 0, 0), 1);

    RectF squareBtn(10, 10, 80, 30);
    SolidBrush squareBtnBrush(currentShape == SQUARE ? Color(255, 30, 100, 150) : Color(255, 70, 130, 180));
    graphics.FillRectangle(&squareBtnBrush, squareBtn);
    graphics.DrawRectangle(&borderPen, squareBtn);
    graphics.DrawString(L"Square", -1, &font, PointF(15, 15), &textBrush);

    RectF circleBtn(100, 10, 80, 30);
    SolidBrush circleBtnBrush(currentShape == CIRCLE ? Color(255, 180, 20, 60) : Color(255, 220, 20, 60));
    graphics.FillRectangle(&circleBtnBrush, circleBtn);
    graphics.DrawRectangle(&borderPen, circleBtn);
    graphics.DrawString(L"Circle", -1, &font, PointF(115, 15), &textBrush);

    RectF triangleBtn(190, 10, 80, 30);
    SolidBrush triangleBtnBrush(currentShape == TRIANGLE ? Color(255, 0, 100, 0) : Color(255, 34, 139, 34));
    graphics.FillRectangle(&triangleBtnBrush, triangleBtn);
    graphics.DrawRectangle(&borderPen, triangleBtn);
    graphics.DrawString(L"Triangle", -1, &font, PointF(195, 15), &textBrush);

    RectF clearBtn(280, 10, 80, 30);
    SolidBrush clearBtnBrush(Color(255, 0, 100, 0));
    graphics.FillRectangle(&clearBtnBrush, clearBtn);
    graphics.DrawRectangle(&borderPen, clearBtn);
    graphics.DrawString(L"Clear", -1, &font, PointF(285, 15), &textBrush);

    // Pole do wprowadzania masy
    RectF massInputRect(370, 10, 80, 30);
    SolidBrush massInputBrush(Color(255, 255, 255, 255));
    graphics.FillRectangle(&massInputBrush, massInputRect);
    graphics.DrawRectangle(&borderPen, massInputRect);

    FontFamily massFontFamily(L"Arial");
    Font massFont(&massFontFamily, 12, FontStyleRegular, UnitPixel);
    SolidBrush massTextBrush(Color(255, 0, 0, 0));

    std::wstringstream labelStream;
    labelStream << L"Masa: " << newMass << L" kg";
    graphics.DrawString(labelStream.str().c_str(), -1, &massFont, PointF(375, 15), &massTextBrush);

    RectF squareTypeBtn(460, 10, 80, 30);
    SolidBrush squareTypeBtnBrush(Color(255, 0, 100, 100));
    graphics.FillRectangle(&squareTypeBtnBrush, squareTypeBtn);
    graphics.DrawRectangle(&borderPen, squareTypeBtn);
    graphics.DrawString(L"S-Tower", -1, &font, PointF(465, 15), &textBrush);

    RectF circleTypeBtn(550, 10, 80, 30);
    SolidBrush circleTypeBtnBrush(Color(255, 100, 100, 100));
    graphics.FillRectangle(&circleTypeBtnBrush, circleTypeBtn);
    graphics.DrawRectangle(&borderPen, circleTypeBtn);
    graphics.DrawString(L"C-Tower", -1, &font, PointF(555, 15), &textBrush);

    RectF triangleTypeBtn(640, 10, 80, 30);
    SolidBrush triangleTypeBtnBrush(Color(255, 100, 100, 0));
    graphics.FillRectangle(&triangleTypeBtnBrush, triangleTypeBtn);
    graphics.DrawRectangle(&borderPen, triangleTypeBtn);
    graphics.DrawString(L"T-Tower", -1, &font, PointF(645, 15), &textBrush);


}

ShapeType GetShapeFromPixelColor(HDC hdc, int x, int y) {
    const int tolerance = 40;
    COLORREF pixel = GetPixel(hdc, x, y);

    int r = GetRValue(pixel);
    int g = GetGValue(pixel);
    int b = GetBValue(pixel);

    if (g > r + tolerance && g > b + tolerance) return SQUARE;
    else if (r > g + tolerance && r > b + tolerance) return CIRCLE;
    else if (b > r + tolerance && b > g + tolerance) return TRIANGLE;
    else if (r == 0 && g == 0 && b == 0) return GROUND;

    return NONE;
}

void OnPaint(HWND hWnd, bool& bulion) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // Utwórz buforowany kontekst urządzenia
    HDC hdcMem = CreateCompatibleDC(hdc);
    RECT rect;
    GetClientRect(hWnd, &rect);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    Graphics graphics(hdcMem);

    // Tło
    SolidBrush bgBrush(Color(255, 240, 240, 240));
    graphics.FillRectangle(&bgBrush, 0, 0, rect.right, rect.bottom);

    SolidBrush mainPartBrush1(MainPartState.color1);
    graphics.FillRectangle(&mainPartBrush1, MainPartState.xPos1, MainPartState.yPos1, MainPartState.width1, MainPartState.height1);

    SolidBrush mainPartBrush2(MainPartState.color2);
    graphics.FillRectangle(&mainPartBrush2, MainPartState.xPos2, MainPartState.yPos2, MainPartState.width2, MainPartState.height2);

    SolidBrush mainPartBrush3(MainPartState.color3);
    graphics.FillRectangle(&mainPartBrush3, MainPartState.xPos3, MainPartState.yPos3, MainPartState.width3, MainPartState.height3);

    SolidBrush mainPartBrush4(MainPartState.color4);
    graphics.FillRectangle(&mainPartBrush4, MainPartState.xPos4, MainPartState.yPos4, MainPartState.width4, MainPartState.height4);

    SolidBrush mainPartBrush5(MainPartState.color5);
    graphics.FillRectangle(&mainPartBrush5, MainPartState.xPos5, MainPartState.yPos5, MainPartState.width5, MainPartState.height5);

    bool lineCollision = CheckLineCollision(animState.xPos, animState.yPos, animState.height);
    if (lineCollision) {
        animState.speed = 0;
        animState.height = animState.height - 5;
    }
    else {
        animState.speed = 5;
    }
    SolidBrush lineBrush(animState.color);
    graphics.FillRectangle(&lineBrush, animState.xPos, animState.yPos, animState.width, animState.height);

    SolidBrush mainPartBrushPoint(animState.colorPoint);
    graphics.FillRectangle(&mainPartBrushPoint, animState.xPos, animState.yPos, animState.widthPoint, animState.heightPoint);

    for (int i = 0; i < figureCounter; ++i) {
        if (whichFigure[i] == 'S') {
            SolidBrush squareBrush(SquareState.color);
            graphics.FillRectangle(&squareBrush, SquareState.xPos[i], SquareState.yPos[i], SquareState.width, SquareState.height);

            FontFamily fontFamily(L"Arial");
            Font font(&fontFamily, 10, FontStyleRegular, UnitPixel);
            SolidBrush textBrush(Color(255, 0, 0, 0));

            std::wstringstream massStream;
            massStream << SquareState.mass[i];
            std::wstring massText = massStream.str();

            graphics.DrawString(
                massText.c_str(),
                -1,
                &font,
                PointF(SquareState.xPos[i] + 5, SquareState.yPos[i] + 5),
                &textBrush
            );
        }
        else if (whichFigure[i] == 'C') {
            SolidBrush circleBrush(CircleState.color);
            graphics.FillEllipse(&circleBrush, CircleState.xPos[i], CircleState.yPos[i], CircleState.width, CircleState.height);

            FontFamily fontFamily(L"Arial");
            Font font(&fontFamily, 10, FontStyleRegular, UnitPixel);
            SolidBrush textBrush(Color(255, 0, 0, 0));

            std::wstringstream massStream;
            massStream << CircleState.mass[i];
            std::wstring massText = massStream.str();

            graphics.DrawString(
                massText.c_str(),
                -1,
                &font,
                PointF(CircleState.xPos[i] + 5, CircleState.yPos[i] + 5),
                &textBrush
            );
        }
        else if (whichFigure[i] == 'T') {
            DrawTriangle(graphics, TriangleState.xPos[i], TriangleState.yPos[i], TriangleState.width, TriangleState.height, TriangleState.color);

            FontFamily fontFamily(L"Arial");
            Font font(&fontFamily, 10, FontStyleRegular, UnitPixel);
            SolidBrush textBrush(Color(255, 0, 0, 0));

            std::wstringstream massStream;
            massStream << TriangleState.mass[i];
            std::wstring massText = massStream.str();

            graphics.DrawString(
                massText.c_str(),
                -1,
                &font,
                PointF(TriangleState.xPos[i] + 5, TriangleState.yPos[i] + 5),
                &textBrush
            );
        }
    }

    DrawButtons(hWnd, graphics);

    FontFamily infoFontFamily(L"Arial");
    Font infoFont(&infoFontFamily, 16, FontStyleRegular, UnitPixel);
    SolidBrush infoTextBrush(Color(255, 0, 0, 0));
    graphics.DrawString(L"Sterowanie dźwigiem - strzałki klawiatury i spacja. Maksymalny udźwig 20kg", -1, &infoFont, PointF(10, 50), &infoTextBrush);

    if (showMassInput) {
        SolidBrush dialogBrush(Color(255, 255, 255, 255));
        RectF dialogRect(200, 200, 200, 100);
        graphics.FillRectangle(&dialogBrush, dialogRect);

        Pen blackPen(Color(255, 0, 0, 0));
        graphics.DrawRectangle(&blackPen, dialogRect);

        FontFamily dialogFontFamily(L"Arial");
        Font dialogFont(&dialogFontFamily, 12, FontStyleRegular, UnitPixel);
        SolidBrush dialogTextBrush(Color(255, 0, 0, 0));

        graphics.DrawString(L"Wprowadź masę (kg):", -1, &dialogFont, PointF(210, 210), &dialogTextBrush);
        graphics.DrawString(massInput, -1, &dialogFont, PointF(210, 230), &dialogTextBrush);

        RectF okBtn(210, 250, 80, 30);
        SolidBrush okBtnBrush(Color(255, 200, 200, 200));
        graphics.FillRectangle(&okBtnBrush, okBtn);

        Pen blackPen2(Color(255, 0, 0, 0));
        graphics.DrawRectangle(&blackPen2, okBtn);

        graphics.DrawString(L"OK", -1, &dialogFont, PointF(240, 255), &dialogTextBrush);
    }

    // Kopiuj buforowany obraz na ekran
    BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

    // Sprzątanie
    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);

    EndPaint(hWnd, &ps);
}

void UpdatePosition(HWND hWnd) {
    int newXPos = animState.xPos;
    int newHeight = animState.height;

    if (animState.moveUp && animState.height > 20) newHeight -= animState.speed;
    if (animState.moveDown && animState.height < 380) newHeight += animState.speed;
    if (animState.moveLeft && animState.xPos > 230) newXPos -= animState.speed;
    if (animState.moveRight && animState.xPos < 600) newXPos += animState.speed;

    if (!isPickedUp && !CheckLineCollision(newXPos, animState.yPos, newHeight)) {
        animState.xPos = newXPos;
        animState.height = newHeight;
    }
    else if (isPickedUp) {
        animState.xPos = newXPos;
        animState.height = newHeight;
    }

    if (animState.moveElement && !isPickedUp) {
        HDC hdc = GetDC(hWnd);
        ShapeType detectedShape = GetShapeFromPixelColor(hdc, animState.xPos, animState.yPos + animState.height + 10);
        ReleaseDC(hWnd, hdc);

        if (currentType == SQUARETYPE && detectedShape == SQUARE) {
            for (int i = 0; i < figureCounter; ++i) {
                if (whichFigure[i] == 'S') {
                    if (animState.xPos >= SquareState.xPos[i] &&
                        animState.xPos <= SquareState.xPos[i] + SquareState.width &&
                        animState.yPos + animState.height + 15 >= SquareState.yPos[i] &&
                        animState.yPos + animState.height + 15 <= SquareState.yPos[i] + SquareState.height) {

                        if (!CheckCollision(SquareState.xPos[i], SquareState.yPos[i],
                            SquareState.width, SquareState.height, i, SQUARE)) {
                            animState.pickedSquareIndex = i;
                            animState.pickedCircleIndex = -1;
                            animState.pickedTriangleIndex = -1;
                            if (SquareState.mass[i] < 21) {
                                isPickedUp = true;
                            }
                            else {
                                isPickedUp = false;
                            }
                            break;
                        }
                    }
                }
            }
        }
        else if (currentType == CIRCLETYPE && detectedShape == CIRCLE) {
            for (int i = 0; i < figureCounter; ++i) {
                if (whichFigure[i] == 'C') {
                    if (animState.xPos >= CircleState.xPos[i] &&
                        animState.xPos <= CircleState.xPos[i] + CircleState.width &&
                        animState.yPos + animState.height + 15 >= CircleState.yPos[i] &&
                        animState.yPos + animState.height + 15 <= CircleState.yPos[i] + CircleState.height) {

                        if (!CheckCollision(CircleState.xPos[i], CircleState.yPos[i],
                            CircleState.width, CircleState.height, i, CIRCLE)) {
                            animState.pickedCircleIndex = i;
                            animState.pickedSquareIndex = -1;
                            animState.pickedTriangleIndex = -1;
                            if (CircleState.mass[i] < 21) {
                                isPickedUp = true;
                            }
                            else {
                                isPickedUp = false;
                            }
                            break;
                        }
                    }
                }
            }
        }
        else if (currentType == TRIANGLETYPE && detectedShape == TRIANGLE) {
            for (int i = 0; i < figureCounter; ++i) {
                if (whichFigure[i] == 'T') {
                    if (animState.xPos >= TriangleState.xPos[i] &&
                        animState.xPos <= TriangleState.xPos[i] + TriangleState.width &&
                        animState.yPos + animState.height + 15 >= TriangleState.yPos[i] &&
                        animState.yPos + animState.height + 15 <= TriangleState.yPos[i] + TriangleState.height) {

                        if (!CheckCollision(TriangleState.xPos[i], TriangleState.yPos[i],
                            TriangleState.width, TriangleState.height, i, TRIANGLE)) {
                            animState.pickedTriangleIndex = i;
                            animState.pickedSquareIndex = -1;
                            animState.pickedCircleIndex = -1;
                            if (TriangleState.mass[i] < 21) {
                                isPickedUp = true;
                            }
                            else {
                                isPickedUp = false;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }

    if (animState.moveElement && isPickedUp) {
        HDC hdc = GetDC(hWnd);
        ShapeType detectedShape = GetShapeFromPixelColor(hdc, animState.xPos, animState.yPos + animState.height + 22);
        ReleaseDC(hWnd, hdc);
        if (detectedShape == GROUND || ((detectedShape == SQUARE || detectedShape == CIRCLE || detectedShape == TRIANGLE) && animState.height >= 325)) {
            int newX = animState.xPos - (animState.pickedSquareIndex != -1 ? SquareState.width : 
                       (animState.pickedCircleIndex != -1 ? CircleState.width : TriangleState.width)) / 2;
            int newY = animState.yPos + animState.height;

            bool canPlace = true;
            if (animState.pickedSquareIndex != -1) {
                canPlace = !CheckCollision(newX, newY, SquareState.width, SquareState.height, animState.pickedSquareIndex, SQUARE);
            }
            else if (animState.pickedCircleIndex != -1) {
                canPlace = !CheckCollision(newX, newY, CircleState.width, CircleState.height, animState.pickedCircleIndex, CIRCLE);
            }
            else if (animState.pickedTriangleIndex != -1) {
                canPlace = !CheckCollision(newX, newY, TriangleState.width, TriangleState.height, animState.pickedTriangleIndex, TRIANGLE);
            }

            if (canPlace) {
                isPickedUp = false;
                if (animState.pickedSquareIndex != -1) {
                    SquareState.xPos[animState.pickedSquareIndex] = newX;
                    SquareState.yPos[animState.pickedSquareIndex] = newY;
                    animState.pickedSquareIndex = -1;
                }
                else if (animState.pickedCircleIndex != -1) {
                    CircleState.xPos[animState.pickedCircleIndex] = newX;
                    CircleState.yPos[animState.pickedCircleIndex] = newY;
                    animState.pickedCircleIndex = -1;
                }
                else if (animState.pickedTriangleIndex != -1) {
                    TriangleState.xPos[animState.pickedTriangleIndex] = newX;
                    TriangleState.yPos[animState.pickedTriangleIndex] = newY;
                    animState.pickedTriangleIndex = -1;
                }
            }
        }
    }

    if (isPickedUp) {
        int newX = animState.xPos - (animState.pickedSquareIndex != -1 ? SquareState.width : 
                  (animState.pickedCircleIndex != -1 ? CircleState.width : TriangleState.width)) / 2;
        int newY = animState.yPos + animState.height;

        if (animState.pickedSquareIndex != -1) {
            SquareState.xPos[animState.pickedSquareIndex] = newX;
            SquareState.yPos[animState.pickedSquareIndex] = newY;
        }
        else if (animState.pickedCircleIndex != -1) {
            CircleState.xPos[animState.pickedCircleIndex] = newX;
            CircleState.yPos[animState.pickedCircleIndex] = newY;
        }
        else if (animState.pickedTriangleIndex != -1) {
            TriangleState.xPos[animState.pickedTriangleIndex] = newX;
            TriangleState.yPos[animState.pickedTriangleIndex] = newY;
        }
    }

    RECT clientRect;
    GetClientRect(hWnd, &clientRect);

    if (animState.xPos < 0) animState.xPos = 0;
    if (animState.yPos < 0) animState.yPos = 0;
    if (animState.xPos > clientRect.right - animState.width)
        animState.xPos = clientRect.right - animState.width;
    if (animState.yPos > clientRect.bottom - animState.height)
        animState.yPos = clientRect.bottom - animState.height;
    InvalidateRect(hWnd, NULL, FALSE);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR lpCmdLine, int nCmdShow) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"GDIControl";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    HWND hWnd = CreateWindowW(
        L"GDIControl", L"Dźwig z wyborem kształtów",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 700,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    SetTimer(hWnd, 1, 16, NULL);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_PAINT:
        OnPaint(hWnd, bulion);
        break;

    case WM_TIMER:
        UpdatePosition(hWnd);
        break;

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_UP: animState.moveUp = true; break;
        case VK_DOWN: animState.moveDown = true; break;
        case VK_LEFT: animState.moveLeft = true; break;
        case VK_RIGHT: animState.moveRight = true; break;
        case VK_SPACE: animState.moveElement = true; break;
        case VK_RETURN:
            if (showMassInput) {
                newMass = (float)_wtof(massInput);
                if (newMass <= 0) newMass = 1.0f;
                showMassInput = false;
                massInput[0] = L'\0';
                InvalidateRect(hWnd, NULL, TRUE);
            }
            break;
        }
        break;

    case WM_KEYUP:
        switch (wParam) {
        case VK_UP: animState.moveUp = false; break;
        case VK_DOWN: animState.moveDown = false; break;
        case VK_LEFT: animState.moveLeft = false; break;
        case VK_RIGHT: animState.moveRight = false; break;
        case VK_SPACE: animState.moveElement = false; break;
        }
        break;

    case WM_LBUTTONDOWN: {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        if (showMassInput) {
            if (x >= 210 && x <= 290 && y >= 250 && y <= 280) {
                newMass = (float)_wtof(massInput);
                if (newMass <= 0) newMass = 1.0f;
                showMassInput = false;
                massInput[0] = L'\0';
            }
        }
        else if (x >= 370 && x <= 450 && y >= 10 && y <= 40) {
            showMassInput = true;
            massInput[0] = L'\0';
        }
        else if (x >= 10 && x <= 90 && y >= 10 && y <= 40) {
            currentShape = SQUARE;
           // currentType = SQUARETYPE;
            if (figureCounter < 10) {
                whichFigure[figureCounter] = 'S';
                SquareState.xPos[figureCounter] = 240 + figureCounter * 30;
                SquareState.yPos[figureCounter] = 580;
                SquareState.mass[figureCounter] = newMass;
                figureCounter++;
            }
        }
        else if (x >= 100 && x <= 180 && y >= 10 && y <= 40) {
            currentShape = CIRCLE;
           // currentType = CIRCLETYPE;
            if (figureCounter < 10) {
                whichFigure[figureCounter] = 'C';
                CircleState.xPos[figureCounter] = 240 + figureCounter * 30;
                CircleState.yPos[figureCounter] = 580;
                CircleState.mass[figureCounter] = newMass;
                figureCounter++;
            }
        }
        else if (x >= 190 && x <= 270 && y >= 10 && y <= 40) {
            currentShape = TRIANGLE;
            //currentType = TRIANGLETYPE;
            if (figureCounter < 10) {
                whichFigure[figureCounter] = 'T';
                TriangleState.xPos[figureCounter] = 240 + figureCounter * 30;
                TriangleState.yPos[figureCounter] = 580;
                TriangleState.mass[figureCounter] = newMass;
                figureCounter++;
            }
        }
        else if (x >= 280 && x <= 360 && y >= 10 && y <= 40) {
            for (int i = 0; i < figureCounter; ++i) {
                whichFigure[i] = 'N';
                SquareState.xPos[i] = 240 + i * 30;
                SquareState.yPos[i] = 580;
                SquareState.mass[i] = 1.0f;
                CircleState.xPos[i] = 240 + i * 30;
                CircleState.yPos[i] = 580;
                CircleState.mass[i] = 1.0f;
                TriangleState.xPos[i] = 240 + i * 30;
                TriangleState.yPos[i] = 580;
                TriangleState.mass[i] = 1.0f;
            }
            currentType = NONET;
            figureCounter = 0;
            isPickedUp = false;
            animState.pickedSquareIndex = -1;
            animState.pickedCircleIndex = -1;
            animState.pickedTriangleIndex = -1;
            bulion = true;
            showMassInput = false;
            massInput[0] = L'\0';
            newMass = 1.0f;
        }else if (x >= 460 && x <= 540 && y >= 10 && y <= 40) {//    RectF squareTypeBtn(460, 10, 80, 30);
            currentType = SQUARETYPE;
        }
        else if (x >= 550 && x <= 630 && y >= 10 && y <= 40) {
            currentType = CIRCLETYPE;
        }
        else if (x >= 640 && x <= 720 && y >= 10 && y <= 40) {
            currentType = TRIANGLETYPE;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }

    case WM_CHAR:
        if (showMassInput) {
            if (wParam == VK_BACK) {
                size_t len = wcslen(massInput);
                if (len > 0) {
                    massInput[len - 1] = L'\0';
                }
            }
            else if ((wParam >= L'0' && wParam <= L'9') || wParam == L'.') {
                size_t len = wcslen(massInput);
                if (len < 31) {
                    massInput[len] = (WCHAR)wParam;
                    massInput[len + 1] = L'\0';
                }
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}