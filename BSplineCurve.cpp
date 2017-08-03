#include <Eigen/Dense>
#include <iostream>
#include <cmath>
#include <vector>
#include <gl/glut.h>

using namespace Eigen;
using namespace std;

void Display();
void Caculate_R(int i);
void Caculate_points(double a, double velocity);
double Caculate_delta(double delta_L);
Vector3d Caculate_Curve(double u);

vector<double> points; //һά�ɱ������洢���������·���㣬ÿ����Ԫ��Ϊһ������
vector<double> end_points; //�ݴ���ٶε�����·���㣬���Ҫ���ϵ�points��
double total_L = 0.0;
bool accelerate = true, decelerate = true; //�Ƿ�λ�ڼ��ٺͼ��ٶεı�־
bool flag = true; double flag_u = 0.0;
int j_accelerate = 0, j_decelerate = 0;  //��¼���ٻ��߼��ٵ��ڼ����׶�
int i_accelerate = 0, i_decelerate = 0;  //��¼���ٽ����Լ����ٿ�ʼ����ֵ������
double u_accelerate = 0.0, u_decelerate = 0.0;  //��¼���ٽ����Լ����ٿ�ʼ��uֵ
int i_biaoshi = 0; //��ʶ��������λ����һ�Σ����Ϊn-1=8
double u_j = 0; //��ǰ�������߶ε�uֵ
double T = 0.05; //�岹����

MatrixXd P(10, 3); //10����ֵ�㣬n+1=10
MatrixXd B(12, 3); //12�����Ƶ㣬10+2=12
Vector3d R3, R2, R1, R0; //B�������ߵĲ�������Pi = R0 + R1*u + R2*u*u + R3*u*u*u��0<=u<=1
int main(int argc, char *argv[])
{
	
	P << -0.5, -0.5, 0.0,
		-0.8, -0.1, 0.0,
		-0.4, 0.5, 0.0,
		-0.1, 0.6, 0.0,
		0.3, 0.4, 0.0,
		0.5, 0.25, 0.0,
		0.7, 0.0, 0.0,
		0.8, -0.2, 0.0,
		0.3, -0.4, 0.0,
		0.4, -0.6, 0.0; 
	
	int C[10];
	C[9] = 1, C[8] = 5;
	for (int i = 7; i >=0; i--){
		C[i] = 4 * C[i + 1] - C[i + 2];
	}
	double tmp[3];
	for (int i = 0; i < 3; i++){
		int k = 1;
		tmp[i] = 0.0;
		for (int j = 8; j >= 0; j--){
			tmp[i] += (C[j + 1] * P(j,i) * k);
			k = k*(-1);
		}
		tmp[i] = 6.0*tmp[i] / (C[1] + C[0]);
	}

	B(0, 0) = tmp[0], B(0, 1) = tmp[1], B(0, 2) = tmp[2];
	B(1, 0) = tmp[0], B(1, 1) = tmp[1], B(1, 2) = tmp[2];
	for (int i = 2; i < 11; i++){
		for (int j = 0; j < 3; j++){
			B(i, j) = 6 * P(i - 2, j) - 4 * B(i - 1, j) - B(i - 2, j);
		}
	}
	B(11, 0) = B(10, 0), B(11, 1) = B(10, 1), B(11, 2) = B(10, 2);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("B��������");
	glutDisplayFunc(&Display);
	glutMainLoop();

	return 0;
}
void Caculate_R(int i)
{
	R3 = (-B.row(i) + 3 * B.row(i + 1) - 3 * B.row(i + 2) + B.row(i + 3)) / 6;
	R2 = (3 * B.row(i) - 6 * B.row(i + 1) + 3 * B.row(i + 2)) / 6;
	R1 = (-3 * B.row(i) + 3 * B.row(i + 2)) / 6;
	R0 = (B.row(i) + 4 * B.row(i + 1) + B.row(i + 2)) / 6;
}
Vector3d Caculate_Curve(double u)
{
	return (R0 + R1*u + R2*u*u + R3*u*u*u);
}
double Caculate_delta(double delta_L)
{
	double a4, a3, a2, a1, a0;
	a4 = 9 * R3.transpose()*R3;
	a3 = 12 * R3.transpose()*R2;
	a2 = 4 * R2.transpose()*R2;
	a2 = a2 + (6 * R3.transpose()*R1);
	a1 = 4 * R2.transpose()*R1;
	a0 = R1.transpose()*R1;
	double delta;
	delta = sqrt(a4*pow(u_j, 4) + a3*pow(u_j, 3) + a2*pow(u_j, 2) + a1*u_j + a0);
	return delta_L/delta;
}
void Caculate_points(double a,double velocity)
{
	Vector3d tmp;
	Caculate_R(i_biaoshi);
	double delta_L = 0;
	while(accelerate){
		if (j_accelerate <= int(velocity / (a*T))){
			delta_L = (0.5 + j_accelerate)*a*T*T;
			total_L += delta_L;
			j_accelerate++;
			if (flag){ 
				flag_u = Caculate_delta(delta_L);
				flag = false;
			}
			u_j = u_j + Caculate_delta(delta_L);
			if (u_j > 1){
				u_j = u_j - 1;
				i_biaoshi++;
				Caculate_R(i_biaoshi);
			}
			tmp = Caculate_Curve(u_j);
			points.push_back(tmp(0));
			points.push_back(tmp(1));
			points.push_back(tmp(2));
		}
		else{
			i_accelerate = i_biaoshi;
			u_accelerate = u_j;
			accelerate = false;
		}
	}
	u_j = 0; //���ٵļ�������ٶԳƣ����Ե���������
	i_biaoshi = 8;  //���������㣬�����ȱ�ʾΪ���һ�����߶�
	//j_decelerate = 1;
	Caculate_R(i_biaoshi);
	while (decelerate){
		if (j_decelerate <= int(velocity / (a*T))){
			delta_L = (0.5 + j_decelerate)*a*T*T;
			total_L += delta_L;
			j_decelerate++;
			u_j = u_j + Caculate_delta(delta_L);
			if (u_j >1){
				u_j = u_j - 1;
				i_biaoshi--;
				Caculate_R(i_biaoshi);
			}
			tmp = Caculate_Curve(u_j);
			end_points.push_back(tmp(0));
			end_points.push_back(tmp(1));
			end_points.push_back(tmp(2));
		}
		else{
			i_decelerate = i_biaoshi;
			u_accelerate = u_j;
			decelerate = false;
		}
	}

	//���濪ʼ�������ٽ׶�
	i_biaoshi = i_accelerate;
	u_j = u_accelerate;
	Caculate_R(i_biaoshi);
	while(i_biaoshi < i_decelerate){
		delta_L = velocity*T;
		total_L += delta_L;
		u_j = u_j + Caculate_delta(delta_L);
		if (u_j >1){
			u_j = u_j - 1;
			i_biaoshi++;
			Caculate_R(i_biaoshi);
		}
		tmp = Caculate_Curve(u_j);
		points.push_back(tmp(0));
		points.push_back(tmp(1));
		points.push_back(tmp(2));
	}
	u_j = 0;
	//���ٽ��뵽���ٶ����ڵ����߶�
	/*while((u_j+u_decelerate<1))
	{
		delta_L = velocity*T;
		total_L += delta_L;
		u_j = u_j + Caculate_delta(delta_L);
		tmp = Caculate_Curve(u_j);
		points.push_back(tmp(0));
		points.push_back(tmp(1));
		points.push_back(tmp(2));
	}*/

	//��end_points���ϵ�points
	/*int count = end_points.size();
	while (count > 0){
		points.push_back(end_points[count - 3]);
		points.push_back(end_points[count - 2]);
		points.push_back(end_points[count-1]);
		count = count - 3;
	}*/
}

