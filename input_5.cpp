#include <iostream>
#include <algorithm>
#include <stack>
#include <vector>
#include <list>
#include <iterator>
#include <chrono>
#include <iomanip>
using namespace std;

void primary_assignment(int tl_k[][3],int C[], int t_re, int n, int k){
    int tl_min[n];
    for(int i=0; i<n; i++){
        tl_min[i] = INT_MAX;
    }
    for(int i=0; i<n; i++){
        for(int j=0; j<k; j++){
            if(tl_min[i]>tl_k[i][j]){
                tl_min[i] = tl_k[i][j];
            }
        }
    }
    // C[i] = 1 --> cloud
    // C[i] = 0 --> local 
    for(int i=0; i<n; i++){
        if(tl_min[i] > t_re){
            C[i] = 1;
        }
        else
            C[i] = 0;
    }
}

void task_prioritzing(int tl_k[][3], int priority[], int priority_f[], int succ[][20], int w[], int C[], int t_re, int n, int k){
    // Cost Cal
    for(int i=0; i<n; i++){
        if(C[i] == 1){
            w[i] = t_re;
        }
        else{
            int sum = 0;
            for(int j=0; j<k; j++){
                sum += tl_k[i][j];
            }
            w[i] = sum/k;
        }
    }
    // Caculate priority
    priority[n-1] = w[n-1];
    for(int i=n-1; i>=0; i--){
        int max_j = 0;
        for(int j=n-1; j>=0; j--){
            if(succ[i][j] == 1 && priority[j] > max_j){
                max_j = priority[j];
                priority[i] = w[i] + max_j;  
            }
        }
    }
    // Final priority cal
    vector<pair<int,int > > vect;
    for (int i=0; i<n; i++){
        vect.push_back(make_pair(priority[i],i));
    }
    sort(vect.begin(), vect.end());
    for(int i=0; i<n; i++){
        priority_f[i] = vect[i].second;
    }
}

void execution_unit_selection(int tl_k[][3],int priority_f[], int succ[][20], int C[], int rt_l[], int rt_c[], int rt_ws[], int ft_ws[], int ft_wr[], int ft_l[], int ft[], int core_t[], int exe_loc[], int n, int k, int t_s, int t_r, int t_c, vector<vector<int> > curr_pos){
    int f_i = priority_f[n-1]; //the highest priority
    rt_l[f_i] = 0;
    rt_ws[f_i] = 0;
    ft_ws[f_i] = rt_ws[f_i] + t_s;
    rt_c[f_i] = ft_ws[f_i];
    if(C[f_i] == 1){ //cloud
        ft_wr[f_i] = rt_c[f_i] + t_c + t_r;
        ft_l[f_i] = 0;
        ft[f_i] = ft_wr[f_i];
        core_t[3] = ft[f_i];
        curr_pos[0].push_back(f_i);
        exe_loc[f_i] = 3;
    }
    else{ //local
        int tl_min = INT_MAX;
        int index;
        for(int i=0; i<k; i++){
            if(tl_k[f_i][i]<tl_min){
                tl_min = tl_k[f_i][i];
                index = i;
            }
        }
        ft_l[f_i] = rt_l[f_i] + tl_min;
        ft_wr[f_i] = rt_c[f_i] + t_c + t_r;
        if(ft_l[f_i] <= ft_wr[f_i]){
            ft[f_i] = ft_l[f_i];
            ft_wr[f_i] = 0;
            core_t[3] = ft_ws[f_i];
            curr_pos[index+1].push_back(f_i);
            exe_loc[f_i] = index;
        }
        else{
            ft[f_i] = ft_wr[f_i];
            ft_l[f_i] = 0;
            core_t[index] = ft[f_i];
            curr_pos[0].push_back(f_i);
            exe_loc[f_i] = 3;
        }
    }
    for(int a=n-2; a>=0; a--){
        int i = priority_f[a]; // Get the task ID in the current order of priority

        // Calculate Ready Time for Local Execution
        int max_j_l = 0;
        for(int j=0; j<n; j++){
            if(succ[j][i] == 1 && max_j_l < max(ft_l[j],ft_wr[j])){
                max_j_l = max(ft_l[j],ft_wr[j]);
            }
        }
        rt_l[i] = max_j_l;

        // Calculate Ready Time for Wireless Sending
        int max_j_ws = 0;
        for(int j=0; j<n; j++){
            if(succ[j][i] == 1 && max_j_ws < max(ft_l[j],ft_ws[j])){
                max_j_ws = max(ft_l[j],ft_ws[j]);
            }
        }
        rt_ws[i] = max_j_ws;

        // Calculate Finish Time on Wireless Sending
        ft_ws[i] = max(core_t[3],rt_ws[i]) + t_s;

        int max_j_c = 0;
        for(int j=0; j<n; j++){
            if(succ[j][i] == 1 && max_j_c < ft_wr[j]-t_r){
                max_j_c = ft_wr[j]-t_r;
            }
        }
        //Calculate Ready Time for Cloud Execution 
        rt_c[i] = max(ft_ws[i],max_j_c);

        // Check if task is a cloud task or a local task
        if(C[i] == 1){
            // Cloud task
            ft_wr[i] = rt_c[i] + t_c + t_r;
            ft[i] = ft_wr[i];
            ft_l[i] = 0;
            core_t[3] = ft_ws[i];
            curr_pos[0].push_back(i);
            exe_loc[i] = 3; // Task is assigned to cloud
        }
        else{
            // Local task
            int rt, index;
            int f = INT_MAX;
            for(int j=0; j<k; j++){
                rt = max(rt_l[i],core_t[j]);
                if(f > rt + tl_k[i][j]){
                    f = rt + tl_k[i][j];
                    index = j;
                }
            }
            rt_l[i] = f - tl_k[i][index];
            ft_l[i] = f;
            ft_wr[i] = rt_c[i] + t_c + t_r;
            // Reassign to local or cloud
            if(ft_l[i] <= ft_wr[i]){
                ft[i] = ft_l[i];
                ft_wr[i] = 0;
                core_t[index] = ft[i];
                curr_pos[index+1].push_back(i);
                exe_loc[i] = index;
            }
            else{
                ft[i] = ft_wr[i];
                ft_l[i] = 0;
                core_t[3] = ft[i];
                curr_pos[0].push_back(i);
                exe_loc[i] = 3;
            }
        }
    }
}

