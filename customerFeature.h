#ifndef CustomerFeature_
#define CustomerFeature_

#include <iostream>
#include <fstream>
#include <string>
#include "ktp.h"
#include "adminFeature.h"
using namespace std;

// ==========================
// Akun Pelanggan
// ==========================
class AkunPelanggan {
private:
    string username, password;
public:
    AkunPelanggan() {}
    AkunPelanggan(string u, string p) : username(u), password(p) {}
    string getUsername() { return username; }
    string getPassword() { return password; }
};

// ==========================
// Array Dinamis 
// ==========================
template<typename T>
class DynamicArraySimple {
public:
    T* data;
    int size;
    int capacity;

    DynamicArraySimple() : data(nullptr), size(0), capacity(0) {}
    ~DynamicArraySimple() { delete[] data; }

    void reserve(int newCap) {
        if (newCap <= capacity) return;
        T* tmp = new T[newCap];
        for (int i = 0; i < size; ++i) tmp[i] = data[i];
        delete[] data;
        data = tmp;
        capacity = newCap;
    }

    void push_back(const T& v) {
        if (capacity == 0) reserve(1);
        if (size >= capacity) reserve(capacity * 2);
        data[size++] = v;
    }

    bool containsUser(const string& u) {
        for (int i = 0; i < size; ++i) if (data[i].getUsername() == u) return true;
        return false;
    }
};

// ==========================
// Sistem Akun Pelanggan 
// ==========================
class SistemAkun {
private:
    DynamicArraySimple<AkunPelanggan> akunList;
    string currentUser;
    string currentPass;
    bool loggedIn = false;
    LinkedList* ktpList;
public:
    SistemAkun() : ktpList(nullptr) {
        muatDariFile();
    }
    
    void setKTPList(LinkedList* ktp) {
        ktpList = ktp;
    }

    void muatDariFile() {
        ifstream file("akun_pelanggan.txt");
        if (!file.is_open()) return;
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            size_t pos = line.find('|');
            if (pos != string::npos) {
                string user = line.substr(0, pos);
                string pass = line.substr(pos + 1);
                akunList.push_back(AkunPelanggan(user, pass));
            }
        }
        file.close();
    }

    void simpanKeFile(const string& username, const string& password) {
        ofstream file("akun_pelanggan.txt", ios::app);
        file << username << "|" << password << endl;
        file.close();
    }

    bool isLogged() const { return loggedIn; }
    string getCurrentUser() const { return currentUser; }
    string getCurrentPass() const { return currentPass; }

    void logout() {
        currentUser.clear();
        currentPass.clear();
        loggedIn = false;
    }

    void registerAkun() {
        string nik, username, password;
        cout << "\n=== REGISTER AKUN BARU ===\n";
        cout << "Masukkan NIK (16 digit): ";
        cin >> nik;
        
        // Validasi NIK harus terdaftar di KTP
        if (ktpList == nullptr) {
            cout << "Sistem KTP belum diinisialisasi!\n";
            return;
        }
        
        if (!ktpList->cekNIKTerdaftar(nik)) {
            cout << "\nNIK tidak terdaftar dalam sistem KTP!\n";
            cout << "Silakan daftarkan KTP Anda terlebih dahulu di Admin.\n";
            return;
        }
        
        // Ambil data KTP untuk menampilkan informasi
        KTP* dataKTP = ktpList->cariDataByNIK(nik);
        if (dataKTP != nullptr) {
            cout << "\nData KTP ditemukan:\n";
            cout << "NIK  : " << dataKTP->nik << "\n";
            cout << "Nama : " << dataKTP->nama << "\n";
        }
        
        cout << "\nMasukkan Username: ";
        cin >> username;
        cout << "Masukkan Password: ";
        cin >> password;

        if (akunList.containsUser(username)) {
            cout << "Username sudah digunakan!\n";
            return;
        }

        simpanKeFile(username, password);
        akunList.push_back(AkunPelanggan(username, password));
        cout << "\nAkun berhasil dibuat! Silakan login dengan akun baru Anda.\n";
    }

    bool loginAkun() {
        string username, password;
        cout << "\n=== LOGIN PELANGGAN ===\n";
        cout << "Masukkan Username: ";
        cin >> username;
        cout << "Masukkan Password: ";
        cin >> password;

        for (int i = 0; i < akunList.size; ++i) {
            if (akunList.data[i].getUsername() == username && akunList.data[i].getPassword() == password) {
                cout << "\nLogin berhasil! Selamat datang, " << username << "!\n";
                currentUser = username;
                currentPass = password;
                loggedIn = true;
                return true;
            }
        }
        cout << "\nLogin gagal! Username atau password salah.\n";
        return false;
    }
};

