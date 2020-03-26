// ITMO University, 2015

// Реализовать защищённый двумерный массив, перегрузить операторы обращения к элементу [][] , сложения + , умножения * ,
// транспонирования ! и присваивания =

#include <iostream>
#include <cstdlib>
#include <assert.h>
#include <time.h>
using namespace std;

class row; // Класс "строка"
class matrix; // Класс "матрица"

class row // Класс "строка"
{
public:
	friend class matrix;
	int& operator [] (int j); // Оператор доступа к элементу
	int get_sz() { return sz; }
	int* get_massiv() { return p; }
private:
	row (int a); // Конструктор
	int* p;
	int sz; 
};

row::row(int a): sz(a) // Конструктор
{
	p=new int[a];
	assert(p!=0);
}

int& row::operator [] (int j) // Оператор доступа к элементу
{
	assert(j>=0 && j<sz); // Проверяется, не вышли ли мы за границы массива
	return p[j];
}

class matrix // Класс "матрица"
{ 
public:
	matrix(int a, int b); // Конструктор, a-количество столбцов
	matrix(const matrix& m); // Копирующий конструктор
	~matrix(); // Деструктор
	void init(); // Функция, инициализирующая массив значениями
	void copy(row** x, int r, int c); // Функция копирования массива
	row& operator [] (int i); // Оператор доступа к строке массива
	matrix& operator = (const matrix& m); // Оператор присваивания
	matrix operator + (const matrix& m); // Оператор сложения матриц
	matrix operator * (const matrix& m); // Оператор умножения матриц
	matrix operator * (int n); // Умножение матрицы на число
	friend matrix operator * (int n, matrix& m); // Умножение числа на матрицу
	matrix operator ! (); // Оператор транспонирования матрицы
	friend ostream& operator << (ostream& out, const matrix& m); // Оператор вывода матрицы в поток
private:
	row** p; // Указатель на двумерный массив
	int r; // Количество строк
	row** create_mas(int a, int b); // Функция, выделяющая память под массив
	void copyMassiv(row** x, int r, int c); // Функция, копирующая в новый массив элементы в нужном порядке (при транспонировании)
	void erase_mas(row** x, int r); // Функция, удаляющая массив
	void changeSquare(); // Функция, транспонирующая квадратную матрицу
	void changeRect(); // Функция, транспонирующая прямоугольную матрицу
};

row** matrix::create_mas(int a, int b) // Функция, выделяющая память под массив
{
	row** q=new row* [a];
	int i, j;
	for(i=0; i<a; i++)
	{
		q[i]=new row(b);
		assert(q[i]!=0); // Проверяется, выделилась ли память
	}
	return q;
}

matrix::matrix(int a, int b): r(a)  // Конструктор, a-количество строк, b-столбцов
{
	p=create_mas(a, b); 
}

matrix::matrix(const matrix& m): r(m.r) // Копирующий конструктор
{
	p=create_mas(m.r, m.p[0]->sz);
	copy(m.p, m.r, m.p[0]->sz);
}

void matrix::copy(row** x, int r, int c) // Функция, копирующая массив
{
	int i, j;
	for (i=0; i<r; i++)
		for (j=0; j<c; j++)
			p[i]->p[j]=x[i]->p[j];
}

void matrix::erase_mas(row** x, int r) // Функция, очищающая массив
{
	int i;
	for(i=0; i<r; i++)
		delete [] x[i];
	delete [] x;
}

matrix::~matrix() // Деструктор
{
	erase_mas(p, r); // Очищение массива
}

matrix& matrix::operator = (const matrix& m) // Оператор присваивания
{
	assert (m.p[0]->sz==p[0]->sz && m.r==r); // Проверяется, равны ли размеры двух матриц
	int i, j;
	copy(m.p, m.r, m.p[0]->sz); 
	return *this;
}

