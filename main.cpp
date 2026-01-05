#include <iostream>
#include <fstream>
#include "adminFeature.h"
#include "customerFeature.h"
using namespace std;

void inisialisasiDataPenerbangan() {
    ifstream cekFile("data_penerbangan.txt");
    bool kosong = false;
    if (!cekFile.is_open()) kosong = true;
    else {
        kosong = (cekFile.peek() == ifstream::traits_type::eof());
        cekFile.close();
    }

    if (!kosong) return; // file sudah berisi, tidak perlu isi ulang

    cout << "Mengisi data penerbangan awal...\n";

    // Array statis data awal (Master Data - SATU SUMBER)
    Penerbangan dataAwal[] = {
        {"Jakarta", "Surabaya", {"2025-11-10", "08:00"}, 20, "Garuda Indonesia"},
        {"Jakarta", "Denpasar", {"2025-11-12", "10:00"}, 20, "Lion Air"},
        {"Surabaya", "Medan", {"2025-11-14", "06:30"}, 20, "Citilink"},
        {"Medan", "Jakarta", {"2025-11-16", "09:45"}, 20, "AirAsia"}
    };

    int jumlahData = sizeof(dataAwal) / sizeof(Penerbangan);

    ofstream file("data_penerbangan.txt", ios::app);
    for (int i = 0; i < jumlahData; i++) {
        file << dataAwal[i].asal << " " << dataAwal[i].tujuan << " "
             << dataAwal[i].jadwal.tanggal << " " << dataAwal[i].jadwal.jam << " "
             << dataAwal[i].maxBeratBagasi << " " << dataAwal[i].maskapai << endl;
    }
    file.close();
}

int main() {
    inisialisasiDataPenerbangan();
    AdminFeature admin;
    CustomerFeature pelanggan;
    
    // Muat data KTP dari file ke dalam memori
    admin.getKTPList()->muatDariFile();
    
    // Set KTP list untuk pelanggan agar bisa validasi NIK saat registrasi
    pelanggan.setKTPList(admin.getKTPList());

    // =====================
    // Data penerbangan statis (GUNAKAN DATAWAL sebagai master)
    // =====================
    Penerbangan daftarPenerbangan[] = {
        {"Jakarta", "Surabaya", {"2025-11-10", "08:00"}, 20, "Garuda Indonesia"},
        {"Jakarta", "Denpasar", {"2025-11-12", "10:00"}, 25, "Lion Air"},
        {"Surabaya", "Medan", {"2025-11-14", "06:30"}, 15, "Citilink"},
        {"Medan", "Jakarta", {"2025-11-16", "09:45"}, 20, "AirAsia"}
    };
    int jumlahPenerbangan = sizeof(daftarPenerbangan) / sizeof(Penerbangan);

    int pilihan;
    do {
        cout << "\n=== SISTEM BANDARA ===\n";
        cout << "1. Menu Admin\n";
        cout << "2. Menu Pelanggan\n";
        cout << "3. Keluar\n";
        cout << "Pilih (1-3): ";
        cin >> pilihan;

        switch (pilihan) {
            case 1:
                if (admin.loginAdmin()) admin.menuAdmin();
                break;
            case 2:
                pelanggan.menuPelanggan(daftarPenerbangan, jumlahPenerbangan);
                break;
            case 3:
                cout << "Keluar dari program...\n";
                break;
            default:
                cout << "Pilihan tidak valid!\n";
                break;
        }
    } while (pilihan != 3);

    cout << "Terima kasih telah menggunakan Sistem Bandara!\n";
    return 0;
}