int kernel( vector<vector<int> > curr_pos, int tl_k[][3], int succ[][20],int exe_loc[], int tmax, int t_total, float e_total, int n, int k, int st[], int et[], int E_c, int E_l[][3]){
    // k = Number of local cores available for execution
    int out = 0;
    int count = 0;
    
    while(out == 0){
        float max_ratio = 0;
        int new_m = 0, new_k = 0, new_index1 = 0, new_index2 = 0, new_t = t_total;
        float new_e = e_total;
        int less_t1 =0, less_t2 = 0;
        // if less_t1 = 1: the migration was found to reduce energy without exceeding Ttotal
        // if less_t2 = 1: the migration was found to reduce energy with Ttotal > tmax but improve energy-to-time ratio
        int temp_core[20], new_st[20], new_ft[20];;
        for(int i=0; i<n; i++){ // task i
            for(int j=0; j<k+1; j++){ // location 

                int pos2[20], curr_core_state[4], rt[20], rt1[20], ft[20], ft1[20], pushed[20];
                vector<vector<int> > curr_exe_loc(4);
                int index1, index2 = 0;
                for(int i=0; i<20; i++){
                    rt[i] = 0;
                    ft[i] = 0;
                    pos2[i] = exe_loc[i];
                    ft1[i] = et[i];
                    rt1[i] = st[i];
                    pushed[i] = 0;
                }
                for(int a=0; a<curr_pos.size(); a++){
                    curr_core_state[a] = 0;
                    for(int b=0; b<curr_pos[a].size(); b++){
                        curr_exe_loc[a].push_back(curr_pos[a][b]);
                    }
                }
                int current_core = exe_loc[i];
                for(int a=0; a<curr_exe_loc[current_core].size(); a++){
                    if(curr_exe_loc[current_core][a] == i){
                        index1 = a;
                    }
                }
                curr_exe_loc[current_core].erase(curr_exe_loc[current_core].begin()+index1);

                // Calculate ready time of the new location
                if(j == 3){ //cloud
                    int max_j_ws = 0;
                    for(int a=0; a<n; a++){
                        if(succ[a][i] == 1 && max_j_ws < ft1[a]){
                            max_j_ws = ft1[a];
                        }
                    }
                    rt[i] = max_j_ws;
                }
                else{
                    int max_j_l = 0;
                    for(int a=0; a<n; a++){
                        if(succ[a][i] == 1 && max_j_l < ft1[a]){
                            max_j_l = ft1[a];
                        }
                    }
                    rt[i] = max_j_l;
                }
                pos2[i] = j;

                // get the insertion index
                if(curr_exe_loc[j].size() == 0){
                    index2 = 0;
                }
                else if(curr_exe_loc[j].size() == 1){
                    if(rt1[curr_exe_loc[j][0]] > rt[i]){
                        index2 = 0;
                    }
                    else{
                        index2 = 1;
                    }
                }
                else{
                    // Finds insertion point based on ready times
                    if(rt1[curr_exe_loc[j][0]] > rt[i]){
                        index2 = 0;
                    }
                    else if(rt1[curr_exe_loc[j][curr_exe_loc[j].size()-1]] <= rt[i]){
                        index2 = curr_exe_loc[j].size();
                    }
                    else{
                        for(int b=0; b<curr_exe_loc[j].size()-1; b++){
                            if(rt[i]>=rt1[curr_exe_loc[j][b]] && rt[i]<=rt1[curr_exe_loc[j][b+1]]){
                                index2 = b+1;
                            }
                        }
                    }
                }
                curr_exe_loc[j].insert(curr_exe_loc[j].begin()+index2,i);
                int ready_num[20], ready_first[20];
                // if ready_num[] = 0: all predecessors are completed
                // if ready_first[] = 0: task is at the beginning of the exe sequence 
                for(int a=0; a<20; a++){
                    ready_num[a] = 0;
                }
                for(int a=0; a<20; a++){
                    for(int b=0; b<20; b++){
                        if(succ[a][b] == 1){
                            ready_num[b] += 1;
                        }
                    }
                    ready_first[a] = 1;
                }
                for(int a=0; a<4; a++){
                    if(curr_exe_loc[a].size()>0){
                        ready_first[curr_exe_loc[a][0]] = 0;
                    }
                }
              stack<int> s;
                for(int a=0; a<20; a++){
                    if(ready_num[a] == 0 && ready_first[a] == 0 && pushed[a] == 0){
                        s.push(a);
                        pushed[a] = 1;
                    }
                }
                int current1 = s.top();
                s.pop();
                rt[current1] = 0;
                if(pos2[current1] == 3){
                    rt[current1] = max(curr_core_state[pos2[current1]],rt[current1]);
                    ft[current1] = rt[current1] + 5;
                    curr_core_state[pos2[current1]] = rt[current1] + 3;
                }
                else{
                    rt[current1] = max(curr_core_state[pos2[current1]],rt[current1]);
                    ft[current1] = rt[current1] + tl_k[current1][pos2[current1]];
                    curr_core_state[pos2[current1]] = ft[current1];
                }
                // Update ready vectors for successors
                for(int a=0; a<20; a++){
                    if(succ[current1][a] == 1){
                        ready_num[a] -= 1;
                    }
                }
                ready_first[current1] = 1;
                if(curr_exe_loc[pos2[current1]].size()>1){
                    for(int a=1; a<curr_exe_loc[pos2[current1]].size(); a++){
                        if(curr_exe_loc[pos2[current1]][a-1] == current1){
                            ready_first[curr_exe_loc[pos2[current1]][a]] = 0;
                        }
                    }
                }
                for(int a=0; a<20; a++){
                    if(ready_num[a] == 0 && ready_first[a] == 0 && pushed[a] == 0){
                        s.push(a);
                        pushed[a] = 1;
                    }
                }
                while(s.size() != 0){
                    int current = s.top();
                    s.pop();
                    if(pos2[current] == 3){
                        int max_j_ws1 = 0;
                        for(int a=0; a<n; a++){
                            if(succ[a][current] == 1 && max_j_ws1 < ft[a]){
                                max_j_ws1 = ft[a];
                            }
                        }
                        rt[current] = max_j_ws1;
                    }
                    else{
                        int max_j_l1 = 0;
                        for(int a=0; a<n; a++){
                            if(succ[a][current] == 1 && max_j_l1 < ft[a]){
                                max_j_l1 = ft[a];
                            }
                        }
                        rt[current] = max_j_l1;
                    }
                    if(pos2[current] == 3){
                        rt[current] = max(curr_core_state[pos2[current]],rt[current]);
                        ft[current] = rt[current] + 5;
                        curr_core_state[pos2[current]] = rt[current] + 3;
                    }
                    else{
                        rt[current] = max(curr_core_state[pos2[current]],rt[current]);
                        ft[current] = rt[current] + tl_k[current][pos2[current]];
                        curr_core_state[pos2[current]] = ft[current];
                    }
                    for(int a=0; a<20; a++){
                        if(succ[current][a] == 1){
                            ready_num[a] -= 1;
                        }
                    }
                    ready_first[current] = 1;
                    if(curr_exe_loc[pos2[current]].size()>1){
                        for(int a=1; a<curr_exe_loc[pos2[current]].size(); a++){
                            if(curr_exe_loc[pos2[current]][a-1] == current){
                                ready_first[curr_exe_loc[pos2[current]][a]] = 0;
                            }
                        }
                    }
                    for(int a=0; a<20; a++){
                        if(ready_num[a] == 0 && ready_first[a] == 0 && pushed[a] == 0){
                            s.push(a);
                            pushed[a] = 1;
                        }
                    }
                }
                int current_t = ft[n-1];
                int current_e = 0;
                for(int a=0; a<20; a++){
                    if(pos2[a] == 3){
                        current_e += E_c;
                    }
                    else{
                        current_e += E_l[a][pos2[a]];
                    }
                }
                if(current_t <= t_total && current_e < new_e){
                    //cout << "[DEBUG] Found less_t1: Task " << i << ", Location " << j << endl;
                    less_t1 = 1;
                    new_m = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = current_t;
                    new_e = current_e;
                    for(int a=0; a<20; a++){
                        temp_core[a] = pos2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = ft[a];
                     }
                }
                if(current_t > t_total && current_t <= tmax && less_t1 == 0 && current_e < e_total && max_ratio < double((e_total - current_e) / (current_t - t_total))){
                    max_ratio = double((e_total - current_e) / (current_t - t_total));
                    less_t2 = 1;
                    new_m = i;
                    new_k = j;
                    new_index1 = index1;
                    new_index2 = index2;
                    new_t = current_t;
                    new_e = current_e;
                    for(int a=0; a<20; a++){
                        temp_core[a] = pos2[a];
                        new_st[a] = rt[a];
                        new_ft[a] = ft[a];
                    }
                }

            }
        }
        if (less_t1 != 1 && less_t2 != 1) {
            out = 1;
        }

        else{
            curr_pos[exe_loc[new_m]].erase(curr_pos[exe_loc[new_m]].begin()+new_index1);
            curr_pos[new_k].insert(curr_pos[new_k].begin()+new_index2,new_m);
            t_total = new_t;
            e_total = new_e;
            for(int a=0; a<20; a++){
                exe_loc[a] = temp_core[a];
                st[a] = new_st[a];
                et[a] = new_ft[a];
            }
            if(less_t1 != 1 && less_t2 != 1){
                out = 1;
            }
            count += 1;
        }
    }
    cout<<endl;
    cout<<"Result by Optimized task scheduling algorithm:"<<endl;

    for(int i=0; i<curr_pos.size(); i++){
        if(i == 3){
            cout<<"wireless sending : ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]]<<"-"<<et[curr_pos[i][j]] - 2 - 2<<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
            cout<<endl;
            cout<<"Cloud: ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]] + 1<<"-"<<et[curr_pos[i][j]] - 3<<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
            cout<<endl;
            cout<<"wireless receiving : ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]] + 2<<"-"<<et[curr_pos[i][j]] - 2<<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
            cout<<endl;
        }
    
        else{
            cout<<"Core "<<i+1<<": ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]]<<"-"<<et[curr_pos[i][j]]<<" for Task"<<curr_pos[i][j]+1<<"; ";

        }
        }
        cout<<endl;
    }

        cout<<"Energy Consumption of scheduling: "<<e_total<<"   Total Completion Time: "<<t_total<<endl;

}

