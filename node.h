#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

using namespace std;

class BTreeNode
{
    int *values;    /// un vector de chei
    int t;          /// numarul minim de valori
    BTreeNode **C;  /// vectori de copii
    int n;          /// numarul de chei
    bool child;     /// este un copil sau nu

public:

    BTreeNode(int _t, bool _child);

    void insertNonFull(int k);              /// inserarea unui chei cand nu s-a ajuns la ordinul maxim

    void traverse();                        /// parcurgerea grafului

    void splitChild(int i, BTreeNode *y);   /// functie de split in functie de nod si indexul cheii (se cheama cand nodul e plin)

    void remove(int k);                     /// wrapper function pentru remove
    void removeFromchild(int info);         /// stergerea unei chei intr-un nod copil
    void removeFromNonchild(int info);      /// stergerea unei chei intr-un nod parinte

    void fill(int info);                    /// umplerea vectorului de copii in al n lea element daca are mai putin decat gradul minim

    void borrowFromPrev(int info);          /// preluarea nodurilor din copil si adaugarea lor in parinte
    void borrowFromNext(int info);          /// preluarea nodurilor din parinte si adaugarea lor in parinte

    void merge(int info);                   /// merge-uim parintele si copilul

    BTreeNode *search(int k);               /// returneaza NULL daca k nu e gasit

    int getPred(int info);                  /// preluarea predecesorului al elementului n
    int getSucc(int info);                  /// preluarea succesorului al elementului n

    int findKey(int k);                     /// returneaza indexul unei chei mai mari sau egale cu k

    friend class BTree;                     /// accesarea membrilor privati
};

BTreeNode::BTreeNode(int t1, bool child1)
{
    ///initializam nodul
    t = t1;
    child = child1;

    values = new int[2*t-1];
    C = new BTreeNode *[2*t];

    n = 0;
}

int BTreeNode::findKey(int k)
{
    int i = 0;
    while (i < n && values[i] < k) ++i;
    return i;
}

void BTreeNode::remove(int k)
{
    int i = findKey(k);             /// gasim pozitia cheii k in nodul curent

    if (i < n && values[i] == k)
    {
        if (child)                  ///eliminam nodul
            removeFromchild(i);
        else
            removeFromNonchild(i);
    }
    else
    {
        if (child)                  ///printare daca nodul e un copil si nu exista cheia
        {
            cout << "Cheia "<< k <<" nu exista\n";
            return;
        }


        bool flag = false;          /// se indica daca cheia de sters se gaseste in sub-tree-ul acestui nod (al ultimei chei)

        if (i == n)
            flag = true;


        if (C[i]->n < t)            ///umplem nodul-copil daca are spatiu
            fill(i);



        if (flag && i > n)
            C[i - 1]->remove(k);    ///daca ultimul nod a merge-uit, atunci apelam recursia pe copilul i-1
        else
            C[i]->remove(k);        ///altfel, mergem pe copilul i
    }
}

void BTreeNode::removeFromchild (int val)
{

    for (int i = val + 1; i < n; ++i)
        values[i-1] = values[i];        ///stergem elementul, mutand toate elementele cu un spatiu

    n--;
}


void BTreeNode::removeFromNonchild(int i)
{

    int k = values[i];

    if (C[i]->n >= t) ///daca nodul copil are minimul de copii, atunci cautam predecesorul lui k si il stergem recursiv
    {
        int pred = getPred(i);
        values[i] = pred;
        C[i]->remove(pred);
    }
    else if  (C[i + 1]->n >= t) ///daca urmatorul nod-copil are mai mult de gradul minim de valori. cautam succesorul lui k si il stergem recursiv
    {
        int succ = getSucc(i);
        values[i] = succ;
        C[i + 1]->remove(succ);
    }

    else ///merge in nodul-copil i+1, delete k recursiv
    {
        merge(i);
        C[i]->remove(k);
    }
}

int BTreeNode::getPred(int info)
{
    BTreeNode *cur=C[info];
    while (!cur->child)
        cur = cur->C[cur->n];

    return cur->values[cur->n-1]; ///ultima cheie a copilului
}

int BTreeNode::getSucc(int info)
{

    BTreeNode *cur = C[info+1];
    while (!cur->child)
        cur = cur->C[0];

    return cur->values[0]; ///returneaza prima cheie a copilului
}

void BTreeNode::fill(int info)
{

    ///daca copilul ant are mai mult de t copii, atunci preia un copil
    if (info!=0 && C[info-1]->n>=t)
        borrowFromPrev(info);

    ///daca copilul urm are mai mult de t copii, atunci preia un copil
    else if (info!=n && C[info+1]->n>=t)
        borrowFromNext(info);

    ///merge-uieste nodul
    else
    {
        if (info != n)
            merge(info);
        else
            merge(info-1);
    }
    return;
}

