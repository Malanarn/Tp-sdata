#ifndef AdminFeature_
#define AdminFeature_

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

template <class T>
class DynamicArray {
public:
    T* data;
    int size;
    int capacity;

    DynamicArray() : data(nullptr), size(0), capacity(0) {}

    ~DynamicArray() { delete[] data; }

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

    void eraseAt(int idx) {
        if (idx < 0 || idx >= size) return;
        for (int i = idx; i < size - 1; ++i) data[i] = data[i + 1];
        --size;
    }
};

// =====================================================
// Struct Penerbangan
// =====================================================
struct Penerbangan {
    string asal;
    string tujuan;
    struct {
        string tanggal;
        string jam;
    } jadwal;
    double maxBeratBagasi;
    string maskapai;
};

// =====================================================
// Linked List Penerbangan Aktif 
// =====================================================
struct NodePenerbangan {
    Penerbangan data;
    int next; 
};

class PenerbanganAktif {
private:
    DynamicArray<NodePenerbangan> pool; 
    int head; 
    const string namaFile = "penerbangan_aktif.txt";

    int allocateNode(const Penerbangan& p) {
        NodePenerbangan n;
        n.data = p;
        n.next = -1;
        pool.push_back(n);
        return pool.size - 1;
    }

public:
    PenerbanganAktif() : head(-1) {
        muatDariFile();
    }

    ~PenerbanganAktif() {
        simpanKeFile();
    }

    void tambahPenerbangan() {
        DynamicArray<Penerbangan> daftarMaster;
        ifstream masterFile("data_penerbangan.txt");
        if (!masterFile.is_open()) {
            cout << "File data_penerbangan.txt tidak ditemukan!\n";
            return;
        }

        string asal, tujuan, tanggal, jam, maskapai;
        double bagasi;

        while (masterFile >> asal >> tujuan >> tanggal >> jam >> bagasi) {
            masterFile.ignore();
            getline(masterFile, maskapai);
            Penerbangan p = {asal, tujuan, {tanggal, jam}, bagasi, maskapai};
            daftarMaster.push_back(p);
        }
        masterFile.close();

        if (daftarMaster.size == 0) {
            cout << "Belum ada data penerbangan di file master.\n";
            return;
        }

        cout << "\n=== DAFTAR PENERBANGAN MASTER ===\n";
        for (int j = 0; j < daftarMaster.size; ++j) {
            Penerbangan& p = daftarMaster.data[j];
            cout << (j+1) << ". " << p.maskapai << " | " << p.asal << " -> " << p.tujuan
                 << " | " << p.jadwal.tanggal << " " << p.jadwal.jam
                 << " | Bagasi: " << p.maxBeratBagasi << "kg\n";
        }

        int pilih;
        cout << "\nPilih nomor penerbangan untuk dijalankan hari ini (0 untuk batal): ";
        cin >> pilih;

        if (pilih == 0) {
            cout << "Dibatalkan.\n";
            return;
        }
        if (pilih < 1 || pilih > daftarMaster.size) {
            cout << "Nomor tidak valid.\n";
            return;
        }

        Penerbangan p = daftarMaster.data[pilih - 1];
        int idx = allocateNode(p);

        if (head == -1) {
            head = idx;
        } else {
            int cur = head;
            while (pool.data[cur].next != -1) cur = pool.data[cur].next;
            pool.data[cur].next = idx;
        }

        cout << "\nPenerbangan " << p.maskapai << " (" << p.asal << " -> " << p.tujuan
             << ") berhasil ditambahkan ke daftar aktif!\n";
    }

