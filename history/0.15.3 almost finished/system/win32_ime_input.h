#pragma once

#include "ime.h"
#include "../common/feature.h"

#include <Windows.h>


BEGIN_NAMESPACE(WndDesign)


class ImeInput : Uncopyable {
private:
	ImeInput() = default;
	~ImeInput() = default;
public:
	static ImeInput& Get();


	//////////////////////////////////////////////////////////
	////                    IME Status                    ////
	//////////////////////////////////////////////////////////
private:
	bool _has_ime = false; // True if the current input context has IMEs.
	LANGID _language = 0;  // !!! Deprecated !!!
public:
	// Enables the IME attached to the given window.
	void EnableIME(HWND window_handle);
	// Disables the IME attached to the given window.
	void DisableIME(HWND window_handle);
	// Cancels an ongoing composition of the IME attached to the given window.
	void CancelIME(HWND window_handle);
	// Retrieves the input language from Windows and update it.
	// Returns true if the given input language has IMEs.
	bool UpdateInputLanguage();


	//////////////////////////////////////////////////////////
	////                    IME Window                    ////
	//////////////////////////////////////////////////////////
private:
	bool _has_system_caret = false;	 // True if the current input context has created a system caret 
									 //   to set the position of a IME window.
	Rect _caret_rect = region_empty; // The rectangle of the input caret retrieved from a renderer process.

private:
	// Updates the position of the IME windows.
	void MoveImeWindow(HIMC imm_context);
public:
	// Creates the IME windows, and allocate required resources for them.
	void CreateImeWindow(HWND window_handle);
	// Updates the style of the IME windows.
	void SetImeWindowStyle(HWND window_handle, UINT message, WPARAM wparam, LPARAM lparam);
	// Destroys the IME windows and all the resources attached to them.
	void DestroyImeWindow(HWND window_handle);
	// Updates the caret position of the given window.
	void UpdateCaretRect(HWND window_handle, Rect caret_rect);
	// Updates the position of the IME windows.
	void UpdateImeWindow(HWND window_handle);


	/////////////////////////////////////////////////////////
	////                   Composition                   ////
	/////////////////////////////////////////////////////////
private:
	bool _is_composing = false;  // True if there is an ongoing composition.
	ImeComposition _composition; // The current composition string.
	ImeComposition _result;      // The composition result string.

private:
	// Retrieves the composition information.
	void GetCompositionInfo(HIMC imm_context, LPARAM lparam, ImeComposition& composition);
	// Completes the ongoing composition if it exists.
	void CompleteComposition(HIMC imm_context);
	// Retrieves a string from the IMM.
	bool GetString(HIMC imm_context, WPARAM lparam, uint type, ImeComposition& composition);

public:
	// Retrieves a composition result of the ongoing composition and returns true if it exists.
	bool UpdateResult(HWND window_handle, LPARAM lparam);
	// Retrieves the current composition status of the ongoing composition and returns true if it is updated.
	bool UpdateComposition(HWND window_handle, LPARAM lparam);

	// Cleans up the all resources attached to the given ImeInput object, and reset its composition status.
	void CleanupComposition(HWND window_handle);
	// Resets the composition status.
	void ResetComposition();

public:
	bool IsComposing() const { return _is_composing; }
	const ImeComposition& GetComposition() const { return _composition; }
	const ImeComposition& GetResult() const { return _result; }
};


END_NAMESPACE(WndDesign)