ostream& operator << (ostream& out, const matrix& a) // Оператор вывода матрицы в поток
{
	int i, j;
	for (i=0; i<a.r; i++)
	{
		for (j=0; j<a.p[0]->get_sz(); j++)
			out<<a.p[i]->get_massiv()[j]<<" ";
		out<<endl;
	}
	out<<endl;
	return out;
}

row& matrix::operator [] (int i) // Оператор доступа к строке матрицы
{
	assert(i>=0 && i<r); // Проверяется, не вышли ли мы за границы массива
	return *p[i];
}

matrix matrix::operator + (const matrix& m) // Оператор сложения двух матриц
{
	assert(m.p[0]->sz==p[0]->sz && m.r==r); // Проверяется, равны ли размеры двух матриц
	matrix n(m); // Создаётся новая матрица (копия 2ой)
	int i, j;
	for (i=0; i<r; i++)
		for (j=0; j<p[0]->sz; j++)
			n.p[i]->p[j]+=p[i]->p[j]; // К скопированной матрице прибавляется 1ая
	return n;
}

matrix matrix::operator * (const matrix& m) // Оператор умножения двух матриц
{
	assert(p[0]->sz==m.r); // Проверяется, равно ли количество столбцов 1ой матрицы количеству строк 2ой
	matrix n(r, m.p[0]->sz); // Создаётся новая матрица необходимого размера
	int i, j, k;
	for (i=0; i<r; i++)
	{
		for (j=0; j<m.p[0]->sz; j++)
		{
			n.p[i]->p[j]=0;
			for (k=0; k<p[0]->sz; k++)
				n.p[i]->p[j]+=p[i]->p[k]*m.p[k]->p[j]; // Результат умножения записывается в новую матрицу
		}
	}
	return n;
}

matrix matrix::operator * (int n){ // Оператор умножения матрицы на число
	matrix a(*this); // Создаётся копия матрицы 
	int i, j;
	for (i=0; i<r; i++)
		for (j=0; j<p[0]->sz; j++)
			a.p[i]->p[j]=p[i]->p[j]*n; // Скопированная матрица домножается на число
	return a;
}

matrix operator * (int n, matrix& m) // Умножение числа на матрицу
{
	return m*n;
}

void matrix::copyMassiv(row** x, int r, int c) // Функция, копирующая в массив элементы в нужном порядке (для транспонирования)
{ 
	int i, j;
	for (i=0; i<c; i++)
		for (j=0; j<r; j++)
			x[i]->p[j]=p[j]->p[i];
}

void matrix::changeSquare() // Функция, транспонирующая квадратную матрицу
{ 
	int i, j, k;
	for(i=0; i<r; i++)
	{
		for(j=0; j<i; j++)
		{ // Элементы, находящиеся под дигональю, меняются с элементами, находящимися над ней
			k=p[i]->p[j];
			p[i]->p[j]=p[j]->p[i];
			p[j]->p[i]=k;
		}
	}
}

void matrix::changeRect() // Функция для транспонирования прямоугольной матрицы
{
	int c=p[0]->sz; // Запоминаем старое количество столбцов
	row** t=create_mas(c, r); // Создание массива для транспонирования
	copyMassiv(t, r, c); // Элементы из старого массива копируются в новый транспонированный
	erase_mas(p, r); // Удаляется старый массив
	p=t;
	r=c; // Меняем количество строк
}

matrix matrix::operator ! () // Оператор транспонирования матрицы
{
	if (r==p[0]->sz) // Если совпадает количество строк и столбцов, вызывается функция транспонирования квадратной матрицы
		changeSquare(); 
	else // Иначе вызывается функция транспонирования прямоугольной матрцицы
		changeRect(); 
	return *this;
}

void matrix::init() // Функция, заполняющая матрицу случайными значениями
{
	srand(time(0));
	int i, j;
	for (i=0; i<r; i++)
		for (j=0; j<p[0]->sz; j++)
			p[i]->p[j]=rand()%6; // Матрица заполняется значениями от 0 до 5
}
