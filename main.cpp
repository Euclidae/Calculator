// wx_calculator.cpp
#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <cmath>
#include <vector>

class CalculatorApp : public wxApp {
public:
    virtual bool OnInit();
};

class CalculatorFrame : public wxFrame {
public:
    CalculatorFrame(const wxString& title);

private:
    wxTextCtrl* display;
    wxCheckBox* radianCheck;
    double currentValue;
    wxString currentOperation;
    bool newNumber;
    double memoryValue;
    double lastAnswer;
    std::vector<double> valueStack;
    std::vector<wxString> operationStack;

    // Button creation helper
    wxButton* CreateButton(wxPanel* panel, const wxString& label,
                          wxColour color = wxNullColour);

    // Event handlers
    void OnNumber(wxCommandEvent& event);
    void OnOperator(wxCommandEvent& event);
    void OnEqual(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnBackspace(wxCommandEvent& event);
    void OnTrigFunction(wxCommandEvent& event);
    void OnInverseTrigFunction(wxCommandEvent& event);
    void OnDecimal(wxCommandEvent& event);
    void OnMemoryOperation(wxCommandEvent& event);
    void OnScientificFunction(wxCommandEvent& event);
    void OnConstant(wxCommandEvent& event);
    void OnNegate(wxCommandEvent& event);
    void OnAnswer(wxCommandEvent& event);
    void OnPower(wxCommandEvent& event);
    void OnSquareRoot(wxCommandEvent& event);
    void OnParenthesis(wxCommandEvent& event);

    // Helper methods
    void UpdateDisplay(double value);
    double ConvertAngle(double angle);
    double GetDisplayValue();
    void SetDisplayValue(double value);
    void ExecuteOperation(const wxString& op, double value);
};

wxIMPLEMENT_APP(CalculatorApp);

bool CalculatorApp::OnInit() {
    CalculatorFrame *frame = new CalculatorFrame("Scientific Calculator");
    frame->Show(true);
    return true;
}

CalculatorFrame::CalculatorFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 600)),
      currentValue(0.0), newNumber(true), memoryValue(0.0), lastAnswer(0.0) {

    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Display
    display = new wxTextCtrl(panel, wxID_ANY, "0",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_RIGHT | wxTE_READONLY);
    display->SetFont(wxFont(24, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(display, 0, wxEXPAND | wxALL, 10);

    // Control panel (for radians/degrees toggle)
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    radianCheck = new wxCheckBox(panel, wxID_ANY, "Use Radians");
    controlSizer->Add(radianCheck, 0, wxALIGN_LEFT | wxALL, 5);

    // Add "M" indicator for memory
    wxStaticText* memIndicator = new wxStaticText(panel, wxID_ANY, "Memory: 0.0");
    controlSizer->Add(memIndicator, 0, wxALIGN_RIGHT | wxALL, 5);

    mainSizer->Add(controlSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Button panel
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);

    // Memory and function buttons (left side)
    wxGridSizer* memoryGrid = new wxGridSizer(7, 2, 3, 3);
    const wxString memoryButtons[] = {
        "MC", "MR", "M+", "M-", "MS", "Ans", "ln", "log",
        "e^x", "10^x", "x^2", "x^3", "sqrt", "cbrt"
    };

    for (const wxString& label : memoryButtons) {
        wxButton* btn = CreateButton(panel, label, wxColour(180, 180, 220));

        if (label == "MC" || label == "MR" || label == "M+" || label == "M-" || label == "MS") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnMemoryOperation, this);
        }
        else if (label == "Ans") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnAnswer, this);
        }
        else if (label == "x^2" || label == "x^3") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnPower, this);
        }
        else if (label == "sqrt" || label == "cbrt") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnSquareRoot, this);
        }
        else {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnScientificFunction, this);
        }

        memoryGrid->Add(btn, 0, wxEXPAND);
    }

    buttonSizer->Add(memoryGrid, 0, wxEXPAND | wxRIGHT, 5);

    // Main calculator buttons (center)
    wxGridSizer* mainGrid = new wxGridSizer(6, 5, 3, 3);

    const wxString mainButtons[] = {
        "sin", "cos", "tan", "(", ")",
        "asin", "acos", "atan", "π", "e",
        "7", "8", "9", "/", "C",
        "4", "5", "6", "*", "⌫",
        "1", "2", "3", "-", "±",
        "0", ".", "=", "+", "^"
    };

    for (const wxString& label : mainButtons) {
        wxColour color = wxNullColour;

        if (label == "sin" || label == "cos" || label == "tan") {
            color = wxColour(255, 215, 0);  // Gold for trig functions
        }
        else if (label == "asin" || label == "acos" || label == "atan") {
            color = wxColour(255, 180, 0);  // Darker gold for inverse trig
        }
        else if (label == "C") {
            color = wxColour(220, 20, 60);  // Red for clear
        }
        else if (label == "⌫") {
            color = wxColour(220, 60, 60);  // Light red for backspace
        }
        else if (label == "=" || label == "±") {
            color = wxColour(34, 139, 34);  // Green
        }
        else if (wxString("+-*/^()").Contains(label)) {
            color = wxColour(100, 149, 237);  // Blue for operators
        }
        else if (label == "π" || label == "e") {
            color = wxColour(200, 200, 100);  // Yellow-ish for constants
        }

        wxButton* btn = CreateButton(panel, label, color);

        if (label == "sin" || label == "cos" || label == "tan") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnTrigFunction, this);
        }
        else if (label == "asin" || label == "acos" || label == "atan") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnInverseTrigFunction, this);
        }
        else if (label == "C") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnClear, this);
        }
        else if (label == "⌫") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnBackspace, this);
        }
        else if (label == "=") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnEqual, this);
        }
        else if (label == "±") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnNegate, this);
        }
        else if (label == "." || (label.Length() == 1 && wxString("0123456789").Contains(label))) {
            if (label == ".") {
                btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnDecimal, this);
            } else {
                btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnNumber, this);
            }
        }
        else if (label == "π" || label == "e") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnConstant, this);
        }
        else if (label == "(" || label == ")") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnParenthesis, this);
        }
        else if (wxString("+-*/^").Contains(label)) {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnOperator, this);
        }

        mainGrid->Add(btn, 0, wxEXPAND);
    }

    buttonSizer->Add(mainGrid, 1, wxEXPAND, 0);

    mainSizer->Add(buttonSizer, 1, wxEXPAND | wxALL, 10);

    panel->SetSizer(mainSizer);
    Center();
}

