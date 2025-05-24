// wx_calculator.cpp
// Scientific calculator - Euclidae (human-revised)
// Features: Basic ops, scientific functions, memory, parentheses
// Layout fixes applied

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <cmath>

class CalculatorApp : public wxApp {
public:
    bool OnInit() override;
};

class CalculatorFrame : public wxFrame {
    wxTextCtrl* display;
    wxCheckBox* radianMode;
    wxStaticText* memDisplay;
    
    double currentValue;
    wxString activeOp;
    bool newInput;
    double memoryStore;
    double lastResult;
    
    // Parenthesis state
    double storedValue;
    wxString storedOp;

public:
    CalculatorFrame(const wxString& title);

private:
    // UI setup
    void AddBtn(wxSizer* grid, wxWindow* parent, 
               const wxString& label, const wxColour& col, 
               void (CalculatorFrame::*handler)(wxCommandEvent&));

    // Event handlers
    void OnNum(wxCommandEvent& e);
    void OnOp(wxCommandEvent& e);
    void OnEqual(wxCommandEvent& e);
    void OnClear(wxCommandEvent& e);
    void OnBack(wxCommandEvent& e);
    void OnDecimal(wxCommandEvent& e);
    void OnTrig(wxCommandEvent& e);
    void OnMemory(wxCommandEvent& e);
    void OnSignSwap(wxCommandEvent& e);
    void OnSciFunc(wxCommandEvent& e);
    void OnParen(wxCommandEvent& e);
    void OnConst(wxCommandEvent& e);
    
    // Helpers
    double GetVal();
    void UpdateDisplay(double v);
    void UpdateMem();
    double ConvertAngle(double a);
};

wxIMPLEMENT_APP(CalculatorApp);

bool CalculatorApp::OnInit() {
    auto* frame = new CalculatorFrame("SciCalc v2.0");
    frame->Show(true);
    return true;
}

