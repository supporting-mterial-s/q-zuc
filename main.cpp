/*
The anf.txt keeps the ANFs of the (inverse) S_0 components.
Note that the XOR of constant 1 should be deleted in anf.txt.
That is, if y_0 = x_0*x_1^x_2^1, we store 'y_0 = x_0*x_1^x_2' in anf.txt, 
After the heuristic returns the implementation of y_0 = x_0*x_1^x_2,
 we can directly calculate the final y_0 by 'y_0 = y_0 ^ 1';
*/

#include"stdio.h"
#include<iostream>
#include <string.h>
#include"vector"
#include<ctype.h>
#include <fstream>
#include<algorithm>
#include<cstring>

using namespace std;

struct row
{
	char p[100];
};

struct operation
{
	vector<uint64_t> oprand;
	//vector<int> flag;
};

ofstream f;
int pool_len = 0;
int min_no_ancilla = 200;
int min_no_and = 100;
int no_add = 0;

void read_anf(vector<operation> &oprand)
{	
	vector<vector<row>> opr;
	fstream f("anf_S0.txt");//or fstream f("anf_inv_S0.txt")
	string line;
	while(getline(f, line))
	{
		if(line.length() > 1) 
    	{
    		vector<row> e;
    		const char *d = "^";
    		char p[1024];
    		strcpy(p, line.c_str());
    		char *q = p;
    		q = strtok(q, d);
    		while(q != NULL)
    		{	
    			row tmp_e;
    			strcpy(tmp_e.p, q);
    			e.push_back(tmp_e);
       			q = strtok(NULL, d);
    		}
    		opr.push_back(e);
    	}
	}

	for(int i = 0; i < opr.size(); i++)
	{
        operation ort;
		for(int j = 0; j < opr[i].size(); j++)
		{
			const char *d = "&";
    		char *q = opr[i][j].p;
    		q = strtok(q, d);
    		uint64_t tmp = 0;
    		while(q != NULL)
    		{	 
    			for(int k = 0; k < strlen(q); k++)
    			{
    				if(isdigit(q[k]))
    				{
    					 tmp ^= (1ll << (63 - (q[k] - '0')));
    				}
    			}
       			q = strtok(NULL, d);
    		}
    		ort.oprand.push_back(tmp);
    	}
        oprand.push_back(ort);
	}
}

void gen_pool(vector<uint64_t> &pool, int m)
{
	for(int i = 0; i < m; i++)
	{
		for(int j = i + 1; j < m; j++)
		{
			pool.push_back(((1ll << (63 - i)) ^ (1ll << (63 - j))));
		}
	}
}

int get_hamming(uint64_t input) 
{
    return __builtin_popcount(input & 0xffffffff) + __builtin_popcount((input >> 32) & 0xffffffff);
}

int variable_involved_depth_1(vector<uint64_t> pool)
{		
	uint64_t tmp = 0;
	for(int i = 0; i < pool.size(); i++)
	{
		tmp = (pool[i] & tmp) ^ (pool[i] ^ tmp);
	}

	return get_hamming(tmp);	
}

int variable_involved_depth_2(vector<vector<operation>> rst, int &no_and_save)
{		
	uint64_t tmp = 0;
	for(int i = 0; i < rst.size(); i++)
	{
		for(int j = 0; j < rst[i].size(); j++)
		{
			for(int k = 0; k < rst[i][j].oprand.size(); k++)
			{
				tmp = (rst[i][j].oprand[k] & tmp) ^ (rst[i][j].oprand[k] ^ tmp);
			}
		}
	}

	no_and_save = (pool_len - get_hamming(tmp & 0xffffffffffffff));

	return get_hamming(tmp);	
}

