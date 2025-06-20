// grammar.cpp
#include <algorithm> // find()
#include <cstdlib> // RAND_MAX 정의
#include <stdexcept> // domain_error,...
#include <ctime> // time()

#include <fstream> // 에런 (파일을 읽으면...)
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "split.h"

using namespace std;

typedef vector<string> Rule;
typedef vector<Rule> Rule_collection;
typedef map<string, Rule_collection> Grammar;

// 주어진 입력 스트림으로 문법 테이블 읽음
Grammar read_grammar(const string& filename) { // 수정
	Grammar ret;
	string line;

	ifstream grammar_file(filename); // 에런 추가

	if (!grammar_file) {
		cerr << "Error opening grammar file: " << filename << endl;
		throw runtime_error("Failed to open grammar file.");
	}

	// 입력 데이터를 읽음
	while (getline(grammar_file, line)) {
		// 입력 데이터를 단어로 나눔
		vector<string> entry = split(line);
		if (!entry.empty()) {
			// 카테고리 및 연관된 규칙을 맵에 저장
			ret[entry[0]].push_back(
				Rule(entry.begin() + 1, entry.end())
			);
		}
	}
	return ret;
}

// 꺾쇠호 있는지 확인
bool bracketed(const string& s) {
	// <....>? = true
	return s.size() > 1
		&& s[0] == '<' && s[s.size() - 1] == '>';
}

// [0,n) 범위에서 임의 정수 하나를 반환
int nrand(int n) {
	if (n <= 0 || n > RAND_MAX)
		throw domain_error("nrand is out of range!");

	const int bucket_size = RAND_MAX / n;
	int r;
	do r = rand() / bucket_size; // 무조건 한 번 이상 실행
	while (r >= n);

	return r;
}

// 문법 사용하고 문장 생성하기
void gen_aux(const Grammar& g, const string& word,
	vector<string>& ret) {

	if (!bracketed(word))
		ret.push_back(word); // 일반적인 단어

	else { // <...> 있는 단어 (문법 규칙)
		// word와 연관된 규칙을 찾음
		Grammar::const_iterator it = g.find(word);

		if (it == g.end()) // 벌써 끝나요???
			throw logic_error("Empty rule!");

		// 선택 가능한 규칙을 모아 놓은 백터를 가져옴
		const Rule_collection& c = it->second;

		// 임의로 하나를 선텍
		const Rule& r = c[nrand(c.size())];

		// 선택한 규칙을 재귀적으로 확장
		for (Rule::const_iterator i = r.begin();
			i != r.end(); ++i) {
			gen_aux(g, *i, ret); // 재귀적 호출
		}
	}
}

// 일반적으로 문장 생성하기
vector<string> gen_sent(const Grammar& g) {
	vector<string> ret;
	gen_aux(g, "<sentence>", ret);
	return ret;
}

// MAIN!!
int main() {
	// Seed the random number generator with current time
	srand(static_cast<unsigned int>(time(0)));

	// Ask user to select grammar file
	cout << "Choose a grammar file: \n";
	cout << "1. grammar.txt\n";
	cout << "2. grammar-long.txt\n";
	cout << "Enter 1 or 2: ";

	string choice;
	cin >> choice;

	string filename;
	if (choice == "1") {
		filename = "grammar.txt";
	}
	else if (choice == "2") {
		filename = "grammar-long.txt";
	}
	else {
		cout << "Invalid choice! Please enter 1 or 2.\n";
		return 1; // 프로그램 종료
	}

	// Load file
	Grammar grammar;
	try {
		grammar = read_grammar(filename);
	}
	catch (const runtime_error& e) {
		cerr << e.what() << endl;
		return 1;
	}

	// 문장 생성
	vector<string> sentence = gen_sent(grammar);

	// 첫 번째 단어 출력
	vector<string>::const_iterator it = sentence.begin();
	if (!sentence.empty()) {
		cout << *it;
		++it;
	}

	// 공백과 함께 나머지 단어 출력
	while (it != sentence.end()) {
		cout << " " << *it;
		++it;
	}

	cout << endl;

	return 0;
}