    void hapusPenerbangan() {
        if (head == -1) {
            cout << "Tidak ada penerbangan aktif.\n";
            return;
        }

        tampilkanPenerbangan();
        string asal, tujuan;
        cout << "\nMasukkan asal penerbangan yang ingin dihapus: ";
        cin >> asal;
        cout << "Masukkan tujuan penerbangan yang ingin dihapus: ";
        cin >> tujuan;

        int cur = head;
        int prev = -1;
        int foundIdx = -1;
        while (cur != -1) {
            if (pool.data[cur].data.asal == asal && pool.data[cur].data.tujuan == tujuan) {
                foundIdx = cur;
                break;
            }
            prev = cur;
            cur = pool.data[cur].next;
        }

        if (foundIdx == -1) {
            cout << "Penerbangan tidak ditemukan.\n";
            return;
        }

        DynamicArray<NodePenerbangan> newPool;
        int newHead = -1;
        int lastNew = -1;

        for (int i = 0; i < pool.size; ++i) {
            if (i == foundIdx) continue;
            NodePenerbangan copy = pool.data[i];
            copy.next = -1;
            newPool.push_back(copy);
            if (lastNew != -1) {
                newPool.data[lastNew].next = (int)newPool.size - 1;
            } else {
                newHead = (int)newPool.size - 1;
            }
            lastNew = (int)newPool.size - 1;
        }

        pool.~DynamicArray();
        pool = newPool;
        head = newHead;

        cout << "Penerbangan aktif berhasil dihapus!\n";
    }

    void tampilkanPenerbangan() {
        if (head == -1) {
            cout << "\nBelum ada penerbangan aktif.\n";
            return;
        }

        cout << "\n=== DAFTAR PENERBANGAN AKTIF ===\n";
        cout << "-------------------------------------------------------------\n";
        cout << "No | Maskapai | Asal -> Tujuan | Tanggal | Jam | Bagasi Maks\n";
        cout << "-------------------------------------------------------------\n";

        int temp = head;
        int i = 1;
        while (temp != -1) {
            Penerbangan& p = pool.data[temp].data;
            cout << i++ << ". " << p.maskapai << " | " << p.asal << " -> " << p.tujuan
                 << " | " << p.jadwal.tanggal << " | " << p.jadwal.jam
                 << " | " << p.maxBeratBagasi << "kg\n";
            temp = pool.data[temp].next;
        }
        cout << "-------------------------------------------------------------\n";
    }

    void simpanKeFile() {
        ofstream file(namaFile, ios::trunc);
        if (!file.is_open()) return;

        int cur = head;
        while (cur != -1) {
            Penerbangan& p = pool.data[cur].data;
            file << p.asal << " " << p.tujuan << " " << p.jadwal.tanggal << " "
                 << p.jadwal.jam << " " << p.maxBeratBagasi << " " << p.maskapai << endl;
            cur = pool.data[cur].next;
        }
        file.close();
    }

    void muatDariFile() {
        ifstream file(namaFile);
        if (!file.is_open()) return;

        string asal, tujuan, tanggal, jam, maskapai;
        double bagasi;

        while (file >> asal >> tujuan >> tanggal >> jam >> bagasi) {
            file.ignore();
            getline(file, maskapai);
            Penerbangan p = {asal, tujuan, {tanggal, jam}, bagasi, maskapai};
            int idx = allocateNode(p);
            if (head == -1) head = idx;
            else {
                int cur = head;
                while (pool.data[cur].next != -1) cur = pool.data[cur].next;
                pool.data[cur].next = idx;
            }
        }
        file.close();
    }

    void menuPenerbanganAktif() {
        int pilih;
        do {
            cout << "\n=== MENU PENERBANGAN AKTIF ===\n";
            cout << "1. Tambah Penerbangan Aktif\n";
            cout << "2. Hapus Penerbangan Aktif\n";
            cout << "3. Lihat Semua Penerbangan Aktif\n";
            cout << "4. Simpan ke File\n";
            cout << "5. Kembali\n";
            cout << "Pilih (1-5): ";
            cin >> pilih;

            switch (pilih) {
                case 1: tambahPenerbangan(); break;
                case 2: hapusPenerbangan(); break;
                case 3: tampilkanPenerbangan(); break;
                case 4: simpanKeFile(); cout << "? Data disimpan ke file.\n"; break;
                case 5: cout << "Kembali ke menu admin...\n"; break;
                default: cout << "Pilihan tidak valid!\n"; break;
            }
        } while (pilih != 5);
    }
};

// =====================================================
// Doubly Linked List Penumpang Penerbangan 
// =====================================================
struct Penumpang {
    string nama;
    int kursi;
    string asal, tujuan, maskapai;
};