void print_result(vector<vector<operation>> anf_poly, vector<uint64_t> pool, int cost)
{
	f.open("Opti_result.txt");
	f<<"ancilla qubits number : "<<cost<<endl;
	f<<"------   The first layer of the AND depth  ------"<<endl;
	for(int i = 0; i < pool.size(); i++)
	{
		f<<"the "<<i<<"_th poly."<<endl;
		for(int k = 0; k < 64; k++)
		{
			if((pool[i]>>(63 - k)) &1)
			{
				if(k < 8)
				{
					f<<"x_"<<k<<" ";
				}
				else if(k >= 8)
				{
					f<<"t_"<<(k - 8)<<" ";
				}
			}
		}
		f<<endl<<endl;
	}
	
	f<<endl;
	f<<"------   The second layer of the AND depth  ------"<<endl;
	for(int i = 0; i < anf_poly.size(); i++)
	{	
		f<<"the "<<i<<"_th poly."<<endl;
		for(int j = 0; j < anf_poly[i].size(); j++)
		{   
			for(int l = 0; l < anf_poly[i][j].oprand.size(); l++)
			{
				for(int k = 0; k < 64; k++)
				{
					if((anf_poly[i][j].oprand[l] >> (63 - k)) &1)
					{
						if(k < 8)
						{
							f<<"x_"<<k<<" ";
						}
						else if(k >= 8)
						{
							f<<"t_"<<(k - 8)<<" ";
						}
					}
				}
				f<<endl;
			}
			f<<endl;
		}
		f<<endl<<endl;
	}
	f.close();
}

bool poly_deg(operation poly)
{
	bool flag = false;
	int max_hw = 0;
	for(int k = 0; k < poly.oprand.size(); k++)
	{
		int hm_weight = get_hamming(poly.oprand[k]);
		if(max_hw < hm_weight)
		{	
			max_hw = hm_weight;
		}
	}
	if(max_hw == 2)
	{
		flag = true;
	}

	return flag;
}

//if degree of anf >= 3, return true 
bool anf_deg(vector<operation> anf)
{
	bool flag = false;
	for(int i = 0; i < anf.size(); i++)
	{
		for(int k = 0; k < anf[i].oprand.size(); k++)
		{
			if(get_hamming(anf[i].oprand[k]) > 2)
			{	
				flag = true;
				return flag;
			}
		}
	}

	return flag;
}

void get_second_layer(vector<operation> anf_poly_tmp, vector<uint64_t> pool)
{
	vector<uint64_t> unit_e;
	for(int i = 0; i < (8 + pool_len); i++)
	{
		unit_e.push_back(1ll<<(63 - i));
	}

	int no_and = 0; 
	vector<vector<operation>> tem_result;
	vector<operation> anf_poly(anf_poly_tmp);

	for(int i = 0; i < anf_poly.size(); i++)
	{	
		vector<operation> poly_i;
		while(poly_deg(anf_poly[i]))
		{	
			int frq[8 + pool_len] = {0};
			int pos = -1;
			int max = -1;

			for(int j = 0; j < anf_poly[i].oprand.size(); j++)
			{
				if(get_hamming(anf_poly[i].oprand[j]) == 2)
				{
					for(int k = 0; k < unit_e.size(); k++)
					{
						if(((anf_poly[i].oprand[j]) & unit_e[k]) == unit_e[k])
						{
							frq[k]++;
							if(max < frq[k])
							{
								max = frq[k];
							}
						}
					}
				}
			}

			vector<int> candi_idx;
			for(int j = 0; j < (8 + pool_len); j++)
			{
				if(frq[j] == max)
				{
					candi_idx.push_back(j);
				}
			}

			int idx = rand()%(candi_idx.size());
			pos = candi_idx[idx];

			operation mono_i;
			for(int j = anf_poly[i].oprand.size() - 1; j >= 0; j--)
			{
				if((get_hamming(anf_poly[i].oprand[j]) == 2)&&(((anf_poly[i].oprand[j]) & unit_e[pos]) == unit_e[pos]))
				{
					mono_i.oprand.push_back(anf_poly[i].oprand[j]);
					anf_poly[i].oprand.erase(anf_poly[i].oprand.begin() + j);
				}
			}
			poly_i.push_back(mono_i);
			no_and ++;
		}
		tem_result.push_back(poly_i);
	}

	if(min_no_and > no_and)
	{
		min_no_and = no_and;
		int no_depth_1 = variable_involved_depth_1(pool);
		int no_and_saved = 0;
		int no_depth_2 = variable_involved_depth_2(tem_result, no_and_saved);
		int no_anc = 0;

		if((min_no_and * 4 - no_depth_2 - (pool_len * 4 - no_depth_1 - pool_len + no_and_saved)) <= 0)
		{
			no_anc = pool_len * 4 - no_depth_1;	
		}

		else if((min_no_and * 4 - no_depth_2 - (pool_len * 4 - no_depth_1 - pool_len + no_and_saved)) > 0)
		{
			no_anc = (pool_len * 4 - no_depth_1) + (min_no_and * 4 - no_depth_2 - (pool_len * 4 - no_depth_1 - pool_len + no_and_saved));
		}

		if(no_anc < min_no_ancilla)
		{
			min_no_ancilla = no_anc;
			cout<<hex<<"min_no_ancilla_qubits (in hex) = "<<	min_no_ancilla <<endl;
			print_result(tem_result, pool, min_no_ancilla);
		}
	}
}

