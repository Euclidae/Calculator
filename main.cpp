// wx_calculator.cpp
// A scientific calculator implementation using wxWidgets
// Created by Euclidae
//N.B fix the buttons that do not appear on te sde. I am too tired for this.
//N.B 2, dear future me, the connvetion used here is pretty much onSOMESYMBOL where this means when we click on SOMESYMBOL

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <cmath>
#include <vector>



class CalculatorApp : public wxApp {
public:
    virtual bool OnInit();
};

// Main calculator frame
class CalculatorFrame : public wxFrame {
public:
    CalculatorFrame(const wxString& title);

private:
    // UI elements
    wxTextCtrl* display;
    wxCheckBox* radianCheck;
    wxStaticText* memIndicator;
    
    // Calculator state
    double currentValue;
    wxString currentOperation;
    bool newNumber;
    double memoryValue;
    double lastAnswer;
    
    // For handling parentheses
    std::vector<double> valueStack;
    std::vector<wxString> operationStack;

    // Helper method to create buttons with consistent styling
    wxButton* CreateButton(wxPanel* panel, const wxString& label, 
                          wxColour color = wxNullColour);

    // ---- Event handlers ----
    
    // Numeric input
    void OnNumber(wxCommandEvent& event);
    void OnDecimal(wxCommandEvent& event);
    
    // Basic operations
    void OnOperator(wxCommandEvent& event);
    void OnEqual(wxCommandEvent& event);
    void OnClear(wxCommandEvent& event);
    void OnBackspace(wxCommandEvent& event);
    void OnNegate(wxCommandEvent& event);
    
    // Advanced functions
    void OnTrigFunction(wxCommandEvent& event);
    void OnInverseTrigFunction(wxCommandEvent& event);
    void OnScientificFunction(wxCommandEvent& event);
    void OnPower(wxCommandEvent& event);
    void OnSquareRoot(wxCommandEvent& event);
    
    // Memory operations
    void OnMemoryOperation(wxCommandEvent& event);
    void OnAnswer(wxCommandEvent& event);
    
    // Constants and parentheses
    void OnConstant(wxCommandEvent& event);
    void OnParenthesis(wxCommandEvent& event);
    
    // ---- Helper methods ----
    void UpdateDisplay(double value);
    double ConvertAngle(double angle);
    double GetDisplayValue();
    void UpdateMemoryDisplay();
    void ExecuteOperation(const wxString& op, double value);
};

// Implement the application class
wxIMPLEMENT_APP(CalculatorApp);

// Initialize the application
bool CalculatorApp::OnInit() {
    // Create the main frame
    CalculatorFrame *frame = new CalculatorFrame("Scientific Calculator");
    frame->Show(true);
    return true;
}

