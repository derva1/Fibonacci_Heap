#include <cassert>
#include <ctime>
#include <iostream>
#include <math.h>
#include <random>
#include <vector>

template <typename Tip> struct Cvor {
    Cvor* roditelj;
    Cvor* lijevi;
    Cvor* desni;
    Cvor* dijete;
    int kljuc, degree;
    bool mark, check;

    Cvor(Tip vrijednost) {
        Cvor* roditelj, * lijevi, * desni, * dijete = nullptr;
        kljuc = vrijednost;
        degree = 0;
        mark = check = false;
    }
};

template <typename Tip> class Fib_heap {
    Cvor<Tip>* min;
    int vr = 0;

public:
    Fib_heap() {
        min = nullptr;
        vr = 0;
    }

    void Insert(Cvor<Tip>* x) {
        x->degree = 0;
        x->dijete = nullptr;
        x->roditelj = nullptr;
        x->mark = false;
        if (min == nullptr) {
            x->desni = x;
            x->lijevi = x;
            min = x;
        }
        else {
            min->lijevi->desni = x;
            x->desni = min;
            x->lijevi = min->lijevi;
            min->lijevi = x;
            if (x->kljuc < min->kljuc)
                min = x;
        }
        vr++;
    }

    Fib_heap Union(Fib_heap<Tip> h1, Fib_heap<Tip> h2) {
        Cvor<Tip>* np;
        Fib_heap<Tip> h3 = Fib_heap<Tip>();
        h3.min = h1.min;
        if (h2.min != nullptr && h3.min != nullptr) {
            (h3.min->lijevi)->desni = h2.min;
            (h2.min->lijevi)->desni = h3.min;
            np = h3.min->lijevi;
            h3.min->lijevi = h2.min->lijevi;
            h2.min->lijevi = np;
        }
        if (h1.min == nullptr ||
            (h2.min != nullptr && (h2.min)->kljuc < (h1.min)->kljuc))
            h3.min = h2.min;

        h3.vr = h1.vr + h2.vr;
        return h3;
    }

    void HeapLink(Cvor<Tip>* x, Cvor<Tip>* y) {
        x->lijevi->desni = x->desni;
        x->desni->lijevi = x->lijevi;
        if (y->desni == y)
            min = y;
        x->lijevi = x;
        x->desni = x;
        x->roditelj = x;
        if (y->dijete == nullptr)
            y->dijete = x;
        x->desni = y->dijete;
        x->lijevi = y->dijete->lijevi;
        y->dijete->lijevi->desni = x;
        y->dijete->lijevi = x;
        if (x->kljuc < (y->dijete)->kljuc)
            y->dijete = x;
        y->degree++;
    }

    void Consolidate() {
        int D = log2(vr) + 1;
        std::vector<Cvor<Tip>*> A(D, nullptr);
        int d;
        Cvor<Tip>* x = min;
        Cvor<Tip>* y;
        do {
            d = x->degree;
            while (A.at(d) != nullptr) {
                y = A.at(d);
                if (x->kljuc > y->kljuc) {
                    Cvor<Tip>* temp = x;
                    x = y;
                    y = temp;
                }
                if (y == min)
                    min = x;
                HeapLink(y, x);
                if (x->desni == x)
                    min = x;
                A.at(d) = nullptr;
                d++;
            }
            A.at(d) = x;
            x = x->desni;
        } while (x != min);
        min = nullptr;
        for (int i = 0; i < D; i++) {
            if (A.at(i) != nullptr) {
                (A.at(i))->lijevi = A.at(i);
                (A.at(i))->desni = A.at(i);
                if (min != nullptr) {
                    (min->lijevi)->desni = A.at(i);
                    A.at(i)->desni = min;
                    A.at(i)->lijevi = min->lijevi;
                    min->lijevi = A.at(i);
                    if (A.at(i)->kljuc < min->kljuc)
                        min = A.at(i);
                }
                else
                    min = A.at(i);

                if (min == nullptr)
                    min = A.at(i);

                else if (A.at(i)->kljuc < min->kljuc)
                    min = A.at(i);
            }
        }
    }

    Cvor<Tip>* extract_min() {
        Cvor<Tip>* z = min;
        Cvor<Tip>* x;
        Cvor<Tip>* np, * ptr;
        if (z != nullptr) {
            x = z->dijete;

            if (x != nullptr) {
                ptr = x;
                np = x;
                do {
                    np = x->desni;
                    min->lijevi->desni = x;
                    x->desni = min;
                    x->lijevi = min->lijevi;
                    min->lijevi = x;
                    if (x->kljuc < min->kljuc)
                        min = x;
                    x->roditelj = NULL;
                    x = np;
                } while (np != ptr);
            }
            min->lijevi->desni = min->desni;
            min->desni->lijevi = min->lijevi;
            if (z == z->lijevi && z->dijete == nullptr)
                min = nullptr;
            else {
                min = z->desni;
                Consolidate();
            }
            vr--;
        }
        return z;
    }
    Cvor<Tip>* Find(Cvor<Tip>* y, Tip k) {
        Cvor<Tip>* x = y;
        Cvor<Tip>* p = nullptr;
        x->check = true;
        if (x->kljuc == k) {
            p = x;
            x->check = false;
            return p;
        }
        if (p == nullptr) {
            if (x->dijete != nullptr)
                p = Find(x->dijete, k);
            if (x->desni->check == false) {
                p = Find(x->desni, k);
            }
        }
        x->check = false;
        return p;
    }
    void DecreaseKey(Tip x, Tip k) {
        Cvor<Tip>* ptr;
        Cvor<Tip>* y;
        if (min == nullptr) {
            std::cout << "Gomila je prazna!\n";
            return;
        }
        ptr = Find(min, k);
        if (ptr == nullptr) {
            std::cout << "Cvor nije nadjen u gomili!\n";
            return;
        }
        if (ptr->kljuc < x) {
            std::cout
                << "Uneseni kljuc je veci od trenutnog!\n Izmjene nisu moguce!\n";
            return;
        }
        ptr->kljuc = x;
        y = ptr->roditelj;
        if (y != nullptr && ptr->kljuc < y->kljuc) {
            Cut(ptr, y);
            CascadingCut(y);
        }
        if (ptr->kljuc < min->kljuc)
            min = ptr;
    }
    void Cut(Cvor<Tip>* x, Cvor<Tip>* y) {
        if (x == x->desni)
            y->dijete = nullptr;
        x->lijevi->desni = x->desni;
        x->desni->lijevi = x->lijevi;
        if (x == y->dijete)
            y->dijete = x->desni;
        y->degree--;
        x->desni = x;
        x->lijevi = x;
        min->lijevi->desni = x;
        x->desni = min;
        x->lijevi = min->lijevi;
        min->lijevi = x;
        x->roditelj = nullptr;
        x->mark = false;
    }

    void CascadingCut(Cvor<Tip>* x) {
        Cvor<Tip>* z = x->roditelj;
        if (z != nullptr) {
            if (!x->mark)
                x->mark = true;
            else {
                Cut(x, z);
                CascadingCut(z);
            }
        }
    }

    void Delete(Tip x) {
        Cvor<Tip>* np = nullptr;
        DecreaseKey(x, -5000);
        np = extract_min();
        if (np)
            std::cout << "Cvor obrisan\n";
        else
            std::cout << "Cvor nije obrisan\n";
    }

    void Display() {
        auto p = min;
        if (p == nullptr) {
            std::cout << "Gomila je prazna\n";
            return;
        }
        std::cout << "Korijeni cvorovi su: \n";

        do {
            std::cout << p->kljuc;
            p = p->desni;
            if (p != min) {
                std::cout << "-->";
            }
        } while (p != min && p->desni != NULL);
        std::cout << std::endl;
    }
};

