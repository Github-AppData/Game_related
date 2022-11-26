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

	// ��� �����ڿ����ε�
	friend ostream& operator <<(ostream& os, const Player& player);
};

Player s_player;
vector <Player> v_player;

// vector ��� ������ �����ε� ����
ostream& operator <<(ostream& os, const Player& s_player)
{
	os << "s_player.x: " << s_player.x << " s_player.y : " << s_player.y << " s_player.width : "
		<< s_player.width << " s_player.height: " << s_player.height << " s_player.plus_speed: " << s_player.plus_speed;

	return os;
}

void ShowAll(const vector<Player>& v_player)
{
	// ��¿����ڰ������ε�Ǿ��ֱ⶧���������ǳ�����
	// ������°�����

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

	// vector �ݺ��� iter�� v�� �������� ����Ŵ
	iter = v_player.begin();

	// iter - �������̴� - [] , *
	// 0 10 20 30 40 50 60 
	cout << " * iter : " << *iter << endl;


	iter = v_player.begin() + 1;
	v_player.clear(); // vector�� ��� �� ����

	// iter - �������̴� - [] , *
	// 0 10 20 30 40 50 60 
	cout << " * iter : " << *iter << endl;
	// ���� ����
	//cout << "iter[1] : " << iter[1] << endl;

	// 0 10 20 30 40 50 60 
	//iter += 2; // += ���� ���
	//cout << "* iter + 2 : " << *iter << endl;

	// �ݺ�
	/*for (iter = v.begin(); iter != v.end(); iter++) {
		cout << *iter << endl;
	}*/


	return 0;
}