// Constructor for the calculator frame
CalculatorFrame::CalculatorFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(600, 600)),
      currentValue(0.0), 
      newNumber(true), 
      memoryValue(0.0), 
      lastAnswer(0.0) {

    // Create main panel and sizer
    wxPanel* panel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // ---- Display area ----
    
    // Create calculator display with large font
    display = new wxTextCtrl(panel, wxID_ANY, "0",
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_RIGHT | wxTE_READONLY);
    display->SetFont(wxFont(24, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(display, 0, wxEXPAND | wxALL, 10);

    // ---- Control area ----
    
    // Create control panel with radians/degrees toggle and memory indicator
    wxBoxSizer* controlSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Toggle for radians vs degrees
    radianCheck = new wxCheckBox(panel, wxID_ANY, "Use Radians");
    controlSizer->Add(radianCheck, 0, wxALIGN_LEFT | wxALL, 5);
    
    // Memory value display
    memIndicator = new wxStaticText(panel, wxID_ANY, "Memory: 0");
    controlSizer->Add(memIndicator, 0, wxALIGN_RIGHT | wxALL, 5);
    
    mainSizer->Add(controlSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // ---- Button area ----
    
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Left side: Memory and function buttons
    wxGridSizer* memoryGrid = new wxGridSizer(7, 2, 3, 3);
    
    // Define memory and special function buttons
    const wxString memoryButtons[] = {
        "MC", "MR",        // Memory clear, recall
        "M+", "M-",        // Memory add, subtract
        "MS", "Ans",       // Memory store, last answer
        "ln", "log",       // Natural log, log base 10
        "e^x", "10^x",     // Exponential functions
        "x^2", "x^3",      // Square and cube
        "sqrt", "cbrt"     // Square root and cube root
    };
    
    // Create memory and function buttons
    for (const wxString& label : memoryButtons) {
        wxButton* btn = CreateButton(panel, label, wxColour(180, 180, 220));
        
        // Attach event handlers based on button type
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
    
    // Main calculator buttons
    wxGridSizer* mainGrid = new wxGridSizer(6, 5, 3, 3);
    
    // Define main calculator buttons
    const wxString mainButtons[] = {
        // Row 1: Trigonometric functions and parentheses
        "sin", "cos", "tan", "(", ")",
        
        // Row 2: Inverse trig functions and constants
        "asin", "acos", "atan", "π", "e",
        
        // Row 3: Numbers and operations
        "7", "8", "9", "/", "C",
        
        // Row 4: Numbers and operations
        "4", "5", "6", "*", "⌫",
        
        // Row 5: Numbers and operations
        "1", "2", "3", "-", "±",
        
        // Row 6: Zero, decimal, equals, operations
        "0", ".", "=", "+", "^"
    };
    
    // Create main calculator buttons
    for (const wxString& label : mainButtons) {
        wxColour color = wxNullColour;
        
        // Color-code buttons by function
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
        else if (label == ".") {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnDecimal, this);
        }
        else if (label.Length() == 1 && wxString("0123456789").Contains(label)) {
            btn->Bind(wxEVT_BUTTON, &CalculatorFrame::OnNumber, this);
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
    
    // Set up the panel with the main sizer
    panel->SetSizer(mainSizer);
    Center();
}

// Create a styled button with consistent formatting
wxButton* CalculatorFrame::CreateButton(wxPanel* panel, const wxString& label, wxColour color) {
    wxButton* btn = new wxButton(panel, wxID_ANY, label);
    btn->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    
    if (color != wxNullColour) {
        btn->SetBackgroundColour(color);
    }
    
    return btn;
}

// Handle number button clicks
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

// Handle operator button clicks
void CalculatorFrame::OnOperator(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    wxString op = btn->GetLabel();
    
    // If we have an existing number, process it first
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
    // Simple case (no parentheses)
    if (operationStack.empty()) {
        double secondValue = GetDisplayValue();
        double result = currentValue;
        
        // Perform the operation
        if (currentOperation == "+") {
            result += secondValue;
        } 
        else if (currentOperation == "-") {
            result -= secondValue;
        }
        else if (currentOperation == "*") {
            result *= secondValue;
        }
        else if (currentOperation == "/") {
            if (secondValue != 0) {
                result /= secondValue;
            } else {
                wxMessageBox("Division by zero is not allowed", "Error", wxICON_ERROR);
                OnClear(event);
                return;
            }
        }
        else if (currentOperation == "^") {
            result = pow(result, secondValue);
        }
        
        // Display the result
        UpdateDisplay(result);
        lastAnswer = result;
    } 
    // Complex case with parentheses
    else {
        double result = GetDisplayValue();
        
        // Process operations from the stack
        while (!operationStack.empty()) {
            wxString op = operationStack.back();
            operationStack.pop_back();
            
            double val = valueStack.back();
            valueStack.pop_back();
            
            // Perform the operation
            if (op == "+") {
                result = val + result;
            }
            else if (op == "-") {
                result = val - result;
            }
            else if (op == "*") {
                result = val * result;
            }
            else if (op == "/") {
                if (result != 0) {
                    result = val / result;
                } else {
                    wxMessageBox("Division by zero is not allowed", "Error", wxICON_ERROR);
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
    
    // Convert to radians if needed
    value = ConvertAngle(value);
    
    // Calculate the result
    if (btn->GetLabel() == "sin") {
        result = sin(value);
    }
    else if (btn->GetLabel() == "cos") {
        result = cos(value);
    }
    else if (btn->GetLabel() == "tan") {
        // Check for undefined values (multiples of π/2)
        if (fabs(fmod(value, M_PI)) < 1e-10 && fabs(fmod(value, M_PI_2)) < 1e-10) {
            wxMessageBox("Tangent is undefined at this value", "Error", wxICON_ERROR);
            return;
        }
        result = tan(value);
    }
    
    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

// Handle inverse trigonometric function buttons
void CalculatorFrame::OnInverseTrigFunction(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;
    
    // Handle domain errors
    if (btn->GetLabel() == "asin" && (value < -1 || value > 1)) {
        wxMessageBox("Arcsin domain error: value must be between -1 and 1", "Error", wxICON_ERROR);
        return;
    }
    else if (btn->GetLabel() == "acos" && (value < -1 || value > 1)) {
        wxMessageBox("Arccos domain error: value must be between -1 and 1", "Error", wxICON_ERROR);
        return;
    }
    
    // Calculate inverse trig function (result is in radians)
    if (btn->GetLabel() == "asin") {
        result = asin(value);
    }
    else if (btn->GetLabel() == "acos") {
        result = acos(value);
    }
    else if (btn->GetLabel() == "atan") {
        result = atan(value);
    }
    
    // Convert to degrees if needed
    if (!radianCheck->GetValue()) {
        result = result * 180.0 / M_PI;
    }
    
    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}

// Handle clear button
void CalculatorFrame::OnClear(wxCommandEvent& event) {
    display->SetValue("0");
    currentValue = 0.0;
    newNumber = true;
    
    // Clear stacks for parentheses
    valueStack.clear();
    operationStack.clear();
}

// Handle backspace button
void CalculatorFrame::OnBackspace(wxCommandEvent& event) {
    wxString current = display->GetValue();
    
    if (current.Length() > 1) {
        current.RemoveLast();
        display->SetValue(current);
    } 
    else {
        display->SetValue("0");
        newNumber = true;
    }
}

// Handle decimal point button
void CalculatorFrame::OnDecimal(wxCommandEvent& event) {
    if (newNumber) {
        display->SetValue("0.");
        newNumber = false;
    }
    else if (!display->GetValue().Contains(".")) {
        display->AppendText(".");
    }
}

// Handle memory operation buttons
void CalculatorFrame::OnMemoryOperation(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    wxString op = btn->GetLabel();
    
    // Perform the memory operation
    if (op == "MC") {  // Memory Clear
        memoryValue = 0.0;
    }
    else if (op == "MR") {  // Memory Recall
        UpdateDisplay(memoryValue);
        newNumber = true;
    }
    else if (op == "M+") {  // Memory Add
        memoryValue += GetDisplayValue();
    }
    else if (op == "M-") {  // Memory Subtract
        memoryValue -= GetDisplayValue();
    }
    else if (op == "MS") {  // Memory Store
        memoryValue = GetDisplayValue();
    }
    
    // Update the memory display
    UpdateMemoryDisplay();
}

// Handle scientific functions
void CalculatorFrame::OnScientificFunction(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;
    
    // Calculate the result on the function
    if (btn->GetLabel() == "ln") {
        if (value > 0) {
            result = log(value);
        } else {
            wxMessageBox("Natural log requires a positive value", "Error", wxICON_ERROR);
            return;
        }
    }
    else if (btn->GetLabel() == "log") {
        if (value > 0) {
            result = log10(value);
        } else {
            wxMessageBox("Log requires a positive value", "Error", wxICON_ERROR);
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

// Handle constant butons
void CalculatorFrame::OnConstant(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = 0.0;
    
    // Set the constant value
    if (btn->GetLabel() == "π") {
        value = M_PI;
    }
    else if (btn->GetLabel() == "e") {
        value = M_E;  // Euler's number
    }
    
    UpdateDisplay(value);
    newNumber = true;
}

// Handle the negate button
void CalculatorFrame::OnNegate(wxCommandEvent& event) {
    double value = GetDisplayValue();
    UpdateDisplay(-value);
}



void CalculatorFrame::OnAnswer(wxCommandEvent& event) {
    UpdateDisplay(lastAnswer);
    newNumber = true;
}

// Handle power functions
void CalculatorFrame::OnPower(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;
    
    // Calculate the result
    if (btn->GetLabel() == "x^2") {
        result = pow(value,2);

    }
    else if (btn->GetLabel() == "x^3") {
        result = pow(value,3);
    }
    
    UpdateDisplay(result);
    lastAnswer = result;
    newNumber = true;
}



void CalculatorFrame::OnSquareRoot(wxCommandEvent& event) {
    wxButton* btn = dynamic_cast<wxButton*>(event.GetEventObject());
    double value = GetDisplayValue();
    double result = 0.0;
    
    // Calculate the result
    if (btn->GetLabel() == "sqrt") {
        if (value >= 0) {
            result = sqrt(value);
        } else {
            wxMessageBox("Cannot take square root of a negative number", "Error", wxICON_ERROR);
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

// Handle parenthesis buttons
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
        currentOperation = "+";  // Default operation
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

// Update the display with a numeric value
void CalculatorFrame::UpdateDisplay(double value) {
    // Format the number nicely
    wxString output;
    
    // Handle very small numbers near zero.Lie really small
    if (fabs(value) < 1e-12) {
        output = "0";
    }
    // Handle large numbers or small numbers
    else if (fabs(value) > 1e9 || fabs(value) < 1e-6) {
        output = wxString::Format("%.6e", value);
    }
    
    else {
        output = wxString::Format("%.12f", value);
        
        
        while (output.Contains(".") && (output.Last() == '0' || output.Last() == '.')) {
            output.RemoveLast();
        }
    }
    
    display->SetValue(output);
}


double CalculatorFrame::ConvertAngle(double angle) {
    if (!radianCheck->GetValue()) { 
        return angle * M_PI / 180.0;
    }
    return angle;
}
//

double CalculatorFrame::GetDisplayValue() {
    return wxAtof(display->GetValue());
}

void CalculatorFrame::UpdateMemoryDisplay() {
    wxString memText;
    
    if (memoryValue == 0.0) {
        memText = "Memory: 0";
    } else {
        memText = wxString::Format("Memory: %.6g", memoryValue);
    }
    
    memIndicator->SetLabel(memText);
}

// Execute a peduing operation
void CalculatorFrame::ExecuteOperation(const wxString& op, double value) {
    double stackValue = valueStack.back();
    valueStack.pop_back();
    
    wxString stackOp = operationStack.back();
    operationStack.pop_back();
    
    double result = 0.0;
    
    if (stackOp == "+") {
        result = stackValue + value;
    }
    else if (stackOp == "-") {
        result = stackValue - value;
    }
    else if (stackOp == "*") {
        result = stackValue * value;
    }
    else if (stackOp == "/") {
        if (value != 0) {
            result = stackValue / value;
        } else {
            wxMessageBox("Divided by zero", "Error", wxICON_ERROR);
            return;
        }
    }
    else if (stackOp == "^") {
        result = pow(stackValue, value);
    }
    
    // Push the result back for future operations
    valueStack.push_back(result);
    operationStack.push_back(op);
}