// ==========================
// Fitur Pelanggan
// ==========================
class CustomerFeature {
private:
    SistemAkun sistemAkun;
    DataPelanggan pelanggan;
    LinkedList* ktpList;

    // Struktur data Tree sederhana untuk menampilkan jadwal secara terurut
    struct FlightNode {
        Penerbangan data;
        FlightNode* left;
        FlightNode* right;
        FlightNode(const Penerbangan& p) : data(p), left(nullptr), right(nullptr) {}
    };

    FlightNode* rootTree = nullptr;
    int pseudoRandSeed;

    struct TicketRecord {
        string nik;
        string nama;
        int umur;
        string gender;
        string asal;
        string tujuan;
        string maskapai;
        string tanggal;
        string jam;
        string status;
        int bagasi;
        string kursi;
        string gate;
        string keterangan;
        long harga;
    };

    const string tiketHeader = "NIK|Nama|Umur|Gender|Asal|Tujuan|Maskapai|Tanggal|Jam|Status|Bagasi|Kursi|Gate|Keterangan|Harga";

    string buatNomorPenerbangan(const Penerbangan& p, int urut) {
        string kode;
        for (char c : p.maskapai) {
            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                if (c >= 'a' && c <= 'z') {
                    c = c - 'a' + 'A';
                }
                kode.push_back(c);
                if (kode.size() == 2) break;
            }
        }
        while (kode.size() < 2) kode.push_back('X');
        int angka = 100 + (urut % 900);
        return kode + to_string(angka);
    }

    int generatePseudoRand() {
        pseudoRandSeed = (pseudoRandSeed * 1103515245 + 12345) % 2147483648;
        return pseudoRandSeed;
    }

    void seedRandom() {
        static bool initialized = false;
        if (!initialized) {
            pseudoRandSeed = 123;
            initialized = true;
        }
    }

    bool hanyaAngka(const string& s) {
        if (s.empty()) return false;
        for (char c : s) {
            if (!(c >= '0' && c <= '9')) return false;
        }
        return true;
    }

    bool isValidGender(const string& g) {
        return (g == "L" || g == "P" || g == "l" || g == "p");
    }

    bool isValidNIK(const string& nik) {
        if (nik.length() != 16) return false;
        return hanyaAngka(nik);
    }

    bool isValidNama(const string& nama) {
        if (nama.length() < 3) return false;
        for (char c : nama) {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ')) {
                return false;
            }
        }
        return true;
    }

    bool isValidKota(const string& kota) {
        if (kota.length() < 2) return false;
        for (char c : kota) {
            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' ')) {
                return false;
            }
        }
        return true;
    }

    bool parseBerat(const string& teks, int& beratOut) {
        string angka;
        for (char c : teks) {
            if (c >= '0' && c <= '9') angka.push_back(c);
        }
        if (angka.empty()) return false;
        try {
            beratOut = stoi(angka);
            return true;
        } catch (...) {
            return false;
        }
    }

    string acakGate() {
        char huruf = 'A' + (generatePseudoRand() % 6);
        int angka = 1 + (generatePseudoRand() % 20);
        return string(1, huruf) + to_string(angka);
    }

    string acakKursi() {
        int row = 1 + (generatePseudoRand() % 30);
        char seat = 'A' + (generatePseudoRand() % 6);
        return to_string(row) + string(1, seat);
    }

    string formatRupiah(long nilai) {
        bool neg = nilai < 0;
        if (neg) nilai = -nilai;
        string digits = to_string(nilai);
        string hasil;
        int count = 0;
        for (int i = (int)digits.size() - 1; i >= 0; --i) {
            hasil.push_back(digits[i]);
            count++;
            if (count == 3 && i != 0) {
                hasil.push_back('.');
                count = 0;
            }
        }
        string reversed;
        for (int i = (int)hasil.size() - 1; i >= 0; --i) {
            reversed.push_back(hasil[i]);
        }
        if (neg) reversed = "-" + reversed;
        return "Rp " + reversed;
    }

    string keyFrom(const Penerbangan& p) {
        return p.asal + "|" + p.tujuan + "|" + p.jadwal.tanggal + "|" + p.jadwal.jam + "|" + p.maskapai;
    }

    FlightNode* insertNode(FlightNode* node, const Penerbangan& p) {
        if (!node) return new FlightNode(p);
        if (keyFrom(p) <= keyFrom(node->data)) node->left = insertNode(node->left, p);
        else node->right = insertNode(node->right, p);
        return node;
    }

    void clearTree(FlightNode* node) {
        if (!node) return;
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }

    void inorderPrint(FlightNode* node, int& urut) {
        if (!node) return;
        inorderPrint(node->left, urut);
        cout << urut++ << ". " << node->data.maskapai << " | "
             << node->data.asal << " -> " << node->data.tujuan
             << " | " << node->data.jadwal.tanggal << " " << node->data.jadwal.jam
             << " | Bagasi Maks: " << node->data.maxBeratBagasi << "kg\n";
        inorderPrint(node->right, urut);
    }

    bool isHeaderLine(const string& line) {
        return line.find("NIK|Nama|Umur|Gender|Asal|Tujuan|Maskapai|Tanggal|Jam") == 0;
    }

    bool parseTicketLine(const string& line, TicketRecord& out) {
        if (line.empty() || isHeaderLine(line)) return false;

        string fields[15];
        int idx = 0;
        string remain = line;
        while (idx < 15) {
            size_t pos = remain.find('|');
            if (pos == string::npos) {
                fields[idx++] = remain;
                break;
            }
            fields[idx++] = remain.substr(0, pos);
            remain = remain.substr(pos + 1);
        }
        while (idx < 15) fields[idx++] = "";

        if (fields[0].empty() || fields[1].empty()) return false;

        out.nik = fields[0];
        out.nama = fields[1];
        out.umur = fields[2].empty() ? 0 : stoi(fields[2]);
        out.gender = fields[3];
        out.asal = fields[4];
        out.tujuan = fields[5];
        out.maskapai = fields[6];
        out.tanggal = fields[7];
        out.jam = fields[8];
        out.status = fields[9].empty() ? "BELUM CHECK-IN" : fields[9];
        out.bagasi = fields[10].empty() ? -1 : stoi(fields[10]);
        out.kursi = fields[11];
        out.gate = fields[12];
        out.keterangan = fields[13];
        out.harga = fields[14].empty() ? 0 : stol(fields[14]);
        return true;
    }

    string serializeTicket(const TicketRecord& t) {
        string bagasiStr = (t.bagasi < 0) ? "" : to_string(t.bagasi);
        return t.nik + "|" + t.nama + "|" + to_string(t.umur) + "|" + t.gender + "|" +
               t.asal + "|" + t.tujuan + "|" + t.maskapai + "|" + t.tanggal + "|" + t.jam + "|" +
               t.status + "|" + bagasiStr + "|" + t.kursi + "|" + t.gate + "|" + t.keterangan + "|" + to_string(t.harga);
    }

    bool fileKosong(const string& namaFile) {
        ifstream cek(namaFile.c_str());
        if (!cek.is_open()) return true;
        bool kosong = (cek.peek() == ifstream::traits_type::eof());
        cek.close();
        return kosong;
    }

    DynamicArraySimple<TicketRecord> bacaSemuaTiket() {
        DynamicArraySimple<TicketRecord> daftar;
        ifstream file("data_pelanggan.txt");
        if (!file.is_open()) return daftar;

        string line;
        while (getline(file, line)) {
            TicketRecord t;
            if (parseTicketLine(line, t)) daftar.push_back(t);
        }
        file.close();
        return daftar;
    }

    DynamicArraySimple<Penerbangan> bacaPenerbanganDariFile() {
        DynamicArraySimple<Penerbangan> daftarFile;
        ifstream f("data_penerbangan.txt");
        if (f.is_open()) {
            while (true) {
                Penerbangan temp;
                string kursi, gate;
                if (!(f >> temp.asal >> temp.tujuan >> temp.jadwal.tanggal >> temp.jadwal.jam >> temp.maxBeratBagasi >> temp.harga >> kursi >> gate)) break;
                temp.kursi = kursi;
                temp.gate = gate;
                f.ignore();
                getline(f, temp.maskapai);
                daftarFile.push_back(temp);
            }
            f.close();
        }
        return daftarFile;
    }

    void simpanSemuaTiket(const DynamicArraySimple<TicketRecord>& daftar) {
        ofstream file("data_pelanggan.txt", ios::trunc);
        file << tiketHeader << "\n";
        for (int i = 0; i < daftar.size; ++i) {
            file << serializeTicket(daftar.data[i]) << "\n";
        }
        file.close();
    }

