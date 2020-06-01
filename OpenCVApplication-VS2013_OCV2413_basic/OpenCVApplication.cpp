#include "stdafx.h"
#include "common.h"
#include <queue>
#include <random>
#include <fstream>
#include <string> 
#include <time.h> 
#include <stdio.h>    
#include <stdlib.h>  
using namespace std;

	void generateImage(Point begin, Point end, cv::Mat src, string filename){
		
		Mat dst(20, 20, CV_8UC3);
		for (int x = 0, i = begin.x; x < dst.rows, i < end.x; x++, i++){
			for (int y = 0, j = begin.y; y < dst.cols, j < end.y; y++, j++){
				dst.at<Vec3b>(x, y) = src.at<Vec3b>(i, j);
			}
		}
		imwrite(filename, dst);
	}

	void generate(vector<Point> points, Mat src){
		string path = "Images/images/24bit/generate/";
		string extension = ".bmp";

		for (int i = 0; i < points.size() - 1; i += 2){
			string filename = "";
			Point begin = points[i];
			Point end = points[i + 1];
			string name = to_string(i);
			filename = filename.append(path).append(name).append(extension);
			generateImage(begin, end, src, filename);
		}
	}

	void imageCropping(int gen){
		Mat src = imread("Images/sky24.bmp", CV_LOAD_IMAGE_COLOR);
		vector<Point> points;
		printf("x: %d, y: %d \n", src.cols, src.rows);

		//imi impart imaginea in imagini mici de 20x20 prin memorarea coordonatelor colturilor stanga-sus si dreapta-jos intr-o stiva
		for (int i = 0; i <= src.rows - 20; i++){
			for (int j = 0; j < src.cols; j++){
				if (i % 20 == 0 && j % 20 == 0){
					points.push_back(Point(i, j));
					points.push_back(Point(i + 20, j + 20));
				}
			}
		}
	
		printf("size %d\n", points.size());

		if (gen == 0){
			generate(points, src); //aici se genereaza imaginile in fisier
		}
		

		//matricea de medii de culori pentru fiecare imagine de 20x20
		Mat midInitial(20, 32, CV_32FC3);

		//matricea de medii extrase dni fisier
		Mat midFromFile(20, 32, CV_32FC3);

		//matricea de medii rezultat de tip float
		Mat midResultF(20, 32, CV_32FC3);
		
		//vectorul de imagini extrase din fisier
		vector<Mat> images;

		//popularea matricii de medii ale imaginilor decupate din imaginea initiala
		int row = 0;
		int col = 0;
		for (int i = 0; i < points.size() - 1; i += 2){

			//punctele begin si end (stanga sus si dreapta jos)
			Point begin = points[i];
			Point end = points[i + 1];

			//in sum imi voi retine suma valorilor culorilor pentru RGB
			float sum[3] = { 0 };

			//imi parcurg imaginea de 20x20 si fac suma culorilor
			for (int i = begin.x; i < end.x; i++){
				for (int j = begin.y; j < end.y; j++){
					sum[0] += src.at<Vec3b>(i, j)[0];
					sum[1] += src.at<Vec3b>(i, j)[1];
					sum[2] += src.at<Vec3b>(i, j)[2];
				}
			}

			//fac media (impartind suma aflata pentru RGB la 20*20) si o salvez in matricea de medii
			midInitial.at<Vec3f>(row, col)[0] = sum[0] / (20 * 20);
			midInitial.at<Vec3f>(row, col)[1] = sum[1] / (20 * 20);
			midInitial.at<Vec3f>(row, col)[2] = sum[2] / (20 * 20);

			//imaginea mea initiala fiind de 400x640 => matricea de valori corespunzatoare se reduce la dimensiunea 20x32 => cand se ajunge la 31 (pentru ca incepe de la 0) trebuie sa se reseteze coloana si sa creasca linia
			if (col == 31){
				row++;
				col = 0;
			}
			else{
				col++;
			}
		}
		
		//fixez path-ul si formatul imaginilor care vor urma citite
		string path = "Images/images/24bit/generate/";
		string extension = ".bmp";
		
		//initializez randul si coloana
		row = 0;
		col = 0;

		//aici am sa generez un vector de denumiri ale imaginilor din fisierul meu (numele imaginilor mele in fisier sunt 0, 2, 4, ... 1278
		//in acest vector am sa salvez numele imaginilor
		vector<int> details = { 0 };
		//acest vector imi va spune daca o anumita denumire s-a mai generat sau nu (o sa fie un fel de vector de aparitii => daca il generez am sa il marchez cu 1 si daca e 0 atunci inseamna ca nu s-a mai generat anterior)
		int busy[640] = { 0 };
		//in aceasta variabila am sa imi retin variabila generata
		int randomNumber;
		//marcheaza daca s-a generat un numar bun si se poate continua flow-ul
		bool ok = false;
		//mi-am luat un contor pentru ca am observat ca in momentul in care sunt putin valori nemarcate ca si "generat" le gaseste greu 
		//si atunci daca s-a generat de 10 ori o valoare care s-a mai generat anterior => o sa se faca variabila "cursive" true si nu o sa se mai genereze random denumirile
		//ci o sa se parcurga vectorul "details" si o sa se extraga denumirile negenerate anterior
		int tooMuch = 0;
		int auxNumber = 0;
		bool cursive = false;

		//se populeaza vectorul details cu valori pare intre 0 si 1278
		for (int i = 0; i < 640; i++){
			details.push_back(i * 2);
		}

		srand(time(NULL));

		for (int i = 0; i < 640; i++){

			ok = false;
			tooMuch = 0;

			while (ok == false && cursive == false){
				//se genereaza random o valoare intre 0 si 640 (640 e dimensiunea vectorului "details")
				randomNumber = rand() % 640;
				if (busy[randomNumber] == 0){
					busy[randomNumber] = 1;
					ok = true;
				}
				else{
					//daca e busy numarul random generat atunci de incrementeaza tooMuch
					tooMuch++;
				}

				//daca s-a generat de 10 ori si tot nu s-a gasit => cursive o sa se faca true si nu o sa se mai intre in while-ul asta
				if (tooMuch == 10){
					cursive = true;
					break;
				}
			}

			//daca cursive este true atunci se iau pe rand pozitiile negenerate anterior
			if (cursive == true){
				for (int j = auxNumber; j < 640; j++){
					if (busy[j] == 0){
						busy[j] = 1;
						auxNumber = j;
						randomNumber = j;
						break;
					}
				}
			}
	
			// se compune path-ul catre imaginea dorita
			string name = to_string(details[randomNumber]);

			string filename = "";
			filename = filename.append(path).append(name).append(extension);

			//se citeste imaginea din fisier si se adauga in vectorul de imagini
			Mat auxImage = imread(filename, CV_LOAD_IMAGE_COLOR);
			images.push_back(auxImage);

			//initializam vectorul suma pentru RGB
			float sum[3] = { 0 };
			for (int x = 0; x < auxImage.rows; x++){
				for (int y = 0; y < auxImage.cols; y++){
					//calculam suma culorilor pentru fiecare pixel
					sum[0] += auxImage.at<Vec3b>(x, y)[0];
					sum[1] += auxImage.at<Vec3b>(x, y)[1];
					sum[2] += auxImage.at<Vec3b>(x, y)[2];
				}
			}

			//se imparte fiecare suma la numarul de pixeli ai imaginii
			midFromFile.at<Vec3f>(row, col)[0] = sum[0] / (20 * 20);
			midFromFile.at<Vec3f>(row, col)[1] = sum[1] / (20 * 20);
			midFromFile.at<Vec3f>(row, col)[2] = sum[2] / (20 * 20);

			//daca s-a ajuns la sfarsit de linie atunci se trece la urmatoarea linie si se revina la prima coloana
			if (col == 31){
				row++;
				col = 0;
			}
			else{
				//daca nu, se tot trece la urmatoarea coloana de pe aceeasi linie
				col++;
			}
		}

		//in "min" am sa imi retin mereu valoarea minima gasita
		Vec3f min;
		//este o variabila in care imi salvez cate o imagine temporar
		Mat aux;
		//in "iAux" si "jAux" imi salvez pozitia pe care mi-am gasit valoarea minima
		int iAux = 0;
		int jAux = 0;
		//un contor pentru vectorul de imagini ca sa stiu la ce imagine ma situez in vector
		int contor = 0;
		//"auxRow" si "auxCol" sunt folositi pentru a retine pozitia liniei si a coloanei in imaginea mare rezultat dupa scrierea fiecare bucati mai mici de imagine
		int auxRow = 0;
		int auxCol = 0;
		//imaginea rezultat
		Mat result(src.rows, src.cols, CV_8UC3);

		//parcurg matricea de medii corespunzatoare imaginii mele initiale (e ca un reper)
		for (int i = 0; i < midInitial.rows; i++){
			for (int j = 0; j < midInitial.cols; j++){

				//imi actualizez minimul cu o valoare mare
				min = Vec3f(255, 255, 255);
				
				//parcurg matricea de medii corespunzatoare matricii de medii ale imaginilor citite din fisiere
				for (int ii = 0; ii < midFromFile.rows; ii++){
					for (int jj = 0; jj < midFromFile.cols; jj++){

						//daca se gaseste o valoare mai mica se va actualiza minimul
						if ((min[0] >= abs(midFromFile.at<Vec3f>(ii, jj)[0] - midInitial.at<Vec3f>(i, j)[0])) && 
							(min[1] >= abs(midFromFile.at<Vec3f>(ii, jj)[1] - midInitial.at<Vec3f>(i, j)[1])) && 
							(min[2] >= abs(midFromFile.at<Vec3f>(ii, jj)[2] - midInitial.at<Vec3f>(i, j)[2]))){
							
							min = Vec3f(abs(midFromFile.at<Vec3f>(ii, jj)[0] - midInitial.at<Vec3f>(i, j)[0]),
								abs(midFromFile.at<Vec3f>(ii, jj)[1] - midInitial.at<Vec3f>(i, j)[1]),
								abs(midFromFile.at<Vec3f>(ii, jj)[2] - midInitial.at<Vec3f>(i, j)[2]));
							
							//si se va retine pozitia minimului gasit
							iAux = ii;
							jAux = jj;
						}
					}
				}
			
				//retin intr-o variabila auxiliara imaginea corespunzatoare minimului gasit in vectorul de imagini
				aux = images[jAux + iAux * 32];

				//daca contorul meu ajunge la 32 si nu e 0 => s-a scris in imaginea rezultat un rand de imagini mici si e nevoie sa se treaca pe urmatoarea linie de imagini
				//o imagine mica este de 20 x 20 => se incrementeaza cu 20 auxRow
				if (contor > 0 && contor % 32 == 0){
					auxRow += 20;
				}
				
				//se scrie in imaginea rezultat imaginea de dimensiune mica
				for (int ii = 0; ii < aux.rows; ii++){
					for (int jj = 0; jj < aux.cols; jj++){
						result.at<Vec3b>(auxRow + ii, auxCol + jj) = aux.at<Vec3b>(ii, jj);
					}
				}
				//se incrementeaza auxCol cu dimensiunea imaginii mici adaugate in rezultat
				auxCol += aux.cols;

				//daca s-a ajuns la sfarsitul liniei atunci se trece pe urmatoare => se reseteaza auxCol
				if (auxCol == 640){
					auxCol = 0;
				}
				
				//contorul se incrementeaza dupa fiecare imagine copiata in rezultat
				contor++;

			}
		}

		//se afiseaza atat rezultatul, cat si imaginea initiala
		imshow("result", result);
		imshow("initial", src);
		waitKey(0);
	}



	
	
	int main()
	{
		
		int op;
		do
		{
			system("cls");
			destroyAllWindows();
			printf("Menu:\n");
			printf(" 0 - Cu generare\n");
			printf(" 1 - Fara generare\n");
			printf("Option: ");
			scanf("%d", &op);
			switch (op)
			{
			case 0:
				//daca se trimite cu parametrul 0 => se vor genera imaginile de dimensiuni mici in fisierul cu imagini
				imageCropping(0);
				break;
			case 1:
				//daca se trimite cu parametrul 1 => nu se vor genera imaginile de dimensiuni mici in fisierul cu imagini
				imageCropping(1);
				break;
			}
		} while (op != 0);
		return 0;
	}