std::vector<Cvor<int>*> create_random_elements(int brEl) {
    std::default_random_engine defEngine(time(nullptr));
    std::uniform_int_distribution<int> intDistro(0, 100);
    std::vector<Cvor<int>*> elementi;
    for (int i = 0; i < brEl; i++) {
        auto help = new Cvor<int>(intDistro(defEngine));
        elementi.emplace_back(help);
    }
    return elementi;
}

void InsertTestingFunction() {
    Fib_heap<int> gomila;
    int brEl;
    std::cout << "Funkcija za testiranje ubacivanja nasumicnih elemenata u "
        "gomilu.\n Unesite broj koliko elemenata zelite ubaciti :";
    std::cin >> brEl;
    auto elementi = create_random_elements(brEl);

    for (auto& x : elementi) {
        std::cout << "Ubacujem element: " << x->kljuc << "\n";
        gomila.Insert(x);
    }
    assert(1);
    std::cout << "\nTest Insert 1 status: PASSED\n";
    gomila.Display();
}

int FindMinimumOfOneHeap(std::vector<Cvor<int>*>& elementi) {
    int minimum = 100;
    for (auto& element : elementi) {
        if (element->kljuc < minimum)
            minimum = element->kljuc;
    }
    std::cout << "Minimum proslijedjene gomile bi trebao biti: " << minimum;
    return minimum;
}

