#ifndef ALU_H
#define ALU_H

#include <bits/stdc++.h>
using namespace std;

// Global flags
extern bool zero_flag;
extern bool carry_flag;

// ci is initial carry
vector<bool> full_adder(bool a, bool b, bool ci);

bool comp(bool input);

vector<bool> full_sub(bool a, bool b, bool bi);

vector<bool> ADD(vector<bool> &ACC, vector<bool> OP);

vector<bool> SUB(vector<bool> &ACC, vector<bool> OP);

vector<bool> MUL(vector<bool> &ACC, vector<bool> OP);

pair<vector<bool>, vector<bool>> DIV(vector<bool> dividend, vector<bool> divisor);

// Implementation (inline functions for header file)

inline vector<bool> full_adder(bool a, bool b, bool ci)
{
    bool sum = (a ^ b ^ ci);
    bool carry_final = ((a & b) | (b & ci) | (ci & a));
    return {sum, carry_final};
}

inline bool comp(bool input)
{
    if (input)
    {
        return false;
    }
    else
    {
        return true;
    }
}

inline vector<bool> full_sub(bool a, bool b, bool bi)
{
    bool diff = (a ^ b ^ bi);
    bool b_next = ((comp(a) & b) | (comp(a) & bi) | (b & bi));
    return {diff, b_next};
}

inline vector<bool> ADD(vector<bool> &ACC, vector<bool> OP)
{
    int ec = 0; // ec ----> extra carry
    vector<bool> res(4, 0);
    for (int i = 3; i >= 0; i--)
    {
        vector<bool> v1 = full_adder(ACC[i], OP[i], ec);
        res[i] = v1[0];
        ec = v1[1];
    }
    zero_flag = ec;
    return res;
}

inline vector<bool> SUB(vector<bool> &ACC, vector<bool> OP)
{
    int eb = 0; // eb ----> extra borrow
    vector<bool> res(4, 0);
    for (int i = 3; i >= 0; i--)
    {
        vector<bool> v1 = full_sub(ACC[i], OP[i], eb);
        res[i] = v1[0];
        eb = v1[1];
    }
    return res;
}

inline vector<bool> MUL(vector<bool> &ACC, vector<bool> OP)
{
    vector<bool> M = ACC; // Multiplicand
    vector<bool> Q = OP;  // Multiplier
    vector<bool> A(4, 0); // Accumulator
    bool Q1 = 0;

    for (int step = 0; step < 4; step++)
    {
        if (Q[3] == 0 && Q1 == 1)
        {
            A = ADD(A, M);
        }
        else if (Q[3] == 1 && Q1 == 0)
        {
            A = SUB(A, M);
        }

        // Arithmetic Right Shift as per the algorithm
        bool sign = A[0];
        Q1 = Q[3];

        for (int i = 3; i > 0; i--)
            Q[i] = Q[i - 1];
        Q[0] = A[3];

        for (int i = 3; i > 0; i--)
            A[i] = A[i - 1];
        A[0] = sign;
    }

    // Combine A and Q -> 8 bit result
    vector<bool> product(8);
    for (int i = 0; i < 4; i++)
        product[i] = A[i];
    for (int i = 0; i < 4; i++)
        product[i + 4] = Q[i];

    return product;
}

inline pair<vector<bool>, vector<bool>> DIV(vector<bool> dividend, vector<bool> divisor)
{
    vector<bool> A(4, 0);
    vector<bool> Q = dividend;
    vector<bool> M = divisor;

    // Handle sign
    bool signQ = Q[0] ^ M[0];
    bool signR = Q[0];

    if (Q[0])
        Q = SUB(vector<bool>(4, 0), Q);
    if (M[0])
        M = SUB(vector<bool>(4, 0), M);

    for (int step = 0; step < 4; step++)
    {
        // Left shift [A,Q]
        for (int i = 0; i < 3; i++)
            A[i] = A[i + 1];
        A[3] = Q[0];
        for (int i = 0; i < 3; i++)
            Q[i] = Q[i + 1];
        Q[3] = 0;

        // A = A - M
        A = SUB(A, M);

        if (A[0] == 1)
        { // Negative
            Q[3] = 0;
            A = ADD(A, M); // Restore
        }
        else
        {
            Q[3] = 1;
        }
    }

    if (signQ)
        Q = SUB(vector<bool>(4, 0), Q);
    if (signR)
        A = SUB(vector<bool>(4, 0), A);

    return {Q, A};
}

#endif // ALU_H
