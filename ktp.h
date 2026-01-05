#ifndef KTP_H
#define KTP_H

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

struct KTP {
    string nik;
    string nama;
    string tempatLahir;
    string tanggalLahir;
    bool jenisKelamin; // true = Laki-laki, false = Perempuan
    string alamat;
    string agama;
    string statusPerkawinan;
    string pekerjaan;
    bool wargaNegara; // true = WNI, false = WNA
    KTP* next;
    KTP* prev;
};

class LinkedList {
private:
    KTP* head;
    KTP* tail;

public:
    LinkedList() {
        head = nullptr;
        tail = nullptr;
    }

    void tambahData() {
        KTP* baru = new KTP;

        cout << "\n=== INPUT DATA KTP ===\n";
        cout << "NIK: ";
        getline(cin, baru->nik);
        cout << "Nama: ";
        getline(cin, baru->nama);
        cout << "Tempat Lahir: ";
        getline(cin, baru->tempatLahir);
        cout << "Tanggal Lahir (DD-MM-YYYY): ";
        getline(cin, baru->tanggalLahir);

        char jk;
        cout << "Jenis Kelamin (L untuk Laki-laki / P untuk Perempuan): ";
        cin >> jk;
        baru->jenisKelamin = (jk == 'L' || jk == 'l');

        cin.ignore();
        cout << "Alamat: ";
        getline(cin, baru->alamat);
        cout << "Agama: ";
        getline(cin, baru->agama);
        cout << "Status Perkawinan: ";
        getline(cin, baru->statusPerkawinan);
        cout << "Pekerjaan: ";
        getline(cin, baru->pekerjaan);

        char wn;
        cout << "Kewarganegaraan (I untuk WNI / A untuk WNA): ";
        cin >> wn;
        baru->wargaNegara = (wn == 'I' || wn == 'i');
        cin.ignore();

        baru->next = nullptr;
        baru->prev = nullptr;

        if (head == nullptr)
            head = tail = baru;
        else {
            KTP* temp = head;
            while (temp->next != nullptr)
                tail->next = baru;
            	baru->prev = tail;
            	tail = baru;
        }

        cout << "\nData KTP berhasil ditambahkan!\n";
    }

    void tampilkanData() {
        if (head == nullptr) {
            cout << "\nBelum ada data KTP di memori.\n";
            return;
        }

        cout << "\n=== DAFTAR DATA KTP (dari Linked List) ===\n";
        KTP* temp = head;
        while (temp != nullptr) {
            cout << "\nNIK: " << temp->nik;
            cout << "\nNama: " << temp->nama;
            cout << "\nTempat/Tanggal Lahir: " << temp->tempatLahir << ", " << temp->tanggalLahir;
            cout << "\nJenis Kelamin: " << (temp->jenisKelamin ? "Laki-laki" : "Perempuan");
            cout << "\nAlamat: " << temp->alamat;
            cout << "\nAgama: " << temp->agama;
            cout << "\nStatus Perkawinan: " << temp->statusPerkawinan;
            cout << "\nPekerjaan: " << temp->pekerjaan;
            cout << "\nKewarganegaraan: " << (temp->wargaNegara ? "WNI" : "WNA");
            cout << "\n-----------------------------";
            temp = temp->next;
        }
    }
    
    void tampilkanMundur() {
        if (tail == nullptr) {
            cout << "\nBelum ada data KTP di memori.\n";
            return;
        }

        cout << "\n=== DAFTAR DATA KTP (Mundur dari Tail) ===\n";
        KTP* temp = tail;
        while (temp != nullptr) {
            cout << "\nNIK: " << temp->nik;
            cout << "\nNama: " << temp->nama;
            cout << "\nTempat/Tanggal Lahir: " << temp->tempatLahir << ", " << temp->tanggalLahir;
            cout << "\nJenis Kelamin: " << (temp->jenisKelamin ? "Laki-laki" : "Perempuan");
            cout << "\nAlamat: " << temp->alamat;
            cout << "\nAgama: " << temp->agama;
            cout << "\nStatus Perkawinan: " << temp->statusPerkawinan;
            cout << "\nPekerjaan: " << temp->pekerjaan;
            cout << "\nKewarganegaraan: " << (temp->wargaNegara ? "WNI" : "WNA");
            cout << "\n-----------------------------";
            temp = temp->prev;
        }
    }

