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
\**********************************************************************/

/*--- #includes der Form <...> ---------------------------------------*/

/*--- #includes der Form "..." ---------------------------------------*/
#include "CalibrateCamera.h"

/*--- #define-Konstanten und Makros ----------------------------------*/

/*--- Globale Variablen ----------------------------------------------*/

void CalibrateCamera::startCapture()
{
	board_sz = cvSize( board_w, board_h );
	capture = cvCreateCameraCapture( 1 );
	assert( capture );
}

int CalibrateCamera::calibrateCamera()
{
	printf("Kamerakalibrierung mit OpenCV 2.1\n");
	cvNamedWindow( "Calibration" );

	// Allocate Sotrage
	CvMat* image_points		= cvCreateMat( n_boards*board_n, 2, CV_32FC1 );
	CvMat* object_points		= cvCreateMat( n_boards*board_n, 3, CV_32FC1 );
	CvMat* point_counts			= cvCreateMat( n_boards, 1, CV_32SC1 );
	CvMat* intrinsic_matrix		= cvCreateMat( 3, 3, CV_32FC1 );
	CvMat* distortion_coeffs	= cvCreateMat( 5, 1, CV_32FC1 );

	CvPoint2D32f* corners = new CvPoint2D32f[ board_n ];
	successes = 0;
	step, frame = 0;
	
	IplImage *image = cvQueryFrame( capture );
	IplImage *gray_image = cvCreateImage( cvGetSize( image ), 8, 1 );

	// Capture Corner views loop until we've got n_boards
	// succesful captures (all corners on the board are found)
		while( successes < n_boards ){
		printf(".");
		// Skp every board_dt frames to allow user to move chessboard
		if( frame++ % board_dt == 0 ){
			// Find chessboard corners:
			int found = cvFindChessboardCorners( image, board_sz, corners,
				&corner_count, CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS );

			// Get subpixel accuracy on those corners
			cvCvtColor( image, gray_image, CV_BGR2GRAY );
			cvFindCornerSubPix( gray_image, corners, corner_count, cvSize( 11, 11 ), 
				cvSize( -1, -1 ), cvTermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));

			// Draw it
			cvDrawChessboardCorners( image, board_sz, corners, corner_count, found );
			cvShowImage( "Calibration", image );

			// If we got a good board, add it to our data
			if( corner_count == board_n ){
				step = successes*board_n;
				for( int i=step, j=0; j < board_n; ++i, ++j ){
					CV_MAT_ELEM( *image_points, float, i, 0 ) = corners[j].x;
					CV_MAT_ELEM( *image_points, float, i, 1 ) = corners[j].y;
					CV_MAT_ELEM( *object_points, float, i, 0 ) = j/board_w;
					CV_MAT_ELEM( *object_points, float, i, 1 ) = j%board_w;
					CV_MAT_ELEM( *object_points, float, i, 2 ) = 0.0f;
				}
				CV_MAT_ELEM( *point_counts, int, successes, 0 ) = board_n;
				successes++;
			}
		} 
		
		// Handle pause/unpause and ESC
		int c = cvWaitKey( 15 );
		if( c == 'p' ){
			c = 0;
			while( c != 'p' && c != 27 ){
				c = cvWaitKey( 250 );
			}
		}
		if( c == 27 )
			return 0;
		image = cvQueryFrame( capture ); // Get next image
	} // End collection while loop
	printf("\n\n");

	
	// Allocate matrices according to how many chessboards found
	CvMat* object_points2 = cvCreateMat( successes*board_n, 3, CV_32FC1 );
	CvMat* image_points2 = cvCreateMat( successes*board_n, 2, CV_32FC1 );
	CvMat* point_counts2 = cvCreateMat( successes, 1, CV_32SC1 );
	
	// Transfer the points into the correct size matrices
	for( int i = 0; i < successes*board_n; ++i ){
		CV_MAT_ELEM( *image_points2, float, i, 0) = CV_MAT_ELEM( *image_points, float, i, 0 );
		CV_MAT_ELEM( *image_points2, float, i, 1) = CV_MAT_ELEM( *image_points, float, i, 1 );
		CV_MAT_ELEM( *object_points2, float, i, 0) = CV_MAT_ELEM( *object_points, float, i, 0 );
		CV_MAT_ELEM( *object_points2, float, i, 1) = CV_MAT_ELEM( *object_points, float, i, 1 );
		CV_MAT_ELEM( *object_points2, float, i, 2) = CV_MAT_ELEM( *object_points, float, i, 2 );
	}

	for( int i=0; i < successes; ++i ){
		CV_MAT_ELEM( *point_counts2, int, i, 0 ) = CV_MAT_ELEM( *point_counts, int, i, 0 );
	}
	cvReleaseMat( &object_points );
	cvReleaseMat( &image_points );
	cvReleaseMat( &point_counts );

	// At this point we have all the chessboard corners we need
	// Initiliazie the intrinsic matrix such that the two focal lengths
	// have a ratio of 1.0

	CV_MAT_ELEM( *intrinsic_matrix, float, 0, 0 ) = 1.0;
	CV_MAT_ELEM( *intrinsic_matrix, float, 1, 1 ) = 1.0;

	// Calibrate the camera
	cvCalibrateCamera2( object_points2, image_points2, point_counts2, cvGetSize( image ), 
		intrinsic_matrix, distortion_coeffs, NULL, NULL, CV_CALIB_FIX_ASPECT_RATIO ); 

			// Save the intrinsics and distortions
	cvSave( "Intrinsics.xml", intrinsic_matrix );
	cvSave( "Distortion.xml", distortion_coeffs );
	//saveCalibrateData ("Intrinsics.xml", "Distortion.xml");


}