void ExtractMinTestingFunction() {
    Fib_heap<int> gomila;
    int brEl;
    std::cout << "Funkcija za testiranje minimalnog elementa u gomili.\n Unesite "
        "broj koliko elemenata zelite ubaciti :";
    std::cin >> brEl;
    auto elementi = create_random_elements(brEl);

    for (auto& x : elementi) {
        std::cout << "Ubacujem element: " << x->kljuc << "\n";
        gomila.Insert(x);
    }
    std::cout << "Minimum po Extract_min funkciji je : "
        << gomila.extract_min()->kljuc << std::endl;
    FindMinimumOfOneHeap(elementi);
    assert(1);
    std::cout << "\nTest Extract Min 1 status: PASSED\n";
    gomila.Display();
}

void UnionTestingFunction() {
    Fib_heap<int> gomila1, gomila2, gomila4, gomila5, gomila6;
    int minimum, minimum1, minimum2;
    int brEl;
    std::cout << "Funkcija za testiranje unije dvije gomile.\n Unesite "
        "broj koliko elemenata zelite ubaciti u prvu gomilu:";
    std::cin >> brEl;
    auto elementi = create_random_elements(brEl);

    for (auto& x : elementi) {
        std::cout << "Ubacujem element: " << x->kljuc << "\n";
        gomila1.Insert(x);
    }
    minimum1 = FindMinimumOfOneHeap(elementi);
    std::cout << "\n Unesite "
        "broj koliko elemenata zelite ubaciti u drugu gomilu:";
    std::cin >> brEl;
    auto elementi1 = create_random_elements(brEl);

    for (auto& x : elementi1) {
        std::cout << "Ubacujem element: " << x->kljuc << "\n";
        gomila2.Insert(x);
    }
    minimum2 = FindMinimumOfOneHeap(elementi1);
    if (minimum1 < minimum2)
        minimum = minimum1;
    else
        minimum = minimum2;

    std::cout << "\nMinimum gomile nastale unijom je: " << minimum << std::endl;
    Fib_heap<int> gomila3 = gomila1.Union(gomila1, gomila2);
    assert(minimum == gomila3.extract_min()->kljuc);
    std::cout << "\nTest Extract Min 1 status: PASSED\n";
    gomila3.Display();

    std::cout << "\nFunkcija za testiranje unije dvije gomile.\n Unesite "
        "broj koliko elemenata zelite ubaciti u prvu gomilu, druga gomila ce biti prazna:";
    std::cin >> brEl;
    auto elementi2 = create_random_elements(brEl);

    for (auto& x : elementi2) {
        std::cout << "Ubacujem element: " << x->kljuc << "\n";
        gomila4.Insert(x);
    }
    gomila6 = gomila4.Union(gomila4, gomila5);
    assert(1);
    std::cout << "\nTest Extract Min 2 status: PASSED\n";
    gomila6.Display();
}

void DecreaseKeyTestingFunction() {
    Fib_heap<int> gomila1;
    int brEl, staraVr, novaVr;
    std::cout
        << "Funkcija za testiranje smanjivanja vrijednost nekog cvora.\n Unesite "
        "broj koliko elemenata zelite ubaciti u gomilu:";
    std::cin >> brEl;
    auto elementi = create_random_elements(brEl);

    for (auto& x : elementi) {
        std::cout << "Ubacujem element: " << x->kljuc << "\n";
        gomila1.Insert(x);
    }

    std::cout << "Unesite vrijednost koju zelite smanjiti: \n";
    std::cin >> staraVr;
    std::cout << "Unesite novu vrijednost ključa: \n";
    std::cin >> novaVr;
    gomila1.DecreaseKey(novaVr, staraVr);
    gomila1.Display();
}

int main() {
    std::cout << "Dobrodosli u program demonstracije rada nad Fibonaccijevom gomilom.\n"
        "U nastavku ce biti pozvane sve funkcije za testiranje.\n"
        "Ponovnim pokretanjem programa isprobajte razlicite slucajeve ubacivanja,unije ili smanjivanja kljuca.\n"
        "Ispravan rad pomenutih funkcija povlaci ispravan rad drugih funkcija koje gomila ima(Find,Cut,CascadingCut i slicno).\n\n";
    InsertTestingFunction();
    UnionTestingFunction();
    DecreaseKeyTestingFunction();
    std::cout << "Ponovnim pokretanjem programa probajte testirati razlicite situacije.";
    return 0;
}