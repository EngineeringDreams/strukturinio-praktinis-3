#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>

using namespace std;

const int MAX = 100;
const int KATEGORIJU_SK = 3;

const string kategorijos[KATEGORIJU_SK] = {
    "Dviraciai", "Dalys", "Aksesuarai"
};
bool nuskaitytiDuomenis(
    string kategorija[],
    string pavadinimas[],
    double kaina[],
    int kiekis[],
    int &n)
{
    ifstream failas("uzsakymas.txt");

    if (!failas.is_open()) {
        cout << "KLAIDA: Nepavyko atidaryti uzsakymas.txt failo!\n";
        return false;
    }

    if (!(failas >> n)) {
        cout << "KLAIDA: Neteisingas prekiu irasu skaicius!\n";
        return false;
    }

    failas.ignore(numeric_limits<streamsize>::max(), '\n');

    if (n < 1 || n > MAX) {
        cout << "KLAIDA: Prekiu skaicius turi buti nuo 1 iki 100!\n";
        return false;
    }

    for (int i = 0; i < n; i++) {
        string eilute;
        string kainosTekstas, kiekioTekstas;
        string papildomas;

        if (!getline(failas, eilute)) {
            cout << "KLAIDA: Nepakanka duomenu faile!\n";
            return false;
        }

        stringstream ss(eilute);

        if (!getline(ss, kategorija[i], ';') ||
            !getline(ss, pavadinimas[i], ';') ||
            !getline(ss, kainosTekstas, ';') ||
            !getline(ss, kiekioTekstas, ';')) {
            cout << "KLAIDA: Netinkamas " << i + 1
                 << " irasas!\n";
            return false;
        }

        if (getline(ss, papildomas, ';')) {
            cout << "KLAIDA: Per daug duomenu "
                 << i + 1 << " irase!\n";
            return false;
        }


        bool tinkama = false;

        for (int j = 0; j < KATEGORIJU_SK; j++) {
            if (kategorija[i] == kategorijos[j]) {
                tinkama = true;
            }
        }

        if (!tinkama || pavadinimas[i].empty()) {
            cout << "KLAIDA: Netinkama kategorija arba "
                 << "prekes pavadinimas!\n";
            return false;
        }


        try {
            size_t poz1, poz2;

            kaina[i] = stod(kainosTekstas, &poz1);
            kiekis[i] = stoi(kiekioTekstas, &poz2);

            if (poz1 != kainosTekstas.size() ||
                poz2 != kiekioTekstas.size() ||
                kaina[i] <= 0 || kiekis[i] <= 0) {
                cout << "KLAIDA: Netinkama kaina arba kiekis!\n";
                return false;
            }
        }
        catch (...) {
            cout << "KLAIDA: Neteisingi skaiciai faile!\n";
            return false;
        }
    }

    failas.close();
    return true;
}


void skaiciuotiPreke(
    double kaina,
    int kiekis,
    double &suma,
    double &nuolaida,
    double &moketi)
{
    suma = kaina * kiekis;

    if (kiekis >= 10) {
        nuolaida = suma * 0.10;
    }
    else {
        nuolaida = 0;
    }

    moketi = suma - nuolaida;
}


double kategorijosSuma(
    string kategorija,
    string kategorijosMasyvas[],
    double kainos[],
    int kiekiai[],
    int n)
{
    double suma = 0;

    for (int i = 0; i < n; i++) {
        if (kategorijosMasyvas[i] == kategorija) {
            double pries, nuolaida, po;

            skaiciuotiPreke(
                kainos[i], kiekiai[i],
                pries, nuolaida, po
            );

            suma += po;
        }
    }

    return suma;
}


void skaiciuotiVisasSumas(
    double kainos[],
    int kiekiai[],
    int n,
    double &bendraSuma,
    double &bendraNuolaida,
    double &galutineSuma)
{
    bendraSuma = 0;
    bendraNuolaida = 0;
    galutineSuma = 0;

    for (int i = 0; i < n; i++) {
        double suma, nuolaida, moketi;

        skaiciuotiPreke(
            kainos[i], kiekiai[i],
            suma, nuolaida, moketi
        );

        bendraSuma += suma;
        bendraNuolaida += nuolaida;
        galutineSuma += moketi;
    }
}


void spausdintiPrekes(
    ostream &out,
    string kategorijosMasyvas[],
    string pavadinimai[],
    double kainos[],
    int kiekiai[],
    int n,
    int pasirinkta = -1)
{
    out << fixed << setprecision(2);

    out << "\n";
    out << left << setw(15) << "Kategorija"
        << setw(27) << "Preke"
        << right << setw(10) << "Kaina"
        << setw(9) << "Kiekis"
        << setw(12) << "Suma"
        << setw(12) << "Nuolaida"
        << setw(12) << "Moketi" << "\n";

    out << string(97, '-') << "\n";

    bool rasta = false;

    for (int i = 0; i < n; i++) {
        if (pasirinkta == -1 ||
            kategorijosMasyvas[i] == kategorijos[pasirinkta]) {

            double suma, nuolaida, moketi;

            skaiciuotiPreke(
                kainos[i], kiekiai[i],
                suma, nuolaida, moketi
            );

            out << left << setw(15) << kategorijosMasyvas[i]
                << setw(27) << pavadinimai[i]
                << right << setw(10) << kainos[i]
                << setw(9) << kiekiai[i]
                << setw(12) << suma
                << setw(12) << nuolaida
                << setw(12) << moketi << "\n";

            rasta = true;
        }
    }

    if (!rasta) {
        out << "Pasirinktoje kategorijoje prekiu nera.\n";
    }
}