struct NodePenumpang {
    Penumpang data;
    int next; 
    int prev; 
};

class DaftarPenumpang {
private:
    DynamicArray<NodePenumpang> pool;
    int head;
    int tail;
    const string namaFile = "penumpang_penerbangan.txt";
    int jumlahKursi;

    int allocateNode(const Penumpang& p) {
        NodePenumpang n;
        n.data = p;
        n.next = -1;
        n.prev = -1;
        pool.push_back(n);
        return pool.size - 1;
    }

public:
    DaftarPenumpang() : head(-1), tail(-1), jumlahKursi(0) {
        muatDariFile();
    }

    ~DaftarPenumpang() {
        simpanKeFile();
    }

    void tambahPenumpang() {
        Penumpang p;
        cout << "\n=== TAMBAH PENUMPANG PESAWAT ===\n";
        cout << "Nama penumpang: ";
        cin.ignore();
        getline(cin, p.nama);
        cout << "Asal: ";
        cin >> p.asal;
        cout << "Tujuan: ";
        cin >> p.tujuan;
        cin.ignore();
        cout << "Maskapai: ";
        getline(cin, p.maskapai);

        p.kursi = ++jumlahKursi;
        int idx = allocateNode(p);

        if (head == -1) {
            head = tail = idx;
        } else {
            pool.data[tail].next = idx;
            pool.data[idx].prev = tail;
            tail = idx;
        }

        cout << "? Penumpang berhasil ditambahkan! (Kursi " << p.kursi << ")\n";
    }

    void hapusPenumpang() {
        if (head == -1) {
            cout << "Tidak ada penumpang di pesawat.\n";
            return;
        }

        tampilkanPenumpangMaju();
        int nomor;
        cout << "\nMasukkan nomor kursi yang ingin dihapus: ";
        cin >> nomor;

        int found = -1;
        for (int i = 0; i < pool.size; ++i) {
            if (pool.data[i].data.kursi == nomor) { found = i; break; }
        }

        if (found == -1) {
            cout << "Penumpang dengan kursi " << nomor << " tidak ditemukan.\n";
            return;
        }

        int pprev = pool.data[found].prev;
        int pnext = pool.data[found].next;

        if (pprev == -1 && pnext == -1) { // only node
            head = tail = -1;
        } else if (pprev == -1) {
            head = pnext;
            pool.data[pnext].prev = -1;
        } else if (pnext == -1) {
            tail = pprev;
            pool.data[pprev].next = -1;
        } else {
            pool.data[pprev].next = pnext;
            pool.data[pnext].prev = pprev;
        }

        DynamicArray<NodePenumpang> newPool;
        int newHead = -1, newTail = -1;
        int* mapIdx = new int[pool.size];
        for (int i = 0; i < pool.size; ++i) mapIdx[i] = -1;
        for (int i = 0; i < pool.size; ++i) {
            if (i == found) continue;
            NodePenumpang copy = pool.data[i];
            copy.next = -1;
            copy.prev = -1;
            newPool.push_back(copy);
            int newI = newPool.size - 1;
            mapIdx[i] = newI;
            if (newTail != -1) {
                newPool.data[newTail].next = newI;
                newPool.data[newI].prev = newTail;
                newTail = newI;
            } else {
                newHead = newTail = newI;
            }
        }
        
        head = newHead;
        tail = newTail;

        delete[] mapIdx;
        pool.~DynamicArray();
        pool = newPool;

        cout << "? Penumpang kursi " << nomor << " berhasil dihapus!\n";
    }

    void tampilkanPenumpangMaju() {
        if (head == -1) {
            cout << "\nBelum ada penumpang.\n";
            return;
        }

        cout << "\n=== DAFTAR PENUMPANG (MAJU) ===\n";
        cout << "---------------------------------------------------------------\n";
        cout << "Kursi | Nama | Maskapai | Asal -> Tujuan\n";
        cout << "---------------------------------------------------------------\n";

        int temp = head;
        while (temp != -1) {
            cout << pool.data[temp].data.kursi << " | " << pool.data[temp].data.nama << " | "
                 << pool.data[temp].data.maskapai << " | " << pool.data[temp].data.asal << " -> "
                 << pool.data[temp].data.tujuan << endl;
            temp = pool.data[temp].next;
        }
    }

