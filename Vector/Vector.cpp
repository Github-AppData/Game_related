#include <iostream>
#include <vector>
#define TRUE 1
#define FALSE 0

// vector - FILO
using namespace std;
struct Player
{
    string name = "";
    int y = 0;
    int width = 0;
    int height = 0;
    int plus_speed = 0;

    // ��¿����ڿ����ε�. ���̷������ʿ�¾��µ�
    // �̹����������
    friend ostream& operator <<(ostream& os, const Player& player);
};
void ShowAll(const vector<Player>& v);

Player s_player;
vector<Player> v_player;

//player.x = 20;

int main()
{
        s_player.name = "����";
        s_player.y = 111;
        s_player.width = 200;
        s_player.height = 222;
        s_player.plus_speed = 100;

        v_player.push_back(s_player);

    printf("v_player.size : ");
    cout << v_player.size();

    printf("\n");
    
    ShowAll(v_player);
    printf("v_player.clear();\n");
    v_player.clear();
    ShowAll(v_player);

    // false = 0 / true = 1�� 
    /*printf("\nv_player.empty : ");
    if (v_player.empty() != TRUE)
    {
        cout << "FALSE \n";
    }

    ShowAll(v_player);*/
	return 0;
}

void ShowAll(const vector<Player>& v)
{
    // ��¿����ڰ������ε�Ǿ��ֱ⶧���������ǳ�����
    // ������°�����

    copy(v.begin(), v.end(), ostream_iterator<Player>(cout, "\n"));
}

ostream& operator <<(ostream& os, const Player& score)
{

    os << "x: " << s_player.name << " y : " << s_player.y << " width : "
        << s_player.width << " height: " << s_player.height << " plus_speed: " << s_player.plus_speed;

    return os;

}
