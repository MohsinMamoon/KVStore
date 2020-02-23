#include<bits/stdc++.h>
#include <unistd.h>
using namespace std;

typedef struct Slice{
    uint8_t size;
    char*   data;
}Slice;

class Node{
	char** keys;
	char** values;
	int degree;
	Node **child_arr;
	int *child_num;
	int key_num;
	bool is_leaf;

	public:

		~Node() {
			for(int i=0; i<2*degree-1; i++) {
				delete(keys[i]);
				delete(values[i]);
			}
			delete(keys);
			delete(values);
			delete(child_num);
			delete(child_arr);
		}

		Node(int t, bool leaf) {
			degree = t;
			is_leaf = leaf;
			
			keys = new char* [2*degree -1];
			values = new char* [2*degree -1];
			for(int i=0; i<2*degree -1; i++) {
				keys[i] = new char[64] {0};
				values[i] = new char[255] {0};
			}

			child_arr = new Node* [2*degree] {0};
			child_num = new int[2*degree] {0};
			
			key_num=0;
		}

		char* search(Slice* k){ 

			int i = 0; 
			while (i < key_num && strncmp(k->data, keys[i], 64) > 0) 
				i++; 
		
			if (strncmp(keys[i], k->data, 64) == 0) {
				cout << "Found!: " << keys[i] << ":" << values[i] << endl;
				return values[i]; 
			}
			
			if (is_leaf == true){ 
				cout << "Not present!" << endl;
				return NULL; 
			}
		
			return child_arr[i]->search(k); 
        } 
		
	void getnthkey(int n,int* count,int *found, Slice *key, Slice *value)
		{
			if(is_leaf)
			{

				for(int index = 0; index < key_num; index++)
				{
					if(*count < n)
						(*count)++;
					if(*count == n && *found == 0)
					{
						key->data = keys[index];
						value->data = values[index];
						key->size = 64;
						value->size = 255;
						*found = 1;
						return;
					}
				}
			}

			else
			{
				for( int i = 0; i< key_num+1; i++)
				{
					child_arr[i]->getnthkey(n,count,found, key, value);
					if(*count < n)
					{
						(*count)++;
					}
					if(*count == n && *found == 0)
					{
							key->data = keys[i];
							value->data = values[i];
							key->size = 64;
							value->size = 255;
							*found = 1;
							return;
					}
				}
			}
		}
		
		void traverse() { 
		    int i; 
		    for (i = 0; i < key_num; i++) 
		    { 
		        if (is_leaf == false) 
		            child_arr[i]->traverse(); 
				printf("%s\n", keys[i]);
		    } 
		
		    if (is_leaf == false) 
		        child_arr[i]->traverse(); 
		} 

		int insertNonFull(Slice* k, Slice* v, bool* updated){
			int index = key_num -1;
			if(is_leaf == true)
			{
				
				while(index >= 0 && (strncmp(keys[index],k->data, 64) > 0))
				{
					strncpy(keys[index+1], keys[index], 64);
					strncpy(values[index+1], values[index], 255);
					index--;
				}
				if( index >= 0 && strncmp(keys[index], k->data, 64) == 0){
					strncmp(values[index], v->data, 64);
					*updated = 1;
					index++;
					while(index < key_num) {
						strncpy(keys[index], keys[index+1], 64);
						strncpy(values[index], values[index+1], 255);
						index++;
					}
					return 0;
				}
				else {
					strncpy(keys[index+1], k->data, 64);
					strncpy(values[index+1], v->data, 255);
					*updated = false;
					key_num++;
					return 1; 	
					
				}
			}
			else
			{
				while(index >=0 && (strncmp(keys[index],k->data, 64) > 0))
					index--;

				if(index >= 0 && strncmp(keys[index], k->data, 64) == 0) {
					strncpy(values[index], v->data, 255);
					*updated = true;
					return 0;
				}
				else {
					int increased = 0;
					if(child_arr[index+1]->key_num == 2*degree - 1)
					{
						splitChild(index+1 , child_arr[index+1]);
						increased = 1;
						if(strncmp(keys[index+1],k->data, 64) < 0)
							index++;
					}
					child_num[index+1] += child_arr[index+1]->insertNonFull(k, v, updated);
					
					return increased;
				}

			}
		}