int main(int argc, char *argv[])
{
    int tl_k[][3]={{9,7,5},
                      {8,6,5},
                      {6,5,4},
                      {7,5,3},
                      {5,4,2},
                      {7,6,4},
                      {8,5,3},
                      {6,4,2},
                      {5,3,2},
                      {7,4,2},
                      {8,7,5},
                      {7,6,5},
                      {6,5,4},
                      {8,5,3},
                      {5,4,2},
                      {7,6,4},
                      {6,5,3},
                      {5,4,2},
                      {4,3,2},
                      {5,4,1}
                      };
    int succ[][20]={{0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

    int C[20]; 
    int priority[20], priority_f[20], w[20], core_t[4], exe_loc[20];
    int rt_l[20], rt_c[20], rt_ws[20], ft_ws[20], ft_wr[20], ft_l[20], ft[20];
    vector<vector<int> > curr_pos(4);
    for(int i=0; i<20; i++){
        C[i] = 0;
        priority[i] = 0;
        priority_f[i] = 0;
        w[i] = 0;
        rt_l[i] = 0;
        rt_ws[i] = 0;
        rt_c[i] = 0;
        ft_ws[i] = 0;
        ft_wr[i] = 0;
        ft_l[i] = 0;
        ft[i] = 0;
        exe_loc[i] = 0;
    }
    for(int i=0; i<4; i++){
        core_t[i] = 0;
    }
    int k = 3, n = 20;
    int t_s = 3, t_c = 1, t_r = 1;
    int t_re = t_s + t_c + t_r;
    int E_l[20][3];
    int pk[] = {1,2,4};
    float ps = 0.5;
    float E_c = ps * t_s;
    for(int i=0; i<20; i++){
      for(int j=0; j<3; j++){
          E_l[i][j] = pk[j] * tl_k[i][j];
      }
    }
    auto start = chrono::high_resolution_clock::now();
    ios_base::sync_with_stdio(false);
    primary_assignment(tl_k,C,t_re,n,k);
    task_prioritzing(tl_k,priority,priority_f,succ,w,C,t_re,n,k);
    execution_unit_selection(tl_k,priority_f,succ,C,rt_l,rt_c,rt_ws,ft_ws,ft_wr,ft_l,ft,core_t,exe_loc,n,k,t_s,t_r,t_c,curr_pos);
    for(int i=0; i<4; i++){
        for(int j=0; j<20; j++){
            if(exe_loc[j] == i){
                curr_pos[i].push_back(j);
            }
        }
    }
    float e_total = 0;
    for(int i=0; i<20; i++){
        if(exe_loc[i] == 3){
            e_total += E_c;
        }
        else{
            e_total += E_l[i][exe_loc[i]];
        }
    }
    int st[20];
    for(int i=0; i<20; i++){
        st[i] = max(rt_l[i],rt_ws[i]);
    }
    int tmin = ft[n-1];
    int tmax = 47;
    cout<<"Task scheduling result by the initial task scheduling algorithm:"<<endl;
    for(int i=0; i<curr_pos.size(); i++){
        if(i == 3){
            cout<<"Wireless Sending : ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]]<<"-"<<ft[curr_pos[i][j]]  - 2<<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
            cout<<endl;
            cout<<"Cloud: ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]] +3<<"-"<<ft[curr_pos[i][j]] -1 <<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
            cout<<endl;
            cout<<"Wireless Receiving : ";
            for(int j=0; j<curr_pos[i].size(); j++){
                cout<<st[curr_pos[i][j]] + 4<<"-"<<ft[curr_pos[i][j]] <<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
            cout<<endl;
        }
        else{
            cout<<"Core"<<i+1<<": ";
            for(int j=0; j<curr_pos[i].size(); j++){
            cout<<st[curr_pos[i][j]]<<"-"<<ft[curr_pos[i][j]]<<" for Task"<<curr_pos[i][j]+1<<"; ";
            }
        }
        cout<<endl;
    }
    cout<<"Energy Consumption: "<<e_total<<endl;
    cout<<"Total Completion Time: "<<tmin<<endl;
    cout<<"-----------------------------"<<endl;
    kernel(curr_pos,tl_k,succ,exe_loc,tmax,tmin,e_total,20,3,st,ft,E_c,E_l);
    return 0;
}