wxButton* CalculatorFrame::CreateButton(wxPanel* panel, const wxString& label, wxColour color) {
    wxButton* btn = new wxButton(panel, wxID_ANY, label);
    btn->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    if (color != wxNullColour) {
        btn->SetBackgroundColour(color);
    }

    return btn;
}

void CalculatorFrame::OnNumber(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    wxString value = btn->GetLabel();

    if (newNumber) {
        display->SetValue(value);
        newNumber = false;
    }
    else {
        display->AppendText(value);
    }
}

void CalculatorFrame::OnOperator(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    wxString op = btn->GetLabel();

    if (!newNumber) {
        double value = GetDisplayValue();

        if (!operationStack.empty()) {
            ExecuteOperation(currentOperation, value);
        } else {
            currentValue = value;
        }
    }

    currentOperation = op;
    newNumber = true;
}

void CalculatorFrame::OnEqual(wxCommandEvent& event) {
    if (operationStack.empty()) {
        double secondValue = GetDisplayValue();

        if (currentOperation == "+") currentValue += secondValue;
        else if (currentOperation == "-") currentValue -= secondValue;
        else if (currentOperation == "*") currentValue *= secondValue;
        else if (currentOperation == "/") {
            if (secondValue != 0) {
                currentValue /= secondValue;
            } else {
                wxMessageBox("Division by zero!", "Error", wxICON_ERROR);
                OnClear(event);
                return;
            }
        }
        else if (currentOperation == "^") {
            currentValue = pow(currentValue, secondValue);
        }

        UpdateDisplay(currentValue);
        lastAnswer = currentValue;
    } else {
        // Handle stack-based calculation (parentheses)
        // This is just a basic implementation
        double result = GetDisplayValue();

        while (!operationStack.empty()) {
            wxString op = operationStack.back();
            operationStack.pop_back();

            double val = valueStack.back();
            valueStack.pop_back();

            if (op == "+") result = val + result;
            else if (op == "-") result = val - result;
            else if (op == "*") result = val * result;
            else if (op == "/") {
                if (result != 0) {
                    result = val / result;
                } else {
                    wxMessageBox("Division by zero!", "Error", wxICON_ERROR);
                    OnClear(event);
                    return;
                }
            }
            else if (op == "^") {
                result = pow(val, result);
            }
        }

        currentValue = result;
        UpdateDisplay(result);
        lastAnswer = result;
    }

    newNumber = true;
}

