#include<bits/stdc++.h>
using namespace std;

bool zero_flag=0;
bool carry_flag=0;

//ci is initial carry
vector<bool>full_adder(bool a, bool b, bool ci){
    bool sum=(a^b^ci);
    bool carry_final=((a&b)|(b&ci)|(ci&a));
    return {sum, carry_final};
}

bool comp(bool input)
{
    if(input)
    {
        return false;
    }
    else{
        return true;
    }
}

vector<bool> full_sub(bool a,bool b, bool bi){
    bool diff = (a^b^ci);

    bool b_next =((comp(a)&b)|(comp(a)&bi)|(b&bi));
    return {diff, b_next}; 

}

vector<bool>ADD(vector<bool>&ACC, vector<bool>OP){
    int ec=0;//ec stands for extra carry
    vector<bool>res(4,0)
    for(int i=3; i>=0;i--){
        vector<bool>v1=full_adder(ACC[i], OP[i], ec);
        res[i]=v1[0];
        ec=v1[1];
    }
    zero_flag=ec;
    return res;
}

vector<bool>SUB(vector<bool>&ACC, vector<bool>OP){
    int eb=0;//eb stands for extra borrow
    vector<bool>res(4,0)
    for(int i=3; i>=0;i--){
        vector<bool>v1=full_sub(ACC[i], OP[i], eb);
        res[i]=v1[0];
        eb=v1[1];
    }
    return res;
}

vector<bool> MUL(vector<bool>&ACC)