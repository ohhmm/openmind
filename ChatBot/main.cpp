#include <OpenMind/trie.h>

int main() {
    Trie<int> t;
    const wchar_t* t1 = L"Some Text1 and some ending.";
    const wchar_t* t2 = L"Some Text2 and some ending.";
    t.AddToRoot(t1);
    t.AddToRoot(t2);

}