    void tampilkanPenumpangMundur() {
        if (tail == -1) {
            cout << "\nBelum ada penumpang.\n";
            return;
        }

        cout << "\n=== DAFTAR PENUMPANG (MUNDUR) ===\n";
        cout << "---------------------------------------------------------------\n";
        cout << "Kursi | Nama | Maskapai | Asal -> Tujuan\n";
        cout << "---------------------------------------------------------------\n";

        int temp = tail;
        while (temp != -1) {
            cout << pool.data[temp].data.kursi << " | " << pool.data[temp].data.nama << " | "
                 << pool.data[temp].data.maskapai << " | " << pool.data[temp].data.asal << " -> "
                 << pool.data[temp].data.tujuan << endl;
            temp = pool.data[temp].prev;
        }
    }

    void simpanKeFile() {
        ofstream file(namaFile, ios::trunc);
        if (!file.is_open()) return;

        int cur = head;
        while (cur != -1) {
            Penumpang& p = pool.data[cur].data;
            file << p.kursi << " " << p.nama << " " << p.asal << " "
                 << p.tujuan << " " << p.maskapai << endl;
            cur = pool.data[cur].next;
        }
        file.close();
    }

    void muatDariFile() {
        ifstream file(namaFile);
        if (!file.is_open()) return;

        int kursi;
        string nama, asal, tujuan, maskapai;
        while (file >> kursi >> nama >> asal >> tujuan >> maskapai) {
            Penumpang p = {nama, kursi, asal, tujuan, maskapai};
            int idx = allocateNode(p);
            if (head == -1) {
                head = tail = idx;
            } else {
                pool.data[tail].next = idx;
                pool.data[idx].prev = tail;
                tail = idx;
            }
            jumlahKursi = kursi;
        }
        file.close();
    }

    void menuPenumpang() {
        int pilih;
        do {
            cout << "\n=== MENU PENUMPANG PESAWAT (DOUBLY LINKED LIST) ===\n";
            cout << "1. Tambah Penumpang\n";
            cout << "2. Hapus Penumpang\n";
            cout << "3. Lihat Daftar Penumpang (Maju)\n";
            cout << "4. Lihat Daftar Penumpang (Mundur)\n";
            cout << "5. Simpan ke File\n";
            cout << "6. Kembali\n";
            cout << "Pilih (1-6): ";
            cin >> pilih;

            switch (pilih) {
                case 1: tambahPenumpang(); break;
                case 2: hapusPenumpang(); break;
                case 3: tampilkanPenumpangMaju(); break;
                case 4: tampilkanPenumpangMundur(); break;
                case 5: simpanKeFile(); cout << "? Data disimpan ke file.\n"; break;
                case 6: cout << "Kembali ke menu admin...\n"; break;
                default: cout << "Pilihan tidak valid!\n"; break;
            }
        } while (pilih != 6);
    }
};

// =====================================================
// Data Pelanggan 
// =====================================================
class DataPelanggan {
public:
    void tampilkanDariFile() {
        ifstream file("data_pelanggan.txt");
        if (!file.is_open()) {
            cout << "File data pelanggan belum ada.\n";
            return;
        }

        string line;
        cout << "\n=== DATA SEMUA PELANGGAN ===\n";
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    }

    void cariData() {
        ifstream file("data_pelanggan.txt");
        if (!file.is_open()) {
            cout << "File belum ada atau gagal dibuka.\n";
            return;
        }

        string cariNama;
        cout << "\nMasukkan nama yang ingin dicari: ";
        cin.ignore();
        getline(cin, cariNama);

        string line;
        bool ditemukan = false;
        cout << "\n=== HASIL PENCARIAN ===\n";

        while (getline(file, line)) {
            if (line.find(cariNama) != string::npos) {
                cout << line << endl;
                ditemukan = true;
            }
        }

        if (!ditemukan)
            cout << "Data dengan nama '" << cariNama << "' tidak ditemukan.\n";

        file.close();
    }
};

