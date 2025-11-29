#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#define MAX 10

//define struct untuk prototipe tipe data 'barang'
//memiliki atribut id, nama, harga, dan kategori
typedef struct Barang{
	char id[6];
	char namaBarang[100];
	int hargaBarang;
	char kategori[15];
}barang;

//node linked list untuk menampung riwayat barang
typedef struct HistoryNode{
	barang bg;
	char note[50];
	struct HistoryNode* next;
}historyNode;
historyNode* histHead;	//pointer ke head dari linked list riwayat barang

//menggunakan array untuk menampung kategori barang
//karena kategori relatif sedikit, maka lebih baik menggunakan array
char categories[][15] = 
	{
		"Makanan",
		"Minuman",
		"Elektronik",
		"Pakaian",
		"Lainnya"
	};

//node linked list untuk hash table with separate chaining
typedef struct HashNode{
	barang bg;
	struct HashNode* next;
}hashNode;
hashNode* buckets[MAX];	//array of node hash table
int size;				//tracker ukuran hash table

//struct untuk queue yang diimplementasikan dengan max heap nantinya
typedef struct PriorityQueue{
	barang bg[MAX];		//menyimpan barang dalam queue
	int size;			//tracker ukuran queue
}queue;
queue q;

//struct untuk stack
typedef struct RollbackStack{
	barang bg[MAX];		//menyimpan barang dalam stack
	char note[MAX][100];//notes tentang perubahan apa pada data
	int top;			//pointer top stack
}stack;
stack st;

//node avl tree
typedef struct Avl{
	barang bg;
	struct Avl* left;
	struct Avl* right;
	int height;
}avl;
avl* root;

//initialize semua pointer, ukuran, dll
void initialize(){
	for(int i = 0; i < MAX; i++){
		buckets[i] = NULL;
	}
	size = 0;
	
	histHead = NULL;
	
	q.size = 0;
	
	st.top = -1;
	
	root = NULL;
}

//fungsi untuk membuat node riwayat barang yang baru
historyNode* createHistoryNode(barang bg, char note[]){
	historyNode* newNode = malloc(sizeof(historyNode));
	
	newNode->bg = bg;
	strcpy(newNode->note, note);
	newNode->next = NULL;
	
	return newNode;
}

//fungsi untuk menambahkan node riwayat barang baru ke list riwayat barang
//menggunakan rear insert
void addHistory(barang bg, char note[]){
	historyNode* newNode = createHistoryNode(bg, note);
	
	if(!histHead){
		histHead = newNode;		//jika head masih kosong, assign newNode ke head
		return;
	}
	
	historyNode* temp = histHead;	
	while(temp->next){
		temp = temp->next;		//traversal sampai node terakhir dari list
	}
	temp->next = newNode;		//link last node dengan newNode
}

//fungsi membuat node avl baru yang menampung barang
avl* createAvlNode(barang bg){
	avl* newNode = malloc(sizeof(avl));
	
	newNode->bg = bg;
	newNode->left = newNode->right = NULL;
	newNode->height = 1;
	
	return newNode;
}

//mendapatkan tinggi sebuah tree dan subtree
int getHeight(avl* root){
	if(!root){
		return 0;			//jika tidak ada tree/subtree, maka return 0
	}
	return root->height;	//jika ada, return heightnya
}

//mencari nilai max antara kedua bilangan
int max(int a, int b){
	return (a > b) ? a : b;
}

//fungsi left rotate
avl* leftRotate(avl* x){
	avl* y = x->right;		//y adalah anak kanan x
	avl* yChild = y->left;	//anak kiri y
	
	y->left = x;			//assign x ke kiri y
	x->right = yChild;		//assign anak kiri y sebagai anak kanan x
	
	x->height = max(getHeight(x->left), getHeight(x->right)) + 1;	//update tinggi x
	y->height = max(getHeight(y->left), getHeight(y->right)) + 1;	//update tinggi y
	
	return y;
}

//fungsi right rotate
//kebanyakan kebalikan dari fungsi left rotate (ganti left dengan right dan sebaliknya)
avl* rightRotate(avl* x){
	avl* y = x->left;
	avl* yChild = y->right;
	
	y->right = x;
	x->left =  yChild;
	
	x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
	y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
	
	return y;
}

//balance factor, tinggi anak kiri-tinggi anak kanan
int getBalance(avl* root){
	return getHeight(root->left) - getHeight(root->right);
}