void CalculatorFrame::OnTrigFunction(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;

    value = ConvertAngle(value);

    if (btn->GetLabel() == "sin") result = sin(value);
    else if (btn->GetLabel() == "cos") result = cos(value);
    else if (btn->GetLabel() == "tan") result = tan(value);

    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

void CalculatorFrame::OnInverseTrigFunction(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;

    // For inverse trig functions, we get the result in radians, then convert if needed
    if (btn->GetLabel() == "asin") result = asin(value);
    else if (btn->GetLabel() == "acos") result = acos(value);
    else if (btn->GetLabel() == "atan") result = atan(value);

    // Convert back to degrees if not using radians
    if (!radianCheck->GetValue()) {
        result = result * 180.0 / M_PI;
    }

    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

void CalculatorFrame::OnClear(wxCommandEvent& event) {
    display->SetValue("0");
    currentValue = 0.0;
    newNumber = true;
    valueStack.clear();
    operationStack.clear();
}

void CalculatorFrame::OnBackspace(wxCommandEvent& event) {
    wxString current = display->GetValue();

    if (current.Length() > 1) {
        current.RemoveLast();
        display->SetValue(current);
    } else {
        display->SetValue("0");
        newNumber = true;
    }
}

void CalculatorFrame::OnDecimal(wxCommandEvent& event) {
    if (newNumber) {
        display->SetValue("0.");
        newNumber = false;
    }
    else if (!display->GetValue().Contains(".")) {
        display->AppendText(".");
    }
}

void CalculatorFrame::OnMemoryOperation(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    wxString op = btn->GetLabel();

    if (op == "MC") {
        memoryValue = 0.0;
    }
    else if (op == "MR") {
        UpdateDisplay(memoryValue);
        newNumber = true;
    }
    else if (op == "M+") {
        memoryValue += GetDisplayValue();
    }
    else if (op == "M-") {
        memoryValue -= GetDisplayValue();
    }
    else if (op == "MS") {
        memoryValue = GetDisplayValue();
    }

    // Update memory indicator
    wxStaticText* memIndicator = dynamic_cast<wxStaticText*>(
        FindWindowByLabel("Memory: " + wxString::Format("%.1f", memoryValue - 0.1))
    );
    if (memIndicator) {
        memIndicator->SetLabel("Memory: " + wxString::Format("%.1f", memoryValue));
    }
}

void CalculatorFrame::OnScientificFunction(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;

    if (btn->GetLabel() == "ln") {
        if (value > 0) {
            result = log(value);
        } else {
            wxMessageBox("Cannot take log of non-positive value!", "Error", wxICON_ERROR);
            return;
        }
    }
    else if (btn->GetLabel() == "log") {
        if (value > 0) {
            result = log10(value);
        } else {
            wxMessageBox("Cannot take log of non-positive value!", "Error", wxICON_ERROR);
            return;
        }
    }
    else if (btn->GetLabel() == "e^x") {
        result = exp(value);
    }
    else if (btn->GetLabel() == "10^x") {
        result = pow(10.0, value);
    }

    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

void CalculatorFrame::OnConstant(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = 0.0;

    if (btn->GetLabel() == "π") {
        value = M_PI;
    }
    else if (btn->GetLabel() == "e") {
        value = M_E;
    }

    UpdateDisplay(value);
    newNumber = true;
}

void CalculatorFrame::OnNegate(wxCommandEvent& event) {
    double value = GetDisplayValue();
    UpdateDisplay(-value);
}

void CalculatorFrame::OnAnswer(wxCommandEvent& event) {
    UpdateDisplay(lastAnswer);
    newNumber = true;
}

void CalculatorFrame::OnPower(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;

    if (btn->GetLabel() == "x^2") {
        result = value * value;
    }
    else if (btn->GetLabel() == "x^3") {
        result = value * value * value;
    }

    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

void CalculatorFrame::OnSquareRoot(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;

    if (btn->GetLabel() == "sqrt") {
        if (value >= 0) {
            result = sqrt(value);
        } else {
            wxMessageBox("Cannot take square root of negative value!", "Error", wxICON_ERROR);
            return;
        }
    }
    else if (btn->GetLabel() == "cbrt") {
        result = cbrt(value);  // Cube root can handle negative values
    }

    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

void CalculatorFrame::OnParenthesis(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());

    if (btn->GetLabel() == "(") {
        // Save current state and push to stack
        if (!newNumber) {
            currentValue = GetDisplayValue();
        }

        valueStack.push_back(currentValue);
        operationStack.push_back(currentOperation);

        currentValue = 0.0;
        currentOperation = "+";  // Default operation inside parenthesis
        newNumber = true;
    }
    else if (btn->GetLabel() == ")") {
        // Compute the value inside the parenthesis
        if (!operationStack.empty()) {
            wxCommandEvent fakeEvent;
            OnEqual(fakeEvent);
        }
    }
}

void CalculatorFrame::UpdateDisplay(double value) {
    wxString output = wxString::Format("%.12f", value);
    // Remove trailing zeros and possible . at end
    while (output.Contains(".") && (output.Last() == '0' || output.Last() == '.')) {
        output.RemoveLast();
    }
    display->SetValue(output);
}

double CalculatorFrame::ConvertAngle(double angle) {
    if (!radianCheck->GetValue()) { // Convert degrees to radians
        return angle * M_PI / 180.0;
    }
    return angle;
}

double CalculatorFrame::GetDisplayValue() {
    return wxAtof(display->GetValue());
}

void CalculatorFrame::SetDisplayValue(double value) {
    UpdateDisplay(value);
}

void CalculatorFrame::ExecuteOperation(const wxString& op, double value) {
    double stackValue = valueStack.back();
    valueStack.pop_back();

    wxString stackOp = operationStack.back();
    operationStack.pop_back();

    double result = 0.0;

    if (stackOp == "+") result = stackValue + value;
    else if (stackOp == "-") result = stackValue - value;
    else if (stackOp == "*") result = stackValue * value;
    else if (stackOp == "/") {
        if (value != 0) {
            result = stackValue / value;
        } else {
            wxMessageBox("Division by zero!", "Error", wxICON_ERROR);
            return;
        }
    }
    else if (stackOp == "^") {
        result = pow(stackValue, value);
    }

    valueStack.push_back(result);
    operationStack.push_back(op);
}