CalculatorFrame::CalculatorFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(500, 550)),
      currentValue(0), newInput(true), memoryStore(0), lastResult(0),
      storedValue(0), storedOp("+") {

    wxPanel* mainPanel = new wxPanel(this);
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Display
    display = new wxTextCtrl(mainPanel, wxID_ANY, "0", 
                           wxDefaultPosition, wxSize(-1, 70),
                           wxTE_RIGHT | wxTE_READONLY);
    display->SetFont(wxFont(22, wxFONTFAMILY_TELETYPE, 
                          wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    mainSizer->Add(display, 0, wxEXPAND | wxALL, 5);

    // Config row
    wxBoxSizer* configSizer = new wxBoxSizer(wxHORIZONTAL);
    radianMode = new wxCheckBox(mainPanel, wxID_ANY, "Radians");
    memDisplay = new wxStaticText(mainPanel, wxID_ANY, "[Mem: 0]");
    configSizer->Add(radianMode, 0, wxLEFT|wxRIGHT, 10);
    configSizer->AddStretchSpacer();
    configSizer->Add(memDisplay, 0, wxALIGN_CENTER | wxRIGHT, 10);
    mainSizer->Add(configSizer, 0, wxEXPAND | wxBOTTOM, 5);

    // Button grid (5x5)
    wxGridSizer* btnGrid = new wxGridSizer(5, 5, 3, 3);

    // Left-side buttons
    const wxString leftBtns[] = {
        "sin", "cos", "tan", "(", ")",
        "asin", "acos", "atan", "π", "e",
        "MC", "MR", "M+", "M-", "MS",
        "ln", "log", "x^2", "x^3", "Ans",
        "sqrt", "±", "⌫", "C", "10^x"
    };
    
    for (const auto& lbl : leftBtns) {
        wxColour col = *wxLIGHT_GREY;
        void (CalculatorFrame::*handler)(wxCommandEvent&) = nullptr;
        
        if (lbl == "C") {
            col = wxColour(255, 150, 150);
            handler = &CalculatorFrame::OnClear;
        }
        else if (lbl == "⌫") handler = &CalculatorFrame::OnBack;
        else if (lbl == "±") handler = &CalculatorFrame::OnSignSwap;
        else if (lbl.StartsWith("M")) {
            col = wxColour(180, 230, 180);
            handler = &CalculatorFrame::OnMemory;
        }
        else if (lbl == "π" || lbl == "e") handler = &CalculatorFrame::OnConst;
        else if (lbl == "(" || lbl == ")") handler = &CalculatorFrame::OnParen;
        else if (lbl == "sin" || lbl == "cos" || lbl == "tan" ||
                 lbl == "asin" || lbl == "acos" || lbl == "atan") {
            col = wxColour(255, 255, 150);
            handler = &CalculatorFrame::OnTrig;
        }
        else handler = &CalculatorFrame::OnSciFunc;

        AddBtn(btnGrid, mainPanel, lbl, col, handler);
    }

    // Right-side buttons
    const wxString rightBtns[] = {
        "7", "8", "9", "/", "^",
        "4", "5", "6", "*", "e^x",
        "1", "2", "3", "-", "log",
        "0", ".", "=", "+", "ln"
    };

    for (const auto& lbl : rightBtns) {
        wxColour col = *wxLIGHT_GREY;
        void (CalculatorFrame::*handler)(wxCommandEvent&) = nullptr;

        if (lbl == "=") {
            col = wxColour(150, 255, 150);
            handler = &CalculatorFrame::OnEqual;
        }
        else if (lbl == ".") handler = &CalculatorFrame::OnDecimal;
        else if (wxString("0123456789").Contains(lbl)) handler = &CalculatorFrame::OnNum;
        else handler = &CalculatorFrame::OnOp;

        AddBtn(btnGrid, mainPanel, lbl, col, handler);
    }

    mainSizer->Add(btnGrid, 1, wxEXPAND | wxALL, 5);
    mainPanel->SetSizer(mainSizer);
    Centre();
}

void CalculatorFrame::AddBtn(wxSizer* grid, wxWindow* parent,
                            const wxString& label, const wxColour& col,
                            void (CalculatorFrame::*handler)(wxCommandEvent&)) {
    wxButton* btn = new wxButton(parent, wxID_ANY, label);
    btn->SetBackgroundColour(col);
    btn->Bind(wxEVT_BUTTON, handler, this);
    grid->Add(btn, 1, wxEXPAND);
}

// Event Handlers Implementation

void CalculatorFrame::OnNum(wxCommandEvent& e) {
    wxString num = ((wxButton*)e.GetEventObject())->GetLabel();
    
    if (newInput) {
        display->SetValue(num);
        newInput = false;
    }
    else {
        if (display->GetValue() == "0") display->SetValue(num);
        else display->AppendText(num);
    }
}

void CalculatorFrame::OnOp(wxCommandEvent& e) {
    wxString op = ((wxButton*)e.GetEventObject())->GetLabel();
    double val = GetVal();
    
    if (!newInput) {
        if (activeOp.empty()) currentValue = val;
        else {
            if (activeOp == "+") currentValue += val;
            else if (activeOp == "-") currentValue -= val;
            else if (activeOp == "*") currentValue *= val;
            else if (activeOp == "/") {
                if (val == 0) {
                    wxMessageBox("Can't divide by zero!", "Error", wxICON_ERROR);
                    return;
                }
                currentValue /= val;
            }
            else if (activeOp == "^") currentValue = pow(currentValue, val);
        }
        UpdateDisplay(currentValue);
    }
    
    activeOp = op;
    newInput = true;
}

void CalculatorFrame::OnEqual(wxCommandEvent& e) {
    if (activeOp.empty()) return;
    
    double val = GetVal();
    if (activeOp == "+") currentValue += val;
    else if (activeOp == "-") currentValue -= val;
    else if (activeOp == "*") currentValue *= val;
    else if (activeOp == "/") {
        if (val == 0) {
            wxMessageBox("Division by zero!", "Error", wxICON_ERROR);
            return;
        }
        currentValue /= val;
    }
    else if (activeOp == "^") currentValue = pow(currentValue, val);
    
    UpdateDisplay(currentValue);
    lastResult = currentValue;
    activeOp.clear();
    newInput = true;
}

void CalculatorFrame::OnClear(wxCommandEvent& e) {
    display->SetValue("0");
    currentValue = 0;
    activeOp.clear();
    newInput = true;
    storedValue = 0;
    storedOp = "+";
}

void CalculatorFrame::OnBack(wxCommandEvent& e) {
    wxString cur = display->GetValue();
    if (cur.Length() > 1) {
        cur.RemoveLast();
        display->SetValue(cur);
    }
    else {
        display->SetValue("0");
        newInput = true;
    }
}

void CalculatorFrame::OnDecimal(wxCommandEvent& e) {
    if (!display->GetValue().Contains(".")) {
        display->AppendText(".");
        newInput = false;
    }
}

void CalculatorFrame::OnTrig(wxCommandEvent& e) {
    wxString func = ((wxButton*)e.GetEventObject())->GetLabel();
    double val = ConvertAngle(GetVal());
    double result = 0;
    
    if (func == "sin") result = sin(val);
    else if (func == "cos") result = cos(val);
    else if (func == "tan") {
        if (fmod(val, M_PI/2) == 0 && fmod(val, M_PI) != 0) {
            wxMessageBox("Undefined tangent value!", "Error", wxICON_ERROR);
            return;
        }
        result = tan(val);
    }
    else if (func == "asin") result = asin(GetVal());
    else if (func == "acos") result = acos(GetVal());
    else if (func == "atan") result = atan(GetVal());
    
    if (func.StartsWith("a") && !radianMode->GetValue()) {
        result = result * 180.0 / M_PI;
    }
    
    UpdateDisplay(result);
    newInput = true;
}

void CalculatorFrame::OnMemory(wxCommandEvent& e) {
    wxString cmd = ((wxButton*)e.GetEventObject())->GetLabel();
    double val = GetVal();
    
    if (cmd == "MC") memoryStore = 0;
    else if (cmd == "MR") UpdateDisplay(memoryStore);
    else if (cmd == "M+") memoryStore += val;
    else if (cmd == "M-") memoryStore -= val;
    else if (cmd == "MS") memoryStore = val;
    
    UpdateMem();
    newInput = true;
}

void CalculatorFrame::OnSignSwap(wxCommandEvent& e) {
    double val = GetVal();
    UpdateDisplay(-val);
}

void CalculatorFrame::OnSciFunc(wxCommandEvent& e) {
    wxString func = ((wxButton*)e.GetEventObject())->GetLabel();
    double val = GetVal();
    double result = 0;
    
    if (func == "ln") result = log(val);
    else if (func == "log") result = log10(val);
    else if (func == "x^2") result = val * val;
    else if (func == "x^3") result = val * val * val;
    else if (func == "sqrt") result = sqrt(val);
    else if (func == "10^x") result = pow(10, val);
    else if (func == "e^x") result = exp(val);
    
    UpdateDisplay(result);
    newInput = true;
}

void CalculatorFrame::OnParen(wxCommandEvent& e) {
    wxString paren = ((wxButton*)e.GetEventObject())->GetLabel();
    
    if (paren == "(") {
        storedValue = currentValue;
        storedOp = activeOp;
        currentValue = 0;
        activeOp = "+";
    }
    else {
        OnEqual(e);
        currentValue = storedValue;
        activeOp = storedOp;
    }
    newInput = true;
}

void CalculatorFrame::OnConst(wxCommandEvent& e) {
    wxString c = ((wxButton*)e.GetEventObject())->GetLabel();
    UpdateDisplay(c == "π" ? M_PI : M_E);
    newInput = true;
}

// Helper Methods

double CalculatorFrame::GetVal() {
    double val;
    display->GetValue().ToDouble(&val);
    return val;
}

void CalculatorFrame::UpdateDisplay(double v) {
    wxString output;
    
    if (v == floor(v) && abs(v) < 1e12) {
        output.Printf("%.0f", v);
    }
    else if (abs(v) > 1e9 || abs(v) < 1e-9) {
        output.Printf("%.4e", v);
    }
    else {
        output.Printf("%.10g", v);
    }
    
    display->SetValue(output);
}

void CalculatorFrame::UpdateMem() {
    memDisplay->SetLabel(wxString::Format("[Mem: %s]", 
        memoryStore == floor(memoryStore) ?
        wxString::Format("%.0f", memoryStore) :
        wxString::Format("%.6g", memoryStore));
}

double CalculatorFrame::ConvertAngle(double a) {
    return radianMode->GetValue() ? a : a * M_PI / 180.0;
}