    void simpanKeFile() {
        if (head == nullptr) {
            cout << "\nTidak ada data untuk disimpan.\n";
            return;
        }

        ofstream file("DataKTP.txt", ios::app);
        if (!file) {
            cout << "Gagal membuka file untuk menulis!\n";
            return;
        }

        KTP* temp = head;
        while (temp != nullptr) {
            file << temp->nik << endl;
            file << temp->nama << endl;
            file << temp->tempatLahir << endl;
            file << temp->tanggalLahir << endl;
            file << temp->jenisKelamin << endl;
            file << temp->alamat << endl;
            file << temp->agama << endl;
            file << temp->statusPerkawinan << endl;
            file << temp->pekerjaan << endl;
            file << temp->wargaNegara << endl;
            file << "-----" << endl;
            temp = temp->next;
        }

        file.close();
        cout << "\nData berhasil disimpan ke file 'DataKTP.txt'!\n";
    }

    void tampilkanDariFile() {
        ifstream file("DataKTP.txt");
        if (!file) {
            cout << "\nFile 'DataKTP.txt' belum ada atau gagal dibuka.\n";
            return;
        }

        string line;
        int count = 0;
        while (getline(file, line)) {
            if (line == "-----")
                count++;
        }

        if (count == 0) {
            cout << "\nBelum ada data di file.\n";
            return;
        }

        file.clear();
        file.seekg(0);

        KTP* data = new KTP[count];
        int index = 0;

        while (index < count && getline(file, data[index].nik)) {
            getline(file, data[index].nama);
            getline(file, data[index].tempatLahir);
            getline(file, data[index].tanggalLahir);
            string jkStr;
            getline(file, jkStr);
            data[index].jenisKelamin = (jkStr == "1");

            getline(file, data[index].alamat);
            getline(file, data[index].agama);
            getline(file, data[index].statusPerkawinan);
            getline(file, data[index].pekerjaan);
            string wnStr;
            getline(file, wnStr);
            data[index].wargaNegara = (wnStr == "1");

            getline(file, line); // baca "-----"
            index++;
        }

        cout << "\n=== DATA KTP DARI FILE ===\n";
        for (int i = 0; i < count; i++) {
            cout << "\n[" << i + 1 << "]";
            cout << "\nNIK: " << data[i].nik;
            cout << "\nNama: " << data[i].nama;
            cout << "\nTempat/Tanggal Lahir: " << data[i].tempatLahir << ", " << data[i].tanggalLahir;
            cout << "\nJenis Kelamin: " << (data[i].jenisKelamin ? "Laki-laki" : "Perempuan");
            cout << "\nAlamat: " << data[i].alamat;
            cout << "\nAgama: " << data[i].agama;
            cout << "\nStatus Perkawinan: " << data[i].statusPerkawinan;
            cout << "\nPekerjaan: " << data[i].pekerjaan;
            cout << "\nKewarganegaraan: " << (data[i].wargaNegara ? "WNI" : "WNA");
            cout << "\n-----------------------------";
        }

        delete[] data;
        file.close();
    }
    
    bool cekNIKTerdaftar(const string& nikCari) {
        if (head == nullptr) {
            return false;
        }

        KTP* temp = head;
        while (temp != nullptr) {
            if (temp->nik == nikCari) {
                return true;
            }
            temp = temp->next;
        }
        return false;
    }
    
    KTP* cariDataByNIK(const string& nikCari) {
        if (head == nullptr) {
            return nullptr;
        }

        KTP* temp = head;
        while (temp != nullptr) {
            if (temp->nik == nikCari) {
                return temp;
            }
            temp = temp->next;
        }
        return nullptr;
    }
    
    void muatDariFile() {
        ifstream file("DataKTP.txt");
        if (!file) {
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty() || line == "-----") continue;
            
            KTP* baru = new KTP;
            baru->nik = line;
            
            if (!getline(file, baru->nama)) break;
            if (!getline(file, baru->tempatLahir)) break;
            if (!getline(file, baru->tanggalLahir)) break;
            
            string jkStr;
            if (!getline(file, jkStr)) break;
            baru->jenisKelamin = (jkStr == "1");
            
            if (!getline(file, baru->alamat)) break;
            if (!getline(file, baru->agama)) break;
            if (!getline(file, baru->statusPerkawinan)) break;
            if (!getline(file, baru->pekerjaan)) break;
            
            string wnStr;
            if (!getline(file, wnStr)) break;
            baru->wargaNegara = (wnStr == "1");
            
            getline(file, line); // baca "-----"
            
            baru->next = nullptr;
            baru->prev = nullptr;

            if (head == nullptr) {
                head = tail = baru;
            } else {
                tail->next = baru;
                baru->prev = tail;
                tail = baru;
            }
        }

        file.close();
    }
};

#endif