		void splitChild(int i, Node *child) {
			Node* z = new Node(child->degree, child->is_leaf);
			z->key_num = degree - 1;
			int z_child_num = z->key_num;

			for(int j=0; j < degree-1; j++) {
				strncpy(z->keys[j], child->keys[j+degree], 64);
				strncpy(z->values[j], child->values[j+degree], 255);
			}

			if(!child->is_leaf) {
				for(int j=0; j<degree; j++) {
					z->child_arr[j] = child->child_arr[j+degree];
					z->child_num[j] += child->child_num[j+degree];
					z_child_num += z->child_num[j];
					child->child_num[j+degree] = 0;
				}
			}

			child_num[i] -= z_child_num;
			child->key_num = degree - 1;


			for(int j=key_num; j >= i+1; j--) {
				child_arr[j+1] = child_arr[j];
				child_num[j+1] = child_num[j];
			}

			child_arr[i+1] = z;
			child_num[i+1] = z_child_num;

			for(int j=key_num-1; j>=i; j--) {
				strncpy(keys[j+1], keys[j], 64);
				strncpy(values[j+1], values[j], 255);		
			}

			strncpy(keys[i], child->keys[degree-1], 64);
			strncpy(values[i], child->values[degree-1], 255);

			key_num++;
			child_num[i]--;
		}

		int remove(Slice* k, bool* removed) {
			int i=0;
			while(i<key_num && strncmp(keys[i], k->data, 64) < 0) ++i;

			if(i<key_num && strncmp(keys[i], k->data, 64) == 0) {
				if(is_leaf) {
					for(int ii = i+1; ii<key_num; ++ii) {
						strncpy(keys[ii-1], keys[ii], 64);
						strncpy(values[ii-1], values[ii], 255);
					}
					key_num--;
					return -1;
				}
				else {
					Slice r;
					r.size = 64;
					r.data = keys[i];
					if(child_arr[i]->key_num >= degree) {
						Slice pred_key;
						pred_key.size=64;

						Node* cur=child_arr[i]; 
						while (!cur->is_leaf) {
							cur->child_num[cur->key_num]--;
							cur = cur->child_arr[cur->key_num]; 
						}
						strncpy(keys[i], cur->keys[cur->key_num-1], 64);
						strncpy(values[i], cur->values[cur->key_num-1], 255);
						pred_key.data = cur->keys[cur->key_num-1];
						child_num[i] += child_arr[i]->remove(&pred_key, removed);
					}

					else if(child_arr[i+1]->key_num >= degree) {
						Slice succ_key;
						succ_key.size = 64;
						
						Node* cur = child_arr[i+1]; 
						while (!cur->is_leaf) {
							cur->child_num[0]--;
							cur = cur->child_arr[0]; 
						}
						succ_key.data = cur->keys[0];
						strncpy(keys[i], cur->keys[0], 64);
						strncpy(values[i], cur->values[0], 255);
						child_num[i+1] += child_arr[i+1]->remove(&succ_key, removed);
					}

					else {
						merge(i);
						child_num[i] += child_arr[i]->remove(&r, removed);
					}
					return -1;
				}
			}

			else {
				if(is_leaf) {
					*removed = false;
					return 0;
				}

				bool flag = (i == key_num);

				if(child_arr[i]->key_num < degree) fill(i);

				if(flag && i > key_num) {
					child_num[i-1] += child_arr[i-1]->remove(k, removed);
				}
				else {
					child_num[i] += child_arr[i]->remove(k, removed);
				}
				return -1;
			}
		}
  
		void fill(int i) {
			if(i != 0 && child_arr[i-1]->key_num >= degree) borrowFromPrev(i);
			else if(i != key_num && child_arr[i+1]->key_num >= degree) borrowFromNext(i);
			
			else {
				if (i != key_num) merge(i);
				else merge(i-1);
			}
			
			return;
		} 
  