void CalibrateCamera::saveCalibrateData(const char* sIntrinsicsFile, const char* sDistortionFile)
{
		// Save the intrinsics and distortions
	cvSave( "Intrinsics.xml", intrinsic_matrix );
	cvSave( "Distortion.xml", distortion_coeffs );
}

void CalibrateCamera::showCalibrationData()
{
	// Default storage
	CvMat *intrinsic = (CvMat*)cvLoad( "Intrinsics.xml" );
	CvMat *distortion = (CvMat*)cvLoad( "Distortion.xml" );
	
	printf("\n");
	printf("********************************************************************************\n");
	printf("\t\t\tFolgende Kalibrierungsdaten wurden gespeichert:\n");
	printf("********************************************************************************\n");

	printf("\nIntrinsische Parameter:\n");
    printf("fx= %f\n",CV_MAT_ELEM( *intrinsic,float,0,0));
    printf("fy= %f\n",CV_MAT_ELEM( *intrinsic,float,1,1));
    printf("cx= %f\n",CV_MAT_ELEM( *intrinsic,float,0,2));
    printf("cy= %f\n\n",CV_MAT_ELEM( *intrinsic,float,1,2));

    printf("Radiale Distortionsparameter:\n");
    printf("k1= %f\n",CV_MAT_ELEM( *distortion,float,0,0));
    printf("k2= %f\n",CV_MAT_ELEM( *distortion,float,1,0));
    if( (distortion->rows)>4 ){
			printf("k3= %f\n\n",CV_MAT_ELEM( *distortion,float,4,0));
	}
	else{
			printf("\n");
	}

    printf("Tangentiale Distortionsparameter:\n");
    printf("p1= %f\n",CV_MAT_ELEM( *distortion,float,2,0));
    printf("p2= %f\n\n",CV_MAT_ELEM( *distortion,float,3,0));

	printf("\n\n\n");
	system("pause");
}


void CalibrateCamera::showCalibrationData(const char* sIntrinsics, const char* sDistortion)
{
	// Default storage
	CvMat *intrinsic = (CvMat*)cvLoad(sIntrinsics);
	CvMat *distortion = (CvMat*)cvLoad(sDistortion);
	
	printf("\n");
	printf("********************************************************************************\n");
	printf("\t\t\tFolgende Kalibrierungsdaten wurden gespeichert:\n");
	printf("********************************************************************************\n");

	printf("\nIntrinsische Parameter:\n");
    printf("fx= %f\n",CV_MAT_ELEM( *intrinsic,float,0,0));
    printf("fy= %f\n",CV_MAT_ELEM( *intrinsic,float,1,1));
    printf("cx= %f\n",CV_MAT_ELEM( *intrinsic,float,0,2));
    printf("cy= %f\n\n",CV_MAT_ELEM( *intrinsic,float,1,2));

    printf("Radiale Distortionsparameter:\n");
    printf("k1= %f\n",CV_MAT_ELEM( *distortion,float,0,0));
    printf("k2= %f\n",CV_MAT_ELEM( *distortion,float,1,0));
    if( (distortion->rows)>4 ){
			printf("k3= %f\n\n",CV_MAT_ELEM( *distortion,float,4,0));
	}
	else{
			printf("\n");
	}

    printf("Tangentiale Distortionsparameter:\n");
    printf("p1= %f\n",CV_MAT_ELEM( *distortion,float,2,0));
    printf("p2= %f\n\n",CV_MAT_ELEM( *distortion,float,3,0));

	printf("\n\n\n");
	system("pause");
}

void CalibrateCamera::showDeskewedImage()
{
		// Default storage
	CvMat *intrinsic = (CvMat*)cvLoad( "Intrinsics.xml" );
	CvMat *distortion = (CvMat*)cvLoad( "Distortion.xml" );

	// Build the undistort map that we will use for all subsequent frames
	IplImage* mapx = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
	IplImage* mapy = cvCreateImage( cvGetSize( image ), IPL_DEPTH_32F, 1 );
	cvInitUndistortMap( intrinsic, distortion, mapx, mapy );

	// Run the camera to the screen, now showing the raw and undistorted image
	cvNamedWindow( "Undistort" );

	while( image ){
		IplImage *t = cvCloneImage( image );
		cvShowImage( "Calibration", image ); // Show raw image
		cvRemap( t, image, mapx, mapy ); // undistort image
		cvReleaseImage( &t );
		cvShowImage( "Undistort", image ); // Show corrected image

		// Handle pause/unpause and esc
		int c = cvWaitKey( 15 );
		if( c == 'p' ){
			c = 0;
			while( c != 'p' && c != 27 ){
				c = cvWaitKey( 250 );
			}
		}
		if( c == 27 )
			break;
		image = cvQueryFrame( capture );
	}
}
