/**********************************************************************\
* Dateiname: camera_calibration.cpp
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
\**********************************************************************/

/*--- #includes der Form <...> ---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

/*--- #includes der Form "..." ---------------------------------------*/
#include "CalibrateCamera.h"


int main(int argc, char* argv[])
{
	// Create new test class
	CalibrateCamera CamTest1(5, 5, 9, 6);
	
	// Start Test
	CamTest1.startCapture();
	CamTest1.calibrateCamera();
	CamTest1.showCalibrationData();

	return 0;
}


