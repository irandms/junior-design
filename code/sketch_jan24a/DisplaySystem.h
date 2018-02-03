#pragma once

class DisplaySystem {
    private:
        int slave_select;
    public:
        void Initialize();
        void WriteDigit(char digit, char val);
        void WriteDigits(int val, int display_num);
        void WriteAllDigits(int val);
        void WriteDouble(double val, int display_num);
        void AttachSlaveSelect(int pin);
};