		void borrowFromPrev(int idx){
			Node* child = child_arr[idx];
			Node* sibling = child_arr[idx-1];

			for(int iter = child->key_num-1 ;iter >=0 ;iter--) {
				strncpy(child->keys[iter+1], child->keys[iter], 64);
				strncpy(child->values[iter+1], child->values[iter], 255);
			}
			if(!child->is_leaf){
				for(int iter2 = child->key_num; iter2 >=0 ;iter2--)
				{
					child->child_arr[iter2+1] = child->child_arr[iter2];
					child->child_num[iter2+1] = child->child_num[iter2];
				}
			}
			strncpy(child->keys[0], keys[idx -1], 64);
			strncpy(child->values[0], values[idx -1], 255);
			child_num[idx]++;	

			
			if(!child ->is_leaf){
				child->child_arr[0] = sibling->child_arr[sibling->key_num];
				child->child_num[0] = sibling->child_num[sibling->key_num];
			}
			
			strncpy(keys[idx-1], sibling->keys[sibling->key_num-1], 64);
			strncpy(values[idx-1], sibling->values[sibling->key_num-1], 255);
			child_num[idx-1]--;

			child->key_num += 1;
			sibling->key_num -= 1;

			return;
		} 
  
		void borrowFromNext(int idx){

			Node *child = child_arr[idx]; 
		    Node *sibling = child_arr[idx+1]; 

			strncpy(child->keys[(child->key_num)], keys[idx], 64); 
		    strncpy(child->values[(child->key_num)], values[idx], 255); 
			child_num[idx]++;

		    if (!(child->is_leaf)) {
		        child->child_arr[(child->key_num)+1] = sibling->child_arr[0]; 
				child->child_num[(child->key_num)+1] = sibling->child_num[0];
			}

		    strncpy(keys[idx], sibling->keys[0], 64); 
		    strncpy(values[idx], sibling->values[0], 255);
			child_num[idx+1]--; 

		    for (int i=1; i<sibling->key_num; ++i){ 
		        strncpy(sibling->keys[i-1], sibling->keys[i], 64); 
		        strncpy(sibling->values[i-1], sibling->values[i], 255); 
			}
		    
			if (!sibling->is_leaf) { 
		        for(int i=1; i<=sibling->key_num; ++i){ 
		            sibling->child_arr[i-1] = sibling->child_arr[i];
					sibling->child_num[i-1] = sibling->child_num[i];	
				} 
		    } 

		    child->key_num++; 
		    sibling->key_num--; 

		    return;
		} 
  
    	void merge(int i) {
			Node* child = child_arr[i];
			Node* sibling = child_arr[i+1];
			
			strncpy(child->keys[degree-1], keys[i], 64);
			strncpy(child->values[degree-1], values[i], 255);
			child_num[i]++;
			
			for(int ii=0; ii<sibling->key_num; ++ii) {
				strncpy(child->keys[ii+degree], sibling->keys[ii], 64);
				strncpy(child->values[ii+degree], sibling->values[ii], 255);
				child_num[i]++;
				child_num[i+1]--;
			}
			
			if(!child->is_leaf) {
				for(int ii=0; ii <= sibling->key_num; ++ii) {
					child->child_arr[ii+degree] = sibling->child_arr[ii];
					child->child_num[ii+degree] = sibling->child_num[ii];
				}
			}

			for(int ii=i+1; ii<key_num; ++ii) {
				strncpy(keys[ii-1], keys[ii], 64);
				strncpy(values[ii-1], values[ii], 255);
			}

			for(int ii=i+2; ii<=key_num; ++ii) {
				child_arr[ii-1] = child_arr[ii];
				child_num[ii-1] = child_num[ii];
			}

			child->key_num += sibling->key_num+1;
			key_num--;

			delete(sibling);
			return;
		}
  
    	friend class BTree; 	
};

class BTree { 
	Node *root; 
	int degree; 
	public: 
		
		BTree(int _t) { 
			
			root = NULL;
			degree = _t; 
		} 

		char* search(Slice* k) { 
			return (root == NULL)? NULL : root->search(k); 
		} 

		void traverse() 
    	{  if (root != NULL) root->traverse(); } 
		
		bool getnthkeyvalue(int n, Slice* key, Slice* value)
		{
			if(root!= NULL)
			{
				int found = 0;
				int count = 0;
				root->getnthkey(n,&count,&found, key, value);
				return (found ? true : false);
			}
			return false;
		}
		
