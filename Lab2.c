#include <REGX52.H>

 sbit clockMASTER = P2^4;	 //USTAWIENIE PINOW CLOCK DLA MASTER
 sbit dataMASTER = P2^5;	 //USTAWIENIE PINOW DATA DLA MASTER
 sbit clockSLAVE = P2^6;	 //USTAWIENIE PINOW CLOCK DLA SLAVE
 sbit dataSLAVE = P2^7;		 //USTAWIENIE PINOW DATA DLA SLAVE

 bit bitMASTER = 1;			 //DEKLARACJA ZMIENNYCH TYPU BIT I INT
 int datatime = 10;			 
 int time = 100;

 volatile unsigned char liczbaMASTER = 0x00;	   //ZMIENNE DLA MASTER I SLAVE
 unsigned char liczbaSLAVE = 0x00;
 unsigned char bajt = 0x00;




 void Delay(int i)				   //FUNKCJA SLUZACA DO OPOZNIANIA DZALANIA PROGRAMU
 {
	unsigned int k, l, m;
	for (l = 0; l < i; l++)
	{
		k = 500;
		m = 1000;
		k = m * l;
	}
 }

 void Int0() interrupt 0		//FUNKCJA DO PRZERWAN
 {
 	EX0 = 0;
	liczbaMASTER ++;
	if (liczbaMASTER == 255) liczbaMASTER = 0;	//ZEROWANIE ZMIENNEJ JEZELI OSIAGNIE WARTOSC 255
	EX0 = 1;
	bitMASTER = 0;						//SPRAWDZA CZY JEST JAKIES PRZERWANIE
 }

 void zapiszBajt(unsigned char bajt)	 //FUNKCJA DO ZAPISU POJEDYNCZEGO BAJTU NA MAGISTRALE
 {
	unsigned char liczbaBitow = 8; 	
	
	do 
	{
		dataMASTER = bajt & 0x01;
		Delay(datatime);
		Delay(time);
		clockMASTER = 0;
		Delay(time);
		clockMASTER = 1;
		Delay(time);
		bajt = (bajt >> 1);			  //PRZESUNIECIE W PRAWO
	} while(--liczbaBitow);
	clockMASTER = 1;
	dataMASTER = 1;
 }

 unsigned char czytajBajt()			 //FUNKCJA DO CZYANIA BAJTU
 {
	unsigned char liczbaBitow = 8; 	
	unsigned char wynik = 0; 	
	do 
	{
		while(clockSLAVE == 1)
		{
			Delay(datatime);
		}
		wynik = wynik >> 1;
		if (dataSLAVE) wynik = wynik | 0x80;
		while(clockSLAVE == 0)
		{
			Delay(datatime);
		}
	} while(--liczbaBitow);
	return wynik;
 }

 void initialize()				   //FUNKCJA DO INICJALIZOWANIA PRZERWAN
 {
 	liczbaMASTER = 0;

	IT0 = 1;	//INT0 aktywne zero
	EX0 = 1;	//Wlaczenia INT0
	EA = 1;		//Wlaczenie wszstkich przerwan
 }

 void main()
 {
 	initialize();
	P1 = 0;
	clockMASTER = 1;
	dataMASTER = 1;

	while (P0_0 == 1)		//MASTER
	{
		while(bitMASTER)	
		{
			Delay(datatime);
		}
		P1 = liczbaMASTER;
		zapiszBajt(liczbaMASTER);
		bitMASTER = 1;	 
	}

	while (P0_0 == 0) 	//SLAVE
	{
		while(clockSLAVE == 1)
		{
			Delay(datatime);
		}
		liczbaSLAVE = czytajBajt();
		P1 = liczbaSLAVE;
	}
 }