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

            //change made using temp
            int  temp=2*degree-1;

			keysizes = new uint8_t [temp];
			valuesizes = new uint8_t [temp];
			keys = new char* [temp];
			values = new char* [temp];
			for(int i=0; i<temp; i++) {
				keys[i] = new char[64] {0};
				values[i] = new char[255] {0};
				keysizes[i] = 0;
                valuesizes[i] = 0;
			}

			child_arr = new Node* [temp+1] {0};
			child_num = new int[temp+1] {0};
			
			key_num=0;
		}

		char* search(Slice* k, Slice* v){ 

			int  i = 0; 
            /// made changes in whike loop
            // char *s=k->data;
			while (i < key_num && strncmp(k->data, keys[i], 64) > 0) 
				i++; 
		
			if (i < key_num && strncmp(keys[i], k->data, 64) == 0) {
				v->data = values[i];
				v->size = valuesizes[i];
				return values[i]; 
			}
			
			if (is_leaf == true){ 
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
				else if(num+1 == n) {
					key->data = keys[i];
					value->data = values[i];
					key->size = keysizes[i];
					value->size = valuesizes[i];
					*found = 1;
					return;
				}
				else {
					n = n - num -1;
				}
			}
			if(!is_leaf && child_num[key_num] >= n) child_arr[key_num]->lognnth(n, found, key, value);
			else *found = 0;
			return;
		}

		
		void traverse(int length) { 
		    int i; 
		    for (i = 0; i < key_num; i++) 
		    { 
		        if (is_leaf == false) 
		            child_arr[i]->traverse(length+1); 
				printf("%s\n",keys[i]);
		    } 
		
		    if (is_leaf == false) 
		        child_arr[i]->traverse(length+1); 
		} 

		int insertNonFull(Slice* k, Slice* v, bool* updated){
			int  index = key_num -1;
			if(is_leaf == true)
			{
                   //change in temp 
                int  temp;
                
				while(index >= 0 && (strncmp(keys[index],k->data, 64) > 0))
				{
                    temp=index+1;

					strncpy(keys[temp], keys[index], keysizes[index]);
                    keysizes[temp] = keysizes[index];	
					strncpy(values[temp], values[index], valuesizes[index]);
                    valuesizes[temp] = valuesizes[index]; 
					index--;
				}
				if( index >= 0 && strncmp(keys[index], k->data, 64) == 0){
					strncpy(values[index], v->data, v->size);
                    valuesizes[index] = v->size; 
					*updated = 1;
					index++;
					while(index < key_num) {
                        
                        temp=index+1;  //change
						
                        strncpy(keys[index], keys[temp], keysizes[temp]);
                        keysizes[index] = keysizes[temp]; 
						strncpy(values[index], values[temp], valuesizes[temp]);
                        valuesizes[index] = valuesizes[temp];	
						index++;
					}
					return 0;
				}
				else {

					temp=index+1; //change

                    strncpy(keys[temp], k->data, k->size);
                    keysizes[temp] = k->size; 
					strncpy(values[temp], v->data, v->size);
                    valuesizes[temp] = v->size; 
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
                        int alpha=index+1,beta=degree-1; //change

						if(strncmp(child_arr[alpha]->keys[beta], k->data, 64) == 0) {
							strncpy(child_arr[alpha]->values[beta], v->data, v->size);
							child_arr[alpha]->valuesizes[beta] = v->size; 
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

            int  temp2;//change
            int  temp3=degree-1;

			for(int j=0; j < temp3; j++) {
                temp2=j+degree;
				strncpy(z->keys[j], child->keys[temp2], child->keysizes[temp2]);
				z->keysizes[j] = child->keysizes[temp2]; 
				strncpy(z->values[j], child->values[temp2], child->valuesizes[temp2]);
				z->valuesizes[j] = child->valuesizes[temp2]; 

			}

			if(!child->is_leaf) {
				for(int j=0; j<degree; j++) {
                
                temp2=j+degree; //change

					z->child_arr[j] = child->child_arr[temp2];
					z->child_num[j] = child->child_num[temp2];
					z_child_num += z->child_num[j];
					child->child_num[temp2] = 0;
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

            int  temp4;//change

			for(int j=key_num-1; j>=i; j--) {
                temp4=j+1;//change
				strncpy(keys[temp4], keys[j], keysizes[j]);
				keysizes[temp4]=keysizes[j]; 
				strncpy(values[temp4], values[j], valuesizes[j]);		
				valuesizes[temp4]=valuesizes[j]; 
			}

            temp3=degree-1;  //change

			strncpy(keys[i], child->keys[temp3], child->keysizes[temp3]);
			keysizes[i] = child->keysizes[temp3]; 
			strncpy(values[i], child->values[temp3], child->valuesizes[temp3]);
			valuesizes[i] = child->valuesizes[temp3]; 

			key_num++;
			child_num[i]--;
		}

		int remove(Slice* k, bool* removed) {
			int  i=0;

            int  temp;//change
			while(i<key_num && strncmp(keys[i], k->data, 64) < 0) ++i;

			if(i<key_num && strncmp(keys[i], k->data, 64) == 0) {
				if(is_leaf) {
					for(int ii = i+1; ii<key_num; ++ii) {
						temp=ii-1;
                        strncpy(keys[temp], keys[ii], keysizes[ii]);
						keysizes[temp]=keysizes[ii]; 
						strncpy(values[temp], values[ii], valuesizes[ii]);
						valuesizes[temp]=valuesizes[ii];
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
                        int  temp2=cur->key_num-1;//change
						strncpy(keys[i], cur->keys[temp2], cur->keysizes[temp2]);
						keysizes[i] = cur->keysizes[temp2]; 
						strncpy(values[i], cur->values[temp2], cur->valuesizes[temp2]);
						valuesizes[i] = cur->valuesizes[temp2]; 
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

            int  temp; //change
			for(int iter = child->key_num-1 ;iter >=0 ;iter--) {
				temp=iter+1; //change
                strncpy(child->keys[temp], child->keys[iter], child->keysizes[iter]);
				child->keysizes[temp] = child->keysizes[iter]; 
				strncpy(child->values[temp], child->values[iter], child->valuesizes[iter]);
				child->valuesizes[temp] = child->valuesizes[iter]; 
			}
			if(!child->is_leaf){
				for(int iter2 = child->key_num; iter2 >=0 ;iter2--)
				{
					child->child_arr[iter2+1] = child->child_arr[iter2];
					child->child_num[iter2+1] = child->child_num[iter2];
				}
			}
            int  temp2=idx-1;
			strncpy(child->keys[0], keys[temp2], keysizes[temp2]);
			child->keysizes[0] = keysizes[temp2]; 
			strncpy(child->values[0], values[temp2], valuesizes[temp2]);
			child->valuesizes[0] = valuesizes[temp2]; 

			child_num[idx]++;	

			
			if(!child ->is_leaf){
                int  temp3=sibling->key_num;  //change

				child->child_arr[0] = sibling->child_arr[temp3];
				child->child_num[0] = sibling->child_num[temp3];
				child_num[idx] += sibling->child_num[temp3];
				child_num[idx-1] -= sibling->child_num[temp3];
			}
			
            temp2=idx-1;  //change
            int  temp3=sibling->key_num-1;  //change
			
            strncpy(keys[temp2], sibling->keys[temp3], sibling->keysizes[temp3]);
			keysizes[temp2] = sibling->keysizes[temp3]; 
			strncpy(values[temp2], sibling->values[temp3], sibling->valuesizes[temp3]);
			valuesizes[temp2] = sibling->valuesizes[temp3]; 

			child_num[temp2]--;

			child->key_num += 1;
			sibling->key_num -= 1;

			return;
		} 
  
		void borrowFromNext(int idx){

			Node *child = child_arr[idx]; 
		    Node *sibling = child_arr[idx+1]; 

            int  temp=child->key_num; //change

			strncpy(child->keys[temp], keys[idx], keysizes[idx]); 
			child->keysizes[temp] = keysizes[idx]; 
		    strncpy(child->values[temp], values[idx], valuesizes[idx]); 
			child->valuesizes[temp] = valuesizes[idx]; 

			child_num[idx]++;

		    if (!(child->is_leaf)) {
                temp=child->key_num+1; //change

		        child->child_arr[temp] = sibling->child_arr[0]; 
				child->child_num[temp] = sibling->child_num[0];
				child_num[idx] += sibling->child_num[0];
				child_num[idx+1] -= sibling->child_num[0];
			}

		    strncpy(keys[idx], sibling->keys[0], sibling->keysizes[0]); 
			keysizes[idx] = sibling->keysizes[0]; 
		    strncpy(values[idx], sibling->values[0], sibling->valuesizes[0]);
			valuesizes[idx] = sibling->valuesizes[0]; 
			child_num[idx+1]--; 

            int  temp3;  //change
		    for (int i=1; i<sibling->key_num; ++i){ 
                temp3=i-1;
		        strncpy(sibling->keys[temp3], sibling->keys[i], sibling->keysizes[i]); 
				sibling->keysizes[temp3] = sibling->keysizes[i]; 
		        strncpy(sibling->values[temp3], sibling->values[i], sibling->valuesizes[i]); 
				sibling->valuesizes[temp3] = sibling->valuesizes[i]; 
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

            int  temp=degree-1;  //change
			
			strncpy(child->keys[temp], keys[i], keysizes[i]);
			child->keysizes[temp] = keysizes[i];	
			strncpy(child->values[temp], values[i], valuesizes[i]);
			child->valuesizes[temp] = valuesizes[i];	

			child_num[i]++;
			
    
			for(int ii=0; ii<sibling->key_num; ++ii) {
                temp=ii+degree; //change
				strncpy(child->keys[temp], sibling->keys[ii], sibling->keysizes[ii]);
				child->keysizes[temp] = sibling->keysizes[ii]; 
				strncpy(child->values[temp], sibling->values[ii], sibling->valuesizes[ii]);
				child->valuesizes[temp] = sibling->valuesizes[ii]; 
				child_num[i]++;
				child_num[i+1]--;
			}
			
			if(!child->is_leaf) {
				for(int ii=0; ii <= sibling->key_num; ++ii) {
                    temp=ii+degree; //change

					child->child_arr[temp] = sibling->child_arr[ii];
					child->child_num[temp] = sibling->child_num[ii];
					child_num[i] += sibling->child_num[ii];
					child_num[i+1] -= sibling->child_num[ii];
				}
			}

			for(int ii=i+1; ii<key_num; ++ii) {
                temp=ii-1;
				strncpy(keys[temp], keys[ii], keysizes[ii]);
				keysizes[temp] = keysizes[ii]; 
				strncpy(values[temp], values[ii], valuesizes[ii]);
				valuesizes[temp] = valuesizes[ii]; 
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
	pthread_mutex_t put_del_lock; //bhavesh

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
                uint8_t temp = k->size ,temp2 = v->size;
				strncpy(root->keys[0], k->data, temp);
				root->keysizes[0] = temp; 
				strncpy(root->values[0], v->data, temp2);
				root->valuesizes[0] = temp2; 
				root->key_num = 1; 
				*updated = false;
			} 
		
			else { 
				if (root->key_num == 2*degree-1) { 
                    uint8_t temp = k->size , temp2 = v->size;
					for(int i=0; i<root->key_num; i++) {
						if(strncmp(root->keys[i], k->data, 64) > 0) break;
						else if(strncmp(root->keys[i], k->data, 64) == 0) {
							strncpy(root->keys[i], k->data, temp);
							root->keysizes[i] = temp; 
							strncpy(root->values[i], v->data, temp2);
							root->valuesizes[i] = temp2; 
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

		friend class kvStore;
}; 

class kvStore {
    BTree *tr;
	int max_ent;
	public:
		int pres_ent;

	    kvStore(uint64_t max_entries){
			tr = new BTree (3);
			max_ent = max_entries;
			pres_ent=0;
		}

	    bool get(Slice &key, Slice &value) {
			char * returned = tr->search(&key,&value);
			if(returned == NULL) return false;
			else {
				return true;
			}
		} 
        
		bool put(Slice &key, Slice &value){
			bool updated = false;
			tr->insert(&key, &value, &updated);
			if(!updated){
				pres_ent++;
				tr->elems++;
			}
			return updated;
		} //returns true if value overwritten
		
		bool del(Slice &key){
			bool removed = true;
			tr->remove(&key, &removed);
			if(removed){
				pres_ent--;
				tr->elems--;
			}
			return removed;
		}       
		
		void traverse() {
			tr->traverse();
		}
		
		bool get(int N, Slice &key, Slice &value) {
			N++;
			return tr->lognnth(N, &key, &value);
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
		 } //delete Nth key-value pair

};