vector<uint64_t> get_first_layer(vector<operation> anf_poly, int len)
{
	vector<uint64_t> rst;
	vector<uint64_t> pool;
	gen_pool(pool, len); //pool:x_0*x_1, x_0*x_2,...,x_6*x_7
	int cnt = 0;
	while(anf_deg(anf_poly))
	{	
		int frq[pool.size()] = {0};
		int pos = -1;
		int max_frq = -1;

		//calculate the frequency of x_i&x_j appears in all the ANFs
		for(int k = 0; k < pool.size(); k++)
		{
			for(int i = 0; i < anf_poly.size(); i++)
			{  //i: the i-th polynomial
				for(int j = 0; j < anf_poly[i].oprand.size(); j++)
				{
					if((((anf_poly[i].oprand[j]) & pool[k]) == pool[k]) && (get_hamming(anf_poly[i].oprand[j]) >= 3))
					{
						frq[k]++;
						if(max_frq < frq[k])
						{
							max_frq = frq[k];
						}
					}
				}
			}
		}

		vector<int> candi_idx;
		for(int j = 0; j < pool.size(); j++)
		{
			if(frq[j] == max_frq)
			{
				candi_idx.push_back(j);
			}
		}
		int idx = rand()%(candi_idx.size());
		pos = candi_idx[idx];// mark the x_i&x_j with the most occurrences

		for(int i = 0; i < anf_poly.size(); i++)
		{
			for(int j = 0; j < anf_poly[i].oprand.size(); j++)
			{
				if(((anf_poly[i].oprand[j]) & pool[pos]) == pool[pos])
				{ 
					anf_poly[i].oprand[j] ^= pool[pos]; //delete x_i&x_j in all the ANFs
					anf_poly[i].oprand[j] ^= (1ll << (55 - cnt));//add new variable t_{cnt}
				}
			}
		}
		cnt ++;
		rst.push_back(pool[pos]);
	}
	return rst;//return {x_i0&x_j0, x_i1&x_j1,...,x_icnt-1&x_jcnt-1}, after which, degree(ANFs) < 3: the AND operations in the first AND depth;
}

vector<uint64_t> find_remainder(vector<uint64_t> pool)
{
	vector<uint64_t> pool_all;
	gen_pool(pool_all, 8);
	for(int i = pool_all.size() - 1; i >= 0; i--)
	{
		vector<uint64_t>::iterator iter = find(pool.begin(), pool.end(), pool_all[i]);
		if(iter != pool.end())
		{
			pool_all.erase(pool_all.begin() + i);
		}
	}
	return pool_all;
}

