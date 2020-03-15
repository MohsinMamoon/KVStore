#include<bits/stdc++.h>
#include <unistd.h>
#include <pthread.h> 

using namespace std;

typedef struct Slice{
    uint8_t size;
    char*   data;
}Slice;

class Node{
	char** keys;
	char** values;
	uint8_t *keysizes;
    uint8_t *valuesizes;
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
			keysizes = new uint8_t [2*degree-1];
			valuesizes = new uint8_t [2*degree-1];
			keys = new char* [2*degree -1];
			values = new char* [2*degree -1];
			for(int i=0; i<2*degree -1; i++) {
				keys[i] = new char[64] {0};
				values[i] = new char[255] {0};
				keysizes[i] = 0;
                valuesizes[i] = 0;
			}

			child_arr = new Node* [2*degree] {0};
			child_num = new int[2*degree] {0};
			
			key_num=0;
		}

		char* search(Slice* k, Slice* v){ 

			int i = 0; 
			while (i < key_num && strncmp(k->data, keys[i], 64) > 0) 
				i++; 
		
			if (i < key_num && strncmp(keys[i], k->data, 64) == 0) {
				// cout << "Found!: " << keys[i] << ":" << values[i] << endl;
				v->data = values[i];
				v->size = valuesizes[i];
				return values[i]; 
			}
			
			if (is_leaf == true){ 
				// cout << "Not present!" << endl;
				return NULL; 
			}
		
			return child_arr[i]->search(k, v); 
        } 
		
		void lognnth(int n, int *found, Slice *key, Slice *value) {
			int num = 0;
			for(int i = 0; i<key_num; i++) {
				if(!is_leaf) num = child_num[i]; 
				if(!is_leaf && num >= n) {
					child_arr[i]->lognnth(n, found, key, value);
					return;
				}
				else if(num + 1 == n) {
					key->data = keys[i];
					value->data = values[i];
					key->size = keysizes[i];
					value->size = valuesizes[i];
					*found = 1;
					return;
				}
				else {
					n = n - num - 1;
				}
			}
			if(!is_leaf && child_num[key_num] >= n) child_arr[key_num]->lognnth(n, found, key, value);
			else *found = 0;
			return;
		}

		void getnthkey(int n,int* count,int *found, Slice *key, Slice *value){
			if(is_leaf){

				for(int index = 0; index < key_num; index++){
					if(*count < n)
						(*count)++;
					if(*count == n && *found == 0){
						key->data = keys[index];
						value->data = values[index];
						key->size = 64;
						value->size = 255;
						*found = 1;
						return;
					}
				}
			}

			else{
				for( int i = 0; i< key_num+1; i++){
					child_arr[i]->getnthkey(n,count,found, key, value);
					if(*count < n){
						(*count)++;
					}
					if(*count == n && *found == 0){
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
		
		void traverse(int length) { 
		    int i; 
		    for (i = 0; i < key_num; i++) 
		    { 
		        if (is_leaf == false) 
		            child_arr[i]->traverse(length+1); 
				// for(int ll=0; ll<length; ll++) printf("\t\t\t");
				// printf("%d->%s\n",length, keys[i]);
				printf("%s\n",keys[i]);
		    } 
		
		    if (is_leaf == false) 
		        child_arr[i]->traverse(length+1); 
		} 
		
		void check(int parent_elem) { 
		    int i, elems = key_num;
			for (int j=0; j<=key_num; j++) {
				elems += child_num[j];
			}
		    for (i = 0; i < key_num; i++) 
		    { 
		        if (is_leaf == false) 
		            child_arr[i]->check(child_num[i]);
		    } 
		
		    if (is_leaf == false) 
		        child_arr[i]->check(child_num[i]); 
			printf("%s -> %d : %d\n", keys[0], elems, parent_elem);
		} 

		int insertNonFull(Slice* k, Slice* v, bool* updated){
			int index = key_num -1;
			if(is_leaf == true)
			{
				
				while(index >= 0 && (strncmp(keys[index],k->data, 64) > 0))
				{
					strncpy(keys[index+1], keys[index], keysizes[index]);
                    keysizes[index+1] = keysizes[index];	
					strncpy(values[index+1], values[index], valuesizes[index]);
                    valuesizes[index+1] = valuesizes[index]; 
					index--;
				}
				if( index >= 0 && strncmp(keys[index], k->data, 64) == 0){
					strncpy(values[index], v->data, v->size);
                    valuesizes[index] = v->size; 
					*updated = 1;
					index++;
					while(index < key_num) {
						strncpy(keys[index], keys[index+1], keysizes[index+1]);
                        keysizes[index] = keysizes[index+1]; 
						strncpy(values[index], values[index+1], valuesizes[index+1]);
                        valuesizes[index] = valuesizes[index+1];	
						index++;
					}
					return 0;
				}
				else {
					strncpy(keys[index+1], k->data, k->size);
                    keysizes[index+1] = k->size; 
					strncpy(values[index+1], v->data, v->size);
                    valuesizes[index+1] = v->size; 
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
					strncpy(values[index], v->data, v->size);
                    valuesizes[index] = v->size; 
					*updated = true;
					return 0;
				}
				else {
					if(child_arr[index+1]->key_num == 2*degree - 1)
					{
						if(strncmp(child_arr[index+1]->keys[degree-1], k->data, 64) == 0) {
							strncpy(child_arr[index+1]->values[degree-1], v->data, v->size);
							child_arr[index+1]->valuesizes[degree-1] = v->size; 
							*updated = true;
							return 0;
						}
						splitChild(index+1 , child_arr[index+1]);
						if(strncmp(keys[index+1],k->data, 64) < 0)
							index++;
					}
					child_num[index+1] += child_arr[index+1]->insertNonFull(k, v, updated);
					
					if(*updated)return 0;
					else return 1;
				}

			}
		}

		void splitChild(int i, Node *child) {
			Node* z = new Node(child->degree, child->is_leaf);
			z->key_num = degree - 1;
			int z_child_num = z->key_num;

			for(int j=0; j < degree-1; j++) {
				strncpy(z->keys[j], child->keys[j+degree], child->keysizes[j+degree]);
				z->keysizes[j] = child->keysizes[j+degree]; 
				strncpy(z->values[j], child->values[j+degree], child->valuesizes[j+degree]);
				z->valuesizes[j] = child->valuesizes[j+degree]; 

			}

			if(!child->is_leaf) {
				for(int j=0; j<degree; j++) {
					z->child_arr[j] = child->child_arr[j+degree];
					z->child_num[j] = child->child_num[j+degree];
					z_child_num += z->child_num[j];
					child->child_num[j+degree] = 0;
				}
			}

			child_num[i] = child_num[i] - z_child_num;
			child->key_num = degree - 1;


			for(int j=key_num; j >= i+1; j--) {
				child_arr[j+1] = child_arr[j];
				child_num[j+1] = child_num[j];
			}

			child_arr[i+1] = z;
			child_num[i+1] = z_child_num;

			for(int j=key_num-1; j>=i; j--) {
				strncpy(keys[j+1], keys[j], keysizes[j]);
				keysizes[j+1]=keysizes[j]; 
				strncpy(values[j+1], values[j], valuesizes[j]);		
				valuesizes[j+1]=valuesizes[j]; 
			}

			strncpy(keys[i], child->keys[degree-1], child->keysizes[degree-1]);
			keysizes[i] = child->keysizes[degree-1]; 
			strncpy(values[i], child->values[degree-1], child->valuesizes[degree-1]);
			valuesizes[i] = child->valuesizes[degree-1]; 

			key_num++;
			child_num[i]--;
		}

		int remove(Slice* k, bool* removed) {
			int i=0;
			while(i<key_num && strncmp(keys[i], k->data, 64) < 0) ++i;

			if(i<key_num && strncmp(keys[i], k->data, 64) == 0) {
				if(is_leaf) {
					for(int ii = i+1; ii<key_num; ++ii) {
						strncpy(keys[ii-1], keys[ii], keysizes[ii]);
						keysizes[ii-1]=keysizes[ii]; 
						strncpy(values[ii-1], values[ii], valuesizes[ii]);
						valuesizes[ii-1]=valuesizes[ii];
					}
					key_num--;
					return -1;
				}
				else {
					int deleted = 0;
					Slice r;
					r.size = 64;
					char pres[64];
					strncpy(pres, keys[i], keysizes[i]);
					r.data = pres;
					r.size = keysizes[i]; 
					if(child_arr[i]->key_num >= degree) {
						Slice pred_key;
						pred_key.size=64;

						Node* cur=child_arr[i]; 
						while (!cur->is_leaf) {
							// cur->child_num[cur->key_num]--;
							cur = cur->child_arr[cur->key_num]; 
						}
						strncpy(keys[i], cur->keys[cur->key_num-1], cur->keysizes[cur->key_num-1]);
						keysizes[i] = cur->keysizes[cur->key_num-1]; 
						strncpy(values[i], cur->values[cur->key_num-1], cur->valuesizes[cur->key_num-1]);
						valuesizes[i] = cur->valuesizes[cur->key_num-1]; 
						pred_key.data = keys[i];
						deleted = child_arr[i]->remove(&pred_key, removed);
						child_num[i] += deleted;
					}

					else if(child_arr[i+1]->key_num >= degree) {
						Slice succ_key;
						succ_key.size = 64;
						
						Node* cur = child_arr[i+1]; 
						while (!cur->is_leaf) {
							// cur->child_num[0]--;
							cur = cur->child_arr[0]; 
						}
						strncpy(keys[i], cur->keys[0], cur->keysizes[0]);
						keysizes[i] = cur->keysizes[0]; 
						strncpy(values[i], cur->values[0], cur->valuesizes[0]);
						valuesizes[i] = cur->valuesizes[0]; 
						succ_key.data = keys[i];
						deleted = child_arr[i+1]->remove(&succ_key, removed);
						child_num[i+1] += deleted;
					}

					else {
						merge(i);
						deleted = child_arr[i]->remove(&r, removed);
						child_num[i] += deleted;
					}
					return deleted;
				}
			}

			else {
				int deleted = 0;
				if(is_leaf) {
					*removed = false;
					return 0;
				}

				bool flag = (i == key_num);

				if(child_arr[i]->key_num < degree) fill(i);

				if(flag && i > key_num) {
					deleted = child_arr[i-1]->remove(k, removed);
					child_num[i-1] += deleted;
				}
				else {
					deleted = child_arr[i]->remove(k, removed);
					child_num[i] += deleted;
				}
				return deleted;
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
				strncpy(child->keys[iter+1], child->keys[iter], child->keysizes[iter]);
				child->keysizes[iter+1] = child->keysizes[iter]; 
				strncpy(child->values[iter+1], child->values[iter], child->valuesizes[iter]);
				child->valuesizes[iter+1] = child->valuesizes[iter]; 
			}
			if(!child->is_leaf){
				for(int iter2 = child->key_num; iter2 >=0 ;iter2--)
				{
					child->child_arr[iter2+1] = child->child_arr[iter2];
					child->child_num[iter2+1] = child->child_num[iter2];
				}
			}
			strncpy(child->keys[0], keys[idx -1], keysizes[idx-1]);
			child->keysizes[0] = keysizes[idx-1]; 
			strncpy(child->values[0], values[idx -1], valuesizes[idx-1]);
			child->valuesizes[0] = valuesizes[idx-1]; 

			child_num[idx]++;	

			
			if(!child ->is_leaf){
				child->child_arr[0] = sibling->child_arr[sibling->key_num];
				child->child_num[0] = sibling->child_num[sibling->key_num];
				child_num[idx] += sibling->child_num[sibling->key_num];
				child_num[idx-1] -= sibling->child_num[sibling->key_num];
			}
			
			strncpy(keys[idx-1], sibling->keys[sibling->key_num-1], sibling->keysizes[sibling->key_num-1]);
			keysizes[idx-1] = sibling->keysizes[sibling->key_num-1]; 
			strncpy(values[idx-1], sibling->values[sibling->key_num-1], sibling->valuesizes[sibling->key_num-1]);
			valuesizes[idx-1] = sibling->valuesizes[sibling->key_num-1]; 

			child_num[idx-1]--;

			child->key_num += 1;
			sibling->key_num -= 1;

			return;
		} 
  
		void borrowFromNext(int idx){

			Node *child = child_arr[idx]; 
		    Node *sibling = child_arr[idx+1]; 

			strncpy(child->keys[(child->key_num)], keys[idx], keysizes[idx]); 
			child->keysizes[child->key_num] = keysizes[idx]; 
		    strncpy(child->values[(child->key_num)], values[idx], valuesizes[idx]); 
			child->valuesizes[child->key_num] = valuesizes[idx]; 

			child_num[idx]++;

		    if (!(child->is_leaf)) {
		        child->child_arr[(child->key_num)+1] = sibling->child_arr[0]; 
				child->child_num[(child->key_num)+1] = sibling->child_num[0];
				child_num[idx] += sibling->child_num[0];
				child_num[idx+1] -= sibling->child_num[0];
			}

		    strncpy(keys[idx], sibling->keys[0], sibling->keysizes[0]); 
			keysizes[idx] = sibling->keysizes[0]; 
		    strncpy(values[idx], sibling->values[0], sibling->valuesizes[0]);
			valuesizes[idx] = sibling->valuesizes[0]; 
			child_num[idx+1]--; 

		    for (int i=1; i<sibling->key_num; ++i){ 
		        strncpy(sibling->keys[i-1], sibling->keys[i], sibling->keysizes[i]); 
				sibling->keysizes[i-1] = sibling->keysizes[i]; 
		        strncpy(sibling->values[i-1], sibling->values[i], sibling->valuesizes[i]); 
				sibling->valuesizes[i-1] = sibling->valuesizes[i]; 
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
			
			strncpy(child->keys[degree-1], keys[i], keysizes[i]);
			child->keysizes[degree-1] = keysizes[i];	
			strncpy(child->values[degree-1], values[i], valuesizes[i]);
			child->valuesizes[degree-1] = valuesizes[i];	

			child_num[i]++;
			
			for(int ii=0; ii<sibling->key_num; ++ii) {
				strncpy(child->keys[ii+degree], sibling->keys[ii], sibling->keysizes[ii]);
				child->keysizes[ii+degree] = sibling->keysizes[ii]; 
				strncpy(child->values[ii+degree], sibling->values[ii], sibling->valuesizes[ii]);
				child->valuesizes[ii+degree] = sibling->valuesizes[ii]; 
				child_num[i]++;
				child_num[i+1]--;
			}
			
			if(!child->is_leaf) {
				for(int ii=0; ii <= sibling->key_num; ++ii) {
					child->child_arr[ii+degree] = sibling->child_arr[ii];
					child->child_num[ii+degree] = sibling->child_num[ii];
					child_num[i] += sibling->child_num[ii];
					child_num[i+1] -= sibling->child_num[ii];
				}
			}

			for(int ii=i+1; ii<key_num; ++ii) {
				strncpy(keys[ii-1], keys[ii], keysizes[ii]);
				keysizes[ii-1] = keysizes[ii]; 
				strncpy(values[ii-1], values[ii], valuesizes[ii]);
				valuesizes[ii-1] = valuesizes[ii]; 
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
	int elems;
	pthread_mutex_t put_del_lock;

	public:
		
		BTree(int _t) { 
			
			root = NULL;
			degree = _t;
			elems = 0;
			pthread_mutex_init(&put_del_lock, NULL);
		} 

		char* search(Slice* k, Slice* v) {
			if(root == NULL) {
				return NULL;
			}
			else {
				pthread_mutex_lock(&put_del_lock); //bhavesh
				char* returned = root->search(k,v);
				pthread_mutex_unlock(&put_del_lock); //bhavesh
				return returned;
			} 
			// return (root == NULL)? NULL : root->search(k,v); 
		} 

		void traverse() 
    	{  if (root != NULL) root->traverse(0); } 

		void check(int parent_elem){ 
			if (root != NULL) {
				// printf("Node -> Total : Child_num\n"); 
				root->check(parent_elem); 
			}
		}
		
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
		bool lognnth(int n, Slice* key, Slice* value) {
			
			if(root != NULL) {
				pthread_mutex_lock(&put_del_lock); //bhavesh
				int found = 0;
				root->lognnth(n, &found, key, value);
				pthread_mutex_unlock(&put_del_lock);   //bhavesh
				return (found ? true : false);
			}

			return false;
		}
		
		void insert(Slice* k, Slice* v, bool* updated) { 
		
			pthread_mutex_lock(&put_del_lock); //bhavesh
			if (root == NULL) { 
				root = new Node(degree, true);
				strncpy(root->keys[0], k->data, k->size);
				root->keysizes[0] = k->size; 
				strncpy(root->values[0], v->data, v->size);
				root->valuesizes[0] = v->size; 
				root->key_num = 1; 
				*updated = false;
			} 
		
			else { 
				if (root->key_num == 2*degree-1) { 
					for(int i=0; i<root->key_num; i++) {
						if(strncmp(root->keys[i], k->data, 64) > 0) break;
						else if(strncmp(root->keys[i], k->data, 64) == 0) {
							strncpy(root->keys[i], k->data, k->size);
							root->keysizes[i] = k->size; 
							strncpy(root->values[i], v->data, v->size);
							root->valuesizes[i] = v->size; 
							*updated = true;
							pthread_mutex_unlock(&put_del_lock); //bhavesh
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
			pthread_mutex_unlock(&put_del_lock); //bhavesh
 
		} 

		void remove(Slice* k, bool* removed) {
			
			if(!root) { 
				*removed = false; 
				return;
			}  
			pthread_mutex_lock(&put_del_lock);  //bhavesh 
			root->remove(k, removed); 
	
			if (root->key_num==0) { 
				Node *tmp = root; 
				if (root->is_leaf) 
					root = NULL; 
				else
					root = root->child_arr[0]; 
				delete tmp; 
			} 
			pthread_mutex_unlock(&put_del_lock); //bhavesh

			return; 
		}

		int tot_elem() {
			if(!root) return 0;
			int elems = 0;
			elems += root->key_num;
			for(int i=0; i<=root->key_num; i++) {
				elems += root->child_num[i];
			}
			return elems;
		}

		friend class kvStore;
}; 

class kvStore {
    BTree **tr;
	int max_ent;
	public:
		int pres_ent;

	    kvStore(uint64_t max_entries){
			tr = new BTree* [345437] {0};
			max_ent = max_entries;
			pres_ent=0;
		}

		int hash (char * key, int len) {
			int x, hash=0;
			switch (len){
			default:
			case 3:
				x = key[2];
				if(x < 'A' || (x > 'Z' && x < 'a')) x = 0;
				else {
					if(x >= 'a') x -= 6;
					x -= 64;
				}
				hash += x;
			case 2:
				x = key[1];
				if(x < 'A' || (x > 'Z' && x < 'a')) x = 0;
				else {
					if(x >= 'a') x -= 6;
					x -= 64;
				}
				hash += x * 81;
			case 1:
				x = key[0];
				if(x < 'A' || (x > 'Z' && x < 'a')) x = 0;
				else {
					if(x >= 'a') x -= 6;
					x -= 64;
				}
				hash += x * 6561;
				break;
			case 0:
				hash = 0;
				break;
			}
			return hash%345437;
		}
		
	    bool get(Slice &key, Slice &value) {
			int hh = hash(key.data, key.size);
			if(tr[hh] != NULL){
				char * returned = tr[hh]->search(&key,&value);
				if(returned == NULL) return false;
				else {
					return true;
				}
			}
			return false;
			
		bool put(Slice &key, Slice &value){
			int hh = hash(key.data, key.size);
			if(tr[hh] == NULL) tr[hh] = new BTree(3);
			bool updated = false;
			tr[hh]->insert(&key, &value, &updated);
			if(!updated){
				pres_ent++;
				tr[hh]->elems++;
			}
			return updated;
		}

		bool del(Slice &key){
			int hh = hash(key.data, key.size);
			if(tr[hh] != NULL){
				bool removed = true;
				tr[hh]->remove(&key, &removed);
				if(removed){
					pres_ent--;
					tr[hh]->elems--;
				}
				return removed;
			}
			return false;
		}       
		 
		 void traverse() {
			 for(int i=0; i<345437; i++) {
				 if(tr[i] != NULL) {
					 tr[i]->traverse();
				 }
			 }
		 }
		 
		 bool get(int N, Slice &key, Slice &value) {
			 N++;
			 for(int i=0; i<345437; i++) {
				 if(tr[i] != NULL) {
					 int elems = tr[i]->elems;
					 if(elems >= N) {
						 return tr[i]->lognnth(N, &key, &value);
					 }
					 else N -= elems; 
				 }
			 }
			 return false;
		 }
		 
		 bool del(int N) {
			 Slice key, value;
			 if(get(N, key, value)) {
				 char* todel = new char[key.size];
				 strncpy(todel, key.data, key.size);
				 key.data = todel;
				 return del(key);
			 }
			 else return false;
		 }

		 int tot_elem() {
			 int t=0;
			 for (int i=0; i<345437; i++) {
				 if(tr[i] != NULL) {
					t += tr[i]->tot_elem();
				 }
			 }
			 return t;
		 }
};

// string random_key(int stringLength){
// 	string key = "";
// 	string letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
// 	//for(char i = 'a';i<='z';i++)letters+=i;
// 	//for(char i = 'A';i<='Z';i++)letters+=i;
// 	for(int i=0;i<stringLength-1;i++)
// 		key = key + letters[rand()%52];
// 	key = key + '\0';

// 	return key;
// }


// string random_value(int stringLength){
// 	string value = "";
// 	string letters = "";
// 	for(int i = 32;i<=127;i++)letters+=char(i);

// 	for(int i=0;i<stringLength-1;i++)
// 		value = value + letters[rand()%96];
// 	value = value + '\0';
// 	return value;
// } struct timespec initial, finall;
// int main() {
//      kvStore t(10000000); // A B-Tree with minium degree 3 
// 	Slice key, data;
// 	// key.size = 3;
// 	// data.size = 3;
// 	bool updated = true;
	
// 	double tdiff = 0;
// 	for(int i=0;i<20;i++){
// 		//key.size = 64;
// 		//key.size = rand()%64 + 1;
// 		//data.size = 255;
// 		data.size = rand()%25 + 1;
// 		//if(key.size == 1) key.size++;
// 		if(data.size == 1) data.size++;
// 		//string k = random_key(key.size);
// 		string k; 
// 		cin >> k;
// 		key.size = k.length();
// 		string v = random_value(data.size);
// 		char k1[64] ={0}, v1[256] = {0};
// 		strcpy(k1, k.c_str());
// 		strcpy(v1, v.c_str());
// 		// db.insert(pair<char[64],char[256]>(k1,v1));
// 		key.data = k1;
// 		data.data = v1;
// 		// cout << k1 << endl;
// 		//`		fprintf(stderr, "%s\n", k1);
// 		clock_gettime(CLOCK_MONOTONIC, &initial);
// 		updated = t.put(key, data);
// 		clock_gettime(CLOCK_MONOTONIC, &finall);
// 		// fprintf(stderr, "%d\n", t.pres_ent);//, t.tot_elem());
// 		// fprintf(stderr, "%d : %d\n", t.pres_ent, t.tot_elem());
// 		tdiff += (finall.tv_sec -initial.tv_sec) + 1e-9*(finall.tv_nsec -initial.tv_nsec);
// 	}
// 	 //cout << "Time: " << tdiff << endl;
// 	 fprintf(stdout, "Time: %lf\n", tdiff);
// 	 fprintf(stdout, "%d:%d\n", t.pres_ent, t.tot_elem());
// 	 t.traverse();
// 	// printf("\n\n\n\n\n");
// 	 while(1) {
// 		 int opt;
// 		 string key;
// 		 char *k1 = new char[65], *v1 = new char[256];
// 		 Slice k, v;
// 		 cin >> opt;
// 		 switch (opt) {
// 			 case 0:
// 				 cin >> key;
// 				 k.size = key.length() + 1;
// 				 strncpy(k1, key.c_str(), k.size);
// 				 k.data = k1;
// 				 if(t.del(k)) {
// 					 printf("Deleted %s\n", k.data);
// 					 t.traverse();
// 				 }
// 				 else {
// 					 printf("Not deleted %s\n", k.data);
// 				 }
// 				 break;
// 			case 1:
// 				 cin >> key;
// 				 k.size = key.length() + 1;
// 				 strncpy(k1, key.c_str(), k.size);
// 				 k.data = k1;
// 				 if(t.get(k, v)) {
// 					 printf("Found %s\n", k.data);
// 				 }
// 				 else {
// 					 printf("Not Found %s\n", k.data);
// 				 }
// 				 break;
// 			case 2:
// 				 cin >> opt;
// 				 if(t.getlogn(opt, k, v)) {
// 					 printf("Found %s\n", k.data);
// 				 }
// 				 else {
// 					 printf("Not found %s\n", k.data);
// 				 }
// 				 break;
// 			case 3:
// 				 cin >> opt;
// 				 if(t.del(opt)) {
// 					 printf("Deleted %dth\n", opt);
// 				 }
// 				 else {
// 					 printf("Not Deleted %dth\n", opt);
// 				 }
// 				 break;
// 			case 4:
// 				 cin >> key;
// 				 k.size = key.length() + 1;
// 				 strncpy(k1, key.c_str(), k.size);
// 				 k.data = k1;
// 				 cin >> key;
// 				 v.size = key.length() + 1;
// 				 strncpy(v1, key.c_str(), v.size);
// 				 v.data = v1;
// 				 if(t.put(k, v)) {
// 					 printf("Updated %s\n", k.data);
// 					 t.traverse();
// 				 }
// 				 else {
// 					 printf("Inserted %s\n", k.data);
// 					 t.traverse();
// 				 }
// 				 break;
// 				 break;
// 			case 5:
// 				 return 0;
			

// 	 }
// 	 }
// 	//for (int i=1; i<=1000; i++) {
// 		//Slice get_key, get_value;
// 		//// if(t.get(i, get_key, get_value)) {
// 		//// 	cout << get_key.data << " : " << get_value.data << "\t";
// 		//// }
// 		//// else {
// 		//// 	cout << "Not found" << "\t";
// 		//// }
// 		//if(t.getlogn(i, get_key, get_value)) {
// 			//fprintf(stderr, "%s : %s\n", get_key.data, get_value.data);
// 			//// cout << get_key.data << " : " << get_value.data << endl;
// 		//}
// 		//else {
// 			//// cout << "Not found" << endl;
// 			//fprintf(stderr, "Not Found!\n");
// 		//}
// 	//}
// 	// for(int i=0; i<1000; i++) {
// 	// 	string k;
// 	// 	cin >> k;
// 	// 	if(k == "quit" || k == "") break;
// 	// 	char k1[64] = {0};
// 	// 	strcpy(k1, k.c_str());
// 	// 	key.data = k1;
// 	// 	fprintf(stderr, "%s%s\n", t.del(key) ? "Deleted : " : "Not Deleted: ", key.data);
// 	// 	fprintf(stderr, "%d : %d\n", t.pres_ent, t.tot_elem());
// 	// 	t.check();
// 	// 	printf("\n\n\n");
// 	// }

// 	// t.traverse();

// 	// char k[] = {'h', 'r', 'x','\0'};
// 	// char d[] = {'H', 'R', 'X','\0'};

// 	// key.data = k;
// 	// data.data = d;
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
// 	// char k1[] = {'b', 'v', 'l','\0'};
// 	// char d1[] = {'B', 'V', 'L', '\0'};

// 	// key.data = k1;
// 	// data.data = d1;
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
// 	// char k2[] = {'r', 's', 't', '\0'};
// 	// char d2[] = {'R', 'S', 'T', '\0'};

// 	// key.data = k2;
// 	// data.data = d2;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");	
	
// 	// char k3[] = {'p', 'q', 'r', '\0'};
// 	// char d3[] = {'P', 'Q', 'R', '\0'};

// 	// key.data = k3;
// 	// data.data = d3;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
	
// 	// char k7[] = {'p', 'q', 'r', '\0'};
// 	// char d7[] = {'Q', 'R', 'S', '\0'};

// 	// key.data = k7;
// 	// data.data = d7;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
	
		
// 	// char k4[] = {'w', 'n', 'g', '\0'};
// 	// char d4[] = {'W', 'N', 'G', '\0'};

// 	// key.data = k4;
// 	// data.data = d4;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
// 	// char k5[] = {'n', 'a', 't', '\0'};
// 	// char d5[] = {'N', 'A', 'T', '\0'};

// 	// key.data = k5;
// 	// data.data = d5;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
	
// 	// char k6[] = {'h', 'e', 'l', '\0'};
// 	// char d6[] = {'H', 'E', 'L', '\0'};

// 	// key.data = k6;
// 	// data.data = d6;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
	
// 	// char k10[] = {'p', 'q', 'r', '\0'};
// 	// char d10[] = {'H', 'E', 'S', '\0'};

// 	// key.data = k10;
// 	// data.data = d10;  
//     // updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
// 	// // printf("Traversal of tree constructed is\n");
//     // // t.traverse(); 
// 	// // printf("\n");
// 	// char k9[]= {'a', 'b', 'c', '\0'};
// 	// key.data = k9;
// 	// updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 

// 	// char k8[] = {'l', 'm', 't', '\0'};
// 	// key.data = k8;
// 	// updated = t.put(key, data);
// 	// // cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
	
// 	// char k11[] = {'c', 'r', 'j', '\0'};
// 	// key.data = k11;
// 	// updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 
	
// 	// char k12[] = {'m', 'o', 'q', '\0'};
// 	// key.data = k12;
// 	// updated = t.put(key, data);
// 	// cout << (updated ? "Updated: " : "Not Updated: ") << key.data << endl; 

// 	// key.data = k;
// 	// t.remove(&key);
// 	// t.traverse();
// 	// cout << endl << endl;

// 	// while(1) {
// 	// 	int i;
// 	// 	scanf("%d", &i);

// 	// 	cout << (t.del(i) ? "Deleted!" : "Not Deleted!" ) << endl;
// 	// 	t.traverse();
// 	// }
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
//      return 0; 
//  } 



