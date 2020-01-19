#include "pch.h"

void sysinit() {
	srand((unsigned)time(NULL));
	initgraph(CANVAS_LENGTH, CANVAS_WIDTH, NOMINIMIZE);
	setbkmode(TRANSPARENT);
	HWND hWnd = GetHWnd();
	SetWindowText(hWnd, _T("�������"));
	cleardevice();
}

int main() {
	sysinit();
	IMAGE bk, bk2;
	MOUSEMSG m;
	deque<obb> o;
	deque<web> w;
	score s;
	fish_collect fc[KIND+1];
	Cannon c;
	DWORD start, stop;
	int game_state = 0, time = 0, numtmp;
	bool is_grass, is_bomb;
	RECT r;
	while (1) {
		switch (game_state) {
		case 0://��ʼ����
			s.init();
			for (int i = 1; i <= KIND; i++) fc[i].init(i);
			c.clear();
			o.clear();
			PlaySound(_T("wave\\bgm1.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			while (game_state == 0) {
				BeginBatchDraw();
				loadimage(&bk, _T("img\\background2.jpg"), CANVAS_LENGTH, CANVAS_WIDTH);
				putimage(0, 0, &bk);
				setheadline(CANVAS_LENGTH / 2 - 80, CANVAS_WIDTH / 2 - 60, 40, _T("�������"));
				createbutton(CANVAS_LENGTH / 2 - 25, CANVAS_LENGTH / 2 + 25, CANVAS_WIDTH / 2 + 40, CANVAS_WIDTH / 2 + 60, 10, _T("��ʼ��Ϸ"));
				createbutton(CANVAS_LENGTH / 2 - 25, CANVAS_LENGTH / 2 + 25, CANVAS_WIDTH / 2 + 70, CANVAS_WIDTH / 2 + 90, 10, _T("�˳���Ϸ"));
				createbutton(CANVAS_LENGTH / 2 - 25, CANVAS_LENGTH / 2 + 25, CANVAS_WIDTH / 2 + 100, CANVAS_WIDTH / 2 + 120, 10, _T("��Ϸ����"));
				EndBatchDraw();
				if (MouseHit()) {
					m = GetMouseMsg();
					if (m.uMsg == WM_LBUTTONDOWN && m.x >= CANVAS_LENGTH / 2 - 25 && m.x <= CANVAS_LENGTH / 2 + 25 && m.y >= CANVAS_WIDTH / 2 + 40 && m.y <= CANVAS_WIDTH / 2 + 60) {
						PlaySound(NULL, NULL, SND_FILENAME);
						PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
						c.init();
						game_state = 1;
					}
					if (m.uMsg == WM_LBUTTONDOWN && m.x >= CANVAS_LENGTH / 2 - 25 && m.x <= CANVAS_LENGTH / 2 + 25 && m.y >= CANVAS_WIDTH / 2 + 100 && m.y <= CANVAS_WIDTH / 2 + 120) {
						PlaySound(NULL, NULL, SND_FILENAME);
						PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
						game_state = 4;
					}
					if (m.uMsg == WM_LBUTTONDOWN && m.x >= CANVAS_LENGTH / 2 - 25 && m.x <= CANVAS_LENGTH / 2 + 25 && m.y >= CANVAS_WIDTH / 2 + 70 && m.y <= CANVAS_WIDTH / 2 + 90) {
						PlaySound(NULL, NULL, SND_FILENAME);
						PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
						exit(0);
					}
				}
			}
			break;
		case 1:
			loadimage(&bk2, _T("img\\background.jpg"), CANVAS_LENGTH, CANVAS_WIDTH);
			PlaySound(_T("wave\\bgm.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			s.numtmp = 0;
			while (1) {
				BeginBatchDraw();
				start = GetTickCount();
				putimage(0, 0, &bk2);
				if (MouseHit()) {
					m = GetMouseMsg();
					if (m.uMsg == WM_LBUTTONDOWN) {
						if (m.x >= 18 && m.x <= 62 && m.y >= 18 && m.y <= 32) game_state = 0;
						if (m.x >= 18 && m.x <= 62 && m.y >= 38 && m.y <= 52) game_state = 2;
						if (m.x >= 20 && m.x <= 60 && m.y >= 460 && m.y <= 470) game_state = 3;
						if (game_state != 1) {
							PlaySound(NULL, NULL, SND_FILENAME);
							PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
							EndBatchDraw();
							break;
						}
						if (!c.update(m)) {
							c.fire();
							s.numtmp = -c.cost;
							s.num += s.numtmp;
							s.show();
						}
						//PlaySound(_T("wave\\fire.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_NOSTOP);
					}
					c.rotate(m);
				}
				if (time == 12) {
					fish ftmp;
					obb otmp;
					otmp.f = ftmp;
					otmp.init();
					o.emplace_back(otmp);
					time = 0;
				}
				if (!c.bullets.empty())
					for (auto i = c.bullets.begin(); i != c.bullets.end();) {
						auto j = o.begin();
						while (j != o.end()) {
							if (in_obb(*j, *i)) {
								web wtmp;
								wtmp.create(*i);
								w.emplace_back(wtmp);
								break;
							}
							else j++;
						}
						if (j != o.end()) i = c.bullets.erase(i);
						else {
							if (!(*i).in_canvas()) i = c.bullets.erase(i);
							else {
								(*i).move();
								i++;
							}
						}
					}
				if (!o.empty())
					for (auto i = o.begin(); i != o.end();) {
						if (!(*i).f.in_canvas()) {
							i = o.erase(i);
						}
						else {
							(*i).f.move();
							(*i).update();
							i++;
						}
					}
				if (!w.empty())
					for (auto i = w.begin(); i != w.end(); i++) {
						if (!(*i).is_show) {
							is_grass = false;
							is_bomb = false;
							numtmp = 0;
							for (auto j = o.begin(); j != o.end();) {
								if (collision(*j, *i)) {
									(*j).is_collision = true;
									if ((*j).f.kind == 7) {
										is_bomb = true;
										j = o.erase(j);
									}
									else if ((*j).f.kind == 6) {
										is_grass = true;
										j = o.erase(j);
									}
									else j++;
								}
								else j++;
							}
							if (is_bomb==true) {
								(*i).is_bomb = true;
								for (auto j = o.begin(); j != o.end();) {
									if ((*j).is_collision==true) {
										(*j).f.dead(fc);
										numtmp += (*j).f.value;
										j = o.erase(j);
									}
									else j++;
								}
							}
							else if (is_grass==true) {
								for (auto j = o.begin(); j != o.end();) {
									if ((*j).is_collision==true) j = o.erase(j);
									else j++;
								}
								numtmp = -20;
							}
							else {
								for (auto j = o.begin(); j != o.end();) {
									if ((*j).is_collision==true){
										if ((*j).f.is_dead(c.cost)) {
											(*j).f.dead(fc);
											numtmp += (*j).f.value;
											j = o.erase(j);
										}
										else {
											(*j).is_collision = false;
											j++;
										}
									}
									else j++;
								}
							}
							(*i).is_show = true;
							(*i).time = 0;
							(*i).g_vx = (590 - (*i).center.x) / 15;
							(*i).g_vy = ((*i).center.y - 50) / 15;
							s.numtmp = numtmp;
							s.num += s.numtmp;
							if (s.numtmp > 0) (*i).is_gold = true;
						}
					}
				if (!o.empty())
					for (auto i = o.begin(); i != o.end(); i++) {
						(*i).f.show();
						//(*i).show();
					}
				if (!w.empty())
					for (auto i = w.begin(); i != w.end();) {
						if ((*i).is_show) {
							if ((*i).time > 15) {
								i = w.erase(i);
							}
							else {
								(*i).show();
								(*i).time++;
								i++;
							}
						}
						else i++;
					}
				c.show_bullets();
				createbutton(18, 62, 18, 32, 10, _T("�����˵�"));
				createbutton(18, 62, 38, 52, 10, _T("��ͣ��Ϸ"));
				createbutton(0, 640, 430, 480, 20, _T(""));
				createbutton(20, 60, 460, 470, 10, _T("�ɾ�ϵͳ"));
				c.show();
				s.show();
				if (s.num < 0) {
					game_state = 5;
					break;
				}
				stop = GetTickCount();
				EndBatchDraw();
				time++;
				if (stop - start < 30) Sleep(30 - (stop - start));
			}
			break;
		case 2://��Ϸ��ͣ
			PlaySound(NULL, NULL, SND_FILENAME);
			BeginBatchDraw();
			createbutton(18, 62, 18, 32, 10, _T("�����˵�"));
			createbutton(18, 62, 38, 52, 10, _T("������Ϸ"));
			EndBatchDraw();
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN && m.x >= 18 && m.x <= 62 && m.y >= 38 && m.y <= 52) {
				PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
				game_state = 1;
			}
			if (m.uMsg == WM_LBUTTONDOWN && m.x >= 18 && m.x <= 62 && m.y >= 18 && m.y <= 32) {
				PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
				game_state = 0;
			}
			break;
		case 3://�ɾ�ϵͳ
			BeginBatchDraw();
			createbutton(40, 600, 50, 430, 20, _T(""));
			createbutton(100, 540, 50, 80, 20, _T("�ɾ�ϵͳ"));
			createbutton(520, 540, 50, 60, 10, _T("����"));
			createbutton(40, 600, 305, 320, 15, _T("ʮȫʮ��������һ���������������ʮ��ʱ��������������һ����ʱ�����������"));
			createbutton(50, 590, 330, 345, 15, _T("ˮ����ʧ��������ˮ��ʮ��ʱ����ˮ�ݣ�ʱ�����������"));
			createbutton(50, 590, 355, 370, 15, _T("ŷ�ʸ��塪������ը�����ʱ����������������һ����ʱ�����������"));
			for (int i = 1; i <=KIND; i++) fc[i].show();
			EndBatchDraw();
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN && m.x >= 520 && m.x <= 540 && m.y >= 50 && m.y <= 60) {
				PlaySound(NULL, NULL, SND_FILENAME);
				PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
				game_state = 1;
			}
			break;
		case 4://��Ϸ����
			BeginBatchDraw();
			putimage(0, 0, &bk);
			createbutton(100, 540, 50, 430, 20, _T(""));
			createbutton(100, 540, 50, 80, 20, _T("��Ϸ����"));
			createbutton(520, 540, 50, 60, 10, _T("����"));
			r = { 110, 90, 530, 540 };
			settextstyle(15, 0, _T("����"));
			drawtext(_T("���洫ͳ�Ĳ������һ��ȥ�������Ϸ\nENJOY YOURSELF:)\n\nTIPS:\n����ˮ��  -20\n����ը��  ��������ȫ���ջ�"), &r, DT_LEFT | DT_WORDBREAK);
			EndBatchDraw();
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN && m.x >= 520 && m.x <= 540 && m.y >= 50 && m.y <= 60) game_state = 0;
			break;
		case 5://��Ϸ����
			PlaySound(_T("wave\\end.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			r = { 0,190,640,250 };
			cleardevice();
			setbkcolor(BLACK);
			settextstyle(50, 0, _T("����"));
			settextcolor(YELLOW);
			while (game_state == 5) {
				BeginBatchDraw();
				drawtext(_T("GAME OVER"), &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				createbutton(295, 345, 300, 310, 10, _T("���¿�ʼ"));
				createbutton(295, 345, 320, 330, 10, _T("�˳���Ϸ"));
				EndBatchDraw();
				m = GetMouseMsg();
				if (m.uMsg == WM_LBUTTONDOWN && m.x >= 295 && m.x <= 345 && m.y >= 300 && m.y <= 310) {
					PlaySound(NULL, NULL, SND_FILENAME);
					PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
					game_state = 0;
				}
				else if (m.uMsg == WM_LBUTTONDOWN && m.x >= 295 && m.x <= 345 && m.y >= 320 && m.y <= 330) {
					PlaySound(NULL, NULL, SND_FILENAME);
					PlaySound(_T("wave\\menu.wav"), NULL, SND_FILENAME | SND_SYNC);
					exit(0);
				}
			}
			break;
		}
	}
}