void BTreeNode::borrowFromPrev(int info)
{

    BTreeNode *child=C[info];
    BTreeNode *sibling=C[info-1];

    for (int i=child->n-1; i>=0; --i)
        child->values[i+1] = child->values[i];

    if (!child->child)
    {
        for(int i=child->n; i>=0; --i)
            child->C[i+1] = child->C[i];
    }

    child->values[0] = values[info-1];

    if(!child->child)
        child->C[0] = sibling->C[sibling->n];

    values[info-1] = sibling->values[sibling->n-1];

    child->n += 1;
    sibling->n -= 1;

    return;
}

void BTreeNode::borrowFromNext(int info)
{

    BTreeNode *child=C[info];
    BTreeNode *sibling=C[info+1];

    /// introducem in nodul-copil
    child->values[(child->n)] = values[info];

    if (!(child->child))
        child->C[(child->n)+1] = sibling->C[0];

    values[info] = sibling->values[0];

    /// mutarea valorilor cu o pozitie in spate
    for (int i=1; i<sibling->n; ++i)
        sibling->values[i-1] = sibling->values[i];

    /// mutarea pointerilor in fata
    if (!sibling->child)
    {
        for(int i=1; i<=sibling->n; ++i)
            sibling->C[i-1] = sibling->C[i];
    }

    child->n += 1;
    sibling->n -= 1;

    return;
}


void BTreeNode::merge(int info)
{
    BTreeNode *child = C[info];
    BTreeNode *sibling = C[info+1];

    /// luam o valoare din nodul curent
    child->values[t-1] = values[info];

    for (int i=0; i<sibling->n; ++i)
        child->values[i+t] = sibling->values[i];

    ///copiam pointerii
    if (!child->child)
    {
        for(int i=0; i<=sibling->n; ++i)
            child->C[i+t] = sibling->C[i];
    }

    for (int i=info+1; i<n; ++i)
        values[i-1] = values[i];

    for (int i=info+2; i<=n; ++i)
        C[i-1] = C[i];

    ///updatam numarul de copii
    child->n += sibling->n+1;
    n--;

    delete(sibling);
    return;
}


BTreeNode *BTreeNode::search(int k)
{

    int i = 0;
    while (i < n && k > values[i])
        i++;

    if (values[i] == k)
        return this;

    if (child == true)
        return NULL;

    return C[i]->search(k);
}

void BTreeNode::traverse()
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (child == false)
            C[i]->traverse();
        cout << values[i] <<" ";
    }

    if (child == false)
        C[i]->traverse();
}

void BTreeNode::splitChild(int i, BTreeNode *y)
{
    /// facem un nod in care sa pastram cele t-1 valori din nodul y
    BTreeNode *z = new BTreeNode(y->t, y->child);
    z->n = t - 1;

    for (int j = 0; j < t-1; j++)
        z->values[j] = y->values[j+t];

    if (y->child == false)
    {
        for (int j = 0; j < t; j++)
            z->C[j] = y->C[j+t];
    }

    /// reducem gradul nodului y
    y->n = t - 1;

    /// facem spatiu pentru noul nod-copil
    for (int j = n; j >= i+1; j--)
        C[j+1] = C[j];

    /// legam noul copil
    C[i+1] = z;

    /// facem spatiu pentru mutarea unei chei a lui y
    for (int j = n-1; j >= i; j--)
        values[j+1] = values[j];

    /// copiam cheia din mijlocul lui y in acest nod
    values[i] = y->values[t-1];

    /// crestem gradul nodului curent
    n = n + 1;
}

void BTreeNode::insertNonFull(int k)
{
    /// consideram index-ul ca fiind cel mai din dreapta
    int i = n-1;

    if (child == true)
    {
        /// gaseste locul unde cheia poate fi introdusa
        /// muta toate valorile mai mari in fata
        while (i >= 0 && values[i] > k)
        {
            values[i+1] = values[i];
            i--;
        }

        values[i+1] = k;
        n = n+1;
    }
    else
    {
        /// gaseste locul unde cheia poate fi introdusa
        while (i >= 0 && values[i] > k)
            i--;

        /// verifica daca e plin
        if (C[i+1]->n == 2*t-1)
        {
            splitChild(i+1, C[i+1]);

            ///spargem in doua, vedem care va avea cheia cea noua
            if (values[i+1] < k)
                i++;
        }
        C[i+1]->insertNonFull(k);
    }
}

#endif // NODE_H_INCLUDED
