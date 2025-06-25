#include <windows.h>
#include <gdiplus.h>
#include <objidl.h>
#include <iostream>
#include <string>
#include <string.h>
using namespace Gdiplus;

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "user32.lib")

enum ShapeType { SQUARE, CIRCLE, TRIANGLE, GROUND, NONE};
ShapeType currentShape = NONE;

enum BuildType { SQUARETYPE, CIRCLETYPE, TRIANGLETYPE, AUTOSTC, AUTOCST};
BuildType currentType = SQUARETYPE;

char whichFigure[10] = {'N','N','N','N','N','N','N','N','N','N'};

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
    int xPos[10] = {240, 270, 300, 330, 360, 390, 420, 450, 480, 510};
    int yPos[10] = {580, 580, 580, 580, 580, 580, 580, 580, 580, 580};
    int width = 20;
    int height = 20; 
    Color color = Color(255, 0, 255, 0);
} SquareState;

bool bulion = true;
int figureCounter = 0;
int figurePos[10] = {0};
bool isPickedUp = false;

bool CheckCollision(int x, int y, int width, int height, int ignoreIndex = -1) {
    for(int i = 0; i < figureCounter; ++i) {
        if(i == ignoreIndex || whichFigure[i] != 'S') continue;
        
        if(x < SquareState.xPos[i] + SquareState.width &&
           x + width > SquareState.xPos[i] &&
           y < SquareState.yPos[i] + SquareState.height &&
           y + height > SquareState.yPos[i]) {
            return true;
        }
    }
    return false;
}

bool CheckLineCollision(int lineX, int lineY, int lineHeight) {
    for(int i = 0; i < figureCounter; ++i) {
        if(whichFigure[i] == 'S') {
            if(lineX < SquareState.xPos[i] + SquareState.width &&
               lineX + animState.width > SquareState.xPos[i] &&
               lineY < SquareState.yPos[i] + SquareState.height &&
               lineY + lineHeight > SquareState.yPos[i]) {
                return true;
            }
        }
    }
    return false;
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
    graphics.DrawString(L"Kwadrat", -1, &font, PointF(15, 15), &textBrush);
    
    RectF circleBtn(100, 10, 80, 30);
    SolidBrush circleBtnBrush(currentShape == CIRCLE ? Color(255, 180, 20, 60) : Color(255, 220, 20, 60));
    graphics.FillRectangle(&circleBtnBrush, circleBtn);
    graphics.DrawRectangle(&borderPen, circleBtn);
    graphics.DrawString(L"Koło", -1, &font, PointF(115, 15), &textBrush);
    
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
    else if(r == 0 && g == 0 && b == 0) return GROUND;
    
    return NONE;
}