void get_element(int step, int start, int end, int cnt, int *base, int *flag, int *rst_tmp, vector<vector<uint64_t>> &and_add)
{
	if(step == cnt) 
    {
    	vector<uint64_t> rst;
        for(int i = 0; i < cnt; i++)
        {
            rst.push_back(rst_tmp[i]);
        }
        and_add.push_back(rst);
    }
    else if(step < cnt) 
    {
    	for(int i = start; i < end; i++) 
    	{
        	if(flag[i] == 1)
           		continue;
        
        	flag[i] = 1;
       	 	rst_tmp[step] = base[i];
       		get_element(step + 1, i + 1, end, cnt, base, flag, rst_tmp, and_add);    
        	flag[i] = 0;
    	}
	}
}

void perm(int *num, int n, int m, vector<vector<uint64_t>> &idx)	 
{
	if(n == m)	 
	{
		vector<uint64_t> idx_row;
		for(int j = 0; j < m; j++)
		{
			idx_row.push_back(num[j]);
		}

		idx.push_back(idx_row);	
	} 
	for(int j = n; j < m; j++)
	{
		swap(num[j], num[n]);
		perm(num, n + 1, m, idx);
		swap(num[j], num[n]);	 
	}
}

void update_first_layer(vector<uint64_t> pool_ori, vector<uint64_t> pool_comp, vector<uint64_t> idx, vector<operation> anf_poly)
{
	vector<uint64_t> pool(pool_ori);
	for(int i = 0; i < idx.size(); i++)
	{
		pool.push_back(pool_comp[idx[i]]);
	}
	pool_len = pool.size();
	vector<operation> anf_poly_tmp(anf_poly);
	for(int i = 0; i < pool.size(); i++)
	{
		for(int j = 0; j < anf_poly_tmp.size(); j++)
		{	
			for(int k = 0; k < anf_poly_tmp[j].oprand.size(); k++)
			{
				if((anf_poly_tmp[j].oprand[k] & pool[i]) == pool[i])
				{
					anf_poly_tmp[j].oprand[k] ^= pool[i];
					anf_poly_tmp[j].oprand[k] ^= (1ll << (55 - i));
				}
			}
		}
	}

	int loop_cnt = 0;
	while(loop_cnt < 1000)
	{
		get_second_layer(anf_poly_tmp, pool);
		loop_cnt ++;
	}
}

void find_and_gate(vector<operation> anf_poly)
{
	vector<uint64_t> pool_ori = get_first_layer(anf_poly, 8);//pool_ori: the AND operations in the first layer
	vector<uint64_t> pool_comp = find_remainder(pool_ori);//pool_comp: C(8, 2)\{pool_ori}

	int base[64] = {0};
	int flag[64] = {0};
	for(int i = 0; i < 64; i++)
    {
        base[i] = i;
       	flag[i] = 0;
    }
 	
    int rst_tmp[64] = {0};
    vector<vector<uint64_t>> and_add_cand_idx;
    get_element(0, 0, pool_comp.size(), no_add, base, flag, rst_tmp, and_add_cand_idx);

	for(int i = 0; i < and_add_cand_idx.size(); i++)
	{
		int csn[and_add_cand_idx[i].size()] = {0};
    	for(int j = 0; j < and_add_cand_idx[i].size(); j++)
    	{
        	csn[j] = and_add_cand_idx[i][j];
    	} 
    	vector<vector<uint64_t>> idx;
		perm(csn, 0, and_add_cand_idx[i].size(), idx);
			
		for(int j = 0; j < idx.size(); j++)
		{
			update_first_layer(pool_ori, pool_comp, idx[j], anf_poly);
		}
	}
}

int main()
{
	vector<operation> anf_poly;
	read_anf(anf_poly);

	min_no_ancilla = 200;

	while(1)
	{
		no_add = 1;
		find_and_gate(anf_poly);
	}
	cout<<"end"<<endl;
	return 0;
}