// =====================================================
// Manajemen Data Penerbangan 
// =====================================================
class ManajemenPenerbangan {
private:
    const string namaFile = "data_penerbangan.txt";

public:
    DynamicArray<Penerbangan> bacaSemua() {
        DynamicArray<Penerbangan> result;
        ifstream file(namaFile);
        if (!file.is_open()) return result;

        string asal, tujuan, tanggal, jam, maskapai;
        double bagasi;
        while (file >> asal >> tujuan >> tanggal >> jam >> bagasi) {
            file.ignore();
            getline(file, maskapai);
            Penerbangan p = {asal, tujuan, {tanggal, jam}, bagasi, maskapai};
            result.push_back(p);
        }
        file.close();
        return result;
    }

    void simpanSemua(Penerbangan* daftar, int jumlah) {
        ofstream file(namaFile, ios::trunc);
        if (!file.is_open()) return;
        for (int i = 0; i < jumlah; ++i) {
            Penerbangan& p = daftar[i];
            file << p.asal << " " << p.tujuan << " " << p.jadwal.tanggal << " "
                 << p.jadwal.jam << " " << p.maxBeratBagasi << " " << p.maskapai << endl;
        }
        file.close();
    }

    void tambahPenerbangan() {
        Penerbangan p;
        cout << "\n=== TAMBAH DATA PENERBANGAN ===\n";
        cout << "Asal: ";
        cin >> p.asal;
        cout << "Tujuan: ";
        cin >> p.tujuan;
        cout << "Tanggal (YYYY-MM-DD): ";
        cin >> p.jadwal.tanggal;
        cout << "Jam (HH:MM): ";
        cin >> p.jadwal.jam;
        cout << "Maksimum Berat Bagasi (20kg): ";
        cin >> p.maxBeratBagasi;
        cin.ignore();
        cout << "Maskapai: ";
        getline(cin, p.maskapai);

        ofstream file(namaFile, ios::app);
        file << p.asal << " " << p.tujuan << " " << p.jadwal.tanggal << " "
             << p.jadwal.jam << " " << p.maxBeratBagasi << " " << p.maskapai << endl;
        file.close();

        cout << "\nPenerbangan berhasil ditambahkan!\n";
    }

    void tampilkanPenerbangan() {
        ifstream file(namaFile);
        if (!file.is_open()) {
            cout << "Belum ada data penerbangan.\n";
            return;
        }

        cout << "\n=== DAFTAR SEMUA PENERBANGAN ===\n";
        string asal, tujuan, tanggal, jam, maskapai;
        double bagasi;
        int i = 1;
        while (file >> asal >> tujuan >> tanggal >> jam >> bagasi) {
            file.ignore();
            getline(file, maskapai);
            cout << i++ << ". " << maskapai << " | " << asal << " -> " << tujuan
                 << " | " << tanggal << " " << jam << " | Bagasi Maks: " << bagasi << "kg\n";
        }
        file.close();
    }