void spausdintiSumas(
    ostream &out,
    string kategorijosMasyvas[],
    double kainos[],
    int kiekiai[],
    int n)
{
    double bendra, nuolaida, galutine;

    skaiciuotiVisasSumas(
        kainos, kiekiai, n,
        bendra, nuolaida, galutine
    );

    out << fixed << setprecision(2);

    out << "\nVISO UZSAKYMO SUMOS\n";
    out << "Suma pries nuolaidas: " << bendra << " Eur\n";
    out << "Bendra nuolaida:      " << nuolaida << " Eur\n";
    out << "Galutine suma:        " << galutine << " Eur\n";

    out << "\nSUMOS PAGAL KATEGORIJAS\n";

    for (int i = 0; i < KATEGORIJU_SK; i++) {
        double suma = kategorijosSuma(
            kategorijos[i],
            kategorijosMasyvas,
            kainos,
            kiekiai,
            n
        );

        out << kategorijos[i] << ": "
            << suma << " Eur\n";
    }
}


void sudarytiAtaskaita(
    ostream &out,
    string kategorijosMasyvas[],
    string pavadinimai[],
    double kainos[],
    int kiekiai[],
    int n)
{
    out << "DVIRACIU PARDUOTUVES UZSAKYMO ATASKAITA\n";
    out << "=======================================\n";

    spausdintiPrekes(
        out, kategorijosMasyvas,
        pavadinimai, kainos, kiekiai, n
    );

    spausdintiSumas(
        out, kategorijosMasyvas,
        kainos, kiekiai, n
    );
}


bool issaugotiAtaskaita(
    string kategorijosMasyvas[],
    string pavadinimai[],
    double kainos[],
    int kiekiai[],
    int n)
{
    ofstream failas("ataskaita.txt");

    if (!failas.is_open()) {
        cout << "KLAIDA: Nepavyko sukurti ataskaitos failo!\n";
        return false;
    }

    sudarytiAtaskaita(
        failas, kategorijosMasyvas,
        pavadinimai, kainos, kiekiai, n
    );

    failas.close();

    if (!failas) {
        cout << "KLAIDA: Nepavyko irasyti ataskaitos!\n";
        return false;
    }

    cout << "Ataskaita sekmingai issaugota faile ataskaita.txt\n";
    return true;
}

int main() {
    string kategorija[MAX];
    string pavadinimas[MAX];
    double kaina[MAX];
    int kiekis[MAX];

    int n;


    if (!nuskaitytiDuomenis(
        kategorija, pavadinimas,
        kaina, kiekis, n)) {
        return 1;
        }

    cout << "Duomenys sekmingai nuskaityti!\n";
    cout << "Prekiu irasu skaicius: " << n << "\n";

    int pasirinkimas;

    do {
        cout << "\n====================================\n";
        cout << " DVIRACIU PARDUOTUVES UZSAKYMAS\n";
        cout << "====================================\n";
        cout << "1. Perziureti visas uzsakymo prekes\n";
        cout << "2. Perziureti pasirinktos kategorijos prekes\n";
        cout << "3. Perziureti viso uzsakymo sumas\n";
        cout << "4. Perziureti sumas pagal kategorijas\n";
        cout << "5. Parodyti visa uzsakymo ataskaita\n";
        cout << "6. Issaugoti visa ataskaita i faila\n";
        cout << "7. Isseiti\n";
        cout << "------------------------------------\n";
        cout << "Jusu pasirinkimas: ";

        if (!(cin >> pasirinkimas)) {
            cout << "KLAIDA: Iveskite skaiciu nuo 1 iki 7!\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        switch (pasirinkimas) {
            case 1:
                spausdintiPrekes(
                    cout, kategorija, pavadinimas,
                    kaina, kiekis, n
                );
                break;
                case 2: {
            int kat;

            cout << "\nPasirinkite kategorija:\n";
            cout << "1. Dviraciai\n";
            cout << "2. Dalys\n";
            cout << "3. Aksesuarai\n";
            cout << "Jusu pasirinkimas: ";

            if (!(cin >> kat)) {
                cout << "KLAIDA: Iveskite skaiciu!\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                break;
            }

            if (kat < 1 || kat > 3) {
                cout << "Neteisingas kategorijos pasirinkimas!\n";
                break;
            }

            spausdintiPrekes(
                cout, kategorija, pavadinimas,
                kaina, kiekis, n, kat - 1
            );
            break;
        }

        case 3: {
            double bendra, nuolaida, galutine;

            skaiciuotiVisasSumas(
                kaina, kiekis, n,
                bendra, nuolaida, galutine
            );

            cout << fixed << setprecision(2);
            cout << "\nSuma pries nuolaidas: "
                 << bendra << " Eur\n";
            cout << "Bendra nuolaidu suma: "
                 << nuolaida << " Eur\n";
            cout << "Galutine moketina suma: "
                 << galutine << " Eur\n";
            break;
        }

        case 4:
            cout << fixed << setprecision(2);
            cout << "\nSUMOS PAGAL KATEGORIJAS:\n";

            for (int i = 0; i < KATEGORIJU_SK; i++) {
                double suma = kategorijosSuma(
                    kategorijos[i],
                    kategorija, kaina, kiekis, n
                );

                cout << kategorijos[i] << ": "
                     << suma << " Eur\n";
            }
            break;

        case 5:
            sudarytiAtaskaita(
                cout, kategorija, pavadinimas,
                kaina, kiekis, n
            );
            break;

        case 6:
            issaugotiAtaskaita(
                kategorija, pavadinimas,
                kaina, kiekis, n
            );
            break;

        case 7:
            cout << "Programa baigia darba. Viso gero!\n";
            break;

        default:
            cout << "Neteisingas pasirinkimas! Bandykite dar karta.\n";
                
        }


    } while (pasirinkimas != 7);
    return 0;
}