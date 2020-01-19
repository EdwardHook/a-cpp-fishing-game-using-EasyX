﻿#include "pch.h"

fish::fish() {
	int random = rand() % 100;
	if (random <= 25) kind = 1;
	else if (random <= 45) kind = 2;
	else if (random <= 60) kind = 3;
	else if (random <= 75) kind = 4;
	else if (random <= 85) kind = 5;
	else if (random <= 95) kind = 7;
	else kind = 6;
	init();
	emerge();
}

bool fish::in_canvas() {
	return x >= -nowlength&&x <= CANVAS_LENGTH+nowlength && y >= -nowwidth&&y <= CANVAS_WIDTH+nowwidth;
}

void fish::init() {
	switch (kind) {
	case 1: 
		v = 1;
		value = 1;
		loadAlpha(&img, _T("img\\fish1.png"), 0,0,true);
		break;
	case 2: 
		v = 2;
		value = 2;
		loadAlpha(&img, _T("img\\fish2.png"), 0, 0, true);
		break;
	case 3: 
		v = 3;
		value = 4;
		loadAlpha(&img, _T("img\\fish3.png"), 0, 0, true);
		break;
	case 4:
		v = 3;
		value = 8;
		loadAlpha(&img, _T("img\\fish4.png"), 0, 0, true);
		break;
	case 5:
		v = 2;
		value = 25;
		loadAlpha(&img, _T("img\\fish5.png"), 0, 0, true);
		break;
	case 6://grass
		v = 2;
		value = 0;
		loadAlpha(&img, _T("img\\grass.png"), 0, 0, true);
		break;
	case 7:
		v = 1;
		value = 0;
		loadAlpha(&img, _T("img\\bomb.png"), 0, 0, true);
		break;
	}
	nowlength=(length = img.getwidth());
	nowwidth=(width = img.getheight());
	state = 1;
}

void fish::emerge() {
	int situation = rand() % 4;//appear on different sides of background
	angle = rand() % 90;
	switch (situation) {
	case 0:
		angle+= 225;
		break;
	case 1:
		angle+=135;
		break;
	case 2:
		angle+=45;
		break;
	case 3:
		angle+= 315;
		break;
	}
	rotateimage(&nowimg, &img, PI*angle / 180, BLACK, true, true);
	nowlength = nowimg.getwidth();
	nowwidth = nowimg.getheight();
	switch (situation) {
	case 0: 
		x = rand() % (CANVAS_LENGTH+nowlength)-nowlength;
		y = -nowwidth;
		break;
	case 1: 
		x = CANVAS_LENGTH+nowlength / 2;
		y = rand() % (CANVAS_WIDTH+nowwidth)-nowwidth;
		break;
	case 2: 
		x = rand() % (CANVAS_LENGTH+nowlength)-nowlength;
		y = CANVAS_WIDTH+nowwidth;
		break;
	case 3: 
		x = -nowlength;
		y = rand() % (CANVAS_WIDTH+nowwidth)-nowwidth;
	}
}

void fish::move() {
	int newangle;
	newangle = rand() % 7 - 3;
	angle += newangle;
	angle %= 360;
	rotateimage(&nowimg, &img, PI*angle / 180 , BLACK, true, true);
	nowlength = nowimg.getwidth();
	nowwidth = nowimg.getheight();
	x += (int)(v*cos(PI*angle / 180)*t);
	y -= (int)(v*sin(PI*angle / 180)*t);
}

void fish::show() {
	transparentimage(NULL, x - nowlength / 2, y - nowwidth / 2, &nowimg, BLACK);
}

bool fish::is_dead(int cost) {
	int p = ((double)cost / value)*100;
	int random = rand() % 100;
	if (random <= p) return true;
	else return false;
}

void fish::dead(fish_collect *fc) {
	fc[kind].num++;
}