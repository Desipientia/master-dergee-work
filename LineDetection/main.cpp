#include <iostream>
#include <cmath>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

int main(int argc, char* argv[]) {
    size_t from, to;
    string filename, saveFilename;
    Mat source, edge, colorEdge;
    vector<Vec4i> lines1, lines2;

    filename = argc >= 2 ? argv[1] : "../Images/01.jpg";
    source = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);

    if(!source.data){
        cout << "ERROR: Cannot open the image: " << filename << endl;
        return -1;
    }

    Canny(source, edge, 50, 200, 3);
    cvtColor(edge, colorEdge, CV_GRAY2BGR);

    HoughLinesP(edge, lines1, 1, CV_PI/180, 80, 5, 2);
    HoughLinesP(edge, lines2, 1, CV_PI/180, 80, 50, 2);

    for (size_t i = 0; i < lines1.size(); i++) {
        line(colorEdge, Point(lines1[i][0], lines1[i][1]),
            Point(lines1[i][2], lines1[i][3]), Scalar(0, 0, 255), 2, 8);
    }

    for (size_t i = 0; i < lines2.size(); i++) {
        line(colorEdge, Point(lines2[i][0], lines2[i][1]),
            Point(lines2[i][2], lines2[i][3]), Scalar(255, 0, 0), 1, 8);
    }

    from = filename.find_last_of("/") + 1;
    to = filename.find_last_of(".");
    saveFilename = filename.substr(from, to - from);

    imwrite("Result/" + saveFilename + ".jpg", source);
    imwrite("Result/" + saveFilename + "_lines.jpg", colorEdge);

    return 0;
}