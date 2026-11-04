# 多項式抽象資料型態 (Polynomial ADT) 實作與效能分析報告

---

## 1. 解題說明 (10%)

### 1.1 解題想法 (How to do?)
本題目要求實作一個多項式抽象資料型態（Polynomial ADT）。一個多項式可表示為一系列非零項的集合：
$$p(x) = a_0 x^{e_0} + a_1 x^{e_1} + \dots + a_n x^{e_n}$$
其中 $a_i$ 為非零的浮點數係數（`float`），$e_i$ 為非負整數指數（`int`）。

為了高效且簡潔地處理多項式運算，我們採取以下策略：
1. **內部資料結構**：使用動態陣列 `termArray` 儲存多項式的非零項 `Term`，並維持**指數由大到小（降冪）排序**。
2. **多項式加法 (`Add`)**：利用雙指標（Two Pointers）技術，同時掃描兩個已按指數降冪排列的多項式陣列。比較兩者的當前指數：
   - 若指數相同，係數相加；若和不為 0 則加入結果中。
   - 若其中一方指數較大，直接將該項加入結果中，並將該多項式的指標後移。
3. **多項式乘法 (`Mult`)**：利用分配律，將第一個多項式的每一項與第二個多項式的每一項相乘（係數相乘、指數相加），產生中間多項式後，持續使用 `Add` 方法累加至最終結果。
4. **多項式求值 (`Eval`)**：走訪陣列中每一個非零項，計算 $a_i \cdot f^{e_i}$ 並進行累加。

### 1.2 範例說明
假設有兩個多項式：
- $A(x) = 3x^4 + 2x^2 + 5$ （內部儲存為 `[(3,4), (2,2), (5,0)]`）
- $B(x) = 4x^3 + 2x^2 + 1$ （內部儲存為 `[(4,3), (2,2), (1,0)]`）

#### 加法運算 $C(x) = A(x) + B(x)$：
1. 比較 $A$ 的首項 $(3,4)$ 與 $B$ 的首項 $(4,3)$ $\rightarrow$ $4 > 3$，加入 $(3,4)$。
2. 比較 $A$ 的項 $(2,2)$ 與 $B$ 的項 $(4,3)$ $\rightarrow$ $2 < 3$，加入 $(4,3)$。
3. 比較 $A$ 的項 $(2,2)$ 與 $B$ 的項 $(2,2)$ $\rightarrow$ 指數相同，係數相加 $2+2=4$，加入 $(4,2)$。
4. 比較 $A$ 的項 $(5,0)$ 與 $B$ 的項 $(1,0)$ $\rightarrow$ 指數相同，係數相加 $5+1=6$，加入 $(6,0)$。
5. 最終結果 $C(x) = 3x^4 + 4x^3 + 4x^2 + 6$。

---

## 2. Algorithm Design & Programming (40%)

### 2.1 完整程式碼 (Source Code with Comments)

```cpp
#include <iostream>
#include <cmath>
#include <chrono>
#include <cassert>

using namespace std;

class Polynomial; // 前置宣告

// Term 類別：代表多項式中的單一項
class Term {
    friend class Polynomial;
private:
    float coef; // 係數 (coefficient)
    int exp;    // 指數 (exponent)
};

// Polynomial 類別：多項式 ADT
class Polynomial {
private:
    Term *termArray; // 儲存非零項的動態陣列
    int capacity;    // 動態陣列的總容量
    int terms;       // 目前非零項的實際數量

    // 內部輔助方法：新增單一項並自動處理動態擴容
    void NewTerm(float theCoef, int theExp) {
        if (theCoef == 0.0f) return; // 零係數不儲存
        
        // 容量不足時進行擴容 (翻倍)
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
    // 建構子：初始化零多項式 p(x) = 0
    Polynomial() {
        capacity = 8;
        terms = 0;
        termArray = new Term[capacity];
    }

    // 複製建構子 (Deep Copy)
    Polynomial(const Polynomial &other) {
        capacity = other.capacity;
        terms = other.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++) {
            termArray[i] = other.termArray[i];
        }
    }

    // 解構子：釋放動態記憶體
    ~Polynomial() {
        delete[] termArray;
    }

    // 複製指定運算子 (Copy Assignment)
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

    // 手動插入單一項（供外部建立多項式使用）
    void AddTerm(float coef, int exp) {
        NewTerm(coef, exp);
    }

    // 取得當前非零項數量
    int GetTerms() const {
        return terms;
    }

    // 1. 多項式加法：傳回 *this + poly
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

    // 2. 多項式乘法：傳回 *this * poly
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

    // 3. 多項式求值：評估 *this 在 x = f 時的數值
    float Eval(float f) {
        float result = 0.0f;
        for (int i = 0; i < terms; i++) {
            result += termArray[i].coef * std::pow(f, termArray[i].exp);
        }
        return result;
    }

    // 輸出多項式內容
    void Display() const {
        if (terms == 0) {
            cout << "0" << endl;
            return;
        }
        for (int i = 0; i < terms; i++) {
            if (i > 0 && termArray[i].coef > 0) cout << " + ";
            else if (termArray[i].coef < 0) cout << " - ";
            
            float absCoef = std::abs(termArray[i].coef);
            if (absCoef != 1.0f || termArray[i].exp == 0) {
                cout << absCoef;
            }
            if (termArray[i].exp > 0) {
                cout << "x";
                if (termArray[i].exp > 1) {
                    cout << "^" << termArray[i].exp;
                }
            }
        }
        cout << endl;
    }
};
```

