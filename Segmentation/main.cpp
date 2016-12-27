#include <opencv2/core/core.hpp>
#include "opencv2/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {
    if (argc != 2) {
        cout <<" Usage: Segmentation ImageToLoad" << endl;
        return -1;
    }

    Mat image = imread(argv[1]), gray, thresh;

    if (!image.data) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    imshow("Image", image);

    cvtColor(image, gray, COLOR_BGR2GRAY);
    imshow("Gray", gray);

    threshold(gray, thresh, 0, 255, THRESH_BINARY_INV + THRESH_OTSU);
    imshow("Threshold", thresh);

    Mat opening, sure_bg, dist_transform, sure_fg, unknown;
    Mat kernel = Mat::ones(3, 3, CV_8U);
    morphologyEx(thresh, opening, MORPH_OPEN, kernel, Point(-1, -1), 2);
    imshow("Opening", opening);

    dilate(opening, sure_bg, kernel, Point(-1, -1), 3);
    imshow("Background", sure_bg);

    double min, max;
    distanceTransform(opening, dist_transform, DIST_L2, 5);
    normalize(dist_transform, dist_transform, 0, 1, NORM_MINMAX);
    imshow("Distance Transform", dist_transform);

    minMaxLoc(dist_transform, &min, &max);
    threshold(dist_transform, sure_fg, 0.5 * max, 255, 0);

    imshow("Foreground", sure_fg);

    sure_fg.convertTo(sure_fg, CV_8U);
    subtract(sure_bg, sure_fg, unknown);

    imshow("Unknown", unknown);

    vector< vector<Point> > contours;
    findContours(sure_fg, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    Mat markers = Mat::zeros(sure_fg.size(), CV_32SC1);

    for (size_t i = 0; i < contours.size(); i++)
        drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1);

    imshow("Markers", markers * 10000);

    watershed(image, markers);
    Mat wshed(markers.size(), CV_8UC3);

    vector<Vec3b> colors;
    for (size_t i = 0; i < contours.size(); i++) {
        int b = theRNG().uniform(0, 255);
        int g = theRNG().uniform(0, 255);
        int r = theRNG().uniform(0, 255);
        colors.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
    }

    for (int i = 0; i < markers.rows; i++) {
        for (int j = 0; j < markers.cols; j++) {
            int value = markers.at<int>(i, j);
            if (value == -1) {
                wshed.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
                image.at<Vec3b>(i, j) = Vec3b(0, 0, 255);
            } else {
                wshed.at<Vec3b>(i, j) = colors[value];
            }
        }
    }

    imshow("Segmentation", image);
    imshow("Watershed", wshed);

    waitKey(0);

    return 0;
}