void Display(void)//OpenGL����������
{
	glClear(GL_COLOR_BUFFER_BIT);
	glEnableClientState(GL_VERTEX_ARRAY);
	float g_CP[24];
	int k = 0;
	for (int i = 0; i < 12; i++){
		for (int j = 0; j < 2; j++){
			g_CP[k] = B(i, j);
			cout << g_CP[k] << endl;
			k++;
		}
	}
	glPointSize(10);
	glColor3d(255, 0, 0);
	glBegin(GL_POINTS);
	k = 0;
	while (k < 24){
		glVertex2f(g_CP[k],g_CP[k+1]);
		k = k + 2;
	}
	glEnd();

	glVertexPointer(2, GL_FLOAT, 0, g_CP);
	glPointSize(2);
	glColor3d(255, 0, 0);
	glDrawArrays(GL_LINE_STRIP, 0, 12); //���պ����߻���

	glColor3d(0, 255, 0);
	glBegin(GL_LINE_STRIP);
	glPointSize(2);
	Caculate_points(0.5,0.1);
	int i = 0;
	while (i < points.size()){
		glVertex2f(points[i], points[i + 1]);
		i = i + 3;
	}
	cout << points.size()*0.05/3<<"��ʱ��" << endl;
	cout << "��·�� :" << total_L << endl;
	glEnd();
	glPointSize(10);
	glBegin(GL_POINTS);
	for (int i=0;i<10;i++){
		glVertex2f(P(i, 0), P(i, 1));
	}
	glEnd();
	glFlush();
}