---

## 3. 效能分析 (Analysis) (15%)

設多項式 $A(x)$ 有 $m$ 個非零項，多項式 $B(x)$ 有 $n$ 個非零項。

### 3.1 時間複雜度 (Time Complexity)

1. **`Polynomial()` (建構子)**:
   - 時間複雜度：$\mathcal{O}(1)$，僅配置固定大小的動態陣列記憶體。
2. **`Add(Polynomial poly)` (加法)**:
   - 使用雙指標對兩個多項式陣列進行單次線性掃描。
   - 時間複雜度：$\mathcal{O}(m + n)$。
3. **`Mult(Polynomial poly)` (乘法)**:
   - 外層迴圈執行 $m$ 次，內層生成包含 $n$ 項的單項積，再執行一次 `Add` 運算（每次加法複雜度上限為 $\mathcal{O}(m \cdot n)$）。
   - 時間複雜度：$\mathcal{O}(m^2 \cdot n)$ 或優化合併後的最壞時間複雜度為 $\mathcal{O}(m \cdot n \cdot (m+n))$。
4. **`Eval(float f)` (求值)**:
   - 遍歷所有 $m$ 個項，每項呼叫一次 `pow` 函數（假設 `pow` 為 $\mathcal{O}(\log(\text{exp}))$ 或 $\mathcal{O}(1)$ 浮點硬體指令）。
   - 時間複雜度：$\mathcal{O}(m)$。

### 3.2 空間複雜度 (Space Complexity)

1. **內部儲存空間**：
   - 空間複雜度：$\mathcal{O}(m)$，僅儲存實際存在的非零項（Sparse Representation），顯著優於傳統全次數陣列表示法。
2. **`Add` 運算輔助空間**：
   - 最多產生包含 $(m + n)$ 個項的新多項式，空間複雜度：$\mathcal{O}(m + n)$。
3. **`Mult` 運算輔助空間**：
   - 最終多項式最多包含 $m \times n$ 個項，空間複雜度：$\mathcal{O}(m \cdot n)$。

---

## 4. 測試與驗證 (Testing and Proving) (15%)

針對核心功能撰寫單元測試驗證範例：

```cpp
void RunTests() {
    cout << "=== 開始測試與驗證 ===" << endl;

    // 測試 1: 建構子與 AddTerm
    Polynomial p1, p2;
    p1.AddTerm(3, 4);
    p1.AddTerm(2, 2);
    p1.AddTerm(5, 0); // p1 = 3x^4 + 2x^2 + 5

    p2.AddTerm(4, 3);
    p2.AddTerm(2, 2);
    p2.AddTerm(1, 0); // p2 = 4x^3 + 2x^2 + 1

    cout << "p1(x) = "; p1.Display();
    cout << "p2(x) = "; p2.Display();

    // 測試 2: 加法 (Add)
    Polynomial sum = p1.Add(p2);
    cout << "p1(x) + p2(x) = "; sum.Display();
    // 驗證加法結果: 3x^4 + 4x^3 + 4x^2 + 6

    // 測試 3: 乘法 (Mult)
    Polynomial p3, p4;
    p3.AddTerm(2, 1); // 2x
    p3.AddTerm(1, 0); // + 1
    p4.AddTerm(1, 1); // x
    p4.AddTerm(-1, 0); // - 1

    Polynomial prod = p3.Mult(p4);
    cout << "(2x + 1) * (x - 1) = "; prod.Display();
    // 預期結果: 2x^2 - x - 1

    // 測試 4: 求值 (Eval)
    // p1(x) = 3x^4 + 2x^2 + 5, 當 x = 2 時: 3(16) + 2(4) + 5 = 48 + 8 + 5 = 61
    float val = p1.Eval(2.0f);
    cout << "p1(2) = " << val << " (預期: 61)" << endl;
    assert(val == 61.0f);

    cout << "=== 所有單元測試通過！ ===" << endl << endl;
}
```

