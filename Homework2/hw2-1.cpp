#include <iostream>
#include <cmath>
#include <algorithm>

using namespace std;

class Polynomial; // 前置宣告[cite: 2]

class Term {
    friend Polynomial;
private:
    float coef; // 係數[cite: 2]
    int exp;    // 指數[cite: 2]
};

class Polynomial {
private:
    Term *termArray; // 非零項的動態陣列[cite: 2]
    int capacity;    // termArray 的配置容量大小[cite: 2]
    int terms;       // 目前非零項的數量[cite: 2]

    // 輔助函數：在維持指數降冪排列的情況下加入新項
    void NewTerm(float theCoef, int theExp) {
        if (theCoef == 0.0f) return;
        if (terms == capacity) {
            capacity *= 2;
            Term *temp = new Term[capacity];
            for (int i = 0; i < terms; i++) {
                temp[i] = termArray[i];
            }
            delete[] termArray;
            termArray = temp;
        }
        termArray[terms].coef = theCoef;
        termArray[terms].exp = theExp;
        terms++;
    }

public:
    // 建構子：建構多項式 p(x) = 0[cite: 1]
    Polynomial() {
        capacity = 10;
        terms = 0;
        termArray = new Term[capacity];
    }

    // 複製建構子（遵循三權法則以確保動態記憶體安全）
    Polynomial(const Polynomial &other) {
        capacity = other.capacity;
        terms = other.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++) {
            termArray[i] = other.termArray[i];
        }
    }

    // 解構子
    ~Polynomial() {
        delete[] termArray;
    }

    // 複製指定運算子
    Polynomial& operator=(const Polynomial &other) {
        if (this == &other) return *this;
        delete[] termArray;
        capacity = other.capacity;
        terms = other.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++) {
            termArray[i] = other.termArray[i];
        }
        return *this;
    }

    // 用於手動加入項目的公開方法（方便建立與測試多項式）
    void AddTerm(float coef, int exp) {
        NewTerm(coef, exp);
    }

    // 傳回多項式 *this 與 poly 的總和[cite: 1]
    Polynomial Add(Polynomial poly) {
        Polynomial c;
        int aPos = 0, bPos = 0;
        
        while ((aPos < terms) && (bPos < poly.terms)) {
            if (termArray[aPos].exp == poly.termArray[bPos].exp) {
                float sum = termArray[aPos].coef + poly.termArray[bPos].coef;
                if (sum != 0.0f) {
                    c.NewTerm(sum, termArray[aPos].exp);
                }
                aPos++;
                bPos++;
            } else if (termArray[aPos].exp < poly.termArray[bPos].exp) {
                c.NewTerm(poly.termArray[bPos].coef, poly.termArray[bPos].exp);
                bPos++;
            } else {
                c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
                aPos++;
            }
        }

        // 加入 *this 剩餘的項
        for (; aPos < terms; aPos++) {
            c.NewTerm(termArray[aPos].coef, termArray[aPos].exp);
        }

        // 加入 poly 剩餘的項
        for (; bPos < poly.terms; bPos++) {
            c.NewTerm(poly.termArray[bPos].coef, poly.termArray[bPos].exp);
        }

        return c;
    }

    // 傳回多項式 *this 與 poly 的乘積[cite: 1]
    Polynomial Mult(Polynomial poly) {
        Polynomial c;
        for (int i = 0; i < terms; i++) {
            Polynomial temp;
            for (int j = 0; j < poly.terms; j++) {
                float newCoef = termArray[i].coef * poly.termArray[j].coef;
                int newExp = termArray[i].exp + poly.termArray[j].exp;
                temp.NewTerm(newCoef, newExp);
            }
            c = c.Add(temp);
        }
        return c;
    }

    // 在 f 處評估多項式 *this 並傳回結果[cite: 1]
    float Eval(float f) {
        float result = 0.0f;
        for (int i = 0; i < terms; i++) {
            result += termArray[i].coef * pow(f, termArray[i].exp);
        }
        return result;
    }

    // 用於印出多項式的公用工具函數
    void Display() const {
        if (terms == 0) {
            cout << "0" << endl;
            return;
        }
        for (int i = 0; i < terms; i++) {
            cout << termArray[i].coef << "x^" << termArray[i].exp;
            if (i < terms - 1 && termArray[i + 1].coef > 0) {
                cout << " + ";
            } else if (i < terms - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }
};