# Warehouse Explorer: Advanced Data Structures Project 📦
Sistem manajemen gudang komprehensif berbasis CLI yang mengintegrasikan 5 struktur data utama: AVL Tree, Hash Table, Max Heap, Stack, dan Linked List untuk efisiensi data

![Language](https://img.shields.io/badge/Language-C-blue)
![Architecture](https://img.shields.io/badge/Architecture-MVC%20(ish)-orange)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey)

**Warehouse Explorer** adalah aplikasi manajemen inventaris berbasis terminal (CLI) yang dibangun untuk mendemonstrasikan integrasi berbagai struktur data kompleks dalam satu ekosistem aplikasi. Program ini mensimulasikan operasi CRUD, pemrosesan antrian prioritas, dan fitur undo/rollback data.

## 🧠 Implementasi Struktur Data

Proyek ini bukan sekadar aplikasi CRUD biasa, melainkan implementasi nyata dari konsep struktur data berikut:

| Struktur Data | Implementasi di Program | Fungsi Utama |
| :--- | :--- | :--- |
| **Hash Table** | *Separate Chaining Collision Handling* | Penyimpanan data utama untuk akses data super cepat (O(1)) berdasarkan ID Barang. |
| **AVL Tree** | *Self-Balancing Binary Search Tree* | Menyimpan data yang terurut secara otomatis berdasarkan Nama Barang untuk keperluan display dan searching yang seimbang. |
| **Max Heap** | *Priority Queue* | Mengelola antrian pemrosesan barang ("Proses Barang"), di mana barang dengan **Harga Tertinggi** diprioritaskan untuk diproses/dikirim duluan. |
| **Stack** | *LIFO (Last In First Out)* | Fitur **Update & Rollback**. Menyimpan riwayat perubahan data sementara, memungkinkan user melakukan *Undo* ke kondisi sebelumnya. |
| **Linked List** | *Singly Linked List* | Mencatat **History/Log** aktivitas (penambahan, penghapusan, edit) secara kronologis. |

## 🌟 Fitur Utama

1.  **Multi-User Access**:
    * **Admin**: Akses penuh (Input, Delete, Update + Rollback, Process Queue).
    * **Guest**: Read-only (Search & Display).
2.  **Smart Validation**: Validasi input ketat untuk Nama (Kapitalisasi), Harga, dan Kategori.
3.  **Data Processing Simulation**: Simulasi memindahkan barang dari gudang ke antrian pengiriman menggunakan Priority Queue.
4.  **Audit Trail**: Setiap aksi modifikasi data tercatat otomatis dalam log history.

## 🔐 Kredensial Login (Default)

Untuk mencoba program, gunakan akun berikut:

| Role | Username | Password |
| :--- | :--- | :--- |
| **Admin** | `Admin` | `Admin` |
| **Guest** | `Guest` | `Guest` |

*(Login bersifat Case Sensitive)*

## 🛠️ Cara Menjalankan

### Prasyarat
Program ini menggunakan library dan command sistem yang spesifik untuk **Windows** (`system("cls")`, `system("pause")`).

### Langkah-langkah
1.  **Clone repository:**
    ```bash
    git clone [https://github.com/gabrielle-jeco/Advanced-Data-Structures-Implementation-C.git](https://github.com/gabrielle-jeco/Advanced-Data-Structures-Implementation-C.git)
    ```
2.  **Compile Program:**
    ```bash
    gcc aol_dastruct.c -o warehouse
    ```
3.  **Jalankan:**
    ```bash
    ./warehouse
    ```