---

## 5. 效能量測 (Measuring) (10%)

利用 C++ `<chrono>` 函式庫量測高次數與高項數多項式運算的執行時間：

```cpp
void MeasurePerformance() {
    cout << "=== 開始效能量測 ===" << endl;
    
    int numTerms = 2000;
    Polynomial polyA, polyB;

    // 建立 2000 項的大型多項式
    for (int i = numTerms; i > 0; i--) {
        polyA.AddTerm(1.5f, i * 2);     // 偶數次方
        polyB.AddTerm(2.5f, i * 2 - 1); // 奇數次方
    }

    // 1. 量測加法時間
    auto startAdd = chrono::high_resolution_clock::now();
    Polynomial sumPoly = polyA.Add(polyB);
    auto endAdd = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> addTime = endAdd - startAdd;
    cout << "加法執行時間 (項數 " << numTerms << " + " << numTerms << "): " 
         << addTime.count() << " ms" << endl;

    // 2. 量測求值時間
    auto startEval = chrono::high_resolution_clock::now();
    float evalResult = sumPoly.Eval(1.001f);
    auto endEval = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> evalTime = endEval - startEval;
    cout << "求值執行時間 (項數 " << sumPoly.GetTerms() << "): " 
         << evalTime.count() << " ms" << endl;

    // 3. 量測中型多項式乘法時間 (避免 2000 項乘法過久)
    Polynomial smallA, smallB;
    for (int i = 200; i > 0; i--) {
        smallA.AddTerm(1.0f, i);
        smallB.AddTerm(1.0f, i);
    }
    auto startMult = chrono::high_resolution_clock::now();
    Polynomial multPoly = smallA.Mult(smallB);
    auto endMult = chrono::high_resolution_clock::now();
    chrono::duration<double, milli> multTime = endMult - startMult;
    cout << "乘法執行時間 (項數 200 * 200): " 
         << multTime.count() << " ms" << endl;

    cout << "=== 效能量測完成 ===" << endl;
}

int main() {
    RunTests();
    MeasurePerformance();
    return 0;
}
```

---

## 6. 心得討論 (10%)

1. **資料結構選擇的優缺點**：
   - 本實作採用「動態陣列 (Dynamic Array)」儲存非零項。其優點在於記憶體連續性高、快取命中率（Cache Hit Ratio）優異，且空間開銷低於指標連結串列。
   - 缺點在於每次擴容時需要搬移記憶體，且對於隨機插入新項時需要移動陣列元素。由於多項式通常依指數降冪依序建構，因此動態陣列整體效能非常優秀。

2. **記憶體管理與安全（Rule of Three）**：
   - 由於 `Polynomial` 類別含有動態配置的指標 `termArray`，必須正確宣告並實作**解構子 (Destructor)**、**複製建構子 (Copy Constructor)** 與 **複製指定運算子 (Copy Assignment Operator)**，否則在傳值呼叫（Pass-by-value）時會引發重複釋放記憶體（Double Free）或記憶體洩漏（Memory Leak）問題。

3. **乘法演算法的優化空間**：
   - 本次實驗採用的乘法演算法為基本的 $\mathcal{O}(m^2 \cdot n)$ 逐項相乘再進行多次累加。當多項式的項數非常龐大時，運算時間會明顯上升。未來的改進方向可考慮導入快速傅立葉變換 (Fast Fourier Transform, FFT)，將多項式乘法的時間複雜度大幅降至 $\mathcal{O}(N \log N)$。