//MainWindow
#include<Windows.h>
#include<string>
#include<sstream>
#include"resource.h"

CONST CHAR g_sz_WINDOW_CLASS[] = "My first window";

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void UpdateWindowTitle(HWND hwnd);
void CenterWindowOnScreen(HWND hwnd, float percentage = 0.75f);
BOOL IsCursorOverChildWindow(HWND hwnd);

// Глобальные переменные для хранения дескрипторов дочерних окон
HWND g_hChild1 = NULL;
HWND g_hChild2 = NULL;

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна. На этом этапе класс нашего окна регистрируется в ОС.
	//	 Класс окна это не тип данных, как в ООП, класс любого окна представляет собой 
	//	 самую обычную строку с именем класса.
	WNDCLASSEX wClass;
	ZeroMemory(&wClass, sizeof(wClass));

	//Инициализируем размеры и стиль:
	wClass.style = NULL;
	wClass.cbSize = sizeof(wClass);	//cb_ - Count Bytes
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;

	//Инициализируем внешний вид окон:
	wClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_BITCOIN));
	wClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON_PALM));
	//wClass.hIcon = (HICON)LoadImage(NULL, "palm.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	//wClass.hIconSm = (HICON)LoadImage(NULL, "bitcoin.ico", IMAGE_ICON, LR_DEFAULTSIZE, LR_DEFAULTSIZE, LR_LOADFROMFILE);
	wClass.hCursor = (HCURSOR)LoadImage
	(
		hInstance,
		"starcraft-original\\Working In Background.ani",
		IMAGE_CURSOR,
		LR_DEFAULTSIZE, LR_DEFAULTSIZE,
		LR_LOADFROMFILE
	);
	//wClass.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	//Инициализация системных переменных:
	wClass.hInstance = hInstance;
	wClass.lpszClassName = g_sz_WINDOW_CLASS;
	wClass.lpszMenuName = NULL;
	wClass.lpfnWndProc = WndProc;

	if (RegisterClassEx(&wClass) == NULL)
	{
		MessageBox(NULL, "Class registration failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}

	//2) Создание окна:
	HWND hwnd = CreateWindowEx
	(
		NULL,	//exStyle
		g_sz_WINDOW_CLASS,	//Имя класса окна
		g_sz_WINDOW_CLASS,	//Заголовок окна - будет обновлен позже
		WS_OVERLAPPEDWINDOW,//Стиль окна. Стили всегда зависят от класса окна. 'WS_OVERLAPPEDWINDOW' - главное окно
		CW_USEDEFAULT, CW_USEDEFAULT,	//Position
		640, 480,	//Размер окна
		NULL,
		NULL,	//Для главного окна это ResourceID главного меню, 
		//для дочернего окна (Control) - ResourceID дочернего окна (IDC_BUTTON_COPY)
		hInstance,
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Windows creation failed", NULL, MB_OK | MB_ICONERROR);
		return 0;
	}

	// Центрирование окна на экране (75% экрана)
	CenterWindowOnScreen(hwnd, 0.75f);

	// Создаем дочерние окна для тестирования
	g_hChild1 = CreateWindow(
		"BUTTON",
		"Child Button 1",
		WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 50, 150, 30,
		hwnd, (HMENU)1, hInstance, NULL);

	g_hChild2 = CreateWindow(
		"STATIC",
		"Child Static Text",
		WS_CHILD | WS_VISIBLE | SS_LEFT | SS_SUNKEN,
		20, 100, 150, 30,
		hwnd, (HMENU)2, hInstance, NULL);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	UpdateWindowTitle(hwnd); // Обновляем заголовок с начальными координатами

	//3) Запуск цикла сообщений:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		// Обновляем заголовок при движении мыши
		if (msg.message == WM_MOUSEMOVE)
		{
			UpdateWindowTitle(hwnd);
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

// Функция для обновления заголовка окна
void UpdateWindowTitle(HWND hwnd)
{
	RECT rect;
	GetWindowRect(hwnd, &rect);

	POINT cursorPos;
	GetCursorPos(&cursorPos);

	// Преобразуем координаты курсора в координаты относительно клиентской области
	POINT clientCursorPos = cursorPos;
	ScreenToClient(hwnd, &clientCursorPos);

	// Проверяем, находится ли курсор над дочерними окнами
	BOOL overChild1 = FALSE;
	BOOL overChild2 = FALSE;

	if (g_hChild1)
	{
		RECT childRect;
		GetWindowRect(g_hChild1, &childRect);
		overChild1 = PtInRect(&childRect, cursorPos);
	}

	if (g_hChild2)
	{
		RECT childRect;
		GetWindowRect(g_hChild2, &childRect);
		overChild2 = PtInRect(&childRect, cursorPos);
	}

	// Формируем строку заголовка
	std::ostringstream title;
	title << "Window: X=" << rect.left << ", Y=" << rect.top
		<< ", W=" << (rect.right - rect.left)
		<< ", H=" << (rect.bottom - rect.top)
		<< " | Cursor: X=" << cursorPos.x << ", Y=" << cursorPos.y
		<< " | Over child1: " << (overChild1 ? "Yes" : "No")
		<< ", child2: " << (overChild2 ? "Yes" : "No");

	SetWindowTextA(hwnd, title.str().c_str());
}

// Функция для центрирования окна на экране
void CenterWindowOnScreen(HWND hwnd, float percentage)
{
	// Получаем размеры экрана
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Вычисляем размеры окна (75% от экрана)
	int windowWidth = static_cast<int>(screenWidth * percentage);
	int windowHeight = static_cast<int>(screenHeight * percentage);

	// Вычисляем позицию для центрирования
	int windowX = (screenWidth - windowWidth) / 2;
	int windowY = (screenHeight - windowHeight) / 2;

	// Устанавливаем размер и позицию окна
	SetWindowPos(hwnd, NULL, windowX, windowY, windowWidth, windowHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		MessageBox(hwnd, "Cursor check", "Info", MB_OK | MB_ICONINFORMATION);
		break;

	case WM_MOVE:
		// При перемещении окна обновляем заголовок
		UpdateWindowTitle(hwnd);
		break;

	case WM_SIZE:
		// При изменении размера окна обновляем заголовок
		UpdateWindowTitle(hwnd);
		break;

	case WM_MOUSEMOVE:
		// При движении мыши проверяем положение курсора и обновляем заголовок
		// Эта строка важна - обновляем заголовок при каждом движении мыши
		UpdateWindowTitle(hwnd);
		break;

	case WM_NCMOUSEMOVE:
		// Также обновляем при движении мыши в неклиентской области (заголовок, рамки)
		UpdateWindowTitle(hwnd);
		break;

	case WM_COMMAND:
		// Обработка нажатия на кнопку
		if (LOWORD(wParam) == 1) // ID кнопки
		{
			MessageBox(hwnd, "Button clicked!", "Info", MB_OK | MB_ICONINFORMATION);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return FALSE;
}