#include <iostream>

using namespace std;[cite: 1]

class Polynomial; // 前向宣告

class Term {
    friend Polynomial;
private:
    float coef; // 係數 (coefficient)
    int exp;    // 指數 (exponent)
};

class Polynomial {
private:
    Term *termArray; // 非零項的陣列 (array of nonzero terms)
    int capacity;    // 陣列容量大小 (size of termArray)
    int terms;       // 非零項數量 (number of nonzero terms)

public:
    // 建構子：初始化預設容量與項數
    Polynomial() {
        capacity = 1;
        terms = 0;
        termArray = new Term[capacity];
    }

    // 解構子：釋放動態配置的記憶體
    ~Polynomial() {
        delete[] termArray;
    }

    // 宣告友元函式 (Friend functions) 以多載輸入與輸出運算子
    friend istream& operator>>(istream& is, Polynomial& p);
    friend ostream& operator<<(ostream& os, const Polynomial& p);
};

// 多載輸入運算子 >>
istream& operator>>(istream& is, Polynomial& p) {
    // 讀取非零項的數量
    is >> p.terms;

    // 若輸入的項數超過目前容量，則重新配置記憶體
    if (p.terms > p.capacity) {
        p.capacity = p.terms;
        delete[] p.termArray;
        p.termArray = new Term[p.capacity];
    }

    // 依序讀取每一項的係數與指數
    for (int i = 0; i < p.terms; ++i) {
        is >> p.termArray[i].coef >> p.termArray[i].exp;
    }

    return is;
}

// 多載輸出運算子 <<
ostream& operator<<(ostream& os, const Polynomial& p) {
    if (p.terms == 0) {
        os << "0";
        return os;
    }

    for (int i = 0; i < p.terms; ++i) {
        float coef = p.termArray[i].coef;
        int exp = p.termArray[i].exp;

        if (coef == 0) continue; // 若係數為 0 則跳過

        // 處理正負號與項與項之間的運算符號
        if (i > 0 && coef > 0) {
            os << " + ";
        } else if (coef < 0) {
            os << " - ";
            coef = -coef; // 轉換為正值以便輸出絕對值
        }

        // 輸出係數（當係數不為 1 或指數為 0 時輸出）
        if (coef != 1 || exp == 0) {
            os << coef;
        }

        // 輸出變數與指數
        if (exp > 0) {
            os << "x";
            if (exp > 1) {
                os << "^" << exp;
            }
        }
    }

    return os;
}

// 測試用的 main 函式
int main() {
    Polynomial p;
    
    cout << "請輸入多項式（格式：先輸入非零項數，接著依序輸入各項的「係數 指數」）：\n";
    cin >> p; // 測試輸入運算子 >>
    
    cout << "您輸入的多項式為： " << p << endl; // 測試輸出運算子 <<
    
    return 0;
}