		void insert(Slice* k, Slice* v, bool* updated) { 

			if (root == NULL) { 
				root = new Node(degree, true);
				strncpy(root->keys[0], k->data, 64);
				strncpy(root->values[0], v->data, 255);
				root->key_num = 1; 
				*updated = false;
			} 
		
			else { 
				if (root->key_num == 2*degree-1) { 
					for(int i=0; i<root->key_num; i++) {
						if(strncmp(root->keys[i], k->data, 64) > 0) break;
						else if(strncmp(root->keys[i], k->data, 64) == 0) {
							strncpy(root->keys[i], k->data, 64);
							strncpy(root->values[i], v->data, 255);
							*updated = true;
							return;
						}
					}
					Node *s = new Node(degree, false); 

					s->child_arr[0] = root;
					for (int i=0; i<2*degree; i++) {
						s->child_num[0] += root->child_num[i];
					}
					s->child_num[0] += root->key_num;
		
					s->splitChild(0, root); 

					int i = 0;
					if (strncmp(s->keys[0], k->data, 64) <  0)
						i++; 

					s->child_num[i] += s->child_arr[i]->insertNonFull(k, v, updated); 

					root = s; 
				} 
				else
					root->insertNonFull(k, v, updated); 
			} 
		} 

		void remove(Slice* k, bool* removed) {
			if(!root) { 
				*removed = false; 
			}  
			root->remove(k, removed); 
	
			if (root->key_num==0) { 
				Node *tmp = root; 
				if (root->is_leaf) 
					root = NULL; 
				else
					root = root->child_arr[0]; 
				delete tmp; 
			} 
			return; 
		}


		friend class kvStore;
}; 

class kvStore {
    BTree *tr;
	int max_ent;
	public:
	    kvStore(uint64_t max_entries){
			tr = new BTree (2);
			max_ent = max_entries;
		}

	    bool get(Slice &key, Slice &value) {
			char * returned = tr->search(&key);
			if(returned == NULL) return false;
			else {
				value.size = 255;
				value.data = returned;
				return 1;
			}
		} //returns false if key didn’t exist
		bool put(Slice &key, Slice &value){
			bool updated = false;
			tr->insert(&key, &value, &updated);
			return updated;
		} //returns true if value overwritten
		bool del(Slice &key){
			bool removed = true;
			tr->remove(&key, &removed);
			return removed;
		}       
		void traverse() {
			tr->traverse();
		}     
		bool get(int N, Slice &key, Slice &value) {
			return tr->getnthkeyvalue(N, &key, &value);
		} //returns Nth key-value pair

		bool del(int N) {
			Slice key, value;
			if(tr->getnthkeyvalue(N, &key, &value)) {
				return del(key);
			}
			else return false;
		} //delete Nth key-value pair
};

// string random_key(int stringLength){
// 	string key = "";
// 	string letters = "";
// 	for(char i = 'a';i<='z';i++)letters+=i;
// 	for(char i = 'A';i<='Z';i++)letters+=i;
// 	for(int i=0;i<stringLength-1;i++)
// 		key = key + letters[rand()%52];
// 	key = key + '\0';

// 	return key;
// }


// string random_value(int stringLength){
// 	string value = "";
// 	string letters = "";
// 	for(int i = 0;i<=255;i++)letters+=char(i);

// 	for(int i=0;i<stringLength-1;i++)
// 		value = value + letters[rand()%256];
// 	value = value + '\0';
// 	return value;
// }

// struct timespec initial, finall;
// int main() {
//     kvStore t(1000); // A B-Tree with minium degree 3 
// 	Slice key, data;
// 	key.size = 3;
// 	data.size = 3;
// 	bool updated = true;
	
// 	// double tdiff = 0;
// 	// for(int i=0;i<10000000;i++)
// 	// {
// 	// 	key.size = 64;
// 	// 	data.size = 255;
// 	// 	string k = random_key(key.size);
// 	// 	string v = random_value(data.size);
// 	// 	char k1[64], v1[256];
// 	// 	strcpy(k1, k.c_str());
// 	// 	strcpy(v1, v.c_str());
// 	// 	// db.insert(pair<char[64],char[256]>(k1,v1));
// 	// 	key.data = k1;
// 	// 	data.data = v1;
// 	// 	// cout << k1 << endl;
// 	// 	clock_gettime(CLOCK_MONOTONIC, &initial);
// 	// 	updated = t.put(key, data);
// 	// 	clock_gettime(CLOCK_MONOTONIC, &finall);
// 	// 	tdiff += (finall.tv_sec -initial.tv_sec) + 1e-9*(finall.tv_nsec -initial.tv_nsec);
// 	// }
// 	// cout << "Time: " << tdiff << endl;

