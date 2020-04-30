#include<iostream>
#include<time.h>
#include "node.h"

#define MAX_BOUND 512
#define RANDOMIZER 1234567
#define LENGTH 20
#define MULTIPLIER 7

using namespace std;

time_t timer;

int rnd(int seedValue)
{
    return (RANDOMIZER % (seedValue * MULTIPLIER)) % MAX_BOUND;
}

int rnd(int seedValue, int bound)
{
    return (seedValue ^ RANDOMIZER) % bound;
}

///B-arbori Cautare, inserare, stergere, succesor, predecesor, afisare elemente sortat

class BTree
{
    private:
        BTreeNode *head;
        int degree; /// ordinul arborelui este 2*t - 1

    public:

        BTree(int degreeValue)
        {
            head = NULL;
            degree = degreeValue;
        }

        void traverse()
        {
            if (head != NULL) head->traverse();
        }

        /// se cauta in arbore o cheie data
        BTreeNode* searchKey(int k)
        {
            return (head == NULL)? NULL : head->search(k);
        }

        /// introducerea unei noi chei in B-Tree
        void insertKey(int k);

        /// scoaterea unei noi chei din B-Tree
        void removeKey(int k);

};

void BTree::insertKey(int k)
{
    /// daca rootul e gol
    if (head == NULL)
    {
        /// construirea rootului
        head = new BTreeNode(degree, true);
        head->values[0] = k;  /// inseram cheia
        head->n = 1;  /// updatam valoarea in nod
    }
    else
    {
        /// nodul a ajuns la capacitate maxima
        if (head->n == 2*degree-1)
        {
            /// construim un nou root
            BTreeNode *s = new BTreeNode(degree, false);

            /// transformam vechiul root intr-un child
            s->C[0] = head;

            /// spargem nodul plin in
            s->splitChild(0, head);

            /// decidem in care copil al nodului introducem noua cheie
            int i = 0;
            if (s->values[0] < k)
                i++;
            s->C[i]->insertNonFull(k);

            ///schimbam capul
            head = s;
        }
        else  ///chemam functia pentru a introduce cheia in nodul incomplet
            head->insertNonFull(k);
    }
}

void BTree::removeKey(int k)
{
    if (!head)
    {
        cout << "Arborele este gol\n";
        return;
    }

    head->remove(k);

    /// daca capul are 0 valori, marcheaza primul copil ca head
    /// altfel fa-l NULL
    if (head->n==0)
    {
        BTreeNode *tmp = head;
        if (head->child)
            head = NULL;
        else
            head = head->C[0];

        delete tmp;
    }
    return;
}

int main()
{
    BTree t(3);

    int *addedNumber = new int[LENGTH];
    int value = rnd(char(time(&timer)));

    /// se construieste arborele si vectorul de valori
    for(int i = 0; i < LENGTH; i++)
    {
        value = rnd(value + i);
        t.insertKey(value);
        addedNumber[i] = value;
    }

    ///se afiseaza vectorul de valori
    cout << "Lista numerelor adaugate este:\n";
    for(int i = 0; i < LENGTH; i++)
        cout<< addedNumber[i] << " ";
    cout << "\n\n\n";

    ///se afiseaza arborele
    cout << "Numerele sortate sunt:\n";
    t.traverse();
    cout << "\n";

    ///se sterg valorile din arbore
    for(int i = 0; i < LENGTH; i++)
    {
        cout << "\n\n";
        t.removeKey(addedNumber[i]);
        cout << "Am sters "<< addedNumber[i] << " :\n";
        t.traverse();
    }

    delete addedNumber;
    return 0;
}