//fungsi memasukkan data ke avl tree
avl* insertToAvl(avl* root, barang bg){
	if(!root){
		return createAvlNode(bg);	//jika tidak ada root, maka buat node baru dan kembalikan
	}
	
	if(strcmp(bg.namaBarang, root->bg.namaBarang) < 0){
		root->left = insertToAvl(root->left, bg);	//traverse kiri jika data bg < root bg
	}else if(strcmp(bg.namaBarang, root->bg.namaBarang) > 0){
		root->right = insertToAvl(root->right, bg);	//traverse kanan jika data bg > root bg
	}else{
		return root;	//data duplikat, kembalikan node yang berisi data yang sama
	}
	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;	//update tinggi root
	
	int balance = getBalance(root);	//hitung balance factor
	
	//proses balancing mengikuti 4 case
	//left left
	if(balance > 1 && strcmp(bg.namaBarang, root->left->bg.namaBarang) < 0){
		return rightRotate(root);
	}
	//left right
	if(balance > 1 && strcmp(bg.namaBarang, root->left->bg.namaBarang) > 0){
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}
	//right right
	if(balance < -1 && strcmp(bg.namaBarang, root->right->bg.namaBarang) > 0){
		return leftRotate(root);
	}
	//right left
	if(balance < -1 && strcmp(bg.namaBarang, root->right->bg.namaBarang) < 0){
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	
	return root;
}

//fungsi mencari inorder successor (node terkecil di sebelah kanan root)
avl* inorderSuc(avl* root){
	avl* temp = root;
	while(temp && temp->left){
		temp = temp->left;
	}
	return temp;
}

//hapus dari avl tree
avl* removeFromAvl(avl* root, barang bg){
	if(!root){
		return createAvlNode(bg);
	}
	
	if(strcmp(bg.namaBarang, root->bg.namaBarang) < 0){
		root->left = insertToAvl(root->left, bg);
	}else if(strcmp(bg.namaBarang, root->bg.namaBarang) > 0){	//proses traversal ke node yang tepat kurang lebih mirip dengan traversal di fungsi insert
		root->right = insertToAvl(root->right, bg);
	}else{
		if(!root->left || !root->right){	//jika anak kiri/anak kanan kosong, atau jika keduanya kosong
			avl* temp = (root->left) ? root->left : root->right;
			free(root);		//hapus root
			return temp;	//kembalikan temp sebagai root yang baru
		}
		//jika punya 2 anak
		avl* temp = inorderSuc(root->right);	//cari inorder successor
		root->bg = temp->bg;	//kopi data successor ke root
		root->right = removeFromAvl(root->right, bg);	//hapus inorder successor dengan algoritma penghapusan avl tree
	}
	root->height = max(getHeight(root->left), getHeight(root->right)) + 1;	//update tinggi root
	
	int balance = getBalance(root);	//hitung balance factor
	
	//left left
	if(balance > 1 && getBalance(root->left) >= 0){
		return rightRotate(root);
	}
	//left right;
	if(balance > 1 && getBalance(root->left) < 0){
		root->left = leftRotate(root->left);
		return rightRotate(root);
	}
	//right right
	if(balance < -1 && getBalance(root->right) <= 0){
		return leftRotate(root);
	}
	//right left
	if(balance < -1 && getBalance(root->right) > 0){
		root->right = rightRotate(root->right);
		return leftRotate(root);
	}
	
	return root;
}

//mencari node di avl tree
avl* searchNode(avl* root, barang bg){
	//base case jika root tidak ada (node tidak ditemukan) atau node ditemukan
	if(!root || strcmp(bg.id, root->bg.id) == 0){
		return root;
	}
	
	//traverse kiri jika bg < root bg
	if(strcmp(bg.namaBarang, root->bg.namaBarang) < 0){
		return searchNode(root->left, bg);
	}
	
	//traverse kanan jika bg > root bg	
	return searchNode(root->right, bg);
}

//buat node baru untuk hashtable
hashNode* createNode(barang bg){
	hashNode* newNode = malloc(sizeof(hashNode));
	
	newNode->bg = bg;
	newNode->next = NULL;
	
	return newNode;
}

//buat instance dari 'barang'
barang createBarang(char id[], char name[], int price, char kategori[]){
	barang bg;
	
	strcpy(bg.id, id);
	strcpy(bg.namaBarang, name);
	bg.hargaBarang = price;
	strcpy(bg.kategori, kategori);
	
	return bg;
}

//fungsi hash
//formula: sum ascii id modulo 10
unsigned int hash(char id[]){
	int sum = 0;
	for(int i = 0; i < strlen(id); i++){
		sum += id[i];
	}
	return sum % MAX;
}

//validasi nama
int isValidName(char name[]){
	if(strlen(name) < 3 || strlen(name) > 90){
		printf("3-90 huruf inklusif!\n");
		system("pause");
		return 0;
	}
	if(islower(name[0])){
		printf("Huruf pertama kapital!\n");
		system("pause");
		return 0;
	}
	return 1;
}

//validasi harga
int isValidPrice(int price){
	if(price < 1000 || price > 100000000){
		printf("1000 <= price <= 100000000 inklusif!\n");
		system("pause");
		return 0;
	}
	return 1;
}

//validasi kategori
int isValidCategory(char kategori[]){
	int rows = sizeof(categories) / sizeof(categories[0]);
	for(int i = 0; i < rows; i++){
		if(strcmp(categories[i], kategori) == 0){	//jika matching dengan salah satu kategori diatas tadi, maka valid, return 1 (true)
			return 1;
		}
	}
	//tidak match, return 0 (false)
	printf("Case sensitive!\n");
	system("pause");
	return 0;
}

//id generator dengan format BRXXX
//X = angka random 0-9
char* generateID(){
	srand(time(NULL));
	char* id = malloc(sizeof(char)*6);
	id[0] = 'B';
	id[1] = 'R';
	for(int i = 2; i < 5; i++){
		id[i] = (rand() % 10) + '0';
	}
	return id;
}

//insert barang ke index tertentu berdasarkan hasil hashing
//jika collision, maka menggunakan separate chaining (linked list), rear insert
void insertBarang(barang bg){
	hashNode* newNode = createNode(bg);
	int index = hash(bg.id);
	hashNode* head = buckets[index];
	
	if(!head){
		buckets[index] = newNode;	//index masih kosong, assign newNode ke head index tersebut
		size++;
		return;
	}
	
	while(head->next){
		head = head->next;		//traverse ke node terakhir
	}
	head->next = newNode;		//link node terakhir dengan newNode
	size++;		//tambahkan size hash table
}

//dekorasi
void inputBarangHeader(){
	printf("Tambah Barang\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//input info barang seperti nama, kategori, harga
void inputInfoBarang(){
	char id[6];
	char name[100];
	int price;
	char kategori[15];
	int valid;
	char again;
	
	do{
		valid = 0;
		do{
			system("cls");
			inputBarangHeader();
			
			printf("Input nama barang [3-90 huruf inklusif | Huruf pertama kapital | 'Exit/exit' to return]: ");
			scanf("%[^\n]", name); getchar();
			
			if(strcmp(name, "exit") == 0 || strcmp(name, "Exit") == 0){
				return;
			}
			
			valid = isValidName(name);
		}while(!valid);
		
		valid = 0;
		do{
			system("cls");
			inputBarangHeader();
			
			printf("Input harga barang [1000 <= price <= 100000000 | 0 to return]: ");
			scanf("%d", &price); getchar();
			
			if(price == 0){
				return;
			}
			
			valid = isValidPrice(price);
		}while(!valid);
		
		valid = 0;
		do{
			system("cls");
			inputBarangHeader();
			
			printf("Kategori [case sensitive | 'Exit/exit' to return]:\n");
			int rows = sizeof(categories) / sizeof(categories[0]);
			for(int i = 0; i < rows; i++){
				printf("- %s\n", categories[i]);
			}
			printf(">> ");
			scanf("%[^\n]", kategori); getchar();
			
			if(strcmp(kategori, "exit") == 0 || strcmp(kategori, "Exit") == 0){
				return;
			}
			
			valid = isValidCategory(kategori);
		}while(!valid);
		
		strcpy(id, generateID());
		
		barang bg = createBarang(id, name, price, kategori);
		insertBarang(bg);				//masukkan ke hash table
		root = insertToAvl(root, bg);	//masukkan ke avl tree
		addHistory(bg, "Insert Barang");	//otomatis menambahkan ke riwayat barang
		
		system("cls");
		inputBarangHeader();
		printf("Barang %s berhasil ditambahkan!\n", name);
		system("pause");
	
		//tanya apakah user ingin menambah barang baru lagi atau tidak
		do{
			system("cls");
			inputBarangHeader();
			
			printf("Input barang lain [y/n | case insensitive]: ");
			scanf("%c", &again); getchar();
			
			again = tolower(again);
			
			if(again != 'y' && again != 'n'){
				printf("WRONG!\n");
				system("pause");
			}
		}while(again != 'y' && again != 'n');
	}while(again == 'y');
}

//dekorasi
void removeBarangHeader(){
	printf("Hapus Barang\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//hapus barang dari hashtable
void removeBarang(){
	//validasi jika tidak ada barang sama sekali
	if(size == 0){
		system("cls");
		removeBarangHeader();
		printf("Empty! Cannot delete data!\n\n");
		system("pause");
		return;
	}
	
	char id[6];
	barang bg;
	hashNode* head;
	int index;
	char again;
	
	do{
		do{
			begin:
			system("cls");
			removeBarangHeader();
			
			printf("Input ID yang akan dihapus ['Exit/exit' to return]: ");
			scanf("%[^\n]", id); getchar();
			
			if(strcmp(id, "exit") == 0 || strcmp(id, "Exit") == 0){
				return;
			}
			
			index = hash(id);
			head = buckets[index];
			
			if(!head){
				printf("ID tidak ditemukan atau invalid ID!\n");
				system("pause");
			}else{
				for(int i = 0; i < q.size; i++){
					if(strcmp(head->bg.id, q.bg[i].id) == 0){
						printf("Cannot update! Item is in queue!\n");	//mencegah item agar tidak bisa dihapus ketika sedang diproses (didalam queue)
						system("pause");
						goto begin;
					}
				}
				break;
			}
		}while(1);
	
		//proses pencarian node yang sesuai
		if(strcmp(id, head->bg.id) == 0){
			bg = head->bg;
			root = removeFromAvl(root, bg);		//hapus dari avl tree
			addHistory(bg, "Remove Barang");	//masukkan ke riwayat
			printf("Barang %s dengan ID %s dihapus\n", bg.namaBarang, bg.id);
			buckets[index] = head->next;
			free(head);		//hapus dari hash table
			size--;
		}else{
			while(head->next->next && strcmp(id, head->next->bg.id) != 0){
				head = head->next;
			}
			if(!head->next->next && strcmp(id, head->next->bg.id) != 0){
				printf("ID tidak ditemukan atau invalid ID!\n");
			}else{
				bg = head->next->bg;
				root = removeFromAvl(root, bg);		//hapus avl
				//masukkan ke riwayat
				addHistory(bg, "Remove Barang");
				printf("Barang %s dengan ID %s dihapus\n", bg.namaBarang, bg.id);
				hashNode* del = head->next;
				head->next = head->next->next;
				free(del);		//hapus hash
				size--;
			}
		}	
		system("pause");
		
		//jika setelah deletion gudang menjadi kosong, maka peringatkan user
		if(size == 0){
			system("cls");
			printf("Empty! No data left!\n\n");
			system("pause");
			return;
		}
		
		//tanya user apakah ingin menghapus lagi atau tidak
		do{
			system("cls");
			removeBarangHeader();
			
			printf("Hapus barang lain [y/n | case insensitive]: ");
			scanf("%c", &again); getchar();
			
			again = tolower(again);
			
			if(again != 'y' && again != 'n'){
				printf("WRONG!\n");
				system("pause");
			}
		}while(again != 'y' && again != 'n');
	}while(again == 'y');
}

//cari barang di hashtable
hashNode* searchBarang(char id[]){
	int index = hash(id);
	hashNode* head = buckets[index];
	
	while(head){		//traverse sepanjang list di suatu index
		if(strcmp(id, head->bg.id) == 0){
			return head;	//node ditemukan
		}
		head = head->next;	//lanjut traverse jika tidak ditemukan
	}
	return NULL;	//tidak ditemukan sampai list habis, return NULL
}

//dekorasi
void updateHeader(){
	printf("Update Item\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//fungsi update kategori, meminta inputan kategori baru untuk barang tsb
barang updateKategori(){
	barang update = st.bg[st.top];
	char kategori[10];
	int valid;
	
	valid = 0;
	//menggunakan validasi yang sama dengan fungsi inputInfoBarang
	do{
		system("cls");
		updateHeader();
		
		printf("Input kategori baru [case sensitive | 'Exit/exit' to return]:\n");
		int rows = sizeof(categories) / sizeof(categories[0]);
		for(int i = 0; i < rows; i++){
			printf("- %s\n", categories[i]);
		}
		printf(">> ");
		scanf("%[^\n]", kategori); getchar();
		
		if(strcmp(kategori, "exit") == 0 || strcmp(kategori, "Exit") == 0){
			return update;
		}
		
		valid = isValidCategory(kategori);
	}while(!valid);
	
	strcpy(update.kategori, kategori);
	
	//perubahan disimpan ke stack
	st.top++;
	st.bg[st.top] = update;
	strcpy(st.note[st.top], "Update Kategori");
	
	return update;
}

//idem
barang updateHarga(){
	barang update = st.bg[st.top];
	int harga;
	int valid;
	
	valid = 0;
	//idem
	do{
		system("cls");
		updateHeader();
		
		printf("Input harga barang [1000 <= price <= 100000000 | 0 to return]: ");
		scanf("%d", &harga); getchar();
		
		if(harga == 0){
			return update;
		}
		
		valid = isValidPrice(harga);
	}while(!valid);
	
	update.hargaBarang = harga;
	
	//idem
	st.top++;
	st.bg[st.top] = update;
	strcpy(st.note[st.top], "Update Harga");
	
	return update;
}

//idem
barang updateNama(){
	barang update = st.bg[st.top];
	char nama[100];
	int valid;
	
	valid = 0;
	//idem
	do{
		system("cls");
		updateHeader();
		
		printf("Input nama barang [3-90 huruf inklusif | 'Exit/exit' to return]: ");
		scanf("%[^\n]", nama); getchar();
		
		if(strcmp(nama, "exit") == 0 || strcmp(nama, "Exit") == 0){
			return update;
		}
		
		valid = isValidName(nama);
	}while(!valid);
	
	strcpy(update.namaBarang, nama);
	
	//idem
	st.top++;
	st.bg[st.top] = update;
	strcpy(st.note[st.top], "Update Nama");
	
	return update;
}

//rollback ke record terakhir sebelumnya
barang rollback(){
	st.top--;
	return st.bg[st.top];
}

//rollback ke record original
barang rollbackToOri(){
	st.top = 0;
	return st.bg[st.top];
}

//fungsi utama update barang
void updateBarang(){
	if(size == 0){
		system("cls");
		updateHeader();
		printf("Empty! Cannot update data!\n\n");
		system("pause");
		return;
	}
	
	char id[6];
	hashNode* node;
	avl* avlNode;
	char again;
	
	do{
		do{
			begin:
			system("cls");
			updateHeader();
			
			printf("Input ID yang akan diperbarui ['Exit/exit' untuk return]: ");
			scanf("%[^\n]", id); getchar();
			
			if(strcmp(id, "exit") == 0 || strcmp(id, "Exit") == 0){
				return;
			}
			
			node = searchBarang(id);
			
			if(!node){
				printf("ID tidak ditemukan atau invalid ID!\n");
				system("pause");
			}else{
				for(int i = 0; i < q.size; i++){
					if(strcmp(node->bg.id, q.bg[i].id) == 0){
						printf("Cannot update! Item is in queue!\n");	//barang sedang di queue, tidak bisa di update
						system("pause");
						goto begin;
					}
				}
				break;
			}
		}while(1);
	
		int opt;
		int curr;
		avlNode = searchNode(root, node->bg);
		st.top++;
		st.bg[st.top] = node->bg;
		strcpy(st.note[st.top], "No Update");
		barang updatedBarang = st.bg[st.top];
		
		do{
			start:
			system("cls");
			updateHeader();
			curr = st.top;
			
			//berbagai menu update
			printf("1. Update Nama Barang\n2. Update Harga Barang\n3. Update Kategori\n4. Rollback\n5. Rollback to original\n6. Display item/updated item\n7. Return/Confirm update\n>> ");
			scanf("%d", &opt); getchar();
			
			if((opt == 4 || opt == 5) && st.top == 0){
				printf("Nothing to rollback!\n");
				system("pause");
				goto start;
			}
			if(st.top == MAX){
				printf("Cannot store more updated data! Updated data may not be saved!\n");
				system("pause");
				goto start;
			}
			
			switch(opt){
				case 1:{
					updatedBarang = updateNama();
					if(st.top != curr){	
						printf("Data updated\n");
						system("pause");
					}
					break;
				}
				case 2:{
					updatedBarang = updateHarga();
					if(st.top != curr){	
						printf("Data updated\n");
						system("pause");
					}
					break;
				}
				case 3:{
					updatedBarang = updateKategori();
					if(st.top != curr){	
						printf("Data updated\n");
						system("pause");
					}
					break;
				}
				case 4:{
					updatedBarang = rollback();
					printf("Data rolled back to previous record\n");
					system("pause");
					break;
				}
				case 5:{
					updatedBarang = rollbackToOri();
					printf("Data rolled back to original\n");
					system("pause");
					break;
				}
				case 6:{
					printf("\nID: %s\nNama: %s\nHarga: Rp %d\nKategori: %s\n", st.bg[st.top].id, st.bg[st.top].namaBarang, st.bg[st.top].hargaBarang, st.bg[st.top].kategori);
					system("pause");
					break;
				}
				case 7:{
					break;
				}
				default:{
					printf("WRONG\n");
					system("pause");
					break;
				}
			}
		}while(opt != 7);
		
		//update disimpan ke node avl dan hashtable
		avlNode->bg = updatedBarang;
		node->bg = updatedBarang;
		//masukkan ke riwayat
		addHistory(st.bg[0], "Original (sebelum diupdate)");
		for(int i = 0; i <= st.top; i++){
			if(strcmp(st.note[i], "No Update") == 0){
				continue;
			}
			addHistory(st.bg[i], st.note[i]);
		}
		st.top = -1;
		printf("Update saved!\n");
		system("pause");
		
		//tanya user apakah ingin update barang lagi atau tidak
		do{
			system("cls");
			updateHeader();
			
			printf("Update barang lain [y/n | case insensitive]: ");
			scanf("%c", &again); getchar();
			
			again = tolower(again);
			
			if(again != 'y' && again != 'n'){
				printf("WRONG!\n");
				system("pause");
			}
		}while(again != 'y' && again != 'n');
	}while(again == 'y');
}

//swap kedua barang
void swapBg(barang* a, barang* b){
	barang temp = *a;
	*a = *b;
	*b = temp;
}

//untuk memastikan properti max heap terjaga
void maxHeapify(int i){
	int largest = i;
	int left = 2*i+1;
	int right = 2*i+2;
	
	if(left < q.size && q.bg[left].hargaBarang > q.bg[largest].hargaBarang){
		largest = left;
	}
	if(right < q.size && q.bg[right].hargaBarang > q.bg[largest].hargaBarang){
		largest = right;
	}
	
	if(largest != i){
		swapBg(&q.bg[i], &q.bg[largest]);
		maxHeapify(largest);
	}
}

//build max heap, dimulai dari index non leave node terakhir, yaitu n/2-1
void buildMaxHeap(){
	for(int i = q.size/2-1; i >= 0; i--){
		maxHeapify(i);
	}
}

//dekorasi
void prosesBarangHeader(){
	printf("Proses Barang\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//insert ke priority queue
void pushHeap(){
	system("cls");
	
	//validasi jika GUDANG kosong, maka tidak ada yang bisa ditambahkan ke queue
	if(size == 0){
		prosesBarangHeader();
		printf("Empty! No data to be processed!\n\n");
		system("pause");
		return;
	}
	
	//validasi jika queue penuh, tidak bisa insert lagi
	if(q.size == MAX){
		printf("Cannot insert! FULL\n\n");
		system("pause");
		return;
	}
	
	char id[6];
	char again;
	hashNode* node;
	
	do{
		do{
			system("cls");
			prosesBarangHeader();
			
			printf("Input ID yang akan ditambah ke antrian ['Exit/exit' to return]: ");
			scanf("%[^\n]", id); getchar();
			
			if(strcmp(id, "exit") == 0 || strcmp(id, "Exit") == 0){
				return;
			}
			
			node = searchBarang(id);
			
			if(!node){
				printf("ID tidak ditemukan atau invalid ID!\n");
				system("pause");
			}else{
				break;
			}
		}while(1);
		
		//barang masuk ke queue
		q.bg[q.size] = node->bg;
		//masukkan ke riwayat
		addHistory(q.bg[q.size], "Barang diproses (masuk ke queue)");
		q.size++;
		
		//untuk menjaga properti max heap dari priority queuenya
		int i = q.size-1;
		
		while(i > 0 && q.bg[(i-1)/2].hargaBarang < q.bg[i].hargaBarang){
			swapBg(&q.bg[(i-1)/2], &q.bg[i]);
			i = (i-1)/2;
		}
		printf("Barang dimasukkan ke antrian dan akan diproses\n");	
		
		system("pause");
		
		//tanya user apakah ingin insert lagi atau tidak
		do{
			system("cls");
			prosesBarangHeader();
			
			printf("Proses barang lain [y/n | case insensitive]: ");
			scanf("%c", &again); getchar();
			
			again = tolower(again);
			
			if(again != 'y' && again != 'n'){
				printf("WRONG!\n");
				system("pause");
			}
		}while(again != 'y' && again != 'n');
	}while(again == 'y');
}

//keluarkan dari queue
void popHeap(){
	system("cls");
	
	//validasi jika QUEUE kosong, tidak ada yang bisa di-pop
	if(q.size == 0){
		prosesBarangHeader();
		printf("Cannot process data! EMPTY\n\n");
		system("pause");
		return;
	}

	char again;
	int opt;
	
	do{
		do{
			system("cls");
			prosesBarangHeader();
			
			//berbagai menu pop
			printf("1. Keluarkan dari queue untuk diupdate/dihapus\n2. Keluarkan dari queue untuk dikirim\n3. Return\n>> ");
			scanf("%d", &opt); getchar();
			
			switch(opt){
				//ini hanya mengeluarkan dari queue, tidak akan menghapus dari hash dan avl
				case 1:{
					addHistory(q.bg[0], "Barang dikeluarkan dari queue untuk dihapus/diedit");
					printf("Barang %s telah dikeluarkan dari antrian\n", q.bg[0].namaBarang);
					
					//jaga properti max heap
					q.size--;
					swapBg(&q.bg[0], &q.bg[q.size]);
					buildMaxHeap();
					break;
				}
				//ini akan mengeluarkan dari queue, DAN menghapus dari hash dan avl
				case 2:{
					root = removeFromAvl(root, q.bg[0]); //hapus avl
					
					int index = hash(q.bg[0].id);
					hashNode* head = buckets[index];
					
					//hapus hash
					if(strcmp(q.bg[0].id, head->bg.id) == 0){
						buckets[index] = head->next;
						free(head);
						size--;
					}else{
						while(head->next->next && strcmp(q.bg[0].id, head->next->bg.id) != 0){
							head = head->next;
						}
						hashNode* del = head->next;
						head->next = head->next->next;
						free(del);
						size--;
					}
					
					//masukkan ke riwayat
					addHistory(q.bg[0], "Barang keluar dari queue untuk dikirim");
					printf("Barang %s telah dikirimkan dan dikeluarkan dari antrian\n", q.bg[0].namaBarang);
					
					//jaga properti max heap
					q.size--;
					swapBg(&q.bg[0], &q.bg[q.size]);
					buildMaxHeap();
					break;
				}
				case 3:{
					return;
				}
				default:{
					printf("WRONG!\n");
					break;
				}
			}
			system("pause");
		}while(opt != 1 && opt != 2);
		
		if(q.size == 0){
			system("cls");
			prosesBarangHeader();
			printf("Queue is empty!\n");
			system("pause");
			return;
		}
		
		//tanya user
		do{
			system("cls");
			prosesBarangHeader();
			
			printf("Proses barang lain [y/n | case insensitive]: ");
			scanf("%c", &again); getchar();
			
			again = tolower(again);
			
			if(again != 'y' && again != 'n'){
				printf("WRONG!\n");
				system("pause");
			}
		}while(again != 'y' && again != 'n');
	}while(again == 'y');
}

//proses barang (masukkan dan keluarkan ke/dari queue)
void prosesBarang(){
	int opt;
	
	do{
		system("cls");
		prosesBarangHeader();
		
		//berbagai menu proses barang
		printf("1. Masukkan ke antrian\n2. Keluarkan dari antrian\n3. Return\n>> ");
		scanf("%d", &opt); getchar();
		
		switch(opt){
			case 1:{
				pushHeap();
				break;
			}
			case 2:{
				popHeap();
				break;
			}
			case 3:{
				break;
			}
			default:{
				printf("WRONG!\n");
				system("pause");
				break;
			}
		}
	}while(opt != 3);
}

//dekorasi
void userSearchHeader(){
	printf("Search Item\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//search barang di hash table
void userSearch(){
	//validasi hash table kosong
	if(size == 0){
		system("cls");
		userSearchHeader();
		printf("Empty! No data!\n\n");
		system("pause");
		return;
	}
	
	char id[6];
	char again;
	
	do{
		system("cls");
		userSearchHeader();
		
		printf("ID barang yang akan dicari ['Exit/exit' to return]: ");
		scanf("%[^\n]", id); getchar();
		
		if(strcmp(id, "exit") == 0 || strcmp(id, "Exit") == 0){
			return;
		}
		
		hashNode* searched = searchBarang(id);
		
		if(!searched){	//tidak ditemukan
			printf("Barang tidak ditemukan!\n");
		}else{			//ditemukan, print info barang
			printf("\nInfo barang:\n");
			printf("ID: %s\nNama: %s\nHarga: Rp %d\nKategori: %s\n\n", searched->bg.id, searched->bg.namaBarang, searched->bg.hargaBarang, searched->bg.kategori);
		}
		system("pause");
		
		//tanya user
		do{
			system("cls");
			userSearchHeader();
			
			printf("Search barang lain [y/n | case insensitive]: ");
			scanf("%c", &again); getchar();
			
			again = tolower(again);
			
			if(again != 'y' && again != 'n'){
				printf("WRONG!\n");
				system("pause");
			}
		}while(again != 'y' && again != 'n');
	}while(again == 'y');
}

//dekorasi
void displayHeader(){
	printf("Display Data\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//traverse ke paling kiri avl tree
void displayMinItemByPrice(avl* root){
	system("cls");
	
	if(size == 0){
		displayHeader();
		printf("No data! EMPTY\n\n");
		system("pause");
		return;
	}
	
	printf("Cheapest item:\n");
	avl* temp = root;
	while(temp && temp->left){
		temp = temp->left;
	}
	printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: %d\nKategori\t: %s\n\n", temp->bg.id, temp->bg.namaBarang, temp->bg.hargaBarang, temp->bg.kategori);
	system("pause");
}

//traverse ke paling kanan avl tree
void displayMaxItemByPrice(avl* root){
	system("cls");
	
	if(size == 0){
		displayHeader();
		printf("No data! EMPTY\n\n");
		system("pause");
		return;
	}
	
	printf("Most expensive item:\n");
	avl* temp = root;
	while(temp && temp->right){
		temp = temp->right;
	}
	printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: %d\nKategori\t: %s\n\n", temp->bg.id, temp->bg.namaBarang, temp->bg.hargaBarang, temp->bg.kategori);
	system("pause");
}

//inorder traversal untuk display yang terurut
void inorderTrv(avl* root){
	if(!root){
		return;
	}
	
	inorderTrv(root->left);
	printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: %d\nKategori\t: %s\n\n", root->bg.id, root->bg.namaBarang, root->bg.hargaBarang, root->bg.kategori);
	inorderTrv(root->right);
}

void displaySorted(){
	system("cls");
	
	if(size == 0){
		displayHeader();
		printf("No data! EMPTY\n\n");
		system("pause");
		return;
	}
	
	displayHeader();
	printf("All items in the warehouse (sorted):\n\n");
	inorderTrv(root);
	printf("\n");
	system("pause");
}

//display elemen teratas heap
void displayHeapTop(){
	system("cls");
	
	if(q.size == 0){
		displayHeader();
		printf("No data! EMPTY\n\n");
		system("pause");
		return;
	}
	
	displayHeader();
	printf("Currently processed item:\n");
	printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: Rp %d\nKategori\t: %s\n\n", q.bg[0].id, q.bg[0].namaBarang, q.bg[0].hargaBarang, q.bg[0].kategori);
	system("pause");
	printf("\n");
}

//display semua elemen heap
void displayAllHeap(){
	system("cls");
	
	if(q.size == 0){
		displayHeader();
		printf("No data! EMPTY\n\n");
		system("pause");
		return;
	}
	
	displayHeader();
	printf("All items in queue:\n\n");
	for(int i = 0; i < q.size; i++){
		printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: Rp %d\nKategori\t: %s\n\n", q.bg[i].id, q.bg[i].namaBarang, q.bg[i].hargaBarang, q.bg[i].kategori);
	}
	system("pause");
	printf("\n");
}

//display semua riwayat
void displayHistory(){
	system("cls");
	
	if(!histHead){
		displayHeader();
		printf("No history!\n\n");
		system("pause");
		return;
	}
	
	displayHeader();
	printf("History\n\n");
	historyNode* temp = histHead;
	while(temp){
		printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: Rp %d\nKategori\t: %s\nNote\t\t: %s\n\n", temp->bg.id, temp->bg.namaBarang, temp->bg.hargaBarang, temp->bg.kategori, temp->note);
		temp = temp->next;
	}
	system("pause");
	printf("\n");
}

//display semua data di table
void displayTable(){
	system("cls");
	
	if(!size){
		displayHeader();
		printf("Warehouse's empty!\n\n");
		system("pause");
		return;
	}
	
	displayHeader();
	printf("All items in the warehouse:\n\n");
	hashNode* head;
	for(int i = 0; i < MAX; i++){
		head = buckets[i];
		while(head){
			printf("ID\t\t: %s\nNama\t\t: %s\nHarga\t\t: Rp %d\nKategori\t: %s\n\n", head->bg.id, head->bg.namaBarang, head->bg.hargaBarang, head->bg.kategori);
			head = head->next;
		}
	}
	system("pause");
	printf("\n");
}

//menu display 
void displayMenu(){
	int opt;
	
	do{
		system("cls");
		displayHeader();
		
		printf("1. Display all items\n2. Display all items (sorted)\n3. Display Most Expensive item\n4. Display Cheapest item\n5. Display currently processed item\n6. Display all queued items\n7. Display history\n8. Return\n>> ");
		scanf("%d", &opt); getchar();
		
		switch(opt){
			case 1:{
				displayTable();
				break;
			}
			case 2:{
				displaySorted();
				break;
			}
			case 3:{
				displayMaxItemByPrice(root);
				break;
			}
			case 4:{
				displayMinItemByPrice(root);
				break;
			}
			case 5:{
				displayHeapTop();	
				break;
			}
			case 6:{
				displayAllHeap();
				break;
			}
			case 7:{
				displayHistory();
				break;
			}
			case 8:{
				break;
			}
			default:{
				printf("WRONG!\n");
				system("pause");
				break;
			}
		}
	}while(opt != 8);
}

//dekorasi
void adminHeader(){
	printf("Welcome, Admin\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//menu admin
void adminPage(){
	int opt;
	
	do{
		system("cls");
		adminHeader();
		
		printf("1. Input barang\n2. Hapus barang\n3. Update Barang\n4. Display\n5. Proses barang\n6. Return\n>> ");
		scanf("%d", &opt); getchar();
		
		switch(opt){
			case 1:{
				inputInfoBarang();
				break;
			}
			case 2:{
				removeBarang();
				break;
			}
			case 3:{
				updateBarang();
				break;
			}
			case 4:{
				displayMenu();
				break;
			}
			case 5:{
				prosesBarang();
				break;
			}
			case 6:{
				break;
			}
			default:{
				printf("WRONG!\n");
				system("pause");
				break;
			}
		}
	}while(opt != 6);
}

//dekorasi
void userHeader(){
	printf("Welcome, Guest\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//menu user
void userPage(){
	int opt;
	
	do{
		system("cls");
		userHeader();
		
		printf("1. Display items\n2. Search item\n3. Return\n>> ");
		scanf("%d", &opt); getchar();
		
		switch(opt){
			case 1:{
				displayMenu();
				break;
			}
			case 2:{
				userSearch();
				break;
			}
			case 3:{
				break;
			}
			default:{
				printf("WRONG!\n");
				system("pause");
				break;
			}
		}
	}while(opt != 3);
}

//dekorasi
void loginHeader(){
	printf("Login\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//menu login
void login(){
	char username[10];
	char password[10];
	
	do{
		system("cls");
		loginHeader();
		
		printf("Username [type 'Exit/exit' to return]: ");
		scanf("%[^\n]", username); getchar();
		
		if(strcmp("exit", username) == 0 || strcmp("Exit", username) == 0){
			return;
		}
		
		printf("Password [type 'Exit/exit' to return]: ");
		scanf("%[^\n]", password); getchar();
		
		if(strcmp("exit", password) == 0 || strcmp("Exit", password) == 0){
			return;
		}
		
		//validasi username dan password, case sensitive
		if(strcmp(username, "Admin") == 0 && strcmp(password, "Admin") == 0){
			adminPage();
		}else if(strcmp(username, "Guest") == 0 && strcmp(password, "Guest") == 0){
			userPage();
		}else{
			printf("\nInvalid credentials!\n");
			system("pause");
		}	
	}while(1);
}

//dekorasi splash screen exit, jika tidak ada maka hanya akan print teks
void quitProgram(){
	system("cls");
	
	FILE* file = fopen("logo_angkatan.txt", "r+");
	if(file){
		char ch;
		while(fscanf(file, "%c", &ch) == 1){
			printf("%c", ch);
		}
	}else{
		printf("Thank you for using this program!\n");
	}
	fclose(file);
	printf("\n");
	system("pause");
}

//dekorasi
void mainMenuHeader(){
	printf("Warehouse Explorer\n");
	for(int i = 0; i < 25; i++){
		printf("=");
	}
	printf("\n");
}

//main menu
void mainMenu(){
	int opt;
	
	do{
		system("cls");
		mainMenuHeader();
		
		printf("1. Login\n2. Exit\n>> ");
		scanf("%d", &opt); getchar();
		
		switch(opt){
			case 1:{
				login();
				break;
			}
			case 2:{
				break;
			}
			default:{
				printf("WRONG!\n");
				system("pause");
				break;
			}
		}
	}while(opt != 2);
	quitProgram();
}

int main(){
	initialize();
	mainMenu();
	return 0;
}