    void ubahPenerbangan() {
        DynamicArray<Penerbangan> daftar = bacaSemua();
        if (daftar.size == 0) {
            cout << "Belum ada data penerbangan.\n";
            return;
        }

        tampilkanPenerbangan();
        int index;
        cout << "\nMasukkan nomor penerbangan yang ingin diubah: ";
        cin >> index;
        if (index < 1 || index > daftar.size) {
            cout << "Nomor tidak valid.\n";
            return;
        }

        Penerbangan& p = daftar.data[index - 1];
        cout << "\nMasukkan data baru (biarkan kosong untuk tidak diubah):\n";
        cout << "Asal [" << p.asal << "]: "; string asalBaru; cin >> asalBaru;
        if (!asalBaru.empty()) p.asal = asalBaru;
        cout << "Tujuan [" << p.tujuan << "]: "; string tujuanBaru; cin >> tujuanBaru;
        if (!tujuanBaru.empty()) p.tujuan = tujuanBaru;
        cout << "Tanggal [" << p.jadwal.tanggal << "]: "; string tanggalBaru; cin >> tanggalBaru;
        if (!tanggalBaru.empty()) p.jadwal.tanggal = tanggalBaru;
        cout << "Jam [" << p.jadwal.jam << "]: "; string jamBaru; cin >> jamBaru;
        if (!jamBaru.empty()) p.jadwal.jam = jamBaru;
        cout << "Bagasi Maks [" << p.maxBeratBagasi << "]: "; double bagasiBaru; cin >> bagasiBaru;
        if (bagasiBaru > 0) p.maxBeratBagasi = bagasiBaru;
        cin.ignore(10000, '\n');
        cout << "Maskapai [" << p.maskapai << "]: "; string maskapaiBaru; getline(cin, maskapaiBaru);
        if (!maskapaiBaru.empty()) p.maskapai = maskapaiBaru;

        ofstream file(namaFile, ios::trunc);
        for (int i = 0; i < daftar.size; ++i) {
            Penerbangan& pi = daftar.data[i];
            file << pi.asal << " " << pi.tujuan << " " << pi.jadwal.tanggal << " "
                 << pi.jadwal.jam << " " << pi.maxBeratBagasi << " " << pi.maskapai << endl;
        }
        file.close();

        cout << "\nData penerbangan berhasil diubah!\n";
    }

    void hapusPenerbangan() {
        DynamicArray<Penerbangan> daftar = bacaSemua();
        if (daftar.size == 0) {
            cout << "Belum ada data penerbangan.\n";
            return;
        }

        tampilkanPenerbangan();
        int index;
        cout << "\nMasukkan nomor penerbangan yang ingin dihapus: ";
        cin >> index;
        if (index < 1 || index > daftar.size) {
            cout << "Nomor tidak valid.\n";
            return;
        }

        for (int i = index - 1; i < daftar.size - 1; ++i) {
            daftar.data[i] = daftar.data[i + 1];
        }
        daftar.size--;

        ofstream file(namaFile, ios::trunc);
        for (int i = 0; i < daftar.size; ++i) {
            Penerbangan& pi = daftar.data[i];
            file << pi.asal << " " << pi.tujuan << " " << pi.jadwal.tanggal << " "
                 << pi.jadwal.jam << " " << pi.maxBeratBagasi << " " << pi.maskapai << endl;
        }
        file.close();

        cout << "\nPenerbangan berhasil dihapus!\n";
    }

    void menuPenerbangan() {
        int pilih;
        do {
            cout << "\n=== MENU KELOLA PENERBANGAN ===\n";
            cout << "1. Tambah Penerbangan\n";
            cout << "2. Lihat Semua Penerbangan\n";
            cout << "3. Ubah Penerbangan\n";
            cout << "4. Hapus Penerbangan\n";
            cout << "5. Kembali\n";
            cout << "Pilih (1-5): ";
            cin >> pilih;

            switch (pilih) {
                case 1: tambahPenerbangan(); break;
                case 2: tampilkanPenerbangan(); break;
                case 3: ubahPenerbangan(); break;
                case 4: hapusPenerbangan(); break;
                case 5: cout << "Kembali ke menu admin...\n"; break;
                default: cout << "Pilihan tidak valid!\n"; break;
            }
        } while (pilih != 5);
    }
};

// =====================================================
// Queue Check-in Penumpang
// =====================================================
class QueueCheckIn {
private:
    DynamicArray<Penumpang> queue;
    int frontIndex;

public:
    QueueCheckIn() : frontIndex(0) {}

    bool isEmpty() {
        return frontIndex >= queue.size;
    }

    void enqueue() {
        Penumpang p;
        cout << "\n=== TAMBAH ANTRIAN CHECK-IN ===\n";
        cin.ignore();
        cout << "Nama: ";
        getline(cin, p.nama);
        cout << "Asal: ";
        cin >> p.asal;
        cout << "Tujuan: ";
        cin >> p.tujuan;
        cin.ignore();
        cout << "Maskapai: ";
        getline(cin, p.maskapai);

        queue.push_back(p);
        cout << "Penumpang masuk antrian check-in!\n";
    }

