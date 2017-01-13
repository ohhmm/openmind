#include <OpenMind/trie.h>
#include <cstdlib>
#include <sstream>
#include <ctime>
#include <iostream>
#include <map>


namespace {
	const int N = 10000000;
}
Trie<int> t;

void ShowTime(std::function<void()> fn){
    const clock_t begin_time = clock();
    fn();
    std::cout   << float( clock () - begin_time ) /  CLOCKS_PER_SEC
                << " sec" << std::endl;
}

int main() {
    const wchar_t* t1 = L"Some Text1 and some ending.";
    const wchar_t* t2 = L"Some Text2 and some ending.";
    t.AddToRoot(t1);
    t.AddToRoot(t2);
    
	
    static std::wstring s(t1);
	{
		std::wcout << "Insert into trie: ";
		ShowTime([]() {
			for (int i = N; i--;) {
				std::wstringstream ss;
				ss << s << std::to_wstring(i);
				t.AddToRoot(ss.str().c_str(), decltype(t)::obj_ptr_t(new int(i)));
			}
		});
		std::wcout << "Accessing the trie: ";
		ShowTime([&]() {
			int sum = 0;
			for (int i = N; i--;) {
				std::wstringstream ss;
				ss << s << std::to_wstring(i);
				auto res = t[ss.str().c_str()];
				sum += *res;
			}
		});
	}

	{
		std::map<std::wstring, int> m;
		std::wcout << "Insert into map: ";
		ShowTime([&]() {
			for (int i = N; i--;) {
				std::wstringstream ss;
				ss << s << std::to_wstring(i);
				m[ss.str()] = i;
			}
		});

		std::wcout << "Accessing the map: ";
		ShowTime([&]() {
			int sum = 0;
			for (int i = N; i--;) {
				std::wstringstream ss;
				ss << s << std::to_wstring(i);
				sum += m[ss.str()];
			}
		});
	}

	std::wcin.get();
}