void OnPaint(HWND hWnd, bool &bulion) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    Graphics graphics(hdc);
    
    SolidBrush bgBrush(Color(255, 240, 240, 240));
    RECT rect;
    GetClientRect(hWnd, &rect);
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
    if(lineCollision){
        animState.speed = 0;
        animState.height = animState.height - 5;
    }else{
        animState.speed = 5;
    }
    SolidBrush lineBrush(animState.color);
    graphics.FillRectangle(&lineBrush, animState.xPos, animState.yPos, animState.width, animState.height); 

    SolidBrush mainPartBrushPoint(animState.colorPoint);
    graphics.FillRectangle(&mainPartBrushPoint, animState.xPos, animState.yPos, animState.widthPoint, animState.heightPoint); 
    
    switch(currentShape) {
        case SQUARE:
            for(int i = 0; i < figureCounter; ++i){
                if(whichFigure[i] == 'S'){
                    SolidBrush squareBrush(SquareState.color);
                    graphics.FillRectangle(&squareBrush, SquareState.xPos[i], SquareState.yPos[i], SquareState.width, SquareState.height);
                }
            }
            break;    
    }

    DrawButtons(hWnd, graphics);
    
    FontFamily infoFontFamily(L"Arial");
    Font infoFont(&infoFontFamily, 16, FontStyleRegular, UnitPixel);
    SolidBrush infoTextBrush(Color(255, 0, 0, 0));
    graphics.DrawString(L"Sterowanie dźwigiem - strzałki klawiatury", -1, &infoFont, PointF(10, 50), &infoTextBrush);
    
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

    if(animState.moveElement && !isPickedUp){
        HDC hdc = GetDC(hWnd);
        ShapeType detectedShape = GetShapeFromPixelColor(hdc, animState.xPos, animState.yPos + animState.height+10);
        ReleaseDC(hWnd, hdc);
        
        if(currentType == SQUARETYPE && detectedShape == SQUARE){
            for(int i = 0; i < figureCounter; ++i){
                if(whichFigure[i] == 'S'){
                    if(animState.xPos >= SquareState.xPos[i] && 
                       animState.xPos <= SquareState.xPos[i] + SquareState.width &&
                       animState.yPos + animState.height + 15 >= SquareState.yPos[i] && 
                       animState.yPos + animState.height + 15 <= SquareState.yPos[i] + SquareState.height) {
                        
                        if(!CheckCollision(SquareState.xPos[i], SquareState.yPos[i], 
                                         SquareState.width, SquareState.height, i)) {
                            animState.pickedSquareIndex = i;
                            isPickedUp = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if(animState.moveElement && isPickedUp){
        HDC hdc = GetDC(hWnd);
        ShapeType detectedShape = GetShapeFromPixelColor(hdc, animState.xPos, animState.yPos + animState.height + 22);
        ReleaseDC(hWnd, hdc);
        std::cout<<animState.height;
        std::cout << "\n";
        if(detectedShape == GROUND || ((detectedShape == SQUARE) && animState.height >= 325)) {
            int newX = animState.xPos - SquareState.width/2;
            int newY = animState.yPos + animState.height;
            
            if(!CheckCollision(newX, newY, SquareState.width, SquareState.height, animState.pickedSquareIndex)) {
                isPickedUp = false;
                SquareState.xPos[animState.pickedSquareIndex] = newX;
                SquareState.yPos[animState.pickedSquareIndex] = newY;
                animState.pickedSquareIndex = -1;
            }
        }
    }

    if(isPickedUp && animState.pickedSquareIndex != -1){
        int newX = animState.xPos - SquareState.width/2;
        int newY = animState.yPos + animState.height;
        
        if(!CheckCollision(newX, newY, SquareState.width, SquareState.height, animState.pickedSquareIndex)) {
            SquareState.xPos[animState.pickedSquareIndex] = newX;
            SquareState.yPos[animState.pickedSquareIndex] = newY;
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
    
    WNDCLASSW wc = {0};
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
            
            if (x >= 10 && x <= 90 && y >= 10 && y <= 40) { // kwadrat
                currentShape = SQUARE;
                if(figureCounter < 10) {
                    whichFigure[figureCounter] = 'S';
                    SquareState.xPos[figureCounter] = 240 + figureCounter * 30;
                    SquareState.yPos[figureCounter] = 580;
                    figureCounter++;
                }
            } 
            else if (x >= 100 && x <= 180 && y >= 10 && y <= 40) { // koło
                currentShape = CIRCLE;
            } 
            else if (x >= 190 && x <= 270 && y >= 10 && y <= 40) { // trójkąt
                currentShape = TRIANGLE;
            }
            else if (x >= 280 && x <= 360 && y >= 10 && y <= 40) { // clear 
                for(int i = 0; i < figureCounter; ++i) {
                    whichFigure[i] = 'N';
                    SquareState.xPos[i] = 240 + i * 30;
                    SquareState.yPos[i] = 580;
                }
                figureCounter = 0;
                isPickedUp = false;
                animState.pickedSquareIndex = -1;
                bulion = true;
            }
            
            InvalidateRect(hWnd, NULL, TRUE);
            break;
        }
            
        case WM_DESTROY:
            KillTimer(hWnd, 1);
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProcW(hWnd, message, wParam, lParam);
    }
    return 0;
}