// 	// t.traverse();

// 	char k[] = {'h', 'r', 'x','\0'};
// 	char d[] = {'H', 'R', 'X','\0'};

// 	key.data = k;
// 	data.data = d;
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
// 	char k1[] = {'b', 'v', 'l','\0'};
// 	char d1[] = {'B', 'V', 'L', '\0'};

// 	key.data = k1;
// 	data.data = d1;
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
// 	char k2[] = {'r', 's', 't', '\0'};
// 	char d2[] = {'R', 'S', 'T', '\0'};

// 	key.data = k2;
// 	data.data = d2;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");	
	
// 	char k3[] = {'p', 'q', 'r', '\0'};
// 	char d3[] = {'P', 'Q', 'R', '\0'};

// 	key.data = k3;
// 	data.data = d3;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
	
// 	char k7[] = {'p', 'q', 'r', '\0'};
// 	char d7[] = {'Q', 'R', 'S', '\0'};

// 	key.data = k7;
// 	data.data = d7;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
	
		
// 	char k4[] = {'w', 'n', 'g', '\0'};
// 	char d4[] = {'W', 'N', 'G', '\0'};

// 	key.data = k4;
// 	data.data = d4;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
// 	char k5[] = {'n', 'a', 't', '\0'};
// 	char d5[] = {'N', 'A', 'T', '\0'};

// 	key.data = k5;
// 	data.data = d5;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
	
// 	char k6[] = {'h', 'e', 'l', '\0'};
// 	char d6[] = {'H', 'E', 'L', '\0'};

// 	key.data = k6;
// 	data.data = d6;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
	
// 	char k10[] = {'p', 'q', 'r', '\0'};
// 	char d10[] = {'H', 'E', 'S', '\0'};

// 	key.data = k10;
// 	data.data = d10;  
//     updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// printf("Traversal of tree constructed is\n");
//     // t.traverse(); 
// 	// printf("\n");
// 	char k9[]= {'a', 'b', 'c', '\0'};
// 	key.data = k9;
// 	updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 

// 	char k8[] = {'l', 'm', 't', '\0'};
// 	key.data = k8;
// 	updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
	
// 	char k11[] = {'c', 'r', 'j', '\0'};
// 	key.data = k11;
// 	updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
	
// 	// char k12[] = {'m', 'o', 'q', '\0'};
// 	// key.data = k12;
// 	// updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 

// 	// key.data = k;
// 	// t.remove(&key);
// 	t.traverse();
// 	cout << endl << endl;

// 	// while(1) {
// 	// 	int i;
// 	// 	scanf("%d", &i);

// 	// 	cout << (t.del(i) ? "Deleted!" : "Not Deleted!" ) << endl;
// 	// 	t.traverse();
// 	// }
// 	for (int i=1; i<=8; i++) {
// 		Slice get_key, get_value;
// 		if(t.get(i, get_key, get_value)) {
// 			cout << get_key.data << " : " << get_value.data << endl;
// 		}
// 	}
// 	// while(1) {
// 	// 	int i;
// 	// 	scanf("%d", &i);

// 	// 	switch(i) {
// 	// 		case 0:  key.data = k;
// 	// 		break;
// 	// 		case 1: key.data = k1;
// 	// 		break;
// 	// 		case 2: key.data = k2;
// 	// 		break;
// 	// 		case 3: key.data = k3;
// 	// 		break;
// 	// 		case 4: key.data = k4;
// 	// 		break;
// 	// 		case 5: key.data = k5;
// 	// 		break;
// 	// 		case 6: key.data = k6;
// 	// 		break;
// 	// 		case 7: key.data = k7;
// 	// 		break;
// 	// 		case 8: key.data = k8;
// 	// 		break;
// 	// 		case 9:	key.data = k9;
// 	// 		break;
// 	// 		case 10: key.data = k10;
// 	// 		break;
// 	// 		case 11: key.data = k11;
// 	// 		break;
// 	// 		case 12: key.data = k12;
// 	// 		break;
// 	// 	}
// 	// 	cout << (t.del(key) ? "Deleted: " : "Not deleted: ") << key.data << endl;
// 	// 	t.traverse();
// 	// 	printf("\n");
// 	// }
//     return 0; 
// } 



