#include<Windows.h>
#include"resource.h"

CONST CHAR* g_sz_VALUES[] = { "This", "is", "my", "first", "List", "Box" };

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcAdd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcEdit(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, (DLGPROC)DlgProc, 0);
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hListBox = GetDlgItem(hwnd, IDC_LIST1);
		for (int i = 0; i < sizeof(g_sz_VALUES) / sizeof(g_sz_VALUES[0]); i++)
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)g_sz_VALUES[i]);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON_ADD:
		{
			DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ADD), hwnd, DlgProcAdd, 0);
		}
		break;
		case IDC_BUTTON_DELETE:
		{
			HWND hListBox = GetDlgItem(hwnd, IDC_LIST1);
			INT i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
			if (i != LB_ERR)
				SendMessage(hListBox, LB_DELETESTRING, i, 0);
		}
		break;
		case IDC_LIST1:
		{
			// Обработка двойного щелчка и нажатия Enter
			if (HIWORD(wParam) == LBN_DBLCLK || (HIWORD(wParam) == 0 && LOWORD(wParam) == IDC_LIST1))
			{
				HWND hListBox = GetDlgItem(hwnd, IDC_LIST1);
				INT i = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
				if (i != LB_ERR)
				{
					// Передаем индекс выбранного элемента в диалог редактирования
					DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_EDIT), hwnd, DlgProcEdit, i);
				}
			}
		}
		break;
		case IDOK:
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}

BOOL CALLBACK DlgProcAdd(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		SetFocus(GetDlgItem(hwnd, IDC_EDIT));
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
			SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);

			// Проверяем, что строка не пустая
			if (strlen(sz_buffer) == 0)
			{
				MessageBox(hwnd, "Элемент не может быть пустым", "Warning", MB_OK | MB_ICONWARNING);
				break;
			}

			HWND hParent = GetParent(hwnd);
			HWND hList = GetDlgItem(hParent, IDC_LIST1);
			if (SendMessage(hList, LB_FINDSTRINGEXACT, 0, (LPARAM)sz_buffer) == LB_ERR)
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)sz_buffer);
			else
			{
				MessageBox(hwnd, "Такой элемент уже существует", "Warning", MB_OK | MB_ICONWARNING);
				break;
			}
		}
		// break; убран для закрытия диалога после добавления
		case IDCANCEL:
			EndDialog(hwnd, 0);
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}

// Диалоговая процедура для редактирования элемента
BOOL CALLBACK DlgProcEdit(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int nIndex; // Сохраняем индекс редактируемого элемента

	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		nIndex = (int)lParam; // Получаем индекс из параметра

		HWND hParent = GetParent(hwnd);
		HWND hList = GetDlgItem(hParent, IDC_LIST1);

		// Получаем текущий текст элемента
		CHAR sz_buffer[256] = {};
		SendMessage(hList, LB_GETTEXT, nIndex, (LPARAM)sz_buffer);

		// Устанавливаем текст в поле редактирования
		HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
		SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)sz_buffer);
		SetFocus(hEdit);
	}
	break;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			CONST INT SIZE = 256;
			CHAR sz_buffer[SIZE] = {};
			HWND hEdit = GetDlgItem(hwnd, IDC_EDIT);
			SendMessage(hEdit, WM_GETTEXT, SIZE, (LPARAM)sz_buffer);

			// Проверяем, что строка не пустая
			if (strlen(sz_buffer) == 0)
			{
				MessageBox(hwnd, "Элемент не может быть пустым", "Warning", MB_OK | MB_ICONWARNING);
				break;
			}

			HWND hParent = GetParent(hwnd);
			HWND hList = GetDlgItem(hParent, IDC_LIST1);

			// Проверяем, не существует ли уже такой элемент (кроме текущего редактируемого)
			int foundIndex = SendMessage(hList, LB_FINDSTRINGEXACT, 0, (LPARAM)sz_buffer);
			if (foundIndex == LB_ERR || foundIndex == nIndex)
			{
				// Заменяем элемент
				SendMessage(hList, LB_DELETESTRING, nIndex, 0);
				SendMessage(hList, LB_INSERTSTRING, nIndex, (LPARAM)sz_buffer);
				SendMessage(hList, LB_SETCURSEL, nIndex, 0);
			}
			else
			{
				MessageBox(hwnd, "Такой элемент уже существует", "Warning", MB_OK | MB_ICONWARNING);
				break;
			}
		}
		// break; убран для закрытия диалога после сохранения
		case IDCANCEL:
			EndDialog(hwnd, 0);
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}