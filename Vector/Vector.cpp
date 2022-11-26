#include <iostream>
#include <vector>

using namespace std;


struct Player
{
	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;
	int plus_speed = 0;

	// 출력 연산자오버로드
	friend ostream& operator <<(ostream& os, const Player& player);
};

Player s_player;
vector <Player> v_player;

// vector 출력 연산자 오버로드 선언
ostream& operator <<(ostream& os, const Player& s_player)
{
	os << "s_player.x: " << s_player.x << " s_player.y : " << s_player.y << " s_player.width : "
		<< s_player.width << " s_player.height: " << s_player.height << " s_player.plus_speed: " << s_player.plus_speed;

	return os;
}

void ShowAll(const vector<Player>& v_player)
{
	// 출력연산자가오버로드되어있기때문에벡터의내용을
	// 쉽게출력가능함

	copy(v_player.begin(), v_player.end(), ostream_iterator<Player>(cout, "\n"));
}

int main(void)
{
	vector <Player> ::iterator iter;

		// 1
		s_player.x = 111;
		s_player.y = 111;
		s_player.height = 111;
		s_player.plus_speed = 111;

	v_player.push_back(s_player);
	//v_player.insert(2, 3);

	// vector 반복자 iter는 v의 시작점을 가리킴
	iter = v_player.begin();

	// iter - 포인터이다 - [] , *
	// 0 10 20 30 40 50 60 
	cout << " * iter : " << *iter << endl;


	iter = v_player.begin() + 1;
	v_player.clear(); // vector의 모든 것 삭제

	// iter - 포인터이다 - [] , *
	// 0 10 20 30 40 50 60 
	cout << " * iter : " << *iter << endl;
	// 임의 접근
	//cout << "iter[1] : " << iter[1] << endl;

	// 0 10 20 30 40 50 60 
	//iter += 2; // += 연산 사용
	//cout << "* iter + 2 : " << *iter << endl;

	// 반복
	/*for (iter = v.begin(); iter != v.end(); iter++) {
		cout << *iter << endl;
	}*/


	return 0;
}