    void dequeue() {
        if (isEmpty()) {
            cout << "? Tidak ada penumpang dalam antrian.\n";
            return;
        }
        Penumpang& p = queue.data[frontIndex];
        cout << "? Penumpang diproses check-in: " << p.nama << endl;
        frontIndex++;
    }

    void lihatDepan() {
        if (isEmpty()) {
            cout << "Antrian kosong.\n";
            return;
        }
        Penumpang& p = queue.data[frontIndex];
        cout << "\n?? Penumpang berikutnya: " << p.nama << " | "
             << p.asal << " ? " << p.tujuan << " | " << p.maskapai << endl;
    }

    void tampilkanSemua() {
        if (isEmpty()) {
            cout << "Antrian kosong.\n";
            return;
        }
        cout << "\n=== DAFTAR ANTRIAN CHECK-IN ===\n";
        int nomor = 1;
        for (int i = frontIndex; i < queue.size; i++) {
            Penumpang& p = queue.data[i];
            cout << nomor++ << ". " << p.nama << " | "
                 << p.asal << " ? " << p.tujuan << " | "
                 << p.maskapai << endl;
        }
    }

    void menuQueue() {
        int pilih;
        do {
            cout << "\n=== MENU ANTRIAN CHECK-IN (QUEUE) ===\n";
            cout << "1. Tambah ke Antrian (Enqueue)\n";
            cout << "2. Proses Penumpang (Dequeue)\n";
            cout << "3. Lihat Penumpang Terdepan\n";
            cout << "4. Lihat Semua Antrian\n";
            cout << "5. Kembali\n";
            cout << "Pilih (1-5): ";
            cin >> pilih;

            switch (pilih) {
                case 1: enqueue(); break;
                case 2: dequeue(); break;
                case 3: lihatDepan(); break;
                case 4: tampilkanSemua(); break;
                case 5: cout << "Kembali...\n"; break;
                default: cout << "Pilihan tidak valid!\n";
            }
        } while (pilih != 5);
    }
};


// =====================================================
// Menu Admin
// =====================================================
class AdminFeature {
private:
    string usernameAdmin = "admin";
    string passwordAdmin = "123";
    DataPelanggan pelanggan;
    ManajemenPenerbangan penerbangan;
    PenerbanganAktif aktif;
    DaftarPenumpang penumpang;
    QueueCheckIn queueCheckin;

public:
    bool loginAdmin() {
        string user, pass;
        cout << "\n=== LOGIN ADMIN ===\n";
        cout << "Username: ";
        cin >> user;
        cout << "Password: ";
        cin >> pass;
        if (user == usernameAdmin && pass == passwordAdmin) {
            cout << "Login berhasil! Selamat datang, Admin.\n";
            return true;
        }
        cout << "Login gagal! Username atau password salah.\n";
        return false;
    }

    void menuAdmin() {
        int pilih;
        do {
            cout << "\n=== MENU ADMIN ===\n";
            cout << "1. Lihat Semua Data Pelanggan\n";
            cout << "2. Cari Data Pelanggan\n";
            cout << "3. Kelola Data Penerbangan (Master)\n";
            cout << "4. Kelola Penerbangan Aktif (Linked List)\n";
            cout << "5. Kelola Penumpang Pesawat (Doubly Linked List)\n";
            cout << "6. Kelola Antrian Check-In Penumpang\n";
            cout << "7. Keluar\n";
            cout << "Pilih (1-6): ";
            cin >> pilih;

            switch (pilih) {
                case 1: pelanggan.tampilkanDariFile(); 
				break;
                case 2: pelanggan.cariData(); 
				break;
                case 3: penerbangan.menuPenerbangan(); 
				break;
                case 4: aktif.menuPenerbanganAktif(); 
				break;
                case 5: penumpang.menuPenumpang(); 
				break;
				case 6: queueCheckin.menuQueue();
				break;
                case 7: cout << "Keluar dari menu admin...\n"; 
				break;
                default: cout << "Pilihan tidak valid!\n"; 
				break;
            }
        } while (pilih != 7);
    }
};

#endif