public:
    CustomerFeature() : pseudoRandSeed(123), ktpList(nullptr) {}
    
    void setKTPList(LinkedList* ktp) {
        ktpList = ktp;
    }
    
    void pesanTiket(Penerbangan daftar[], int jumlah) {
        string nik, nama, gender, asal, tujuan;
        int umur;

        cin.ignore();
        cout << "\n=== PESAN TIKET PENERBANGAN ===\n";
        
        do {
            cout << "Masukkan NIK (16 digit): "; getline(cin, nik);
            if (!isValidNIK(nik)) {
                cout << "NIK harus 16 digit angka. Coba lagi.\n";
            }
        } while (!isValidNIK(nik));
        
        do {
            cout << "Masukkan Nama (min 3 karakter, huruf saja): "; getline(cin, nama);
            if (!isValidNama(nama)) {
                cout << "Nama minimal 3 karakter dan hanya boleh huruf. Coba lagi.\n";
            }
        } while (!isValidNama(nama));
        
        do {
            cout << "Masukkan Umur (1-120): "; cin >> umur; cin.ignore();
            if (umur < 1 || umur > 120) {
                cout << "Umur harus antara 1-120 tahun. Coba lagi.\n";
            }
        } while (umur < 1 || umur > 120);
        
        do {
            cout << "Masukkan Jenis Kelamin (L/P): "; getline(cin, gender);
            if (!isValidGender(gender)) {
                cout << "Jenis kelamin hanya boleh L (Laki-laki) atau P (Perempuan). Coba lagi.\n";
            }
        } while (!isValidGender(gender));
        
        do {
            cout << "Asal (nama kota, min 2 karakter): "; getline(cin, asal);
            if (!isValidKota(asal)) {
                cout << "Asal tidak valid. Gunakan nama kota (min 2 karakter, huruf saja). Coba lagi.\n";
            }
        } while (!isValidKota(asal));
        
        do {
            cout << "Tujuan (nama kota, min 2 karakter): "; getline(cin, tujuan);
            if (!isValidKota(tujuan)) {
                cout << "Tujuan tidak valid. Gunakan nama kota (min 2 karakter, huruf saja). Coba lagi.\n";
            }
            if (asal == tujuan) {
                cout << "Asal dan tujuan tidak boleh sama! Coba lagi.\n";
            }
        } while (!isValidKota(tujuan) || asal == tujuan);

        // Muat jadwal dari file agar sinkron dengan data terbaru admin
        DynamicArraySimple<Penerbangan> daftarFile = bacaPenerbanganDariFile();

        // Jika file kosong/gagal, gunakan data bawaan parameter
        if (daftarFile.size == 0) {
            for (int i = 0; i < jumlah; ++i) daftarFile.push_back(daftar[i]);
        }

        int ditemukan = 0;
        DynamicArraySimple<int> listIndex;

        cout << "\n=== PENERBANGAN TERSEDIA ===\n\n";
        for (int i = 0; i < daftarFile.size; i++) {
            if (daftarFile.data[i].asal == asal && daftarFile.data[i].tujuan == tujuan) {
                listIndex.push_back(i);
                ditemukan++;
                cout << ditemukan << ". " << daftarFile.data[i].maskapai << "\n";
                cout << "   Jadwal : " << daftarFile.data[i].jadwal.tanggal
                     << " | " << daftarFile.data[i].jadwal.jam << "\n";
                cout << "   Bagasi : " << daftarFile.data[i].maxBeratBagasi << " kg\n";
                cout << "   Harga  : " << formatRupiah(daftarFile.data[i].harga) << "\n\n";
            }
        }

        if (ditemukan == 0) {
            cout << "\nTidak ada penerbangan.\n";
            return;
        }

        int pilih;
        cout << "\nPilih penerbangan: ";
        if (!(cin >> pilih)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus berupa angka.\n";
            return;
        }
        cin.ignore(10000, '\n');

        if (pilih < 1 || pilih > ditemukan) {
            cout << "Pilihan tidak valid.\n";
            return;
        }

        Penerbangan p = daftarFile.data[listIndex.data[pilih - 1]];

        // Payment flow
        cout << "\n=== PEMBAYARAN TIKET ===\n";
        cout << "Nama Pemesan : " << nama << "\n";
        cout << "Total Harga  : " << formatRupiah(p.harga) << "\n\n";
        
        cout << "Metode Pembayaran:\n";
        cout << "1. Transfer Bank\n";
        cout << "2. E-Wallet\n";
        cout << "3. Tunai\n";
        
        int metodePembayaran;
        cout << "\nPilih metode pembayaran (1-3): ";
        if (!(cin >> metodePembayaran)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus berupa angka.\n";
            return;
        }
        cin.ignore(10000, '\n');
        
        if (metodePembayaran < 1 || metodePembayaran > 3) {
            cout << "Pilihan tidak valid. Pembayaran dibatalkan.\n";
            return;
        }

        string namaMetode[] = {"", "Transfer Bank", "E-Wallet", "Tunai"};
        
        // Input nominal pembayaran
        long nominalBayar;
        cout << "\nMasukkan nominal pembayaran: Rp ";
        if (!(cin >> nominalBayar)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Input harus berupa angka.\n";
            return;
        }
        cin.ignore(10000, '\n');
        
        // Validasi pembayaran
        if (nominalBayar < p.harga) {
            // Uang kurang
            long kurang = p.harga - nominalBayar;
            cout << "\n----------------------------------------\n";
            cout << " Pembayaran GAGAL\n";
            cout << "Uang Anda kurang : " << formatRupiah(kurang) << "\n";
            cout << "Silakan lakukan pembayaran ulang\n";
            cout << "----------------------------------------\n";
            return;
        } else if (nominalBayar == p.harga) {
            // Uang pas
            cout << "\n----------------------------------------\n";
            cout << " Pembayaran BERHASIL\n";
            cout << "Uang dibayarkan : " << formatRupiah(nominalBayar) << "\n";
            cout << "Keterangan      : Uang pas\n";
            cout << "Status Tiket    : BELUM CHECK-IN\n";
            cout << "----------------------------------------\n";
        } else {
            // Uang lebih
            long kembalian = nominalBayar - p.harga;
            cout << "\n----------------------------------------\n";
            cout << " Pembayaran BERHASIL\n";
            cout << "Uang dibayarkan : " << formatRupiah(nominalBayar) << "\n";
            cout << "Kembalian       : " << formatRupiah(kembalian) << "\n";
            cout << "Status Tiket    : BELUM CHECK-IN\n";
            cout << "----------------------------------------\n";
        }

        // Simpan tiket hanya jika pembayaran berhasil
        DynamicArraySimple<TicketRecord> tiket = bacaSemuaTiket();

        TicketRecord baru;
        baru.nik = nik;
        baru.nama = nama;
        baru.umur = umur;
        baru.gender = gender;
        baru.asal = p.asal;
        baru.tujuan = p.tujuan;
        baru.maskapai = p.maskapai;
        baru.tanggal = p.jadwal.tanggal;
        baru.jam = p.jadwal.jam;
        baru.status = "BELUM CHECK-IN";
        baru.bagasi = -1;
        baru.kursi = "";
        baru.gate = "";
        baru.keterangan = "";
        baru.harga = p.harga;

        tiket.push_back(baru);
        simpanSemuaTiket(tiket);
    }

    void checkInTiket(Penerbangan daftar[], int jumlah) {
        string nik, nama;
        cout << "\n=== CHECK-IN TIKET ===\n";
        cout << "Masukkan NIK              : ";
        getline(cin, nik);

        cout << "Masukkan Nama             : ";
        getline(cin, nama);

        cout << "Masukkan Berat Bagasi (kg): ";
        string inputBerat;
        getline(cin, inputBerat);

        int beratBagasi;
        if (!parseBerat(inputBerat, beratBagasi) || beratBagasi < 1) {
            cout << "Input berat bagasi tidak valid. Berat harus minimal 1 kg.\n";
            return;
        }

        DynamicArraySimple<TicketRecord> tiket = bacaSemuaTiket();
        int idx = -1;
        for (int i = 0; i < tiket.size; ++i) {
            if (tiket.data[i].nik == nik && tiket.data[i].nama == nama) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            cout << "\n----------------------------------------\n";
            cout << "Data tidak ditemukan\n";
            cout << "Check-in dibatalkan.\n";
            cout << "----------------------------------------\n";
            return;
        }

        TicketRecord& data = tiket.data[idx];
        if (data.status == "SUDAH CHECK-IN") {
            cout << "\n----------------------------------------\n";
            cout << "Status Check-In : SUDAH CHECK-IN\n";
            cout << "Nomor Kursi     : " << (data.kursi.empty() ? "-" : data.kursi) << "\n";
            cout << "Gate            : " << (data.gate.empty() ? "-" : data.gate) << "\n";
            cout << "Keterangan      : Tiket sudah diproses check-in" << "\n";
            cout << "----------------------------------------\n";
            return;
        }

        // Find matching flight to get kursi and gate
        DynamicArraySimple<Penerbangan> daftarFile = bacaPenerbanganDariFile();
        string kursiPenerbangan = "";
        string gatePenerbangan = "";
        
        for (int i = 0; i < daftarFile.size; ++i) {
            if (daftarFile.data[i].asal == data.asal && 
                daftarFile.data[i].tujuan == data.tujuan &&
                daftarFile.data[i].jadwal.tanggal == data.tanggal &&
                daftarFile.data[i].jadwal.jam == data.jam &&
                daftarFile.data[i].maskapai == data.maskapai) {
                kursiPenerbangan = daftarFile.data[i].kursi;
                gatePenerbangan = daftarFile.data[i].gate;
                break;
            }
        }
        
        if (kursiPenerbangan.empty() || gatePenerbangan.empty()) {
            cout << "\n----------------------------------------\n";
            cout << "Error: Data penerbangan tidak ditemukan\n";
            cout << "Check-in dibatalkan.\n";
            cout << "----------------------------------------\n";
            return;
        }

        data.status = "SUDAH CHECK-IN";
        data.bagasi = beratBagasi;
        data.kursi = kursiPenerbangan;
        data.gate = gatePenerbangan;

        const int BATAS_BAGASI = 20;
        if (beratBagasi > BATAS_BAGASI) {
            int kelebihan = beratBagasi - BATAS_BAGASI;
            long biaya = static_cast<long>(kelebihan) * 50000L;
            data.keterangan = "Kelebihan bagasi " + to_string(kelebihan) + " kg, biaya " + formatRupiah(biaya);
        } else {
            data.keterangan = "Bagasi sesuai batas, tidak ada biaya tambahan";
        }

        simpanSemuaTiket(tiket);

        cout << "\n----------------------------------------\n";
        cout << "Status Check-In : BERHASIL\n";
        cout << "Nomor Kursi     : " << data.kursi << "\n";
        cout << "Gate            : " << data.gate << "\n";
        cout << "Bagasi          : " << beratBagasi << " kg\n";
        
        if (beratBagasi > BATAS_BAGASI) {
            int kelebihan = beratBagasi - BATAS_BAGASI;
            long biaya = static_cast<long>(kelebihan) * 50000L;
            cout << "\nKeterangan      : " << data.keterangan << "\n";
        }
        cout << "----------------------------------------\n";
    }

    void tampilkanTiketSaya() {
        DynamicArraySimple<TicketRecord> daftar = bacaSemuaTiket();
        if (daftar.size == 0) {
            cout << "Belum ada tiket yang dipesan.\n";
            return;
        }

        cout << "\n=== TIKET SAYA ===\n\n";
        for (int i = 0; i < daftar.size; ++i) {
            TicketRecord& t = daftar.data[i];
            cout << i + 1 << ". Nama  : " << t.nama << " | NIK: " << t.nik << "\n";
            cout << "   Rute   : " << t.asal << " -> " << t.tujuan << "\n";
            cout << "   Jadwal : " << t.tanggal << " | " << t.jam << "\n";
            cout << "   Harga  : " << formatRupiah(t.harga) << "\n";
            cout << "   Status : " << t.status << "\n";

            if (t.status == "SUDAH CHECK-IN" || t.status == "SELESAI") {
                cout << "   Kursi  : " << (t.kursi.empty() ? "-" : t.kursi) << "\n";
                cout << "   Gate   : " << (t.gate.empty() ? "-" : t.gate) << "\n";
            }
            cout << "\n";
        }
    }

    void tampilkanRiwayatPemesanan() {
        DynamicArraySimple<TicketRecord> daftar = bacaSemuaTiket();
        if (daftar.size == 0) {
            cout << "Belum ada riwayat pemesanan.\n";
            return;
        }

        cout << "\n=== RIWAYAT PEMESANAN ===\n\n";
        for (int i = 0; i < daftar.size; ++i) {
            TicketRecord& t = daftar.data[i];
            string bagasiStr = (t.bagasi < 0) ? "-" : to_string(t.bagasi) + " kg";
            string kursiStr = t.kursi.empty() ? "-" : t.kursi;
            string gateStr = t.gate.empty() ? "-" : t.gate;

            cout << i + 1 << ". " << t.nama << " | " << t.asal << " -> " << t.tujuan << " | " << t.tanggal << " " << t.jam << "\n";
            cout << "   Harga       : " << formatRupiah(t.harga) << "\n";
            cout << "   Bagasi      : " << bagasiStr << "\n";
            
            if (t.status == "SELESAI") {
                cout << "   Status Akhir: SELESAI\n";
            } else {
                cout << "   Status      : " << t.status << "\n";
            }
            
            if (!t.keterangan.empty()) {
                cout << "   Ket         : " << t.keterangan << "\n";
            }
            cout << "\n";
        }
    }

    void tandaiPenerbanganSelesai() {
        DynamicArraySimple<TicketRecord> daftar = bacaSemuaTiket();
        if (daftar.size == 0) {
            cout << "Belum ada tiket.\n";
            return;
        }

        string nik, nama;
        cout << "Masukkan NIK  : ";
        getline(cin, nik);
        cout << "Masukkan Nama : ";
        getline(cin, nama);

        int idx = -1;
        for (int i = 0; i < daftar.size; ++i) {
            if (daftar.data[i].nik == nik && daftar.data[i].nama == nama) {
                idx = i;
                break;
            }
        }

        if (idx == -1) {
            cout << "Data tidak ditemukan.\n";
            return;
        }

        TicketRecord& t = daftar.data[idx];
        if (t.status == "SELESAI") {
            cout << "Status sudah SELESAI.\n";
            return;
        }

        t.status = "SELESAI";
        simpanSemuaTiket(daftar);
        cout << "Status penerbangan telah ditandai SELESAI.\n";
    }

    void menuRiwayat() {
        int pilih;
        do {
            cout << "\n=== MENU RIWAYAT PEMESANAN ===\n";
            cout << "1. Lihat Riwayat Pemesanan\n";
            cout << "2. Tandai Penerbangan Selesai\n";
            cout << "3. Kembali\n";
            cout << "Pilih (1-3): ";
            if (!(cin >> pilih)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Input tidak valid.\n";
                continue;
            }
            cin.ignore(10000, '\n');

            switch (pilih) {
                case 1: tampilkanRiwayatPemesanan(); break;
                case 2: tandaiPenerbanganSelesai(); break;
                case 3: cout << "Kembali...\n"; break;
                default: cout << "Pilihan tidak valid.\n"; break;
            }
        } while (pilih != 3);
    }

    void tampilkanJadwalPenerbangan() {
        ifstream file("data_penerbangan.txt");
        if (!file.is_open()) {
            cout << "Belum ada data penerbangan.\n";
            return;
        }

        cout << "\n=== JADWAL PENERBANGAN ===\n\n";
        string asal, tujuan, tanggal, jam, maskapai, kursi, gate;
        double bagasi;
        long harga;
        int i = 1;

        while (file >> asal >> tujuan >> tanggal >> jam >> bagasi >> harga >> kursi >> gate) {
            file.ignore();
            getline(file, maskapai);
            cout << i++ << ". " << maskapai << "\n";
            cout << "   Rute   : " << asal << " -> " << tujuan << "\n";
            cout << "   Jadwal : " << tanggal << " | " << jam << "\n";
            cout << "   Bagasi : " << bagasi << " kg\n";
            cout << "   Harga  : " << formatRupiah(harga) << "\n\n";
        }

        if (i == 1) cout << "Tidak ada data penerbangan.\n";

        file.close();
    }

    void tampilkanTreePenerbangan(Penerbangan daftar[], int jumlah) {
        // Bangun daftar penerbangan dari file; jika kosong pakai data default parameter
        DynamicArraySimple<Penerbangan> daftarFile = bacaPenerbanganDariFile();
        if (daftarFile.size == 0) {
            for (int i = 0; i < jumlah; ++i) daftarFile.push_back(daftar[i]);
        }

        // Bangun BST
        clearTree(rootTree);
        rootTree = nullptr;
        for (int i = 0; i < daftarFile.size; ++i) {
            rootTree = insertNode(rootTree, daftarFile.data[i]);
        }

        if (!rootTree) {
            cout << "Belum ada data penerbangan.\n";
            return;
        }

        cout << "\n=== TREE JADWAL PENERBANGAN (Inorder) ===\n";
        int urut = 1;
        inorderPrint(rootTree, urut);
    }

    void tampilkanProfilSaya() {
        if (!sistemAkun.isLogged()) {
            cout << "Anda belum login. Silakan login terlebih dahulu.\n";
            return;
        }

        cout << "\n=== PROFIL SAYA ===\n";
        cout << "Username : " << sistemAkun.getCurrentUser() << "\n";
        cout << "Password : " << sistemAkun.getCurrentPass() << "\n";
    }

    void menuPelanggan(Penerbangan daftar[], int jumlah) {
        // Set KTP list ke sistemAkun jika sudah diinisialisasi
        if (ktpList != nullptr) {
            sistemAkun.setKTPList(ktpList);
        }
        
        int sub;
        bool loginBerhasil = false;

        do {
            cout << "\n=== MENU PELANGGAN ===\n";
            cout << "1. Register Akun Baru\n";
            cout << "2. Login\n";
            cout << "3. Kembali\n";
            cout << "Pilih: ";
            if (!(cin >> sub)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Input tidak valid! Masukkan angka 1-3.\n";
                continue;
            }
            cin.ignore(10000, '\n');

            switch (sub) {
                case 1:
                    sistemAkun.registerAkun();
                    break;

                case 2:
                    loginBerhasil = sistemAkun.loginAkun();
                    if (loginBerhasil) {
                        int menu;
                        do {
                            cout << "\n=== MENU PELANGGAN ===\n";
                            cout << "Haloo (Tuan/Nona), Selamat datang di website kami ><.\n";
                            cout << "1. Lihat Jadwal Penerbangan\n";
                            cout << "2. Pesan Tiket\n";
                            cout << "3. Tiket Saya\n";
                            cout << "4. Check-In Tiket\n";
                            cout << "5. Riwayat Pemesanan\n";
                            cout << "6. Cari Data Pelanggan\n";
                            cout << "7. Profil Saya\n";
                            cout << "8. Keluar\n";
                            cout << "Pilih (1-8): ";
                            if (!(cin >> menu)) {
                                cin.clear();
                                cin.ignore(10000, '\n');
                                cout << "Input tidak valid! Masukkan angka 1-8.\n";
                                continue;
                            }
                            cin.ignore(10000, '\n');

                            switch (menu) {
                                case 1:
                                    tampilkanJadwalPenerbangan();
                                    break;

                                case 2:
                                    pesanTiket(daftar, jumlah);
                                    break;

                                case 3:
                                    tampilkanTiketSaya();
                                    break;

                                case 4:
                                    checkInTiket(daftar, jumlah);
                                    break;

                                case 5:
                                    menuRiwayat();
                                    break;

                                case 6:
                                    pelanggan.cariData();
                                    break;

                                case 7:
                                    tampilkanProfilSaya();
                                    break;

                                case 8:
                                    cout << "Keluar dari akun...\n";
                                    sistemAkun.logout();
                                    break;

                                default:
                                    cout << "Pilihan tidak valid!\n";
                                    break;
                            }
                        } while (menu != 8);
                    }
                    break;

                case 3:
                    cout << "Kembali ke menu utama...\n";
                    break;

                default:
                    cout << "Pilihan tidak valid!\n";
                    break;
            }
        } while (sub != 3);
    }
};

#endif

