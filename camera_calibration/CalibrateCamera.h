/**********************************************************************\
* Dateiname: CalibrateCamera.h
* Autor : W. Kubinger
* Projekt : SafeCon
* Copyright (C) FH Technikum Wien 2011 sowie OpenCV
*
* Kurzbeschreibung: Dieses Programm demonstriert, wie man mit Hilfe von
*    Open CV (2.1) eine Kamera kalibrieren kann. Das Programm basiert auf
*    einem Beispiel des OpenCV-Tutorials.
*
* Abhängigkeiten:
*   - Open CV 2.1 muss installiert sein (siehe http://opencv.willowgarage.com/wiki/)
*   - MS Visual C++ 2010 Express
*
* Datum: Autor: Grund der Aenderung:
* 2009: N. Kuntz: Ersterstellung für das OpenCV-Tutorial
* 19.10.2011: W. Kubinger: Adaptierung für FH Technikum Wien
* 28.10.2011: W. kubinger: Test unter MS Visual C++ 2010 Express
*
Diese Klasse soll das Folgende ermöglichen:

*) Kalibrieren einer angeschlossenen Kamera (z.B. einer Webcam - nehmen Sie dazu einen Ausdruck von chess.png als Referenzbild, das vor die Webcam gehalten wird)
*) Ausgabe der Kalibrierdaten am Bildschrim
*) Abspeichern der Kalibrierdaten in Dateien
*) Ausgabe eines Originalbildes und eines entzerrten Bildes
*) Die obigen Funktionen sollen parametrierbar sein

Die Klasse soll auf OpenCV 2.1 basieren (siehe http://opencv.willowgarage.com/wiki/) und mit MS Visual C++ 2010 Express erstellt werden. Sie können davon ausgehen, dass auf der Testmachine des Lektors
sowohl OpenCV als auch Visual Studie installiert sind.

Im Detail sollen Sie die folgenden Teilaufgaben lösen:

*) Erstellen einer Klasse "CalibrateCamera" (Dateien "CalibrateCamera.cpp/.h"
*) Erstellen eines kurzen Testprogramms, das die Klasse "CalibrateCamera" nützt und die selbe Funktionalität wie das Demoprogram hat (Datei "TestCalibrateCamera.cpp")
*) Die Klasse und das Testprogramm sind zwei getrennte CPP-Dateien, die getrennt übersetzt und dann gelinkt werden.

Hinweis: Bitte geben Sie dieses Programm entweder als MS Visual C++ 2010 Projekt oder mit einem Batchfile ab, das das korrekte Übersetzen und Linken übernimmt.
\**********************************************************************/

/*--- #includes der Form <...> ---------------------------------------*/
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*--- #includes der Form "..." ---------------------------------------*/

/*--- #define-Konstanten und Makros ----------------------------------*/
#define NOB 20     // Number of boards for calibration
#define DEFAULT_INTRINISCS "Intrinsics.xml" 
#define DEFAULT_DISTORTION "Distortion.xml"
/*--- Globale Variablen ----------------------------------------------*/


class CalibrateCamera
{
	public:

		void startCapture();
		void showCalibrationData();
		void showCalibrationData(std::string sIntrinsicsFile, std::string sDistortionFile);
		int calibrateCamera();
		int calibrateCamera(std::string sIntrinsicsFile, std::string sDistortionFile);
		void saveCalibrateData(std::string sIntrinsicsFile, std::string sDistortionFile);
		void showDeskewedImage();
		void showDeskewedImage(std::string sIntrinsicsFile, std::string sDistortionFile);


		// Konstruktor
		CalibrateCamera(int set_n_boards = 20, const int set_board_dt = 1, int set_board_w = 0, int set_board_h = 0) : 
		n_boards(set_n_boards),
		board_dt(set_board_dt),
		board_w(set_board_w),
		board_h(set_board_h),
		successes(0),
		step(0),
		frame(0)

		{ 
			board_n = board_w * board_h;
		};

	protected:
	private:
		int n_boards;
		int board_n;
		const int board_dt;
		int board_w;
		int board_h;
		int corner_count;
		int successes;
		int step;
		int frame;

		CvSize board_sz;
		CvCapture* capture;

		CvMat* image_points;
		CvMat* object_points;
		CvMat* point_counts;
		CvMat* intrinsic_matrix;
		CvMat* distortion_coeffs;
		
		CvMat* object_points2 ;
		CvMat* image_points2;
		CvMat* point_counts2;
		CvPoint2D32f* corners;

		IplImage *image;
		IplImage *gray_image;


};

